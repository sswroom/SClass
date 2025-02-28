#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayList.h"
#include "Data/DateTime.h"
#include "IO/Stream.h"
#include "Net/HTTPClient.h"
#include "Net/HTTPMyClient.h"
#include "Net/HTTPOSClient.h"
#include "Net/SocketFactory.h"
#include "Net/TCPClient.h"
#include "Sync/Event.h"
#include "Text/Encoding.h"
#include "Text/EncodingFactory.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/TextBinEnc/FormEncoding.h"
#include "Text/TextBinEnc/URIEncoding.h"

Net::HTTPClient::HTTPClient(NN<Net::TCPClientFactory> clif, Bool kaConn) : IO::Stream(CSTR("HTTPClient"))
{
	this->clif = clif;
	this->canWrite = false;
	this->contLeng = 0;
	this->respStatus = Net::WebStatus::SC_UNKNOWN;
	this->url = Text::String::NewEmpty();
	this->sbForm = 0;
	this->hdrLen = 0;
	this->totalUpload = 0;
	this->totalDownload = 0;
	this->forceHost = 0;
	this->kaConn = kaConn;
	this->svrAddr.addrType = Net::AddrType::Unknown;
}

Net::HTTPClient::~HTTPClient()
{
	this->headers.FreeAll();
	this->sbForm.Delete();
	this->url->Release();
	OPTSTR_DEL(this->forceHost);
}

Bool Net::HTTPClient::IsDown() const
{
	return this->IsError();
}

IO::StreamType Net::HTTPClient::GetStreamType() const
{
	return IO::StreamType::HTTPClient;
}

Bool Net::HTTPClient::FormBegin()
{
	if (this->canWrite && this->sbForm.IsNull())
	{
		this->AddContentType(CSTR("application/x-www-form-urlencoded"));
		NEW_CLASSOPT(this->sbForm, Text::StringBuilderUTF8());
		return true;
	}
	return false;
}

Bool Net::HTTPClient::FormAdd(Text::CStringNN name, Text::CString value)
{
	NN<Text::StringBuilderUTF8> sbForm;
	if (!this->sbForm.SetTo(sbForm))
	{
		return false;
	}
	UTF8Char sbuff[256];
	UnsafeArray<UTF8Char> sptr;
	sptr = Text::TextBinEnc::URIEncoding::URIEncode(sbuff, name.v);
	if (sbForm->GetLength() > 0)
	{
		sbForm->AppendUTF8Char('&');
	}
	sbForm->AppendC(sbuff, (UOSInt)(sptr - sbuff));
	sbForm->AppendUTF8Char('=');
	Text::CStringNN nnval;
	if (value.SetTo(nnval))
	{
		sptr = Text::TextBinEnc::URIEncoding::URIEncode(sbuff, nnval.v);
		sbForm->AppendC(sbuff, (UOSInt)(sptr - sbuff));
	}
	return true;
}

void Net::HTTPClient::AddTimeHeader(Text::CStringNN name, NN<Data::DateTime> dt)
{
	UTF8Char sbuff[64];
	UnsafeArray<UTF8Char> sptr;
	sptr = Net::WebUtil::Date2Str(sbuff, dt);
	this->AddHeaderC(name, CSTRP(sbuff, sptr));
}

void Net::HTTPClient::AddTimeHeader(Text::CStringNN name, Data::Timestamp ts)
{
	UTF8Char sbuff[64];
	UnsafeArray<UTF8Char> sptr;
	sptr = Net::WebUtil::Date2Str(sbuff, ts);
	this->AddHeaderC(name, CSTRP(sbuff, sptr));
}

void Net::HTTPClient::AddContentType(Text::CStringNN contType)
{
	this->AddHeaderC(CSTR("Content-Type"), contType);
}

void Net::HTTPClient::AddContentLength(UInt64 leng)
{
	UTF8Char sbuff[32];
	UnsafeArray<UTF8Char> sptr;
	sptr = Text::StrUInt64(sbuff, leng);
	this->AddHeaderC(CSTR("Content-Length"), CSTRP(sbuff, sptr));
}

