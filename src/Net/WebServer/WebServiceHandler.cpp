#include "Stdafx.h"
#include "MyMemory.h"
#include "Net/WebServer/WebServiceHandler.h"
#include "Text/StringBuilderUTF8.h"

Net::WebServer::WebServiceHandler::~WebServiceHandler()
{
	NN<Net::WebServer::WebServiceHandler::ServiceInfo> service;
	UIntOS i = this->services.GetCount();
	while (i-- > 0)
	{
		service = this->services.GetItemNoCheck(i);
		service->svcPath->Release();
		service.Delete();
	}
}

Bool Net::WebServer::WebServiceHandler::ProcessRequest(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq)
{
	Optional<Net::WebServer::WebServiceHandler::ServiceInfo> service;
	NN<Net::WebServer::WebServiceHandler::ServiceInfo> nnservice;
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
		if (req->GetReqMethod() == Net::WebUtil::RequestMethod::HTTP_OPTIONS)
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
			return this->ResponseAllowOptions(req, resp, 86400, sb.ToCString());
		}
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
	if (this->DoRequest(req, resp, subReq))
	{
		return true;
	}
	if (this->rootDir->leng > 0)
	{
		return this->DoFileRequest(req, resp, subReq);
	}
	else
	{
		return this->DoPackageRequest(req, resp, subReq);
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
	NN<Net::WebServer::WebServiceHandler::ServiceInfo> service;
	if (svcPath.leng == 0 || svcPath.v[0] != '/')
		return;
	if (!this->services.GetC(svcPath).SetTo(service))
	{
		NEW_CLASSNN(service, Net::WebServer::WebServiceHandler::ServiceInfo());
		service->svcPath = Text::String::New(svcPath);
		this->services.PutNN(service->svcPath, service);
	}
	service->funcs.Put((Int32)reqMeth, func);
}
