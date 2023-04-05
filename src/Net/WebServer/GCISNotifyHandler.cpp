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
			resp->SetStatusCode(Net::WebStatus::SC_INTERNAL_SERVER_ERROR);
			builder.ObjectAddStr(CSTR("description"), CSTR("Client Cert is missing"));
			builder.ObjectAddStr(CSTR("code"), CSTR("9999"));
		}
		else
		{
			Bool failed = false;
			UOSInt dataSize;
			const UInt8 *content = req->GetReqData(&dataSize);
			Text::JSONBase *json = Text::JSONBase::ParseJSONBytes(content, dataSize);
			Text::CString resultCd = CSTR("0000");
			Text::CString resultMsg = CSTR("Request processed successfully.");
			if (json == 0)
			{
				failed = true;
				resultCd = CSTR("0021");
				resultMsg = CSTR("Invalid content type");
			}
			else if (json->GetType() != Text::JSONType::Object)
			{
				failed = true;
				resultCd = CSTR("0021");
				resultMsg = CSTR("Invalid content type");
			}
			else
			{
				//////////////////////////////
			}

			builder.ObjectAddStr(CSTR("Status"), failed?CSTR("F"):CSTR("S"));
			builder.ObjectBeginObject(CSTR("NotificationStatus"));
			if (json)
			{
				if (json->GetType() == Text::JSONType::Object)
					builder.ObjectAdd((Text::JSONObject*)json);
				json->EndUse();
			}
			if (!failed)
			{
				Text::StringBuilderUTF8 sb2;
				cert->GetSubjectCN(&sb2);
				builder.ObjectAddStr(CSTR("Client"), sb2.ToCString());

				builder.ObjectAddStr(CSTR("ResultMesg"), resultMsg);
				builder.ObjectAddStr(CSTR("ResultCd"), resultCd);
			}
		}
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