Bool Net::HTTPClient::WriteContent(Text::CStringNN contType, Text::CStringNN content)
{
	this->AddContentType(contType);
	this->AddContentLength(content.leng);
	return this->Write(content.ToByteArray()) == content.leng;
}

void Net::HTTPClient::ForceHostName(Text::CStringNN hostName)
{
	OPTSTR_DEL(this->forceHost);
	if (hostName.leng > 0)
		this->forceHost = Text::String::New(hostName);
}

UOSInt Net::HTTPClient::GetRespHeaderCnt() const
{
	return this->headers.GetCount();
}

UnsafeArrayOpt<UTF8Char> Net::HTTPClient::GetRespHeader(UOSInt index, UnsafeArray<UTF8Char> buff)
{
	NN<Text::String> s;
	if (this->headers.GetItem(index).SetTo(s))
		return s->ConcatTo(buff);
	else
		return 0;
}

UnsafeArrayOpt<UTF8Char> Net::HTTPClient::GetRespHeader(Text::CStringNN name, UnsafeArray<UTF8Char> valueBuff)
{
	Text::CStringNN v;
	if (GetRespHeader(name).SetTo(v))
	{
		return v.ConcatTo(valueBuff);
	}
	return 0;
}

Bool Net::HTTPClient::GetRespHeader(Text::CStringNN name, NN<Text::StringBuilderUTF8> sb)
{
	Text::CStringNN v;
	if (GetRespHeader(name).SetTo(v))
	{
		sb->Append(v);
		return true;
	}
	return false;
}

Text::CString Net::HTTPClient::GetRespHeader(Text::CStringNN name)
{
	UTF8Char buff[256];
	UnsafeArray<UTF8Char> s2;
	NN<Text::String> s;
	s2 = Text::StrConcatC(name.ConcatTo(buff), UTF8STRC(":"));
	Data::ArrayIterator<NN<Text::String>> it = this->headers.Iterator();
	while (it.HasNext())
	{
		s = it.Next();
		if (s->StartsWithICase(buff, (UOSInt)(s2 - buff)))
		{
			return Text::CStringNN(&s->v[s2-buff], s->leng - (UOSInt)(s2 - buff)).LTrim();
		}
	}
	return CSTR_NULL;
}

Optional<Text::String> Net::HTTPClient::GetRespHeader(UOSInt index) const
{
	return this->headers.GetItem(index);
}

Data::ArrayIterator<NN<Text::String>> Net::HTTPClient::RespHeaderIterator() const
{
	return this->headers.Iterator();
}

UInt64 Net::HTTPClient::GetContentLength()
{
	this->EndRequest(0, 0);
	return this->contLeng;
}

UInt32 Net::HTTPClient::GetContentCodePage()
{
	UTF8Char sbuff[256];
	UnsafeArray<UTF8Char> sptr;
	Text::PString sarr[2];
	UOSInt arrCnt;
	this->EndRequest(0, 0);
	if (this->GetRespHeader(CSTR("Content-Type"), sbuff).SetTo(sptr))
	{
		sarr[1].v = sbuff;
		sarr[1].leng = (UOSInt)(sptr - sbuff);
		arrCnt = 2;
		while (arrCnt > 1)
		{
			arrCnt = Text::StrSplitP(sarr, 2, sarr[1], ';');
			if (sarr[0].StartsWith(UTF8STRC("charset=")))
			{
				Text::EncodingFactory encFact;
				return encFact.GetCodePage(sarr[0].ToCString().Substring(8));
			}
		}
	}
	return 0;
}

void Net::HTTPClient::GetContentFileName(NN<Text::StringBuilderUTF8> sb)
{
	this->EndRequest(0, 0);
	UOSInt i;
	Text::CStringNN hdr;
	if (this->GetRespHeader(CSTR("Content-Disposition")).SetTo(hdr))
	{
		i = hdr.IndexOf(CSTR("filename=\""));
		if (i != INVALID_INDEX)
		{
			hdr = hdr.Substring(i + 10);
			i = hdr.IndexOf('\"');
			if (i != INVALID_INDEX)
			{
				Text::TextBinEnc::FormEncoding::FormDecode(sb, Text::CStringNN(hdr.v, i));
			}
			else
			{
				Text::TextBinEnc::FormEncoding::FormDecode(sb, hdr);
			}
			return;
		}
	}
	i = this->url->LastIndexOf('/');
	if (i == INVALID_INDEX || i < 8)
	{
		sb->Append(CSTR("index.html"));
	}
	else
	{
		Text::CStringNN url = this->url->ToCString().Substring(i + 1);
		i = url.IndexOf('?');
		if (i == INVALID_INDEX)
		{
			Text::TextBinEnc::FormEncoding::FormDecode(sb, url);
		}
		else
		{
			Text::TextBinEnc::FormEncoding::FormDecode(sb, Text::CStringNN(url.v, i));
		}
	}
}

