#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/MemoryStream.h"
#include "IO/StreamWriter.h"
#include "Text/MyString.h"
#include "Text/URLString.h"
#include "Text/UTF8Writer.h"
#include "Net/WebServer/HTTPServerUtil.h"
#include "Net/WebServer/WebStandardHandler.h"

Bool Net::WebServer::WebStandardHandler::DoRequest(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq)
{
	if (subReq.v[0] != '/')
		return false;
	UTF8Char tmpBuff[256];
	UTF8Char *sbuff;
	UTF8Char c;
	UOSInt i = 1;
	Net::WebServer::WebStandardHandler *subHdlr;

	while (true)
	{
		c = subReq.v[i];
		if (c == 0)
		{
			if (i == 1)
			{
				subHdlr = this->hdlrs.GetC({&subReq.v[1], i - 1});
				subReq = subReq.Substring(i);
				break;
			}
			else
			{
				subHdlr = this->hdlrs.GetC({&subReq.v[1], i - 1});
				subReq = subReq.Substring(i);
				break;
			}
		}
		else if (c == '/' || c == '?')
		{
			if (i > 256)
			{
				sbuff = MemAlloc(UTF8Char, i);
				MemCopyNO(sbuff, &subReq.v[1], (i - 1) * sizeof(UTF8Char));
				sbuff[i - 1] = 0;
				subHdlr = this->hdlrs.GetC({sbuff, i - 1});
				MemFree(sbuff);
			}
			else
			{
				Text::StrConcatC(tmpBuff, &subReq.v[1], (i - 1));
				subHdlr = this->hdlrs.GetC({tmpBuff, i - 1});
			}
			subReq = subReq.Substring(i);
			break;
		}
		i++;
	}
	if (subHdlr)
	{
		return subHdlr->ProcessRequest(req, resp, subReq);
	}
	return false;
}

void Net::WebServer::WebStandardHandler::AddResponseHeaders(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp)
{
	resp->AddDefHeaders(req);
	if (this->allowOrigin)
	{
		resp->AddHeader(CSTR("Access-Control-Allow-Origin"), this->allowOrigin->ToCString());
	}
}

Bool Net::WebServer::WebStandardHandler::ResponseJSONStr(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, OSInt cacheAge, Text::CStringNN json)
{
	this->AddResponseHeaders(req, resp);
	resp->AddCacheControl(cacheAge);
	return resp->ResponseText(json, CSTR("application/json"));
}

Bool Net::WebServer::WebStandardHandler::ResponseAllowOptions(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, UOSInt maxAge, Text::CStringNN options)
{
	Text::StringBuilderUTF8 sb;
	if (req->GetHeaderC(sb, CSTR("Access-Control-Request-Method")) && this->allowOrigin)
	{
		resp->SetStatusCode(Net::WebStatus::SC_NO_CONTENT);
		resp->AddDefHeaders(req);
		sb.ClearStr();
		if (req->GetHeaderC(sb, CSTR("Origin")))
		{
			resp->AddHeader(CSTR("Access-Control-Allow-Origin"), sb.ToCString());
		}
		sb.ClearStr();
		if (req->GetHeaderC(sb, CSTR("Access-Control-Request-Headers")))
		{
			resp->AddHeader(CSTR("Access-Control-Allow-Headers"), sb.ToCString());
		}
		resp->AddHeader(CSTR("Access-Control-Allow-Methods"), options);
		sb.ClearStr();
		sb.AppendUOSInt(maxAge);
		resp->AddHeader(CSTR("Access-Control-Max-Age"), sb.ToCString());
		resp->Write(0, 0);
		return true;
	}
	else
	{
		resp->SetStatusCode(Net::WebStatus::SC_NO_CONTENT);
		this->AddResponseHeaders(req, resp);
		resp->AddHeader(CSTR("Allow"), options);
		resp->Write(0, 0);
		return true;
	}

}

