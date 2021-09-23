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
	NEW_CLASS(this->forwardAddrs, Data::ArrayList<const UTF8Char*>());
	this->forwardAddrs->Add(Text::StrCopyNew(forwardURL));
	this->nextURL = 0;
	NEW_CLASS(this->mut, Sync::Mutex());
}

Net::WebServer::HTTPForwardHandler::~HTTPForwardHandler()
{
	LIST_FREE_FUNC(this->forwardAddrs, Text::StrDelNew);
	DEL_CLASS(this->forwardAddrs);
	DEL_CLASS(this->mut);
}

Bool Net::WebServer::HTTPForwardHandler::ProcessRequest(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, const UTF8Char *subReq)
{
	if (subReq[0] == 0)
	{
		subReq = (const UTF8Char*)"/";
	}
	Text::StringBuilderUTF8 sb;
	sb.Append(this->GetNextURL(req));
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

	i = 0;
	j = hdrNames.GetCount();
	while (i < j)
	{
		hdr = hdrNames.GetItem(i);
		if (Text::StrEquals(hdr, (const UTF8Char*)"Host"))
		{

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
			resp->AddHeader(sarr[0], sarr[1]);
		}
		i++;
	}
	UInt8 buff[2048];
	while (true)
	{
		i = cli->Read(buff, 2048);
		if (i > 0)
		{
			if (resp->Write(buff, i) != i)
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
	return true;
}

void Net::WebServer::HTTPForwardHandler::AddForwardURL(const UTF8Char *url)
{
	Sync::MutexUsage mutUsage(this->mut);
	this->forwardAddrs->Add(Text::StrCopyNew(url));
}
