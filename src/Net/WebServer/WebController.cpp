#include "Stdafx.h"
#include "MyMemory.h"
#include "Net/WebServer/WebController.h"
#include "Text/StringBuilderUTF8.h"

Net::WebServer::WebController::WebController(Text::CStringNN svcPath)
{
	this->svcPath = Text::String::New(svcPath);
}

Net::WebServer::WebController::~WebController()
{
	NN<ServiceInfo> service;
	UIntOS i = this->services.GetCount();
	while (i-- > 0)
	{
		service = this->services.GetItemNoCheck(i);
		service->svcPath->Release();
		service.Delete();
	}
	this->svcPath->Release();
}

Bool Net::WebServer::WebController::ProcessRequest(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq)
{
	Optional<ServiceInfo> service;
	NN<ServiceInfo> nnservice;
	service = this->services.GetC(subReq);
	if (service.IsNull() && (subReq.Equals(UTF8STRC("/")) || (subReq.leng == 0)))
	{
		if (service.IsNull()) service = this->services.GetC(CSTR("/Default.htm"));
		if (service.IsNull()) service = this->services.GetC(CSTR("/Default.asp"));
		if (service.IsNull()) service = this->services.GetC(CSTR("/index.htm"));
		if (service.IsNull()) service = this->services.GetC(CSTR("/index.html"));
		if (service.IsNull()) service = this->services.GetC(CSTR("/iisstart.htm"));
		if (service.IsNull()) service = this->services.GetC(CSTR("/default.aspx"));
	}
	if (service.SetTo(nnservice))
	{
		ServiceFunc func = nnservice->funcs.Get((Int32)req->GetReqMethod());
		if (func)
		{
			return func(req, resp, subReq, *this);
		}
		else
		{
			Text::StringBuilderUTF8 sb;
			UIntOS i = 0;
			UIntOS j = nnservice->funcs.GetCount();
			while (i < j)
			{
				if (i > 0)
				{
					sb.AppendC(UTF8STRC(", "));
				}
				Text::CStringNN name = Net::WebUtil::RequestMethodGetName((Net::WebUtil::RequestMethod)nnservice->funcs.GetKey(i));
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

void Net::WebServer::WebController::AddService(Text::CStringNN svcPath, Net::WebUtil::RequestMethod reqMeth, ServiceFunc func)
{
	NN<ServiceInfo> service;
	Text::StringBuilderUTF8 sb;
	sb.Append(this->svcPath);
	sb.Append(svcPath);
	if (svcPath.leng == 0 || svcPath.v[0] != '/')
		return;
	if (!this->services.GetC(sb.ToCString()).SetTo(service))
	{
		NEW_CLASSNN(service, ServiceInfo());
		service->svcPath = Text::String::New(sb.ToCString());
		this->services.PutNN(service->svcPath, service);
	}
	service->funcs.Put((Int32)reqMeth, func);
}
