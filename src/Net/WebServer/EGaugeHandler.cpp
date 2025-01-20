#include "Stdafx.h"
#include "MyMemory.h"
#include "Net/WebServer/EGaugeHandler.h"

Net::WebServer::EGaugeHandler::EGaugeHandler()
{
	this->dataHdlr = 0;
	this->dataHdlrObj = 0;
}

Net::WebServer::EGaugeHandler::~EGaugeHandler()
{
}

Bool Net::WebServer::EGaugeHandler::ProcessRequest(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq)
{
	if (this->DoRequest(req, resp, subReq))
	{
		return true;
	}

	if (req->GetProtocol() != Net::WebServer::WebRequest::RequestProtocol::HTTP1_0 && req->GetProtocol() != Net::WebServer::WebRequest::RequestProtocol::HTTP1_1)
	{
		resp->ResponseError(req, Net::WebStatus::SC_METHOD_NOT_ALLOWED);
		return true;
	}
	if (req->GetReqMethod() != Net::WebUtil::RequestMethod::HTTP_POST)
	{
		resp->ResponseError(req, Net::WebStatus::SC_NOT_ACCEPTABLE);
		return true;
	}
	UOSInt buffSize;
	UnsafeArray<const UTF8Char> buff = req->GetReqData(buffSize).Or((const UTF8Char*)&buffSize);
	resp->SetStatusCode(Net::WebStatus::SC_OK);
	resp->AddContentLength(2);
	resp->AddContentType(CSTR("text/plain"));
	resp->Write(CSTR("ok").ToByteArray());
	
	if (this->dataHdlr)
	{
		this->dataHdlr(this->dataHdlrObj, buff, buffSize);
	}
	return true;
}

void Net::WebServer::EGaugeHandler::HandleEGaugeData(DataHandler dataHdlr, AnyType userObj)
{
	this->dataHdlrObj = userObj;
	this->dataHdlr = dataHdlr;
}
