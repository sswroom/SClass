#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/MemoryStream.h"
#include "IO/StreamWriter.h"
#include "Text/MyString.h"
#include "Text/URLString.h"
#include "Text/UTF8Writer.h"
#include "Net/WebServer/WebStandardHandler.h"

Net::WebServer::WebStandardHandler::~WebStandardHandler()
{
	Net::WebServer::WebStandardHandler *hdlr;
	UOSInt i = this->relHdlrs->GetCount();
	while (i-- > 0)
	{
		hdlr = this->relHdlrs->GetItem(i);
		hdlr->Release();
	}
	DEL_CLASS(this->hdlrs);
	DEL_CLASS(this->relHdlrs);
}

Bool Net::WebServer::WebStandardHandler::ProcessRequest(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, const UTF8Char *subReq)
{
	return DoRequest(req, resp, subReq);
}

Bool Net::WebServer::WebStandardHandler::DoRequest(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, const UTF8Char *subReq)
{
	if (subReq[0] != '/')
		return false;
	UTF8Char *sbuff;
	UTF8Char c;
	UOSInt i = 1;
	Net::WebServer::WebStandardHandler *subHdlr;

	while (true)
	{
		c = subReq[i];
		if (c == 0)
		{
			if (i == 1)
			{
				subHdlr = this->hdlrs->Get(&subReq[1]);
				subReq = &subReq[i];
				break;
			}
			else
			{
				subHdlr = this->hdlrs->Get(&subReq[1]);
				subReq = &subReq[i];
				break;
			}
		}
		else if (c == '/' || c == '?')
		{
			sbuff = MemAlloc(UTF8Char, i);
			MemCopyNO(sbuff, &subReq[1], (i - 1) * sizeof(UTF8Char));
			sbuff[i - 1] = 0;
			subHdlr = this->hdlrs->Get(sbuff);
			MemFree(sbuff);
			subReq = &subReq[i];
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
	NEW_CLASS(this->hdlrs, Data::StringUTF8Map<Net::WebServer::WebStandardHandler*>());
	NEW_CLASS(this->relHdlrs, Data::ArrayList<Net::WebServer::WebStandardHandler*>());
}

void Net::WebServer::WebStandardHandler::WebRequest(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp)
{
	const UTF8Char *reqURL = req->GetRequestURI();
	Net::WebServer::IWebRequest::RequestMethod reqMeth = req->GetReqMethod();
	if (reqMeth == Net::WebServer::IWebRequest::REQMETH_RTSP_OPTIONS)
	{
		UTF8Char sbuff[512];
		if (req->GetHeader(sbuff, (const UTF8Char*)"CSeq", 512))
		{
			resp->AddHeader((const UTF8Char*)"CSeq", sbuff);
		}
		resp->AddContentLength(0);
		resp->AddHeader((const UTF8Char*)"Public", (const UTF8Char*)"DESCRIBE,SETUP,TEARDOWN,PLAY,PAUSE");
		resp->SetStatusCode(Net::WebStatus::SC_OK);
	}
	else
	{
		UTF8Char sbuff[512];
		Text::URLString::GetURLPathSvr(sbuff, reqURL);
		if (!this->ProcessRequest(req, resp, sbuff))
		{
			resp->SetStatusCode(Net::WebStatus::SC_NOT_FOUND);
			if (req->GetProtocol() == Net::WebServer::IWebRequest::REQPROTO_HTTP1_0 || req->GetProtocol() == Net::WebServer::IWebRequest::REQPROTO_HTTP1_1)
			{
				resp->AddDefHeaders(req);

				IO::MemoryStream *mstm;
				IO::Writer *writer;
				NEW_CLASS(mstm, IO::MemoryStream((const UTF8Char*)"Net.WebServer.WebStandardHandler.WebRequest"));
				NEW_CLASS(writer, Text::UTF8Writer(mstm));
				writer->WriteLine((const UTF8Char*)"<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML 2.0//EN\">");
				writer->WriteLine((const UTF8Char*)"<html><head>");
				writer->WriteLine((const UTF8Char*)"<title>404 Not Found</title>");
				writer->WriteLine((const UTF8Char*)"</head><body>");
				writer->WriteLine((const UTF8Char*)"<h1>Not Found</h1>");
				writer->Write((const UTF8Char*)"<p>The requested URL ");
				writer->Write(req->GetRequestURI());
				writer->WriteLine((const UTF8Char*)" was not found on this server.</p>");
				writer->WriteLine((const UTF8Char*)"</body></html>");
				DEL_CLASS(writer);

				UOSInt size;
				UInt8 *buff = mstm->GetBuff(&size);
				resp->AddContentLength(size);
				resp->AddContentType((const UTF8Char*)"text/html");
				resp->Write(buff, size);
				DEL_CLASS(mstm);
			}
		}
	}
}

void Net::WebServer::WebStandardHandler::Release()
{
	DEL_CLASS(this);
}

void Net::WebServer::WebStandardHandler::HandlePath(const UTF8Char *absolutePath, Net::WebServer::WebStandardHandler *hdlr, Bool needRelease)
{
	Net::WebServer::WebStandardHandler *subHdlr;
	if (hdlr == 0)
		return;
	if (absolutePath[0] != '/')
	{
		if (needRelease)
		{
			hdlr->Release();
		}
		return;
	}
	OSInt i = Text::StrIndexOf(&absolutePath[1], '/');
	UTF8Char *sbuff;
	if (i == -1)
	{
		this->hdlrs->Put(&absolutePath[1], hdlr);
		if (needRelease)
		{
			this->relHdlrs->Add(hdlr);
		}
	}
	else
	{
		sbuff = MemAlloc(UTF8Char, (UOSInt)i + 1);
		MemCopyNO(sbuff, &absolutePath[1], sizeof(UTF8Char) * (UOSInt)i);
		sbuff[i] = 0;

		subHdlr = this->hdlrs->Get(sbuff);
		if (subHdlr == 0)
		{
			NEW_CLASS(subHdlr, Net::WebServer::WebStandardHandler());
			this->hdlrs->Put(sbuff, subHdlr);
			this->relHdlrs->Add(subHdlr);
		}
		MemFree(sbuff);
		subHdlr->HandlePath(&absolutePath[i + 1], hdlr, needRelease);
	}
}
