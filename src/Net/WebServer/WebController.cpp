#include "Stdafx.h"
#include "MyMemory.h"
#include "Net/WebServer/WebController.h"
#include "Text/StringBuilderUTF8.h"

Net::WebServer::WebController::WebController(Text::CString svcPath)
{
	this->svcPath = Text::String::New(svcPath);
}

Net::WebServer::WebController::~WebController()
{
	ServiceInfo *service;
	UOSInt i = this->services.GetCount();
	while (i-- > 0)
	{
		service = this->services.GetItem(i);
		service->svcPath->Release();
		DEL_CLASS(service);
	}
	this->svcPath->Release();
}

Bool Net::WebServer::WebController::ProcessRequest(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq)
{
	ServiceInfo *service;
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
	return false;
}

void Net::WebServer::WebController::AddService(Text::CString svcPath, Net::WebUtil::RequestMethod reqMeth, ServiceFunc func)
{
	ServiceInfo *service;
	Text::StringBuilderUTF8 sb;
	sb.Append(this->svcPath);
	sb.Append(svcPath);
	if (svcPath.leng == 0 || svcPath.v[0] != '/')
		return;
	service = this->services.GetC(sb.ToCString());
	if (service == 0)
	{
		NEW_CLASS(service, ServiceInfo());
		service->svcPath = Text::String::New(sb.ToCString());
		this->services.Put(service->svcPath, service);
	}
	service->funcs.Put((Int32)reqMeth, func);
}
