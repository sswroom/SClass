#include "Stdafx.h"
#include "MyMemory.h"
#include "Net/WebServer/WebServiceHandler.h"
#include "Text/StringBuilderUTF8.h"

Net::WebServer::WebServiceHandler::~WebServiceHandler()
{
	Net::WebServer::WebServiceHandler::ServiceInfo *service;
	UOSInt i = this->services->GetCount();
	while (i-- > 0)
	{
		service = this->services->GetItem(i);
		DEL_CLASS(service->funcs);
		service->svcPath->Release();
		MemFree(service);
	}
	DEL_CLASS(this->services);
}

Bool Net::WebServer::WebServiceHandler::ProcessRequest(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, const UTF8Char *subReq, UOSInt subReqLen)
{
	Net::WebServer::WebServiceHandler::ServiceInfo *service;
	service = this->services->GetC({subReq, subReqLen});
	if (service == 0 && (Text::StrEqualsC(subReq, subReqLen, UTF8STRC("/")) || (subReqLen == 0)))
	{
		if (service == 0) service = this->services->GetC(CSTR("/Default.htm"));
		if (service == 0) service = this->services->GetC(CSTR("/Default.asp"));
		if (service == 0) service = this->services->GetC(CSTR("/index.htm"));
		if (service == 0) service = this->services->GetC(CSTR("/index.html"));
		if (service == 0) service = this->services->GetC(CSTR("/iisstart.htm"));
		if (service == 0) service = this->services->GetC(CSTR("/default.aspx"));
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
				Text::CString name = Net::WebUtil::RequestMethodGetName((Net::WebUtil::RequestMethod)methods->GetItem(i));
				sb.AppendC(name.v, name.leng);
				i++;
			}
			resp->AddHeaderC(UTF8STRC("Allow"), sb.ToString(), sb.GetLength());
			resp->ResponseError(req, Net::WebStatus::SC_METHOD_NOT_ALLOWED);
			return true;
		}
	}
	return this->DoRequest(req, resp, subReq, subReqLen);
}

Net::WebServer::WebServiceHandler::WebServiceHandler()
{
	NEW_CLASS(this->services, Data::FastStringMap<Net::WebServer::WebServiceHandler::ServiceInfo*>());
}

void Net::WebServer::WebServiceHandler::AddService(const UTF8Char *svcPath, UOSInt svcPathLen, Net::WebUtil::RequestMethod reqMeth, ServiceFunc func)
{
	Net::WebServer::WebServiceHandler::ServiceInfo *service;
	if (svcPath[0] != '/')
		return;
	service = this->services->GetC({svcPath, svcPathLen});
	if (service == 0)
	{
		service = MemAlloc(Net::WebServer::WebServiceHandler::ServiceInfo, 1);
		service->svcPath = Text::String::New(svcPath, svcPathLen);
		NEW_CLASS(service->funcs, Data::Int32Map<ServiceFunc>());
		this->services->Put(service->svcPath, service);
	}
	service->funcs->Put((Int32)reqMeth, func);
}
