#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/MemoryStream.h"
#include "IO/StreamWriter.h"
#include "Text/MyString.h"
#include "Text/URLString.h"
#include "Text/UTF8Writer.h"
#include "Net/WebServer/HTTPServerUtil.h"
#include "Net/WebServer/WebStandardHandler.h"

Bool Net::WebServer::WebStandardHandler::DoRequest(NN<Net::WebServer::IWebRequest> req, NN<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq)
{
	if (subReq.v[0] != '/')
		return false;
	UTF8Char tmpBuff[256];
	UnsafeArray<UTF8Char> sbuff;
	UTF8Char c;
	UOSInt i = 1;
	Optional<Net::WebServer::WebStandardHandler> subHdlr;
	NN<Net::WebServer::WebStandardHandler> nnsubHdlr;

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
				sbuff = MemAllocArr(UTF8Char, i);
				MemCopyNO(sbuff.Ptr(), &subReq.v[1], (i - 1) * sizeof(UTF8Char));
				sbuff[i - 1] = 0;
				subHdlr = this->hdlrs.GetC({sbuff, i - 1});
				MemFreeArr(sbuff);
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
	if (subHdlr.SetTo(nnsubHdlr))
	{
		return nnsubHdlr->ProcessRequest(req, resp, subReq);
	}
	return false;
}

void Net::WebServer::WebStandardHandler::AddResponseHeaders(NN<Net::WebServer::IWebRequest> req, NN<Net::WebServer::IWebResponse> resp)
{
	resp->AddDefHeaders(req);
	NN<Text::String> s;
	if (this->allowOrigin.SetTo(s))
	{
		resp->AddHeader(CSTR("Access-Control-Allow-Origin"), s->ToCString());
	}
	if (this->contentSecurityPolicy.SetTo(s))
	{
		resp->AddHeader(CSTR("content-security-policy"), s->ToCString());
	}
}

Bool Net::WebServer::WebStandardHandler::ResponseJSONStr(NN<Net::WebServer::IWebRequest> req, NN<Net::WebServer::IWebResponse> resp, OSInt cacheAge, Text::CStringNN json)
{
	Text::CStringNN mime = CSTR("application/json");
	this->AddResponseHeaders(req, resp);
	resp->AddCacheControl(cacheAge);
	resp->AddContentType(mime);
	return Net::WebServer::HTTPServerUtil::SendContent(req, resp, mime, json);
}

Bool Net::WebServer::WebStandardHandler::ResponseAllowOptions(NN<Net::WebServer::IWebRequest> req, NN<Net::WebServer::IWebResponse> resp, UOSInt maxAge, Text::CStringNN options)
{
	Text::StringBuilderUTF8 sb;
	if (req->GetHeaderC(sb, CSTR("Access-Control-Request-Method")) && !this->allowOrigin.IsNull())
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
		resp->Write(Data::ByteArrayR(U8STR(""), 0));
		return true;
	}
	else
	{
		resp->SetStatusCode(Net::WebStatus::SC_NO_CONTENT);
		this->AddResponseHeaders(req, resp);
		resp->AddHeader(CSTR("Allow"), options);
		resp->Write(Data::ByteArrayR(U8STR(""), 0));
		return true;
	}

}

Net::WebServer::WebStandardHandler::WebStandardHandler()
{
	this->allowOrigin = 0;
	this->contentSecurityPolicy = 0;
}

Net::WebServer::WebStandardHandler::~WebStandardHandler()
{
	UOSInt i = this->relHdlrs.GetCount();
	while (i-- > 0)
	{
		this->relHdlrs.GetItem(i).Delete();
	}
	OPTSTR_DEL(this->allowOrigin);
	OPTSTR_DEL(this->contentSecurityPolicy);
}

void Net::WebServer::WebStandardHandler::WebRequest(NN<Net::WebServer::IWebRequest> req, NN<Net::WebServer::IWebResponse> resp)
{
	NN<Text::String> reqURL = req->GetRequestURI();
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	sptr = Text::URLString::GetURLPathSvr(sbuff, reqURL->v, reqURL->leng);
	if (!this->ProcessRequest(req, resp, CSTRP(sbuff, sptr)))
	{
		resp->SetStatusCode(Net::WebStatus::SC_NOT_FOUND);
		if (req->GetProtocol() == Net::WebServer::IWebRequest::RequestProtocol::HTTP1_0 || req->GetProtocol() == Net::WebServer::IWebRequest::RequestProtocol::HTTP1_1)
		{
			resp->AddDefHeaders(req);

			IO::MemoryStream mstm;
			Text::UTF8Writer writer(mstm);
			writer.WriteLine(CSTR("<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML 2.0//EN\">"));
			writer.WriteLine(CSTR("<html><head>"));
			writer.WriteLine(CSTR("<title>404 Not Found</title>"));
			writer.WriteLine(CSTR("</head><body>"));
			writer.WriteLine(CSTR("<h1>Not Found</h1>"));
			writer.Write(CSTR("<p>The requested URL "));
			writer.Write(reqURL->ToCString());
			writer.WriteLine(CSTR(" was not found on this server.</p>"));
			writer.WriteLine(CSTR("</body></html>"));
			resp->AddContentType(CSTR("text/html"));
			mstm.SeekFromBeginning(0);
			Net::WebServer::HTTPServerUtil::SendContent(req, resp, CSTR("text/html"), mstm.GetLength(), mstm);
		}
	}
}

Bool Net::WebServer::WebStandardHandler::ProcessRequest(NN<Net::WebServer::IWebRequest> req, NN<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq)
{
	return DoRequest(req, resp, subReq);
}

void Net::WebServer::WebStandardHandler::HandlePath(Text::CStringNN relativePath, NN<Net::WebServer::WebStandardHandler> hdlr, Bool needRelease)
{
	NN<Net::WebServer::WebStandardHandler> subHdlr;
	if (relativePath.v[0] != '/')
	{
		if (needRelease)
		{
			hdlr.Delete();
		}
		return;
	}
	UOSInt i = relativePath.IndexOf('/', 1);
	UnsafeArray<UTF8Char> sbuff;
	if (i == INVALID_INDEX)
	{
		this->hdlrs.PutC(relativePath.Substring(1), hdlr);
		if (needRelease)
		{
			this->relHdlrs.Add(hdlr);
		}
	}
	else
	{
		sbuff = MemAllocArr(UTF8Char, i);
		MemCopyNO(sbuff.Ptr(), &relativePath.v[1], sizeof(UTF8Char) * (i - 1));
		sbuff[i] = 0;

		if (!this->hdlrs.GetC({sbuff, i - 1}).SetTo(subHdlr))
		{
			NEW_CLASSNN(subHdlr, Net::WebServer::WebStandardHandler());
			this->hdlrs.PutC({sbuff, i - 1}, subHdlr);
			this->relHdlrs.Add(subHdlr);
		}
		MemFreeArr(sbuff);
		subHdlr->HandlePath(relativePath.Substring(i), hdlr, needRelease);
	}
}

void Net::WebServer::WebStandardHandler::SetAllowOrigin(Text::CString origin)
{
	OPTSTR_DEL(this->allowOrigin);
	this->allowOrigin = Text::String::NewOrNull(origin);
}

void Net::WebServer::WebStandardHandler::SetContentSecurityPolicy(Text::CString csp)
{
	OPTSTR_DEL(this->contentSecurityPolicy);
	this->contentSecurityPolicy = Text::String::NewOrNull(csp);
}
