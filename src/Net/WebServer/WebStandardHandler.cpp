#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/MemoryStream.h"
#include "IO/StreamWriter.h"
#include "Text/MyString.h"
#include "Text/URLString.h"
#include "Text/UTF8Writer.h"
#include "Net/WebServer/HTTPServerUtil.h"
#include "Net/WebServer/WebStandardHandler.h"

Net::WebServer::WebStandardHandler::~WebStandardHandler()
{
	Net::WebServer::WebStandardHandler *hdlr;
	UOSInt i = this->relHdlrs.GetCount();
	while (i-- > 0)
	{
		hdlr = this->relHdlrs.GetItem(i);
		hdlr->Release();
	}
}

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

Net::WebServer::WebStandardHandler::WebStandardHandler()
{
}

void Net::WebServer::WebStandardHandler::WebRequest(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp)
{
	NotNullPtr<Text::String> reqURL = req->GetRequestURI();
	Net::WebUtil::RequestMethod reqMeth = req->GetReqMethod();
	if (reqMeth == Net::WebUtil::RequestMethod::RTSP_OPTIONS)
	{
		UTF8Char sbuff[512];
		UTF8Char *sptr;
		if ((sptr = req->GetHeader(sbuff, CSTR("CSeq"), 512)) != 0)
		{
			resp->AddHeader(CSTR("CSeq"), CSTRP(sbuff, sptr));
		}
		resp->AddContentLength(0);
		resp->AddHeader(CSTR("Public"), CSTR("DESCRIBE,SETUP,TEARDOWN,PLAY,PAUSE"));
		resp->SetStatusCode(Net::WebStatus::SC_OK);
	}
	else
	{
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
				Net::WebServer::HTTPServerUtil::SendContent(req, resp, CSTR("text/html"), mstm.GetLength(), &mstm);
			}
		}
	}
}

void Net::WebServer::WebStandardHandler::Release()
{
	DEL_CLASS(this);
}

Bool Net::WebServer::WebStandardHandler::ProcessRequest(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq)
{
	return DoRequest(req, resp, subReq);
}

void Net::WebServer::WebStandardHandler::HandlePath(Text::CString relativePath, Net::WebServer::WebStandardHandler *hdlr, Bool needRelease)
{
	Net::WebServer::WebStandardHandler *subHdlr;
	if (hdlr == 0)
		return;
	if (relativePath.v[0] != '/')
	{
		if (needRelease)
		{
			hdlr->Release();
		}
		return;
	}
	UOSInt i = relativePath.IndexOf('/', 1);
	UTF8Char *sbuff;
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
		sbuff = MemAlloc(UTF8Char, i);
		MemCopyNO(sbuff, &relativePath.v[1], sizeof(UTF8Char) * i);
		sbuff[i] = 0;

		subHdlr = this->hdlrs.GetC({sbuff, i});
		if (subHdlr == 0)
		{
			NEW_CLASS(subHdlr, Net::WebServer::WebStandardHandler());
			this->hdlrs.PutC({sbuff, i}, subHdlr);
			this->relHdlrs.Add(subHdlr);
		}
		MemFree(sbuff);
		subHdlr->HandlePath(relativePath.Substring(i), hdlr, needRelease);
	}
}
