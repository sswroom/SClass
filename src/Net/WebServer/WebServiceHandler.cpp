#include "Stdafx.h"
#include "MyMemory.h"
#include "Net/WebServer/WebServiceHandler.h"
#include "Text/StringBuilderUTF8.h"

Net::WebServer::WebServiceHandler::~WebServiceHandler()
{
	Net::WebServer::WebServiceHandler::ServiceInfo *service;
	UOSInt i = this->services.GetCount();
	while (i-- > 0)
	{
		service = this->services.GetItem(i);
		service->svcPath->Release();
		DEL_CLASS(service);
	}
}

Bool Net::WebServer::WebServiceHandler::ProcessRequest(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq)
{
	Net::WebServer::WebServiceHandler::ServiceInfo *service;
	service = this->services.GetC(subReq);
	if (service == 0 && (subReq.Equals(UTF8STRC("/")) || (subReq.leng == 0)))
	{
		if (service == 0) service = this->services.GetC(CSTR("/Default.htm"));
		if (service == 0) service = this->services.GetC(CSTR("/Default.asp"));
		if (service == 0) service = this->services.GetC(CSTR("/index.htm"));
		if (service == 0) service = this->services.GetC(CSTR("/index.html"));
		if (service == 0) service = this->services.GetC(CSTR("/iisstart.htm"));
		if (service == 0) service = this->services.GetC(CSTR("/default.aspx"));
	}
	if (service != 0)
	{
		if (req->GetReqMethod() == Net::WebUtil::RequestMethod::HTTP_OPTIONS)
		{
			Text::StringBuilderUTF8 sb;
			UOSInt i = 0;
			UOSInt j = service->funcs.GetCount();
			while (i < j)
			{
				if (i > 0)
				{
					sb.AppendC(UTF8STRC(", "));
				}
				Text::CString name = Net::WebUtil::RequestMethodGetName((Net::WebUtil::RequestMethod)service->funcs.GetKey(i));
				sb.AppendC(name.v, name.leng);
				i++;
			}
			return this->ResponseAllowOptions(req, resp, 86400, sb.ToCString());
		}
		ServiceFunc func = service->funcs.Get((Int32)req->GetReqMethod());
		if (func)
		{
			return func(req, resp, subReq, this);
		}
		else
		{
			Text::StringBuilderUTF8 sb;
			UOSInt i = 0;
			UOSInt j = service->funcs.GetCount();
			while (i < j)
			{
				if (i > 0)
				{
					sb.AppendC(UTF8STRC(", "));
				}
				Text::CString name = Net::WebUtil::RequestMethodGetName((Net::WebUtil::RequestMethod)service->funcs.GetKey(i));
				sb.AppendC(name.v, name.leng);
				i++;
			}
			resp->AddHeader(CSTR("Allow"), sb.ToCString());
			resp->ResponseError(req, Net::WebStatus::SC_METHOD_NOT_ALLOWED);
			return true;
		}
	}
	if (this->DoRequest(req, resp, subReq))
	{
		return true;
	}
	if (this->rootDir->leng > 0)
	{
		return this->DoFileRequest(req, resp, subReq);
	}
	return false;
}

Net::WebServer::WebServiceHandler::WebServiceHandler() : HTTPDirectoryHandler(CSTR(""), false, 0, false)
{
}

Net::WebServer::WebServiceHandler::WebServiceHandler(Text::CStringNN rootDir) : HTTPDirectoryHandler(rootDir, false, 0, false)
{
}

void Net::WebServer::WebServiceHandler::AddService(Text::CStringNN svcPath, Net::WebUtil::RequestMethod reqMeth, ServiceFunc func)
{
	Net::WebServer::WebServiceHandler::ServiceInfo *service;
	if (svcPath.leng == 0 || svcPath.v[0] != '/')
		return;
	service = this->services.GetC(svcPath);
	if (service == 0)
	{
		NEW_CLASS(service, Net::WebServer::WebServiceHandler::ServiceInfo());
		service->svcPath = Text::String::New(svcPath);
		this->services.PutNN(service->svcPath, service);
	}
	service->funcs.Put((Int32)reqMeth, func);
}
