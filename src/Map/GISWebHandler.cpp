#include "Stdafx.h"
#include "Map/GISWebHandler.h"
#include "Net/WebServer/HTTPServerUtil.h"
#include "Text/XML.h"

// http://192.168.1.25:8080/geoserver/ows?service=WMS&version=1.1.1&request=GetCapabilities
// http://192.168.1.25:8080/geoserver/ows?service=WMS&version=1.3.0&request=GetCapabilities
// http://192.168.1.25:8080/geoserver/gwc/service/wmts?service=WMTS&version=1.1.1&request=GetCapabilities
// http://192.168.1.25:8080/geoserver/gwc/service/wms?service=WMS&version=1.1.1&request=GetCapabilities&tiled=true
// http://192.168.1.25:8080/geoserver/gwc/service/tms/1.0.0
// http://192.168.1.25:8080/geoserver/ows?service=WFS&version=1.0.0&request=GetCapabilities
// http://192.168.1.25:8080/geoserver/ows?service=WFS&version=1.1.0&request=GetCapabilities
// http://192.168.1.25:8080/geoserver/ows?service=WFS&acceptversions=2.0.0&request=GetCapabilities

Bool __stdcall Map::GISWebHandler::OWSFunc(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq, NN<WebServiceHandler> svcHdlr)
{
	NN<Map::GISWebHandler> me = NN<Map::GISWebHandler>::ConvertFrom(svcHdlr);
	NN<Text::String> service;
	if (!req->GetQueryValue(CSTR("service")).SetTo(service))
	{
		return me->ResponseException(req, resp, CSTR("InvalidParameterValue"), CSTR("service"), CSTR("No service: ( ows )"));
	}
	if (service->EqualsICase(CSTR("WFS")))
	{
		return me->wfs.DoWFS(req, resp, me->features, me);
	}
	else if (service->EqualsICase(CSTR("WMS")))
	{

	}
	else
	{
		return me->ResponseException(req, resp, CSTR("InvalidParameterValue"), CSTR("service"), (Text::StringBuilderUTF8() + CSTR("No service: ( ") + service + CSTR(" )")).ToCString());
	}
	return false;
}

Bool __stdcall Map::GISWebHandler::WFSFunc(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq, NN<WebServiceHandler> svcHdlr)
{
	NN<Map::GISWebHandler> me = NN<Map::GISWebHandler>::ConvertFrom(svcHdlr);
	return me->wfs.DoWFS(req, resp, me->features, me);
}

Bool __stdcall Map::GISWebHandler::WMTSFunc(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq, NN<WebServiceHandler> svcHdlr)
{
	return false;
}

Bool __stdcall Map::GISWebHandler::WMSCFunc(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq, NN<WebServiceHandler> svcHdlr)
{
	return false;
}

Bool __stdcall Map::GISWebHandler::TMSFunc(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq, NN<WebServiceHandler> svcHdlr)
{
	return false;
}

Bool Map::GISWebHandler::ResponseException(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN exceptionCode, Text::CStringNN locator, Text::CStringNN exceptionText)
{
	NN<Text::String> s;
	Text::StringBuilderUTF8 sb;
	sb.Append(CSTR("<?xml version=\"1.0\" encoding=\"UTF-8\"?>"));
	sb.Append(CSTR("<ows:ExceptionReport xmlns:xs=\"http://www.w3.org/2001/XMLSchema\" xmlns:ows=\"http://www.opengis.net/ows\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" version=\"1.0.0\">\r\n"));
	sb.Append(CSTR("<ows:Exception exceptionCode="));
	s = Text::XML::ToNewAttrText(exceptionCode.v);
	sb.Append(s);
	s->Release();
	sb.Append(CSTR(" locator="));
	s = Text::XML::ToNewAttrText(locator.v);
	sb.Append(s);
	s->Release();
	sb.Append(CSTR(">\r\n"));
	sb.Append(CSTR("<ows:ExceptionText>"));
	s = Text::XML::ToNewXMLText(exceptionText.v);
	sb.Append(s);
	s->Release();
	sb.Append(CSTR("</ows:ExceptionText>\r\n"));
	sb.Append(CSTR("</ows:Exception>\r\n"));
	sb.Append(CSTR("</ows:ExceptionReport>\r\n"));
	resp->AddDefHeaders(req);
	resp->AddContentType(CSTR("application/xml"));
	this->AddRespHeaders(req, resp);
	return Net::WebServer::HTTPServerUtil::SendContent(req, resp, CSTR("application/xml"), sb.ToCString());

}

void Map::GISWebHandler::AddRespHeaders(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp)
{
}

Map::GISWebHandler::GISWebHandler()
{
	this->AddService(CSTR("/ows"), Net::WebUtil::RequestMethod::HTTP_GET, OWSFunc);
	this->AddService(CSTR("/wfs"), Net::WebUtil::RequestMethod::HTTP_GET, WFSFunc);
	this->AddService(CSTR("/gwc/servcie/wmts"), Net::WebUtil::RequestMethod::HTTP_GET, WMTSFunc);
	this->AddService(CSTR("/gwc/servcie/wms"), Net::WebUtil::RequestMethod::HTTP_GET, WMSCFunc);
	this->AddService(CSTR("/gwc/service/tms/1.0.0"), Net::WebUtil::RequestMethod::HTTP_GET, TMSFunc);
}

Map::GISWebHandler::~GISWebHandler()
{
	this->assets.DeleteAll();
	this->features.FreeAll(WFSHandler::FreeFeature);
}

UOSInt Map::GISWebHandler::AddAsset(NN<Map::MapDrawLayer> layer)
{
	return this->assets.Add(layer);
}

Bool Map::GISWebHandler::AddFeature(Text::CStringNN featureName, UOSInt assetIndex)
{
	NN<Map::MapDrawLayer> layer;
	if (!this->assets.GetItem(assetIndex).SetTo(layer))
	{
		return false;
	}
	if (this->features.GetC(featureName).NotNull())
	{
		return false;
	}
	NN<WFSHandler::GISFeature> feature = MemAllocNN(WFSHandler::GISFeature);
	feature->name = Text::String::New(featureName);
	feature->layer = layer;
	this->features.PutC(featureName, feature);
	return true;
}
