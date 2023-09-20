#include "Stdafx.h"
#include "Net/WebServer/WellKnownHandler.h"
#include "Sync/MutexUsage.h"

Bool Net::WebServer::WellKnownHandler::ProcessRequest(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq)
{
	if (subReq.StartsWith(UTF8STRC("/acme-challenge/")))
	{
		Sync::MutexUsage mutUsage(this->acmeMut);
		Text::String *s = this->acmeMap.GetC(subReq.Substring(16));
		if (s)
		{
			resp->AddDefHeaders(req);
			return resp->ResponseText(s->ToCString(), CSTR("text/plain"));
		}
		return resp->ResponseError(req, Net::WebStatus::SC_NOT_FOUND);
	}
	return WebServiceHandler::ProcessRequest(req, resp, subReq);
}

Bool __stdcall Net::WebServer::WellKnownHandler::AddFunc(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, WebServiceHandler *svc)
{
	Net::WebServer::WellKnownHandler *me = (Net::WebServer::WellKnownHandler*)svc;
	NotNullPtr<Text::String> t;
	NotNullPtr<Text::String> name;
	NotNullPtr<Text::String> val;
	if (t.Set(req->GetQueryValue(CSTR("t"))) && name.Set(req->GetQueryValue(CSTR("name"))) && val.Set(req->GetQueryValue(CSTR("val"))))
	{
		if (t->Equals(UTF8STRC("acme")))
		{
			Sync::MutexUsage mutUsage(me->acmeMut);
			if (val.Set(me->acmeMap.PutNN(name, val->Clone().Ptr())))
			{
				val->Release();
			}
			resp->AddDefHeaders(req);
			return resp->ResponseText(CSTR("Acme added successfully"));
		}
	}
	return resp->ResponseError(req, Net::WebStatus::SC_FORBIDDEN);
}

Net::WebServer::WellKnownHandler::WellKnownHandler()
{
	this->AddService(CSTR("/add"), Net::WebUtil::RequestMethod::HTTP_GET, AddFunc);
}

Net::WebServer::WellKnownHandler::~WellKnownHandler()
{
	UOSInt i = this->acmeMap.GetCount();
	while (i-- > 0)
	{
		this->acmeMap.GetItem(i)->Release();
	}
}