Bool Net::HTTPClient::GetLastModified(NN<Data::DateTime> dt)
{
	UTF8Char sbuff[64];
	UnsafeArray<UTF8Char> sptr;
	this->EndRequest(0, 0);
	if (this->GetRespHeader(CSTR("Last-Modified"), sbuff).SetTo(sptr))
	{
		ParseDateStr(dt, CSTRP(sbuff, sptr));
		return true;
	}
	return false;
}

Bool Net::HTTPClient::GetLastModified(OutParam<Data::Timestamp> ts)
{
	UTF8Char sbuff[64];
	UnsafeArray<UTF8Char> sptr;
	this->EndRequest(0, 0);
	if (this->GetRespHeader(CSTR("Last-Modified"), sbuff).SetTo(sptr))
	{
		ts.Set(ParseDateStr(CSTRP(sbuff, sptr)));
		return true;
	}
	return false;
}

Bool Net::HTTPClient::GetServerDate(NN<Data::DateTime> dt)
{
	UTF8Char sbuff[64];
	UnsafeArray<UTF8Char> sptr;
	this->EndRequest(0, 0);
	if (this->GetRespHeader(CSTR("Date"), sbuff).SetTo(sptr))
	{
		ParseDateStr(dt, CSTRP(sbuff, sptr));
		return true;
	}
	return false;
}

Text::CString Net::HTTPClient::GetTransferEncoding()
{
	return this->GetRespHeader(CSTR("Transfer-Encoding"));
}

Text::CString Net::HTTPClient::GetContentType()
{
	return this->GetRespHeader(CSTR("Content-Type"));
}

Optional<Text::String> Net::HTTPClient::GetURL()
{
	if (this->url->leng > 0)
		return this->url;
	else
		return 0;
}

Net::WebStatus::StatusCode Net::HTTPClient::GetRespStatus()
{
	this->EndRequest(0, 0);
	return this->respStatus;
}

Double Net::HTTPClient::GetTotalTime()
{
	return this->clk.GetTimeDiff();
}

UOSInt Net::HTTPClient::GetHdrLen()
{
	return this->hdrLen;
}

UInt64 Net::HTTPClient::GetTotalUpload()
{
	return this->totalUpload;
}

UInt64 Net::HTTPClient::GetTotalDownload()
{
	return this->totalDownload;
}

Bool Net::HTTPClient::ReadAllContent(NN<IO::Stream> outStm, UOSInt buffSize, UInt64 maxSize)
{
	UInt64 contLeng = this->GetContentLength();
	UOSInt currPos = 0;
	UOSInt readSize;
	if (contLeng > 0 && contLeng <= maxSize)
	{
		UInt8 *readBuff = MemAlloc(UInt8, buffSize);
		while ((readSize = this->Read(Data::ByteArray(readBuff, buffSize))) > 0)
		{
			outStm->Write(Data::ByteArrayR(readBuff, readSize));
			currPos += readSize;
			if (currPos >= contLeng)
			{
				break;
			}
		}
		MemFree(readBuff);
		return currPos >= contLeng;
	}
	else
	{
		Text::CStringNN tranEnc;
		if (this->GetTransferEncoding().SetTo(tranEnc) && tranEnc.Equals(UTF8STRC("chunked")))
		{
			UInt8 *readBuff = MemAlloc(UInt8, buffSize);
			while ((readSize = this->Read(Data::ByteArray(readBuff, buffSize))) > 0)
			{
				outStm->Write(Data::ByteArrayR(readBuff, readSize));
				currPos += readSize;
				if (currPos > maxSize)
				{
					MemFree(readBuff);
					return false;
				}
			}
			MemFree(readBuff);
			return true;
		}
		return false;
	}
	return false;
}