Net::WebServer::WebStandardHandler::WebStandardHandler()
{
	this->allowOrigin = 0;
}

Net::WebServer::WebStandardHandler::~WebStandardHandler()
{
	UOSInt i = this->relHdlrs.GetCount();
	while (i-- > 0)
	{
		this->relHdlrs.GetItem(i).Delete();
	}
	SDEL_STRING(this->allowOrigin);
}

void Net::WebServer::WebStandardHandler::WebRequest(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp)
{
	NotNullPtr<Text::String> reqURL = req->GetRequestURI();
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	sptr = Text::URLString::GetURLPathSvr(sbuff, reqURL->v, reqURL->leng);
	if (!this->ProcessRequest(req, resp, CSTRP(sbuff, sptr)))
	{
		resp->SetStatusCode(Net::WebStatus::SC_NOT_FOUND);
		if (req->GetProtocol() == Net::WebServer::IWebRequest::RequestProtocol::HTTP1_0 || req->GetProtocol() == Net::WebServer::IWebRequest::RequestProtocol::HTTP1_1)
		{
			resp->AddDefHeaders(req);

			IO::MemoryStream mstm;
			Text::UTF8Writer writer(mstm);
			writer.WriteLineC(UTF8STRC("<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML 2.0//EN\">"));
			writer.WriteLineC(UTF8STRC("<html><head>"));
			writer.WriteLineC(UTF8STRC("<title>404 Not Found</title>"));
			writer.WriteLineC(UTF8STRC("</head><body>"));
			writer.WriteLineC(UTF8STRC("<h1>Not Found</h1>"));
			writer.WriteStrC(UTF8STRC("<p>The requested URL "));
			writer.WriteStrC(reqURL->v, reqURL->leng);
			writer.WriteLineC(UTF8STRC(" was not found on this server.</p>"));
			writer.WriteLineC(UTF8STRC("</body></html>"));
			resp->AddContentType(CSTR("text/html"));
			mstm.SeekFromBeginning(0);
			Net::WebServer::HTTPServerUtil::SendContent(req, resp, CSTR("text/html"), mstm.GetLength(), mstm);
		}
	}
}

Bool Net::WebServer::WebStandardHandler::ProcessRequest(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq)
{
	return DoRequest(req, resp, subReq);
}

void Net::WebServer::WebStandardHandler::HandlePath(Text::CStringNN relativePath, NotNullPtr<Net::WebServer::WebStandardHandler> hdlr, Bool needRelease)
{
	NotNullPtr<Net::WebServer::WebStandardHandler> subHdlr;
	if (relativePath.v[0] != '/')
	{
		if (needRelease)
		{
			hdlr.Delete();
		}
		return;
	}
	UOSInt i = relativePath.IndexOf('/', 1);
	UTF8Char *sbuff;
	if (i == INVALID_INDEX)
	{
		this->hdlrs.PutC(relativePath.Substring(1), hdlr.Ptr());
		if (needRelease)
		{
			this->relHdlrs.Add(hdlr);
		}
	}
	else
	{
		sbuff = MemAlloc(UTF8Char, i);
		MemCopyNO(sbuff, &relativePath.v[1], sizeof(UTF8Char) * i);
		sbuff[i] = 0;

		if (!subHdlr.Set(this->hdlrs.GetC({sbuff, i})))
		{
			NEW_CLASSNN(subHdlr, Net::WebServer::WebStandardHandler());
			this->hdlrs.PutC({sbuff, i}, subHdlr.Ptr());
			this->relHdlrs.Add(subHdlr);
		}
		MemFree(sbuff);
		subHdlr->HandlePath(relativePath.Substring(i), hdlr, needRelease);
	}
}

void Net::WebServer::WebStandardHandler::SetAllowOrigin(Text::CString origin)
{
	SDEL_STRING(this->allowOrigin);
	this->allowOrigin = Text::String::NewOrNull(origin);
}
