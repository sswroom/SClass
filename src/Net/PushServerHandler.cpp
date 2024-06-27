#include "Stdafx.h"
#include "Net/PushServerHandler.h"
#include "Text/Builder/HTMLDocumentBuilder.h"
#include "Text/JSONBuilder.h"

Bool __stdcall Net::PushServerHandler::SendHandler(NN<Net::WebServer::IWebRequest> req, NN<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, NN<WebServiceHandler> svc)
{
	NN<Net::PushServerHandler> me = NN<Net::PushServerHandler>::ConvertFrom(svc);
	UOSInt dataLen;
	UnsafeArray<const UInt8> data = req->GetReqData(dataLen).Or((const UInt8*)&dataLen);
	NN<Text::JSONBase> json;
	if (Text::JSONBase::ParseJSONBytes(data, dataLen).SetTo(json))
	{
		me->ParseJSONSend(json);
		json->EndUse();
	}
	me->AddResponseHeaders(req, resp);
	resp->AddCacheControl(0);
	resp->AddContentLength(0);
	resp->Write(Data::ByteArrayR(U8STR(""), 0));
	return true;
}

Bool __stdcall Net::PushServerHandler::SendBatchHandler(NN<Net::WebServer::IWebRequest> req, NN<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, NN<WebServiceHandler> svc)
{
	NN<Net::PushServerHandler> me = NN<Net::PushServerHandler>::ConvertFrom(svc);
	UOSInt dataLen;
	UnsafeArray<const UInt8> data = req->GetReqData(dataLen).Or((const UInt8*)&dataLen);
	NN<Text::JSONBase> json;
	if (Text::JSONBase::ParseJSONBytes(data, dataLen).SetTo(json))
	{
		NN<Text::JSONBase> notifBase;
		if (json->GetValue(CSTR("notifications")).SetTo(notifBase) && notifBase->GetType() == Text::JSONType::Array)
		{
			NN<Text::JSONArray> notifArr = NN<Text::JSONArray>::ConvertFrom(notifBase);
			NN<Text::JSONBase> o;
			UOSInt i = 0;
			UOSInt j = notifArr->GetArrayLength();
			while (i < j)
			{
				if (notifArr->GetArrayValue(i).SetTo(o))
					me->ParseJSONSend(o);
				i++;
			}
		}
		json->EndUse();
	}
	me->AddResponseHeaders(req, resp);
	resp->AddCacheControl(0);
	resp->AddContentLength(0);
	resp->Write(Data::ByteArrayR(U8STR(""), 0));
	return true;
}

Bool __stdcall Net::PushServerHandler::SubscribeHandler(NN<Net::WebServer::IWebRequest> req, NN<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, NN<WebServiceHandler> svc)
{
	NN<Net::PushServerHandler> me = NN<Net::PushServerHandler>::ConvertFrom(svc);
	UOSInt dataLen;
	UnsafeArray<const UInt8> data = req->GetReqData(dataLen).Or((const UInt8*)&dataLen);
	NN<Text::JSONBase> json;
	if (Text::JSONBase::ParseJSONBytes(data, dataLen).SetTo(json))
	{
		NN<Text::String> sToken;
		NN<Text::String> sType;
		NN<Text::String> sUser;
		if (!json->GetValueString(CSTR("token")).SetTo(sToken))
		{
			me->mgr->LogMessage(CSTR("Token Missing"), IO::LogHandler::LogLevel::Error);
		}
		else if (!json->GetValueString(CSTR("type")).SetTo(sType))
		{
			me->mgr->LogMessage(CSTR("Type Missing"), IO::LogHandler::LogLevel::Error);
		}
		else if (!json->GetValueString(CSTR("user")).SetTo(sUser))
		{
			me->mgr->LogMessage(CSTR("User Missing"), IO::LogHandler::LogLevel::Error);
		}
		else
		{
			sUser->Trim();
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
				Text::StringBuilderUTF8 sb;
				sb.AppendC(UTF8STRC("Unknown type: "));
				sb.Append(sType);
				me->mgr->LogMessage(sb.ToCString(), IO::LogHandler::LogLevel::Error);
				succ = false;
			}
			if (succ)
			{
				me->mgr->Subscribe(sToken->ToCString(), sUser->ToCString(), devType, req->GetClientAddr(), req->GetDevModel());
			}
		}
		json->EndUse();
	}
	else
	{
		me->mgr->LogMessage(CSTR("Not JSON"), IO::LogHandler::LogLevel::Error);
	}
	me->AddResponseHeaders(req, resp);
	resp->AddCacheControl(0);
	resp->AddContentLength(0);
	resp->Write(Data::ByteArrayR(U8STR(""), 0));
	return true;
}

Bool __stdcall Net::PushServerHandler::UnsubscribeHandler(NN<Net::WebServer::IWebRequest> req, NN<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, NN<WebServiceHandler> svc)
{
	NN<Net::PushServerHandler> me = NN<Net::PushServerHandler>::ConvertFrom(svc);
	UOSInt dataLen;
	UnsafeArray<const UInt8> data = req->GetReqData(dataLen).Or((const UInt8*)&dataLen);
	NN<Text::JSONBase> json;
	if (Text::JSONBase::ParseJSONBytes(data, dataLen).SetTo(json))
	{
		NN<Text::String> token;
	//	Bool succ = false;
		if (json->GetValueString(CSTR("token")).SetTo(token) && me->mgr->Unsubscribe(token->ToCString()))
		{
	//		succ = true;
		}
		json->EndUse();
	}
	me->AddResponseHeaders(req, resp);
	resp->AddCacheControl(0);
	resp->AddContentLength(0);
	resp->Write(Data::ByteArrayR(U8STR(""), 0));
	return true;
}

