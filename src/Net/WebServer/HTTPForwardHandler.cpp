#include "Stdafx.h"
#include "Net/HTTPClient.h"
#include "Net/WebServer/HTTPForwardHandler.h"
#include "Sync/MutexUsage.h"
#include "Text/CPPText.h"
#include "Text/StringBuilderUTF8.h"

Optional<Text::String> Net::WebServer::HTTPForwardHandler::GetNextURL(NN<Net::WebServer::IWebRequest> req)
{
	Sync::MutexUsage mutUsage(this->mut);
	UOSInt i = this->nextURL;
	this->nextURL = (i + 1) % this->forwardAddrs.GetCount();
	return this->forwardAddrs.GetItem(i);
}

Net::WebServer::HTTPForwardHandler::HTTPForwardHandler(NN<Net::TCPClientFactory> clif, Optional<Net::SSLEngine> ssl, Text::CStringNN forwardURL, ForwardType fwdType)
{
	this->clif = clif;
	this->ssl = ssl;
	this->forceHost = 0;
	this->fwdType = fwdType;
	this->reqHdlr = 0;
	this->reqHdlrObj = 0;
	this->forwardAddrs.Add(Text::String::New(forwardURL));
	this->nextURL = 0;
	this->log = 0;
	this->logContent = false;
}

Net::WebServer::HTTPForwardHandler::~HTTPForwardHandler()
{
	this->forwardAddrs.FreeAll();
	this->injHeaders.FreeAll();
	OPTSTR_DEL(this->forceHost);
}

