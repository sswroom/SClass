#include "Stdafx.h"
#include "Net/PushServerHandler.h"
#include "Text/JSONBuilder.h"

Bool __stdcall Net::PushServerHandler::SendHandler(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, WebServiceHandler *svc)
{
	Net::PushServerHandler *me = (Net::PushServerHandler*)svc;
	UOSInt dataLen;
	const UInt8 *data = req->GetReqData(&dataLen);
	Text::JSONBase *json = Text::JSONBase::ParseJSONBytes(data, dataLen);
	if (json)
	{
		me->ParseJSONSend(json);
		json->EndUse();
	}
	resp->AddDefHeaders(req);
	resp->AddCacheControl(0);
	resp->AddContentLength(0);
	resp->Write(0, 0);
	return true;
}

Bool __stdcall Net::PushServerHandler::SendBatchHandler(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, WebServiceHandler *svc)
{
	Net::PushServerHandler *me = (Net::PushServerHandler*)svc;
	UOSInt dataLen;
	const UInt8 *data = req->GetReqData(&dataLen);
	Text::JSONBase *json = Text::JSONBase::ParseJSONBytes(data, dataLen);
	if (json)
	{
		Text::JSONBase *notifBase = json->GetValue(CSTR("notifications"));
		if (notifBase && notifBase->GetType() == Text::JSONType::Array)
		{
			Text::JSONArray *notifArr = (Text::JSONArray*)notifBase;
			UOSInt i = 0;
			UOSInt j = notifArr->GetArrayLength();
			while (i < j)
			{
				me->ParseJSONSend(notifArr->GetArrayValue(i));
				i++;
			}
		}
		json->EndUse();
	}
	resp->AddDefHeaders(req);
	resp->AddCacheControl(0);
	resp->AddContentLength(0);
	resp->Write(0, 0);
	return true;
}

Bool __stdcall Net::PushServerHandler::SubscribeHandler(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, WebServiceHandler *svc)
{
	Net::PushServerHandler *me = (Net::PushServerHandler*)svc;
	UOSInt dataLen;
	const UInt8 *data = req->GetReqData(&dataLen);
	Text::JSONBase *json = Text::JSONBase::ParseJSONBytes(data, dataLen);
	if (json)
	{
		Text::String *sToken = json->GetValueString(CSTR("token"));
		Text::String *sType = json->GetValueString(CSTR("type"));
		Text::String *sUser = json->GetValueString(CSTR("user"));
		if (sToken && sType && sUser)
		{
			Bool succ = true;
			Net::PushManager::DeviceType devType = PushManager::DeviceType::Android;
			if (sType->Equals(UTF8STRC("android")))
			{
				devType = PushManager::DeviceType::Android;
			}
			else if (sType->Equals(UTF8STRC("ios")))
			{
				devType = PushManager::DeviceType::IOS;
			}
			else
			{
				succ = false;
			}
			if (succ)
			{
				me->mgr->Subscribe(sToken->ToCString(), sUser->ToCString(), devType);
			}
		}
		json->EndUse();
	}
	resp->AddDefHeaders(req);
	resp->AddCacheControl(0);
	resp->AddContentLength(0);
	resp->Write(0, 0);
	return true;
}

Bool __stdcall Net::PushServerHandler::UnsubscribeHandler(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, WebServiceHandler *svc)
{
	Net::PushServerHandler *me = (Net::PushServerHandler*)svc;
	UOSInt dataLen;
	const UInt8 *data = req->GetReqData(&dataLen);
	Text::JSONBase *json = Text::JSONBase::ParseJSONBytes(data, dataLen);
	if (json)
	{
		Text::String *token = json->GetValueString(CSTR("token"));
	//	Bool succ = false;
		if (token && me->mgr->Unsubscribe(token->ToCString()))
		{
	//		succ = true;
		}
		json->EndUse();
	}
	resp->AddDefHeaders(req);
	resp->AddCacheControl(0);
	resp->AddContentLength(0);
	resp->Write(0, 0);
	return true;
}

Bool __stdcall Net::PushServerHandler::UsersHandler(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, WebServiceHandler *svc)
{
	Net::PushServerHandler *me = (Net::PushServerHandler*)svc;
	Text::StringBuilderUTF8 sb;
	{
		Text::JSONBuilder json(&sb, Text::JSONBuilder::OT_OBJECT);
		json.ObjectBeginArray(CSTR("users"));
		Sync::MutexUsage mutUsage;
		Data::ArrayList<Text::String*> userList;
		UOSInt i = 0;
		UOSInt j = me->mgr->GetUsers(&userList, &mutUsage);
		while (i < j)
		{
			json.ArrayAddStr(userList.GetItem(i));
			i++;
		}
		json.ArrayEnd();
		json.ObjectEnd();
	}

	resp->AddDefHeaders(req);
	resp->AddCacheControl(0);
	resp->AddContentType(CSTR("application/json"));
	resp->AddContentLength(sb.leng);
	resp->Write(sb.v, sb.leng);
	return true;
}

void Net::PushServerHandler::ParseJSONSend(Text::JSONBase *sendJson)
{
	Text::JSONBase *usersBase = sendJson->GetValue(CSTR("users"));
	Text::JSONBase *androidBase = sendJson->GetValue(CSTR("android"));
	Bool succ = false;
	if (usersBase && androidBase && usersBase->GetType() == Text::JSONType::Array)
	{
		Text::String *message = androidBase->GetValueString(CSTR("data.message"));
		Data::ArrayList<Text::String*> userList;
		Text::JSONArray *usersArr = (Text::JSONArray*)usersBase;
		succ = true;
		UOSInt i = 0;
		UOSInt j = usersArr->GetArrayLength();
		Text::String *s;
		while (i < j)
		{
			s = usersArr->GetArrayString(i);
			if (s == 0)
			{
				succ = false;
			}
			else
			{
				userList.Add(s);
			}
			i++;
		}
		if (succ && message && userList.GetCount() > 0)
		{
			this->mgr->Send(&userList, message);
		}
	}
}

Net::PushServerHandler::PushServerHandler(PushManager *mgr)
{
	this->mgr = mgr;
	
	this->AddService(CSTR("/send"), Net::WebUtil::RequestMethod::HTTP_POST, SendHandler);
	this->AddService(CSTR("/sendBatch"), Net::WebUtil::RequestMethod::HTTP_POST, SendBatchHandler);
	this->AddService(CSTR("/subscribe"), Net::WebUtil::RequestMethod::HTTP_POST, SubscribeHandler);
	this->AddService(CSTR("/unsubscribe"), Net::WebUtil::RequestMethod::HTTP_POST, UnsubscribeHandler);
	this->AddService(CSTR("/users"), Net::WebUtil::RequestMethod::HTTP_GET, UsersHandler);
}

Net::PushServerHandler::~PushServerHandler()
{

}
