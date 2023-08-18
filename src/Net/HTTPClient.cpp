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
#include "Text/TextBinEnc/URIEncoding.h"

Net::HTTPClient::HTTPClient(NotNullPtr<Net::SocketFactory> sockf, Bool kaConn) : IO::Stream(CSTR("HTTPClient"))
{
	this->sockf = sockf;
	this->canWrite = false;
	this->contLeng = 0;
	this->respStatus = Net::WebStatus::SC_UNKNOWN;
	this->url = Text::String::NewEmpty();
	this->hasForm = false;
	this->formSb = 0;
	this->hdrLen = 0;
	this->totalUpload = 0;
	this->totalDownload = 0;
	this->kaConn = kaConn;
	this->svrAddr.addrType = Net::AddrType::Unknown;
}

Net::HTTPClient::~HTTPClient()
{
	LIST_FREE_STRING(&this->headers);
	SDEL_CLASS(this->formSb);
	this->url->Release();
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
	if (this->canWrite && !this->hasForm)
	{
		this->hasForm = true;
		this->AddContentType(CSTR("application/x-www-form-urlencoded"));
		NEW_CLASS(this->formSb, Text::StringBuilderUTF8());
		return true;
	}
	return false;
}

Bool Net::HTTPClient::FormAdd(Text::CString name, Text::CString value)
{
	if (!this->hasForm)
	{
		return false;
	}
	UTF8Char sbuff[256];
	UTF8Char *sptr;
	sptr = Text::TextBinEnc::URIEncoding::URIEncode(sbuff, name.v);
	if (this->formSb->GetLength() > 0)
	{
		this->formSb->AppendUTF8Char('&');
	}
	this->formSb->AppendC(sbuff, (UOSInt)(sptr - sbuff));
	sptr = Text::TextBinEnc::URIEncoding::URIEncode(sbuff, value.v);
	this->formSb->AppendUTF8Char('=');
	this->formSb->AppendC(sbuff, (UOSInt)(sptr - sbuff));
	return true;
}

void Net::HTTPClient::AddTimeHeader(Text::CString name, Data::DateTime *dt)
{
	UTF8Char sbuff[64];
	UTF8Char *sptr;
	sptr = Net::WebUtil::Date2Str(sbuff, dt);
	this->AddHeaderC(name, CSTRP(sbuff, sptr));
}

void Net::HTTPClient::AddContentType(Text::CString contType)
{
	this->AddHeaderC(CSTR("Content-Type"), contType);
}

void Net::HTTPClient::AddContentLength(UInt64 leng)
{
	UTF8Char sbuff[32];
	UTF8Char *sptr;
	sptr = Text::StrUInt64(sbuff, leng);
	this->AddHeaderC(CSTR("Content-Length"), CSTRP(sbuff, sptr));
}

UOSInt Net::HTTPClient::GetRespHeaderCnt()
{
	return this->headers.GetCount();
}

UTF8Char *Net::HTTPClient::GetRespHeader(UOSInt index, UTF8Char *buff)
{
	return this->headers.GetItem(index)->ConcatTo(buff);
}

UTF8Char *Net::HTTPClient::GetRespHeader(Text::CString name, UTF8Char *valueBuff)
{
	Text::CString v = GetRespHeader(name);
	if (v.v)
	{
		return v.ConcatTo(valueBuff);
	}
	return 0;
}

Bool Net::HTTPClient::GetRespHeader(Text::CString name, NotNullPtr<Text::StringBuilderUTF8> sb)
{
	Text::CString v = GetRespHeader(name);
	if (v.v)
	{
		sb->Append(v);
		return true;
	}
	return false;
}

Text::CString Net::HTTPClient::GetRespHeader(Text::CString name)
{
	UTF8Char buff[256];
	UTF8Char *s2;
	Text::String *s;
	UOSInt i;
	s2 = Text::StrConcatC(name.ConcatTo(buff), UTF8STRC(": "));
	i = this->headers.GetCount();
	while (i-- > 0)
	{
		s = this->headers.GetItem(i);
		if (s->StartsWithICase(buff, (UOSInt)(s2 - buff)))
		{
			return Text::CString(&s->v[s2-buff], s->leng - (UOSInt)(s2 - buff));
		}
	}
	return CSTR_NULL;
}

Text::String *Net::HTTPClient::GetRespHeader(UOSInt index)
{
	return this->headers.GetItem(index);
}

UInt64 Net::HTTPClient::GetContentLength()
{
	this->EndRequest(0, 0);
	return this->contLeng;
}