Bool Net::WebServer::HTTPForwardHandler::ProcessRequest(NN<Net::WebServer::IWebRequest> req, NN<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq)
{
	NN<IO::LogTool> log;
	UInt8 buff[2048];
	UnsafeArray<UTF8Char> sptr;
	if (subReq.v[0] == 0)
	{
		subReq = CSTR("/");
	}
	NN<Text::String> fwdBaseUrl;
	if (!this->GetNextURL(req).SetTo(fwdBaseUrl))
		return false;
	Text::StringBuilderUTF8 sb;
	sb.Append(fwdBaseUrl);
	if (sb.EndsWith('/'))
	{
		sb.RemoveChars(1);
	}
	sb.Append(subReq);
	NN<Text::String> uri = req->GetRequestURI();
	UOSInt i = uri->IndexOf('?');
	UOSInt j;
	if (i != INVALID_INDEX)
	{
		sb.AppendC(&uri->v[i], uri->leng - i);
	}
	Bool kaConn = true;
	Text::StringBuilderUTF8 sbHeader;
	if (req->GetHeaderC(sbHeader, CSTR("Connection")))
	{
		if (sbHeader.Equals(UTF8STRC("close")))
		{
			kaConn = false;
		}
	}
	if (this->log.SetTo(log))
	{
		sbHeader.ClearStr();
		sbHeader.AppendC(UTF8STRC("Forwarding Req: "));
		sbHeader.Append(sb.ToCString());
		sbHeader.AppendUTF8Char(' ');
		sbHeader.Append(req->GetReqMethodStr());
		log->LogMessage(sbHeader.ToCString(), IO::LogHandler::LogLevel::Action);
	}
	NN<Net::HTTPClient> cli = Net::HTTPClient::CreateClient(this->clif, this->ssl, CSTR("sswr/1.0"), kaConn, sb.StartsWith(UTF8STRC("https://")));
	NN<Text::String> hdr;
	if (this->forceHost.SetTo(hdr))
	{
		cli->ForceHostName(hdr->ToCString());
	}
	if (!cli->Connect(sb.ToCString(), req->GetReqMethod(), 0, 0, false))
	{
		cli.Delete();
		resp->ResponseError(req, Net::WebStatus::SC_NOT_FOUND);
		if (this->reqHdlr) this->reqHdlr(this->reqHdlrObj, req, resp);
		return true;
	}
	Data::ArrayListStringNN hdrNames;
	req->GetHeaderNames(hdrNames);
	Data::ArrayIterator<NN<Text::String>> it;

	Text::String *svrHost = 0;
	UInt16 svrPort = 0;
	Text::String *fwdFor = 0;
	UnsafeArrayOpt<const UTF8Char> fwdPrefix = 0;
	UnsafeArrayOpt<const UTF8Char> fwdHost = 0;
	UnsafeArrayOpt<const UTF8Char> fwdProto = 0;
	UnsafeArrayOpt<const UTF8Char> fwdPort = 0;
	UnsafeArrayOpt<const UTF8Char> fwdSsl = 0;
	it = hdrNames.Iterator();
	while (it.HasNext())
	{
		hdr = it.Next();
		if (hdr->EqualsICase(UTF8STRC("Host")))
		{
			sbHeader.ClearStr();
			if (req->GetHeaderC(sbHeader, hdr->ToCString()))
			{
				UOSInt k = sbHeader.IndexOf(':');
				svrHost = Text::String::New(sbHeader.ToString(), sbHeader.GetLength()).Ptr();
				if (k >= 0)
				{
					Text::StrToUInt16(sbHeader.ToString() + k + 1, svrPort);
				}
			}
		}
		else if (hdr->EqualsICase(UTF8STRC("X-Forwarded-For")))
		{
			sbHeader.ClearStr();
			if (req->GetHeaderC(sbHeader, hdr->ToCString()))
			{
				fwdFor = Text::String::New(sbHeader.ToString(), sbHeader.GetLength()).Ptr();
			}
		}
		else
		{
			sbHeader.ClearStr();
			if (req->GetHeaderC(sbHeader, hdr->ToCString()))
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
			sbHeader.AppendUTF8Char(',');
		}
		sptr = Net::SocketUtil::GetAddrName(buff, req->GetClientAddr()).Or(buff);
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

	UnsafeArray<const UInt8> reqData;
	Text::PString sarr[2];
	if (req->GetReqData(i).SetTo(reqData))
	{
		if (this->log.SetTo(log) && this->logContent)
		{
			sbHeader.ClearStr();
			sbHeader.AppendC(UTF8STRC("RequestContent = "));
			Text::CPPText::ToCPPString(sbHeader, reqData, i);
			log->LogMessage(sbHeader.ToCString(), IO::LogHandler::LogLevel::Raw);
		}
		cli->Write(Data::ByteArrayR(reqData, i));
	}
	Net::WebStatus::StatusCode scode = cli->GetRespStatus();
	if (scode == Net::WebStatus::SC_UNKNOWN)
	{
		cli.Delete();
		resp->ResponseError(req, Net::WebStatus::SC_NOT_FOUND);
		if (this->reqHdlr) this->reqHdlr(this->reqHdlrObj, req, resp);
		return true;
	}
	resp->SetStatusCode(scode);
	i = 0;
	j = cli->GetRespHeaderCnt();
	while (i < j)
	{
		if (cli->GetRespHeader(i).SetTo(hdr))
		{
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
						req->GetHeaderC(sb, CSTR("Host"));
						UOSInt urlLen = fwdBaseUrl->leng;
						if (fwdBaseUrl->v[urlLen - 1] == '/')
						{
							sb.AppendC(&sarr[1].v[urlLen - 1], sarr[1].leng - urlLen - 1);
						}
						else
						{
							sb.AppendC(&sarr[1].v[urlLen], sarr[1].leng - urlLen);
						}
						resp->AddHeader(sarr[0].ToCString(), sb.ToCString());
					}
					else
					{
						resp->AddHeader(sarr[0].ToCString(), sarr[1].ToCString());
					}
				}
				else
				{
					resp->AddHeader(sarr[0].ToCString(), sarr[1].ToCString());
				}
			}
		}
		i++;
	}
	it = this->injHeaders.Iterator();
	while (it.HasNext())
	{
		sbHeader.ClearStr();
		sbHeader.Append(it.Next());
		if (Text::StrSplitP(sarr, 2, sbHeader, ':') == 2)
		{
			sarr[1].Trim();
			resp->AddHeader(sarr[0].ToCString(), sarr[1].ToCString());
		}
	}

	UInt64 totalSize = 0;
	sbHeader.ClearStr();
	while (true)
	{
		i = cli->Read(BYTEARR(buff));
		if (i > 0)
		{
			j = resp->Write(Data::ByteArrayR(buff, i));
			if (j != i)
			{
				break;
			}
			totalSize += i;
			if (this->logContent)
				sbHeader.AppendC(buff, i);
		}
		else
		{
			break;
		}
	}
	if (this->log.SetTo(log))
	{
		sb.ClearStr();
		sb.AppendC(UTF8STRC("Response Status = "));
		sb.AppendU32((UInt32)scode);
		sb.AppendC(UTF8STRC(", size = "));
		sb.AppendU64(totalSize);
		log->LogMessage(sb.ToCString(), IO::LogHandler::LogLevel::Action);
		if (this->logContent)
		{
			sb.ClearStr();
			sb.AppendC(UTF8STRC("ResponseContent = "));
			Text::CPPText::ToCPPString(sb, sbHeader.v, sbHeader.leng);
			log->LogMessage(sb.ToCString(), IO::LogHandler::LogLevel::Raw);
		}
	}
	cli.Delete();
	if (this->reqHdlr) this->reqHdlr(this->reqHdlrObj, req, resp);
	return true;
}

void Net::WebServer::HTTPForwardHandler::AddForwardURL(Text::CStringNN url)
{
	Sync::MutexUsage mutUsage(this->mut);
	this->forwardAddrs.Add(Text::String::New(url));
}

void Net::WebServer::HTTPForwardHandler::AddInjectHeader(NN<Text::String> header)
{
	this->injHeaders.Add(header->Clone());
}

void Net::WebServer::HTTPForwardHandler::AddInjectHeader(Text::CStringNN header)
{
	this->injHeaders.Add(Text::String::New(header));
}

void Net::WebServer::HTTPForwardHandler::SetLog(Optional<IO::LogTool> log, Bool logContent)
{
	this->logContent = logContent;
	this->log = log;
}

void Net::WebServer::HTTPForwardHandler::SetForceHost(Text::CStringNN forceHost)
{
	OPTSTR_DEL(this->forceHost);
	if (forceHost.leng > 0)
	{
		this->forceHost = Text::String::New(forceHost);
	}
}

void Net::WebServer::HTTPForwardHandler::HandleForwardRequest(ReqHandler reqHdlr, AnyType userObj)
{
	this->reqHdlrObj = userObj;
	this->reqHdlr = reqHdlr;
}
