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

Net::HTTPClient::HTTPClient(Net::SocketFactory *sockf, Bool kaConn) : IO::Stream(CSTR("HTTPClient"))
{
	this->sockf = sockf;
	this->canWrite = false;
	this->contLeng = 0;
	this->respStatus = Net::WebStatus::SC_UNKNOWN;
	this->url = 0;
	this->hasForm = false;
	this->formSb = 0;
	this->hdrLen = 0;
	this->totalUpload = 0;
	this->totalDownload = 0;
	this->kaConn = kaConn;
	this->svrAddr.addrType = Net::AddrType::Unknown;
	NEW_CLASS(this->headers, Data::ArrayList<Text::String*>());
	NEW_CLASS(this->clk, Manage::HiResClock());
}

Net::HTTPClient::~HTTPClient()
{
	if (this->headers)
	{
		LIST_FREE_STRING(this->headers);
		DEL_CLASS(this->headers);
	}
	SDEL_CLASS(this->formSb);
	SDEL_STRING(this->url);
	DEL_CLASS(this->clk);
}

Bool Net::HTTPClient::IsDown()
{
	return this->IsError();
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
	sptr = Date2Str(sbuff, dt);
	this->AddHeaderC(name, CSTRP(sbuff, sptr));
}

void Net::HTTPClient::AddContentType(Text::CString contType)
{
	this->AddHeaderC(CSTR("Content-Type"), contType);
}

void Net::HTTPClient::AddContentLength(UOSInt leng)
{
	UTF8Char sbuff[32];
	UTF8Char *sptr;
	sptr = Text::StrUOSInt(sbuff, leng);
	this->AddHeaderC(CSTR("Content-Length"), CSTRP(sbuff, sptr));
}

UOSInt Net::HTTPClient::GetRespHeaderCnt()
{
	return this->headers->GetCount();
}

UTF8Char *Net::HTTPClient::GetRespHeader(UOSInt index, UTF8Char *buff)
{
	return this->headers->GetItem(index)->ConcatTo(buff);
}

UTF8Char *Net::HTTPClient::GetRespHeader(Text::CString name, UTF8Char *valueBuff)
{
	UTF8Char buff[256];
	UTF8Char *s2;
	Text::String *s;
	UOSInt i;
	s2 = Text::StrConcatC(name.ConcatTo(buff), UTF8STRC(": "));
	i = this->headers->GetCount();
	while (i-- > 0)
	{
		s = this->headers->GetItem(i);
		if (s->StartsWith(buff, (UOSInt)(s2 - buff)))
		{
			return Text::StrConcatC(valueBuff, &s->v[s2 - buff], s->leng - (UOSInt)(s2 - buff));
		}
	}
	return 0;
}

Bool Net::HTTPClient::GetRespHeader(Text::CString name, Text::StringBuilderUTF8 *sb)
{
	UTF8Char buff[256];
	UTF8Char *s2;
	Text::String *s;
	UOSInt i;
	s2 = Text::StrConcatC(name.ConcatTo(buff), UTF8STRC(": "));
	i = this->headers->GetCount();
	while (i-- > 0)
	{
		s = this->headers->GetItem(i);
		if (s->StartsWithICase(buff, (UOSInt)(s2 - buff)))
		{
			sb->AppendC(&s->v[s2-buff], s->leng - (UOSInt)(s2 - buff));
			return true;
		}
	}
	return false;
}

Text::String *Net::HTTPClient::GetRespHeader(UOSInt index)
{
	return this->headers->GetItem(index);
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

Bool Net::HTTPClient::GetLastModified(Data::DateTime *dt)
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

Bool Net::HTTPClient::GetServerDate(Data::DateTime *dt)
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

Text::String *Net::HTTPClient::GetURL()
{
	return this->url;
}

Net::WebStatus::StatusCode Net::HTTPClient::GetRespStatus()
{
	this->EndRequest(0, 0);
	return this->respStatus;
}

Double Net::HTTPClient::GetTotalTime()
{
	return this->clk->GetTimeDiff();
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

const Net::SocketUtil::AddressInfo *Net::HTTPClient::GetSvrAddr()
{
	return &this->svrAddr;
}

void Net::HTTPClient::ParseDateStr(Data::DateTime *dt, Text::CString dateStr)
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
					dt->SetValue((UInt16)Text::StrToUInt32(ptrs[2].v), Data::DateTime::ParseMonthStr(ptrs[1].v, ptrs[1].leng), Text::StrToInt32(ptrs[0].v), Text::StrToInt32(ptrs2[0].v), Text::StrToInt32(ptrs2[1].v), Text::StrToInt32(ptrs2[2].v), 0);
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
				dt->SetValue((UInt16)(Text::StrToUInt32(ptrs3[2].v) + (UInt32)((dt->GetYear() / 100) * 100)), Data::DateTime::ParseMonthStr(ptrs3[1].v, ptrs3[1].leng), Text::StrToInt32(ptrs3[0].v), Text::StrToInt32(ptrs2[0].v), Text::StrToInt32(ptrs2[1].v), Text::StrToInt32(ptrs2[2].v), 0);
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
				dt->SetValue((UInt16)Text::StrToUInt32(ptrs[i - 1].v), Data::DateTime::ParseMonthStr(ptrs[1].v, ptrs[1].leng), Text::StrToInt32(ptrs[i - 3].v), Text::StrToInt32(ptrs2[0].v), Text::StrToInt32(ptrs2[1].v), Text::StrToInt32(ptrs2[2].v), 0);
			}
		}
	}
}

UTF8Char *Net::HTTPClient::Date2Str(UTF8Char *sbuff, Data::DateTime *dt)
{
	const Char *wds[] = {"Sun, ", "Mon, ", "Tue, ", "Wed, ", "Thu, ", "Fri, ", "Sat, "};
	Data::DateTime t;
	t.SetValue(dt);
	t.ToUTCTime();
	Int32 wd = (Int32)t.GetWeekday();
	return Text::StrConcatC(t.ToString(Text::StrConcat(sbuff, (const UTF8Char*)wds[wd]), "dd MMM yyyy HH:mm:ss"), UTF8STRC(" GMT"));
}

Net::HTTPClient *Net::HTTPClient::CreateClient(Net::SocketFactory *sockf, Net::SSLEngine *ssl, Text::CString userAgent, Bool kaConn, Bool isSecure)
{
	Net::HTTPClient *cli;
	if (isSecure && ssl == 0)
	{
		NEW_CLASS(cli, Net::HTTPOSClient(sockf, userAgent, kaConn));
	}
	else
	{
		NEW_CLASS(cli, Net::HTTPMyClient(sockf, ssl, userAgent, kaConn));
	}
	return cli;
}

Net::HTTPClient *Net::HTTPClient::CreateConnect(Net::SocketFactory *sockf, Net::SSLEngine *ssl, Text::CString url, Net::WebUtil::RequestMethod method, Bool kaConn)
{
	Net::HTTPClient *cli = Net::HTTPClient::CreateClient(sockf, ssl, CSTR_NULL, kaConn, url.StartsWithICase(UTF8STRC("HTTPS://")));
	cli->Connect(url, method, 0, 0, true);
	return cli;
}

Bool Net::HTTPClient::IsHTTPURL(Text::CString url)
{
	if (url.v == 0)
	{
		return false;
	}
	return url.StartsWith(UTF8STRC("http://")) || url.StartsWith(UTF8STRC("https://"));
}

void Net::HTTPClient::PrepareSSL(Net::SSLEngine *ssl)
{
}
