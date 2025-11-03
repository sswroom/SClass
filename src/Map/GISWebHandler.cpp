#include "Stdafx.h"
#include "Map/GISWebHandler.h"

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
	return false;
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

Map::GISWebHandler::GISWebHandler()
{
	this->AddService(CSTR("/ows"), Net::WebUtil::RequestMethod::HTTP_GET, OWSFunc);
	this->AddService(CSTR("/gwc/servcie/wmts"), Net::WebUtil::RequestMethod::HTTP_GET, WMTSFunc);
	this->AddService(CSTR("/gwc/servcie/wms"), Net::WebUtil::RequestMethod::HTTP_GET, WMSCFunc);
	this->AddService(CSTR("/gwc/service/tms/1.0.0"), Net::WebUtil::RequestMethod::HTTP_GET, TMSFunc);
}

Map::GISWebHandler::~GISWebHandler()
{
	this->assets.DeleteAll();
}

UOSInt Map::GISWebHandler::AddAsset(NN<Map::MapDrawLayer> layer)
{
	return this->assets.Add(layer);
}