Bool Net::HTTPClient::ReadAllContent(NN<Text::StringBuilderUTF8> sb, UOSInt buffSize, UInt64 maxSize)
{
	UInt64 contLeng = this->GetContentLength();
	UOSInt currPos = 0;
	UOSInt readSize;
	if (contLeng > 0 && contLeng <= maxSize)
	{
		UInt8 *readBuff = MemAlloc(UInt8, buffSize);
		while ((readSize = this->Read(Data::ByteArray(readBuff, buffSize))) > 0)
		{
			sb->AppendC(readBuff, readSize);
			currPos += readSize;
			if (currPos >= contLeng)
			{
				break;
			}
		}
		MemFree(readBuff);
		return currPos >= contLeng;
	}
	else
	{
		Text::CStringNN tranEnc;
		if (this->GetTransferEncoding().SetTo(tranEnc) && tranEnc.Equals(UTF8STRC("chunked")))
		{
			UInt8 *readBuff = MemAlloc(UInt8, buffSize);
			while ((readSize = this->Read(Data::ByteArray(readBuff, buffSize))) > 0)
			{
				sb->AppendC(readBuff, readSize);
				currPos += readSize;
				if (currPos > maxSize)
				{
					MemFree(readBuff);
					return false;
				}
			}
			MemFree(readBuff);
			return true;
		}
		return false;
	}
	return false;
}

NN<const Net::SocketUtil::AddressInfo> Net::HTTPClient::GetSvrAddr()
{
	return this->svrAddr;
}

void Net::HTTPClient::ParseDateStr(NN<Data::DateTime> dt, Text::CStringNN dateStr)
{
	UnsafeArray<UTF8Char> tmps;
	Text::PString ptrs[6];
	Text::PString ptrs2[3];
	Text::PString ptrs3[3];
	UTF8Char sbuff[64];
	UnsafeArray<UTF8Char> sptr;
	UOSInt i;
	UOSInt j;
	if ((i = dateStr.IndexOf(UTF8STRC(", "))) != INVALID_INDEX)
	{
		sptr = dateStr.Substring(i + 2).ConcatTo(sbuff);
		tmps = sbuff;
		if (Text::StrIndexOfChar(tmps, '-') == INVALID_INDEX)
		{
			i = Text::StrSplitP(ptrs, 6, {tmps, (UOSInt)(sptr - sbuff)}, ' ');
			if (i >= 4)
			{
				j = Text::StrSplitP(ptrs2, 3, ptrs[3], ':');
				if (j == 3)
				{
					dt->SetValue(Text::StrToInt32(ptrs[2].v), Data::DateTimeUtil::ParseMonthStr(ptrs[1].ToCString()), Text::StrToInt32(ptrs[0].v), Text::StrToInt32(ptrs2[0].v), Text::StrToInt32(ptrs2[1].v), Text::StrToInt32(ptrs2[2].v), 0);
				}
			}
		}
		else
		{
			i = Text::StrSplitP(ptrs, 6, {tmps, (UOSInt)(sptr - sbuff)}, ' ');
			if (i >= 2)
			{
				Text::StrSplitP(ptrs2, 3, ptrs[1], ':');
				Text::StrSplitP(ptrs3, 3, ptrs[0], '-');
				dt->SetValue((ptrs3[2].ToInt32() + ((dt->GetYear() / 100) * 100)), Data::DateTimeUtil::ParseMonthStr(ptrs3[1].ToCString()), Text::StrToInt32(ptrs3[0].v), Text::StrToInt32(ptrs2[0].v), Text::StrToInt32(ptrs2[1].v), Text::StrToInt32(ptrs2[2].v), 0);
			}
		}
	}
	else
	{
		sptr = dateStr.ConcatTo(sbuff);
		i = Text::StrSplitP(ptrs, 6, {sbuff, (UOSInt)(sptr - sbuff)}, ' ');
		if (i > 3)
		{
			j = Text::StrSplitP(ptrs2, 3, ptrs[i - 2], ':');
			if (j == 3)
			{
				dt->SetValue(ptrs[i - 1].ToInt32(), Data::DateTimeUtil::ParseMonthStr(ptrs[1].ToCString()), Text::StrToInt32(ptrs[i - 3].v), Text::StrToInt32(ptrs2[0].v), Text::StrToInt32(ptrs2[1].v), Text::StrToInt32(ptrs2[2].v), 0);
			}
		}
	}
}

