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

Net::WebServer::HTTPForwardHandler::HTTPForwardHandler(Net::SocketFactory *sockf, Net::SSLEngine *ssl, const UTF8Char *forwardURL, ForwardType fwdType)
{
	this->sockf = sockf;
	this->ssl = ssl;
	this->fwdType = fwdType;
	this->reqHdlr = 0;
	this->reqHdlrObj = 0;
	NEW_CLASS(this->forwardAddrs, Data::ArrayList<const UTF8Char*>());
	NEW_CLASS(this->injHeaders, Data::ArrayList<Text::String*>());
	this->forwardAddrs->Add(Text::StrCopyNew(forwardURL));
	this->nextURL = 0;
	NEW_CLASS(this->mut, Sync::Mutex());
}

Net::WebServer::HTTPForwardHandler::~HTTPForwardHandler()
{
	LIST_FREE_FUNC(this->forwardAddrs, Text::StrDelNew);
	DEL_CLASS(this->forwardAddrs);
	LIST_FREE_FUNC(this->injHeaders, STR_REL);
	DEL_CLASS(this->injHeaders);
	DEL_CLASS(this->mut);
}

Bool Net::WebServer::HTTPForwardHandler::ProcessRequest(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, const UTF8Char *subReq)
{
	UInt8 buff[2048];
	UTF8Char *sptr;
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
	Text::String *uri = req->GetRequestURI();
	UOSInt i = uri->IndexOf('?');
	UOSInt j;
	if (i >= 0)
	{
		sb.Append(&uri->v[i]);
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
		if (this->reqHdlr) this->reqHdlr(this->reqHdlrObj, req, resp);
		return true;
	}
	if (!cli->Connect(sb.ToString(), req->GetReqMethodStr(), 0, 0, false))
	{
		DEL_CLASS(cli);
		resp->ResponseError(req, Net::WebStatus::SC_NOT_FOUND);
		if (this->reqHdlr) this->reqHdlr(this->reqHdlrObj, req, resp);
		return true;
	}
	Text::String *hdr;
	Data::ArrayList<Text::String*> hdrNames;
	req->GetHeaderNames(&hdrNames);

	Text::String *svrHost = 0;
	UInt16 svrPort = 0;
	Text::String *fwdFor = 0;
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
		if (hdr->EqualsICase((const UTF8Char*)"Host"))
		{
			sbHeader.ClearStr();
			if (req->GetHeader(&sbHeader, hdr->v))
			{
				UOSInt k = sbHeader.IndexOf(':');
				svrHost = Text::String::New(sbHeader.ToString(), sbHeader.GetLength());
				if (k >= 0)
				{
					Text::StrToUInt16(sbHeader.ToString() + k + 1, &svrPort);
				}
			}
		}
		else if (hdr->EqualsICase((const UTF8Char*)"X-Forwarded-For"))
		{
			sbHeader.ClearStr();
			if (req->GetHeader(&sbHeader, hdr->v))
			{
				fwdFor = Text::String::New(sbHeader.ToString(), sbHeader.GetLength());
			}
		}
		else
		{
			sbHeader.ClearStr();
			if (req->GetHeader(&sbHeader, hdr->v))
			{
				cli->AddHeaderC(hdr->v, hdr->leng, sbHeader.ToString(), sbHeader.GetLength());
			}
		}
		i++;
	}

	if (this->fwdType == ForwardType::Normal)
	{
		if (fwdProto == 0)
		{
			if (req->IsSecure())
			{
				cli->AddHeaderC(UTF8STRC("X-Forwarded-Proto"), UTF8STRC("https"));
			}
			else
			{
				cli->AddHeaderC(UTF8STRC("X-Forwarded-Proto"), UTF8STRC("http"));
			}
		}
		if (fwdSsl == 0)
		{
			if (req->IsSecure())
			{
				cli->AddHeaderC(UTF8STRC("X-Forwarded-Ssl"), UTF8STRC("on"));
			}
		}
		if (fwdHost == 0)
		{
			if (svrHost)
			{
				cli->AddHeaderC(UTF8STRC("X-Forwarded-Host"), svrHost->v, svrHost->leng);
			}
		}
		sbHeader.ClearStr();
		if (fwdFor)
		{
			sbHeader.Append(fwdFor);
			sbHeader.AppendChar(',', 1);
		}
		sptr = Net::SocketUtil::GetAddrName(buff, req->GetClientAddr());
		sbHeader.AppendC(buff, (UOSInt)(sptr - buff));
		cli->AddHeaderC(UTF8STRC("X-Forwarded-For"), sbHeader.ToString(), sbHeader.GetLength());

		if (fwdPort == 0 && svrPort != 0)
		{
			sptr = Text::StrUInt16(buff, svrPort);
			cli->AddHeaderC(UTF8STRC("X-Forwarded-Port"), buff, (UOSInt)(sptr - buff));
		}
	}
	else
	{
		if (fwdFor)
		{
			cli->AddHeaderC(UTF8STRC("X-Forwarded-For"), fwdFor->v, fwdFor->leng);
		}
	}
	SDEL_STRING(fwdFor);
	SDEL_TEXT(fwdSsl);
	SDEL_TEXT(fwdHost);
	SDEL_TEXT(fwdPort);
	SDEL_TEXT(fwdProto);
	SDEL_TEXT(fwdPrefix);
	SDEL_STRING(svrHost);

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
		if (this->reqHdlr) this->reqHdlr(this->reqHdlrObj, req, resp);
		return true;
	}
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
			if (this->fwdType == ForwardType::Transparent && Text::StrEqualsICase(sarr[0], (const UTF8Char*)"LOCATION"))
			{
				if (Text::StrStartsWith(sarr[1], fwdBaseUrl))
				{
					sb.ClearStr();
					if (req->IsSecure())
					{
						sb.AppendC(UTF8STRC("https://"));
					}
					else
					{
						sb.AppendC(UTF8STRC("http://"));
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
					resp->AddHeaderC(sarr[0], Text::StrCharCnt(sarr[0]), sb.ToString(), sb.GetLength());
				}
				else
				{
					resp->AddHeaderC(sarr[0], Text::StrCharCnt(sarr[0]), sarr[1], Text::StrCharCnt(sarr[1]));
				}
			}
			else
			{
				resp->AddHeaderC(sarr[0], Text::StrCharCnt(sarr[0]), sarr[1], Text::StrCharCnt(sarr[1]));
			}
		}
		i++;
	}
	i = 0;
	j = this->injHeaders->GetCount();
	while (i < j)
	{
		sbHeader.ClearStr();
		sbHeader.Append(this->injHeaders->GetItem(i));
		if (Text::StrSplit(sarr, 2, sbHeader.ToString(), ':') == 2)
		{
			UTF8Char *sptr = Text::StrTrim(sarr[1]);
			resp->AddHeaderC(sarr[0], Text::StrCharCnt(sarr[0]), sarr[1], (UOSInt)(sptr - sarr[1]));
		}
		i++;
	}

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
	DEL_CLASS(cli);
	if (this->reqHdlr) this->reqHdlr(this->reqHdlrObj, req, resp);
	return true;
}

void Net::WebServer::HTTPForwardHandler::AddForwardURL(const UTF8Char *url)
{
	Sync::MutexUsage mutUsage(this->mut);
	this->forwardAddrs->Add(Text::StrCopyNew(url));
}

void Net::WebServer::HTTPForwardHandler::AddInjectHeader(Text::String *header)
{
	this->injHeaders->Add(header->Clone());
}

void Net::WebServer::HTTPForwardHandler::AddInjectHeader(const UTF8Char *header)
{
	this->injHeaders->Add(Text::String::NewNotNull(header));
}

void Net::WebServer::HTTPForwardHandler::HandleForwardRequest(ReqHandler reqHdlr, void *userObj)
{
	this->reqHdlrObj = userObj;
	this->reqHdlr = reqHdlr;
}
