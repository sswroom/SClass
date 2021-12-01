#include "Stdafx.h"
#include "Net/HTTPClient.h"
#include "Net/WebServer/HTTPForwardHandler.h"
#include "Sync/MutexUsage.h"
#include "Text/StringBuilderUTF8.h"

const UTF8Char *Net::WebServer::HTTPForwardHandler::GetNextURL(Net::WebServer::IWebRequest *req)
{
	Sync::MutexUsage mutUsage(this->mut);
	UOSInt i = this->nextURL;
	this->nextURL = (i + 1) % this->forwardAddrs->GetCount();
	return this->forwardAddrs->GetItem(i);
}

Net::WebServer::HTTPForwardHandler::HTTPForwardHandler(Net::SocketFactory *sockf, Net::SSLEngine *ssl, const UTF8Char *forwardURL)
{
	this->sockf = sockf;
	this->ssl = ssl;
	this->xForwardHeaders = true;
	this->locationRemap = true;
	NEW_CLASS(this->forwardAddrs, Data::ArrayList<const UTF8Char*>());
	NEW_CLASS(this->injHeaders, Data::ArrayList<const UTF8Char*>());
	this->forwardAddrs->Add(Text::StrCopyNew(forwardURL));
	this->nextURL = 0;
	NEW_CLASS(this->mut, Sync::Mutex());
}

Net::WebServer::HTTPForwardHandler::~HTTPForwardHandler()
{
	LIST_FREE_FUNC(this->forwardAddrs, Text::StrDelNew);
	DEL_CLASS(this->forwardAddrs);
	LIST_FREE_FUNC(this->injHeaders, Text::StrDelNew);
	DEL_CLASS(this->injHeaders);
	DEL_CLASS(this->mut);
}

