#include "Stdafx.h"
#include "IO/Path.h"
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
			this->AddResponseHeaders(req, resp);
			return resp->ResponseText(s->ToCString(), CSTR("text/plain"));
		}
		return resp->ResponseError(req, Net::WebStatus::SC_NOT_FOUND);
	}
	return WebServiceHandler::ProcessRequest(req, resp, subReq);
}

Bool __stdcall Net::WebServer::WellKnownHandler::AddFunc(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, NotNullPtr<WebServiceHandler> svc)
{
	NotNullPtr<Net::WebServer::WellKnownHandler> me = NotNullPtr<Net::WebServer::WellKnownHandler>::ConvertFrom(svc);
	NotNullPtr<Text::String> t;
	NotNullPtr<Text::String> name;
	NotNullPtr<Text::String> val;
	if (req->GetQueryValue(CSTR("t")).SetTo(t) && req->GetQueryValue(CSTR("name")).SetTo(name) && req->GetQueryValue(CSTR("val")).SetTo(val))
	{
		if (t->Equals(UTF8STRC("acme")))
		{
			Sync::MutexUsage mutUsage(me->acmeMut);
			if (val.Set(me->acmeMap.PutNN(name, val->Clone().Ptr())))
			{
				val->Release();
			}
			me->AddResponseHeaders(req, resp);
			return resp->ResponseText(CSTR("Acme added successfully"));
		}
	}
	return resp->ResponseError(req, Net::WebStatus::SC_FORBIDDEN);
}

Net::WebServer::WellKnownHandler::WellKnownHandler()
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	sptr = IO::Path::GetProcessFileName(sbuff);
	sptr = IO::Path::AppendPath(sbuff, sptr, CSTR(".well-known"));
	this->SetRootDir(CSTRP(sbuff, sptr));
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
