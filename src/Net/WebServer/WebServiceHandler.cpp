#include "Stdafx.h"
#include "MyMemory.h"
#include "Net/WebServer/WebServiceHandler.h"
#include "Text/StringBuilderUTF8.h"

Net::WebServer::WebServiceHandler::~WebServiceHandler()
{
	Data::ArrayList<Net::WebServer::WebServiceHandler::ServiceInfo *> *serviceList = this->services->GetValues();
	Net::WebServer::WebServiceHandler::ServiceInfo *service;
	UOSInt i = serviceList->GetCount();
	while (i-- > 0)
	{
		service = serviceList->GetItem(i);
		DEL_CLASS(service->funcs);
		Text::StrDelNew(service->svcPath);
		MemFree(service);
	}
	DEL_CLASS(this->services);
}

Bool Net::WebServer::WebServiceHandler::ProcessRequest(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, const UTF8Char *subReq)
{
	Net::WebServer::WebServiceHandler::ServiceInfo *service;
	service = this->services->Get(subReq);
	if (service == 0 && (Text::StrEquals(subReq, (const UTF8Char*)"/") || Text::StrEquals(subReq, (const UTF8Char*)"")))
	{
		if (service == 0) service = this->services->Get((const UTF8Char*)"/Default.htm");
		if (service == 0) service = this->services->Get((const UTF8Char*)"/Default.asp");
		if (service == 0) service = this->services->Get((const UTF8Char*)"/index.htm");
		if (service == 0) service = this->services->Get((const UTF8Char*)"/index.html");
		if (service == 0) service = this->services->Get((const UTF8Char*)"/iisstart.htm");
		if (service == 0) service = this->services->Get((const UTF8Char*)"/default.aspx");
	}
	if (service != 0)
	{

		ServiceFunc func = service->funcs->Get((Int32)req->GetReqMethod());
		if (func)
		{
			return func(req, resp, subReq, this);
		}
		else
		{
			Text::StringBuilderUTF8 sb;
			Data::ArrayList<Int32> *methods = service->funcs->GetKeys();
			UOSInt i = 0;
			UOSInt j = methods->GetCount();
			while (i < j)
			{
				if (i > 0)
				{
					sb.AppendC(UTF8STRC(", "));
				}
				sb.Append((const UTF8Char*)Net::WebServer::IWebRequest::RequestMethodGetName((Net::WebServer::IWebRequest::RequestMethod)methods->GetItem(i)));
				i++;
			}
			resp->AddHeaderC(UTF8STRC("Allow"), sb.ToString(), sb.GetLength());
			resp->ResponseError(req, Net::WebStatus::SC_METHOD_NOT_ALLOWED);
			return true;
		}
	}
	return this->DoRequest(req, resp, subReq);
}

Net::WebServer::WebServiceHandler::WebServiceHandler()
{
	NEW_CLASS(this->services, Data::StringUTF8Map<Net::WebServer::WebServiceHandler::ServiceInfo*>());
}

void Net::WebServer::WebServiceHandler::AddService(const UTF8Char *svcPath, Net::WebServer::IWebRequest::RequestMethod reqMeth, ServiceFunc func)
{
	Net::WebServer::WebServiceHandler::ServiceInfo *service;
	if (svcPath[0] != '/')
		return;
	service = this->services->Get(svcPath);
	if (service == 0)
	{
		service = MemAlloc(Net::WebServer::WebServiceHandler::ServiceInfo, 1);
		service->svcPath = Text::StrCopyNew(svcPath);
		NEW_CLASS(service->funcs, Data::Int32Map<ServiceFunc>());
		this->services->Put(svcPath, service);
	}
	service->funcs->Put((Int32)reqMeth, func);
}