Bool Net::WebServer::HTTPForwardHandler::ProcessRequest(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, const UTF8Char *subReq)
{
	UInt8 buff[2048];
	if (subReq[0] == 0)
	{
		subReq = (const UTF8Char*)"/";
	}
	Text::StringBuilderUTF8 sb;
	const UTF8Char *fwdBaseUrl = this->GetNextURL(req);
	sb.Append(fwdBaseUrl);
	if (sb.EndsWith('/'))
	{
		sb.RemoveChars(1);
	}
	sb.Append(subReq);
	const UTF8Char *uri = req->GetRequestURI();
	UOSInt i = Text::StrIndexOf(uri, '?');
	UOSInt j;
	if (i >= 0)
	{
		sb.Append(&uri[i]);
	}
	Bool kaConn = true;
	Text::StringBuilderUTF8 sbHeader;
	if (req->GetHeader(&sbHeader, (const UTF8Char*)"Connection"))
	{
		if (sbHeader.Equals((const UTF8Char*)"close"))
		{
			kaConn = false;
		}
	}
	Net::HTTPClient *cli = Net::HTTPClient::CreateClient(this->sockf, this->ssl, (const UTF8Char*)"sswr/1.0", kaConn, sb.StartsWith((const UTF8Char*)"https://"));
	if (cli == 0)
	{
		resp->ResponseError(req, Net::WebStatus::SC_NOT_FOUND);
		return true;
	}
	if (!cli->Connect(sb.ToString(), req->GetReqMethodStr(), 0, 0, false))
	{
		DEL_CLASS(cli);
		resp->ResponseError(req, Net::WebStatus::SC_NOT_FOUND);
		return true;
	}
	const UTF8Char *hdr;
	Data::ArrayList<const UTF8Char*> hdrNames;
	req->GetHeaderNames(&hdrNames);

	const UTF8Char *svrHost = 0;
	UInt16 svrPort = 0;
	const UTF8Char *fwdFor = 0;
	const UTF8Char *fwdPrefix = 0;
	const UTF8Char *fwdHost = 0;
	const UTF8Char *fwdProto = 0;
	const UTF8Char *fwdPort = 0;
	const UTF8Char *fwdSsl = 0;
	i = 0;
	j = hdrNames.GetCount();
	while (i < j)
	{
		hdr = hdrNames.GetItem(i);
		if (Text::StrEqualsICase(hdr, (const UTF8Char*)"Host"))
		{
			sbHeader.ClearStr();
			if (req->GetHeader(&sbHeader, hdr))
			{
				UOSInt k = sbHeader.IndexOf(':');
				svrHost = Text::StrCopyNew(sbHeader.ToString());
				if (k >= 0)
				{
					Text::StrToUInt16(sbHeader.ToString() + k + 1, &svrPort);
				}
			}
		}
		else if (Text::StrEqualsICase(hdr, (const UTF8Char*)"X-Forwarded-For"))
		{
			sbHeader.ClearStr();
			if (req->GetHeader(&sbHeader, hdr))
			{
				fwdFor = Text::StrCopyNew(sbHeader.ToString());
			}
		}
		else
		{
			sbHeader.ClearStr();
			if (req->GetHeader(&sbHeader, hdr))
			{
				cli->AddHeader(hdr, sbHeader.ToString());
			}
		}
		i++;
	}

	if (this->xForwardHeaders)
	{
		if (fwdProto == 0)
		{
			if (req->IsSecure())
			{
				cli->AddHeader((const UTF8Char *)"X-Forwarded-Proto", (const UTF8Char*)"https");
			}
			else
			{
				cli->AddHeader((const UTF8Char *)"X-Fowrarded-Proto", (const UTF8Char*)"http");
			}
		}
		if (fwdSsl == 0)
		{
			if (req->IsSecure())
			{
				cli->AddHeader((const UTF8Char*)"X-Forwarded-Ssl", (const UTF8Char*)"on");
			}
		}
		if (fwdHost == 0)
		{
			if (svrHost)
			{
				cli->AddHeader((const UTF8Char*)"X-Forwarded-Host", svrHost);
			}
		}
		sbHeader.ClearStr();
		if (fwdFor)
		{
			sbHeader.Append(fwdFor);
			sbHeader.AppendChar(',', 1);
		}
		Net::SocketUtil::GetAddrName(buff, req->GetClientAddr());
		sbHeader.Append(buff);
		cli->AddHeader((const UTF8Char*)"X-Forwarded-For", sbHeader.ToString());

		if (fwdPort == 0 && svrPort != 0)
		{
			Text::StrUInt16(buff, svrPort);
			cli->AddHeader((const UTF8Char*)"X-Forwarded-Port", buff);
		}
	}
	else
	{
		if (fwdFor)
		{
			cli->AddHeader((const UTF8Char*)"X-Forwarded-For", fwdFor);
		}
	}
	SDEL_TEXT(fwdFor);
	SDEL_TEXT(fwdSsl);
	SDEL_TEXT(fwdHost);
	SDEL_TEXT(fwdPort);
	SDEL_TEXT(fwdProto);
	SDEL_TEXT(fwdPrefix);
	SDEL_TEXT(svrHost);

	const UInt8 *reqData = req->GetReqData(&i);
	UTF8Char *sarr[2];
	if (reqData)
	{
		cli->Write(reqData, i);
	}
	Net::WebStatus::StatusCode scode = cli->GetRespStatus();
	if (scode == Net::WebStatus::SC_UNKNOWN)
	{
		DEL_CLASS(cli);
		resp->ResponseError(req, Net::WebStatus::SC_NOT_FOUND);
		return true;
	}
	UInt32 tranEnc = 0;
	resp->SetStatusCode(scode);
	i = 0;
	j = cli->GetRespHeaderCnt();
	while (i < j)
	{
		hdr = cli->GetRespHeader(i);
		sbHeader.ClearStr();
		sbHeader.Append(hdr);
		if (Text::StrSplit(sarr, 2, sbHeader.ToString(), ':') == 2)
		{
			Text::StrTrim(sarr[1]);
			if (this->locationRemap && Text::StrEqualsICase(sarr[0], (const UTF8Char*)"LOCATION"))
			{
				if (Text::StrStartsWith(sarr[1], fwdBaseUrl))
				{
					sb.ClearStr();
					if (req->IsSecure())
					{
						sb.Append((const UTF8Char*)"https://");
					}
					else
					{
						sb.Append((const UTF8Char*)"http://");
					}
					req->GetHeader(&sb, (const UTF8Char*)"Host");
					UOSInt urlLen = Text::StrCharCnt(fwdBaseUrl);
					if (fwdBaseUrl[urlLen - 1] == '/')
					{
						sb.Append(&sarr[1][urlLen - 1]);
					}
					else
					{
						sb.Append(&sarr[1][urlLen]);
					}
					resp->AddHeader(sarr[0], sb.ToString());
				}
				else
				{
					resp->AddHeader(sarr[0], sarr[1]);
				}
			}
			else
			{
				resp->AddHeader(sarr[0], sarr[1]);
			}
			if (Text::StrEqualsICase(sarr[0], (const UTF8Char*)"Transfer-Encoding"))
			{
				if (Text::StrEquals(sarr[1], (const UTF8Char*)"chunked"))
				{
					tranEnc = 1;
				}
			}
		}
		i++;
	}
	i = 0;
	j = this->injHeaders->GetCount();
	while (i < j)
	{
		hdr = this->injHeaders->GetItem(i);
		sbHeader.ClearStr();
		sbHeader.Append(hdr);
		if (Text::StrSplit(sarr, 2, sbHeader.ToString(), ':') == 2)
		{
			Text::StrTrim(sarr[1]);
			resp->AddHeader(sarr[0], sarr[1]);
		}
		i++;
	}

	if (tranEnc == 1)
	{
		UTF8Char sbuff[16];
		UTF8Char *sptr;
		while (true)
		{
			i = cli->Read(buff, 2046);
			if (i > 0)
			{
				sptr = Text::StrConcat(Text::StrHexVal32V(sbuff, (UInt32)i), (const UTF8Char*)"\r\n");
				j = resp->Write(sbuff, (UOSInt)(sptr - sbuff));
				buff[i] = 13;
				buff[i + 1] = 10;
				j = resp->Write(buff, i + 2);
				if (j != i + 2)
				{
					break;
				}
			}
			else
			{
				resp->Write((const UInt8*)"0\r\n\r\n", 5);
				break;
			}
		}
	}
	else
	{
		while (true)
		{
			i = cli->Read(buff, 2048);
			if (i > 0)
			{
				j = resp->Write(buff, i);
				if (j != i)
				{
					break;
				}
			}
			else
			{
				break;
			}
		}
	}
	DEL_CLASS(cli);
	return true;
}

void Net::WebServer::HTTPForwardHandler::SetXForwardHeaders(Bool xForwardHeaders)
{
	this->xForwardHeaders = xForwardHeaders;
}

void Net::WebServer::HTTPForwardHandler::AddForwardURL(const UTF8Char *url)
{
	Sync::MutexUsage mutUsage(this->mut);
	this->forwardAddrs->Add(Text::StrCopyNew(url));
}

void Net::WebServer::HTTPForwardHandler::AddInjectHeader(const UTF8Char *header)
{
	this->injHeaders->Add(Text::StrCopyNew(header));
}