Bool __stdcall Net::PushServerHandler::UsersHandler(NN<Net::WebServer::IWebRequest> req, NN<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, NN<WebServiceHandler> svc)
{
	NN<Net::PushServerHandler> me = NN<Net::PushServerHandler>::ConvertFrom(svc);
	Text::JSONBuilder json(Text::JSONBuilder::OT_OBJECT);
	json.ObjectBeginArray(CSTR("users"));
	Sync::MutexUsage mutUsage;
	Data::ArrayListStringNN userList;
	me->mgr->GetUsers(userList, mutUsage);
	Data::ArrayIterator<NN<Text::String>> it = userList.Iterator();
	while (it.HasNext())
	{
		json.ArrayAddStr(it.Next());
	}
	json.ArrayEnd();
	json.ObjectEnd();
	return me->ResponseJSONStr(req, resp, 0, json.Build());
}

Bool __stdcall Net::PushServerHandler::ListDevicesHandler(NN<Net::WebServer::IWebRequest> req, NN<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, NN<WebServiceHandler> svc)
{
	NN<Net::PushServerHandler> me = NN<Net::PushServerHandler>::ConvertFrom(svc);
	Text::Builder::HTMLDocumentBuilder builder(Text::Builder::HTMLDocumentBuilder::DocType::HTML5, CSTR("Devices"));
	Text::Builder::HTMLBodyBuilder *body = builder.StartBody(CSTR_NULL);
	body->BeginTable();
	body->BeginTableRow();
	body->AddTableHeader(CSTR("UserName"));
	body->AddTableHeader(CSTR("DevModel"));
	body->AddTableHeader(CSTR("Address"));
	body->AddTableHeader(CSTR("Time"));
	body->AddTableHeader(CSTR("DevType"));
	body->EndElement();
	{
		UTF8Char sbuff[128];
		UnsafeArray<UTF8Char> sptr;
		Sync::MutexUsage mutUsage;
		NN<const Data::ReadingListNN<Net::PushManager::DeviceInfo2>> devList = me->mgr->GetDevices(mutUsage);
		NN<Net::PushManager::DeviceInfo2> dev;
		UOSInt i = 0;
		UOSInt j = devList->GetCount();
		while (i < j)
		{
			dev = devList->GetItemNoCheck(i);
			body->BeginTableRow();
			body->AddTableData(dev->userName->ToCString());
			body->AddTableData(STR_CSTR(dev->devModel).OrEmpty());
			sptr = Net::SocketUtil::GetAddrName(sbuff, dev->subscribeAddr).Or(sbuff);
			body->AddTableData(CSTRP(sbuff, sptr));
			sptr = dev->lastSubscribeTime.ToString(sbuff);
			body->AddTableData(CSTRP(sbuff, sptr));
			body->AddTableData(Net::PushManager::DeviceTypeGetName(dev->devType));
			body->EndElement();
			body->BeginTableRowPixelHeight(50);
			body->AddTableData(dev->token->ToCString(), 5, 1, Text::HAlignment::Unknown, Text::VAlignment::Top);
			body->EndElement();
			i++;
		}
	}
	body->EndElement();
	Text::CStringNN content = builder.Build();

	resp->AddDefHeaders(req);
	resp->AddCacheControl(0);
	resp->AddContentType(CSTR("text/html"));
	resp->AddContentLength(content.leng);
	resp->Write(content.ToByteArray());
	return true;
}

void Net::PushServerHandler::ParseJSONSend(NN<Text::JSONBase> sendJson)
{
	NN<Text::JSONBase> usersBase;
	NN<Text::JSONBase> androidBase;
	Bool succ = false;
	if (sendJson->GetValue(CSTR("users")).SetTo(usersBase) && sendJson->GetValue(CSTR("android")).SetTo(androidBase) && usersBase->GetType() == Text::JSONType::Array)
	{
		Optional<Text::String> message = androidBase->GetValueString(CSTR("data.message"));
		Data::ArrayListStringNN userList;
		NN<Text::JSONArray> usersArr = NN<Text::JSONArray>::ConvertFrom(usersBase);
		succ = true;
		UOSInt i = 0;
		UOSInt j = usersArr->GetArrayLength();
		NN<Text::String> s;
		while (i < j)
		{
			if (usersArr->GetArrayString(i).SetTo(s))
			{
				userList.Add(s);
			}
			else
			{
				succ = false;
			}
			i++;
		}
		if (succ && message.SetTo(s) && userList.GetCount() > 0)
		{
			this->mgr->Send(userList, s);
		}
	}
}

Net::PushServerHandler::PushServerHandler(NN<PushManager> mgr)
{
	this->mgr = mgr;
	
	this->AddService(CSTR("/send"), Net::WebUtil::RequestMethod::HTTP_POST, SendHandler);
	this->AddService(CSTR("/sendBatch"), Net::WebUtil::RequestMethod::HTTP_POST, SendBatchHandler);
	this->AddService(CSTR("/subscribe"), Net::WebUtil::RequestMethod::HTTP_POST, SubscribeHandler);
	this->AddService(CSTR("/unsubscribe"), Net::WebUtil::RequestMethod::HTTP_POST, UnsubscribeHandler);
	this->AddService(CSTR("/users"), Net::WebUtil::RequestMethod::HTTP_GET, UsersHandler);
	this->AddService(CSTR("/listDevices"), Net::WebUtil::RequestMethod::HTTP_GET, ListDevicesHandler);
}

Net::PushServerHandler::~PushServerHandler()
{

}