Data::Timestamp Net::HTTPClient::ParseDateStr(Text::CStringNN dateStr)
{
	UnsafeArray<UTF8Char> tmps;
	Text::PString ptrs[6];
	Text::PString ptrs2[3];
	Text::PString ptrs3[3];
	UTF8Char sbuff[64];
	UnsafeArray<UTF8Char> sptr;
	UOSInt i;
	UOSInt j;
	if ((i = dateStr.IndexOf(UTF8STRC(", "))) != INVALID_INDEX)
	{
		sptr = dateStr.Substring(i + 2).ConcatTo(sbuff);
		tmps = sbuff;
		if (Text::StrIndexOfChar(tmps, '-') == INVALID_INDEX)
		{
			i = Text::StrSplitP(ptrs, 6, {tmps, (UOSInt)(sptr - sbuff)}, ' ');
			if (i >= 4)
			{
				j = Text::StrSplitP(ptrs2, 3, ptrs[3], ':');
				if (j == 3)
				{
					return Data::Timestamp::FromTimeValue(ptrs[2].ToInt32(), Data::DateTimeUtil::ParseMonthStr(ptrs[1].ToCString()), (UInt8)Text::StrToInt32(ptrs[0].v), (UInt8)Text::StrToInt32(ptrs2[0].v), (UInt8)Text::StrToInt32(ptrs2[1].v), (UInt8)Text::StrToInt32(ptrs2[2].v), 0, 0);
				}
			}
		}
		else
		{
			i = Text::StrSplitP(ptrs, 6, {tmps, (UOSInt)(sptr - sbuff)}, ' ');
			if (i >= 2)
			{
				if (Text::StrSplitP(ptrs2, 3, ptrs[1], ':') == 3 && Text::StrSplitP(ptrs3, 3, ptrs[0], '-') == 3)
				{
					return Data::Timestamp::FromTimeValue(ptrs3[2].ToInt32() + (Int32)((2000 / 100) * 100), Data::DateTimeUtil::ParseMonthStr(ptrs3[1].ToCString()), (UInt8)Text::StrToInt32(ptrs3[0].v), (UInt8)Text::StrToInt32(ptrs2[0].v), (UInt8)Text::StrToInt32(ptrs2[1].v), (UInt8)Text::StrToInt32(ptrs2[2].v), 0, 0);
				}
				else
				{
					return 0;
				}
			}
		}
	}
	else
	{
		sptr = dateStr.ConcatTo(sbuff);
		i = Text::StrSplitP(ptrs, 6, {sbuff, (UOSInt)(sptr - sbuff)}, ' ');
		if (i > 3)
		{
			j = Text::StrSplitP(ptrs2, 3, ptrs[i - 2], ':');
			if (j == 3)
			{
				return Data::Timestamp::FromTimeValue(ptrs[i - 1].ToInt32(), Data::DateTimeUtil::ParseMonthStr(ptrs[1].ToCString()), (UInt8)Text::StrToInt32(ptrs[i - 3].v), (UInt8)Text::StrToInt32(ptrs2[0].v), (UInt8)Text::StrToInt32(ptrs2[1].v), (UInt8)Text::StrToInt32(ptrs2[2].v), 0, 0);
			}
		}
	}
	return 0;
}

NN<Net::HTTPClient> Net::HTTPClient::CreateClient(NN<Net::TCPClientFactory> clif, Optional<Net::SSLEngine> ssl, Text::CString userAgent, Bool kaConn, Bool isSecure)
{
	NN<Net::HTTPClient> cli;
	if (isSecure && ssl.IsNull())
	{
		NEW_CLASSNN(cli, Net::HTTPOSClient(clif, userAgent, kaConn));
	}
	else
	{
		NEW_CLASSNN(cli, Net::HTTPMyClient(clif, ssl, userAgent, kaConn));
	}
	return cli;
}

