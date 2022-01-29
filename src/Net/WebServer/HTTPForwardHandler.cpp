#include "Stdafx.h"
#include "Net/HTTPClient.h"
#include "Net/WebServer/HTTPForwardHandler.h"
#include "Sync/MutexUsage.h"
#include "Text/StringBuilderUTF8.h"

Text::String *Net::WebServer::HTTPForwardHandler::GetNextURL(Net::WebServer::IWebRequest *req)
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
	NEW_CLASS(this->forwardAddrs, Data::ArrayList<Text::String*>());
	NEW_CLASS(this->injHeaders, Data::ArrayList<Text::String*>());
	this->forwardAddrs->Add(Text::String::NewNotNull(forwardURL));
	this->nextURL = 0;
	NEW_CLASS(this->mut, Sync::Mutex());
}

Net::WebServer::HTTPForwardHandler::~HTTPForwardHandler()
{
	LIST_FREE_STRING(this->forwardAddrs);
	DEL_CLASS(this->forwardAddrs);
	LIST_FREE_FUNC(this->injHeaders, STR_REL);
	DEL_CLASS(this->injHeaders);
	DEL_CLASS(this->mut);
}

Bool Net::WebServer::HTTPForwardHandler::ProcessRequest(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, const UTF8Char *subReq, UOSInt subReqLen)
{
	UInt8 buff[2048];
	UTF8Char *sptr;
	if (subReq[0] == 0)
	{
		subReq = (const UTF8Char*)"/";
		subReqLen = 1;
	}
	Text::StringBuilderUTF8 sb;
	Text::String *fwdBaseUrl = this->GetNextURL(req);
	sb.Append(fwdBaseUrl);
	if (sb.EndsWith('/'))
	{
		sb.RemoveChars(1);
	}
	sb.AppendC(subReq, subReqLen);
	Text::String *uri = req->GetRequestURI();
	UOSInt i = uri->IndexOf('?');
	UOSInt j;
	if (i >= 0)
	{
		sb.AppendC(&uri->v[i], uri->leng - i);
	}
	Bool kaConn = true;
	Text::StringBuilderUTF8 sbHeader;
	if (req->GetHeaderC(&sbHeader, UTF8STRC("Connection")))
	{
		if (sbHeader.Equals(UTF8STRC("close")))
		{
			kaConn = false;
		}
	}
	Net::HTTPClient *cli = Net::HTTPClient::CreateClient(this->sockf, this->ssl, CSTR("sswr/1.0"), kaConn, sb.StartsWith(UTF8STRC("https://")));
	if (cli == 0)
	{
		resp->ResponseError(req, Net::WebStatus::SC_NOT_FOUND);
		if (this->reqHdlr) this->reqHdlr(this->reqHdlrObj, req, resp);
		return true;
	}
	if (!cli->Connect(sb.ToCString(), req->GetReqMethod(), 0, 0, false))
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
		if (hdr->EqualsICase(UTF8STRC("Host")))
		{
			sbHeader.ClearStr();
			if (req->GetHeaderC(&sbHeader, hdr->v, hdr->leng))
			{
				UOSInt k = sbHeader.IndexOf(':');
				svrHost = Text::String::New(sbHeader.ToString(), sbHeader.GetLength());
				if (k >= 0)
				{
					Text::StrToUInt16(sbHeader.ToString() + k + 1, &svrPort);
				}
			}
		}
		else if (hdr->EqualsICase(UTF8STRC("X-Forwarded-For")))
		{
			sbHeader.ClearStr();
			if (req->GetHeaderC(&sbHeader, hdr->v, hdr->leng))
			{
				fwdFor = Text::String::New(sbHeader.ToString(), sbHeader.GetLength());
			}
		}
		else
		{
			sbHeader.ClearStr();
			if (req->GetHeaderC(&sbHeader, hdr->v, hdr->leng))
			{
				cli->AddHeaderC(hdr->ToCString(), sbHeader.ToCString());
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
				cli->AddHeaderC(CSTR("X-Forwarded-Proto"), CSTR("https"));
			}
			else
			{
				cli->AddHeaderC(CSTR("X-Forwarded-Proto"), CSTR("http"));
			}
		}
		if (fwdSsl == 0)
		{
			if (req->IsSecure())
			{
				cli->AddHeaderC(CSTR("X-Forwarded-Ssl"), CSTR("on"));
			}
		}
		if (fwdHost == 0)
		{
			if (svrHost)
			{
				cli->AddHeaderC(CSTR("X-Forwarded-Host"), svrHost->ToCString());
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
		cli->AddHeaderC(CSTR("X-Forwarded-For"), sbHeader.ToCString());

		if (fwdPort == 0 && svrPort != 0)
		{
			sptr = Text::StrUInt16(buff, svrPort);
			cli->AddHeaderC(CSTR("X-Forwarded-Port"), {buff, (UOSInt)(sptr - buff)});
		}
	}
	else
	{
		if (fwdFor)
		{
			cli->AddHeaderC(CSTR("X-Forwarded-For"), fwdFor->ToCString());
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
	Text::PString sarr[2];
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
		if (Text::StrSplitP(sarr, 2, sbHeader, ':') == 2)
		{
			sarr[1].Trim();
			if (this->fwdType == ForwardType::Transparent && Text::StrEqualsICaseC(sarr[0].v, sarr[0].leng, UTF8STRC("LOCATION")))
			{
				if (Text::StrStartsWithC(sarr[1].v, sarr[1].leng, fwdBaseUrl->v, fwdBaseUrl->leng))
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
					req->GetHeaderC(&sb, UTF8STRC("Host"));
					UOSInt urlLen = fwdBaseUrl->leng;
					if (fwdBaseUrl->v[urlLen - 1] == '/')
					{
						sb.AppendC(&sarr[1].v[urlLen - 1], sarr[1].leng - urlLen - 1);
					}
					else
					{
						sb.AppendC(&sarr[1].v[urlLen], sarr[1].leng - urlLen);
					}
					resp->AddHeaderC(sarr[0].v, sarr[0].leng, sb.ToString(), sb.GetLength());
				}
				else
				{
					resp->AddHeaderC(sarr[0].v, sarr[0].leng, sarr[1].v, sarr[1].leng);
				}
			}
			else
			{
				resp->AddHeaderC(sarr[0].v, sarr[0].leng, sarr[1].v, sarr[1].leng);
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
		if (Text::StrSplitP(sarr, 2, sbHeader, ':') == 2)
		{
			sarr[1].Trim();
			resp->AddHeaderC(sarr[0].v, sarr[0].leng, sarr[1].v, sarr[1].leng);
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
	this->forwardAddrs->Add(Text::String::NewNotNull(url));
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
