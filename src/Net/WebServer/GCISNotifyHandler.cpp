#include "Stdafx.h"
#include "Net/WebServer/GCISNotifyHandler.h"
#include "Text/JSONBuilder.h"

Bool __stdcall Net::WebServer::GCISNotifyHandler::NotifyFunc(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, WebServiceHandler *me)
{
	Crypto::Cert::X509Cert *cert = req->GetClientCert();
	Text::StringBuilderUTF8 sb;
	{
		Text::JSONBuilder builder(&sb, Text::JSONBuilder::OT_OBJECT);
		if (cert == 0)
		{
			builder.ObjectAddStr(CSTR("Client"), CSTR("null"));
		}
		else
		{
			Text::StringBuilderUTF8 sb2;
			cert->GetSubjectCN(&sb2);
			builder.ObjectAddStr(CSTR("Client"), sb2.ToCString());
		}
		UOSInt dataSize;
		const UInt8 *content = req->GetReqData(&dataSize);
		builder.ObjectAddStr(CSTR("Content"), Text::CString(content, dataSize));
	}
	resp->AddDefHeaders(req);
	resp->AddCacheControl(0);
	resp->AddContentType(CSTR("application/json"));
	resp->AddContentLength(sb.GetLength());
	resp->Write(sb.v, sb.leng);
	return true;
}

Net::WebServer::GCISNotifyHandler::GCISNotifyHandler(Text::CString notifyPath)
{
	this->AddService(notifyPath, Net::WebUtil::RequestMethod::HTTP_POST, NotifyFunc);
}

Net::WebServer::GCISNotifyHandler::~GCISNotifyHandler()
{
}