NN<Net::HTTPClient> Net::HTTPClient::CreateConnect(NN<Net::TCPClientFactory> clif, Optional<Net::SSLEngine> ssl, Text::CStringNN url, Net::WebUtil::RequestMethod method, Bool kaConn)
{
	NN<Net::HTTPClient> cli = Net::HTTPClient::CreateClient(clif, ssl, CSTR_NULL, kaConn, url.StartsWithICase(UTF8STRC("HTTPS://")));
	cli->Connect(url, method, 0, 0, true);
	return cli;
}

NN<Net::HTTPClient> Net::HTTPClient::CreateGet(NN<Net::TCPClientFactory> clif, Optional<Net::SSLEngine> ssl, Text::CStringNN url, Bool kaConn)
{
	Data::ArrayListStringNN urlList;
	while (true)
	{
		NN<Net::HTTPClient> cli = Net::HTTPClient::CreateClient(clif, ssl, CSTR_NULL, kaConn, url.StartsWithICase(UTF8STRC("HTTPS://")));
		cli->Connect(url, Net::WebUtil::RequestMethod::HTTP_GET, 0, 0, true);
		Net::WebStatus::StatusCode status = cli->GetRespStatus();
		if (status != Net::WebStatus::SC_MOVED_TEMPORARILY && status != Net::WebStatus::SC_MOVED_PERMANENTLY)
		{
			urlList.FreeAll();
			return cli;
		}
		Text::CStringNN newUrl = cli->GetRespHeader(CSTR("Location")).OrEmpty();
		if (newUrl.leng == 0 || urlList.IndexOfC(newUrl) >= 0 || urlList.GetCount() >= 20)
		{
			urlList.FreeAll();
			return cli;
		}
		urlList.Add(Text::String::New(url));
		cli.Delete();
	}
}

Bool Net::HTTPClient::IsHTTPURL(Text::CStringNN url)
{
	return url.StartsWith(UTF8STRC("http://")) || url.StartsWith(UTF8STRC("https://"));
}

void Net::HTTPClient::PrepareSSL(Optional<Net::SSLEngine> ssl)
{
}

Bool Net::HTTPClient::LoadContent(NN<Net::TCPClientFactory> clif, Optional<Net::SSLEngine> ssl, Text::CStringNN url, NN<IO::Stream> stm, UInt64 maxSize)
{
	NN<Net::HTTPClient> cli = Net::HTTPClient::CreateConnect(clif, ssl, url, Net::WebUtil::RequestMethod::HTTP_GET, true);
	if (cli->GetRespStatus() != Net::WebStatus::SC_OK)
	{
		cli.Delete();
		return false;
	}
	UInt8 buff[2048];
	UOSInt readSize;
	while ((readSize = cli->Read(Data::ByteArray(buff, sizeof(buff)))) > 0)
	{
		if (readSize > maxSize)
		{
			cli.Delete();
			return false;
		}
		stm->Write(Data::ByteArray(buff, readSize));
		maxSize -= readSize;
	}
	cli.Delete();
	return true;
}

Bool Net::HTTPClient::LoadContent(NN<Net::TCPClientFactory> clif, Optional<Net::SSLEngine> ssl, Text::CStringNN url, NN<Text::StringBuilderUTF8> sb, UInt64 maxSize)
{
	NN<Net::HTTPClient> cli = Net::HTTPClient::CreateConnect(clif, ssl, url, Net::WebUtil::RequestMethod::HTTP_GET, true);
	if (cli->GetRespStatus() != Net::WebStatus::SC_OK)
	{
		cli.Delete();
		return false;
	}
	UInt8 buff[2048];
	UOSInt readSize;
	while ((readSize = cli->Read(Data::ByteArray(buff, sizeof(buff)))) > 0)
	{
		if (readSize > maxSize)
		{
			cli.Delete();
			return false;
		}
		sb->AppendC(buff, readSize);
		maxSize -= readSize;
	}
	cli.Delete();
	return true;
}