UInt32 Net::HTTPClient::GetContentCodePage()
{
	UTF8Char sbuff[256];
	UTF8Char *sptr;
	Text::PString sarr[2];
	UOSInt arrCnt;
	this->EndRequest(0, 0);
	if ((sptr = this->GetRespHeader(CSTR("Content-Type"), sbuff)) != 0)
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

Bool Net::HTTPClient::GetLastModified(NotNullPtr<Data::DateTime> dt)
{
	UTF8Char sbuff[64];
	UTF8Char *sptr;
	this->EndRequest(0, 0);
	if ((sptr = this->GetRespHeader(CSTR("Last-Modified"), sbuff)) != 0)
	{
		ParseDateStr(dt, CSTRP(sbuff, sptr));
		return true;
	}
	return false;
}

Bool Net::HTTPClient::GetServerDate(NotNullPtr<Data::DateTime> dt)
{
	UTF8Char sbuff[64];
	UTF8Char *sptr;
	this->EndRequest(0, 0);
	if ((sptr = this->GetRespHeader(CSTR("Date"), sbuff)) != 0)
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

Text::String *Net::HTTPClient::GetURL()
{
	return (this->url->leng > 0)?this->url.Ptr():0;
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

Bool Net::HTTPClient::ReadAllContent(NotNullPtr<IO::Stream> outStm, UOSInt buffSize, UInt64 maxSize)
{
	UInt64 contLeng = this->GetContentLength();
	UOSInt currPos = 0;
	UOSInt readSize;
	if (contLeng > 0 && contLeng <= maxSize)
	{
		UInt8 *readBuff = MemAlloc(UInt8, buffSize);
		while ((readSize = this->Read(Data::ByteArray(readBuff, buffSize))) > 0)
		{
			outStm->Write(readBuff, readSize);
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
		Text::CString tranEnc = this->GetTransferEncoding();
		if (tranEnc.Equals(UTF8STRC("chunked")))
		{
			UInt8 *readBuff = MemAlloc(UInt8, buffSize);
			while ((readSize = this->Read(Data::ByteArray(readBuff, buffSize))) > 0)
			{
				outStm->Write(readBuff, readSize);
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

Bool Net::HTTPClient::ReadAllContent(NotNullPtr<Text::StringBuilderUTF8> sb, UOSInt buffSize, UInt64 maxSize)
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
		Text::CString tranEnc = this->GetTransferEncoding();
		if (tranEnc.Equals(UTF8STRC("chunked")))
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

const Net::SocketUtil::AddressInfo *Net::HTTPClient::GetSvrAddr()
{
	return &this->svrAddr;
}

void Net::HTTPClient::ParseDateStr(NotNullPtr<Data::DateTime> dt, Text::CString dateStr)
{
	UTF8Char *tmps;
	Text::PString ptrs[6];
	Text::PString ptrs2[3];
	Text::PString ptrs3[3];
	UTF8Char sbuff[64];
	UTF8Char *sptr;
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
					dt->SetValue((UInt16)Text::StrToUInt32(ptrs[2].v), Data::DateTimeUtil::ParseMonthStr(ptrs[1].v, ptrs[1].leng), Text::StrToInt32(ptrs[0].v), Text::StrToInt32(ptrs2[0].v), Text::StrToInt32(ptrs2[1].v), Text::StrToInt32(ptrs2[2].v), 0);
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
				dt->SetValue((UInt16)(Text::StrToUInt32(ptrs3[2].v) + (UInt32)((dt->GetYear() / 100) * 100)), Data::DateTimeUtil::ParseMonthStr(ptrs3[1].v, ptrs3[1].leng), Text::StrToInt32(ptrs3[0].v), Text::StrToInt32(ptrs2[0].v), Text::StrToInt32(ptrs2[1].v), Text::StrToInt32(ptrs2[2].v), 0);
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
				dt->SetValue((UInt16)Text::StrToUInt32(ptrs[i - 1].v), Data::DateTimeUtil::ParseMonthStr(ptrs[1].v, ptrs[1].leng), Text::StrToInt32(ptrs[i - 3].v), Text::StrToInt32(ptrs2[0].v), Text::StrToInt32(ptrs2[1].v), Text::StrToInt32(ptrs2[2].v), 0);
			}
		}
	}
}

NotNullPtr<Net::HTTPClient> Net::HTTPClient::CreateClient(NotNullPtr<Net::SocketFactory> sockf, Net::SSLEngine *ssl, Text::CString userAgent, Bool kaConn, Bool isSecure)
{
	NotNullPtr<Net::HTTPClient> cli;
	if (isSecure && ssl == 0)
	{
		NEW_CLASSNN(cli, Net::HTTPOSClient(sockf, userAgent, kaConn));
	}
	else
	{
		NEW_CLASSNN(cli, Net::HTTPMyClient(sockf, ssl, userAgent, kaConn));
	}
	return cli;
}

NotNullPtr<Net::HTTPClient> Net::HTTPClient::CreateConnect(NotNullPtr<Net::SocketFactory> sockf, Net::SSLEngine *ssl, Text::CStringNN url, Net::WebUtil::RequestMethod method, Bool kaConn)
{
	NotNullPtr<Net::HTTPClient> cli = Net::HTTPClient::CreateClient(sockf, ssl, CSTR_NULL, kaConn, url.StartsWithICase(UTF8STRC("HTTPS://")));
	cli->Connect(url, method, 0, 0, true);
	return cli;
}

Bool Net::HTTPClient::IsHTTPURL(Text::CStringNN url)
{
	return url.StartsWith(UTF8STRC("http://")) || url.StartsWith(UTF8STRC("https://"));
}

void Net::HTTPClient::PrepareSSL(Net::SSLEngine *ssl)
{
}

Bool Net::HTTPClient::LoadContent(NotNullPtr<Net::SocketFactory> sockf, Net::SSLEngine *ssl, Text::CStringNN url, IO::Stream *stm, UInt64 maxSize)
{
	NotNullPtr<Net::HTTPClient> cli = Net::HTTPClient::CreateConnect(sockf, ssl, url, Net::WebUtil::RequestMethod::HTTP_GET, true);
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
		stm->Write(buff, readSize);
		maxSize -= readSize;
	}
	cli.Delete();
	return true;
}

Bool Net::HTTPClient::LoadContent(NotNullPtr<Net::SocketFactory> sockf, Net::SSLEngine *ssl, Text::CStringNN url, NotNullPtr<Text::StringBuilderUTF8> sb, UInt64 maxSize)
{
	NotNullPtr<Net::HTTPClient> cli = Net::HTTPClient::CreateConnect(sockf, ssl, url, Net::WebUtil::RequestMethod::HTTP_GET, true);
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
