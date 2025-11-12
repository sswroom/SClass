#ifndef _SM_MAP_GISWEBHANDLER
#define _SM_MAP_GISWEBHANDLER
#include "Data/ArrayListNN.h"
#include "Data/FastStringMapNN.h"
#include "Map/GISWebService.h"
#include "Map/MapDrawLayer.h"
#include "Map/WFSHandler.h"
#include "Net/WebServer/WebServiceHandler.h"

namespace Map
{
	class GISWebHandler : public Net::WebServer::WebServiceHandler, public GISWebService
	{
	private:
		Data::ArrayListNN<Map::MapDrawLayer> assets;
		Data::FastStringMapNN<WFSHandler::GISFeature> features;
		Data::FastStringMapNN<GISWebService::GISWorkspace> ws;
		WFSHandler wfs;

		static Bool __stdcall OWSFunc(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq, NN<WebServiceHandler> svcHdlr);
		static Bool __stdcall WFSFunc(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq, NN<WebServiceHandler> svcHdlr);
		static Bool __stdcall WMTSFunc(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq, NN<WebServiceHandler> svcHdlr);
		static Bool __stdcall WMSCFunc(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq, NN<WebServiceHandler> svcHdlr);
		static Bool __stdcall TMSFunc(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq, NN<WebServiceHandler> svcHdlr);
		virtual Bool ResponseException(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN exceptionCode, Text::CStringNN locator, Text::CStringNN exceptionText);
		virtual void AddRespHeaders(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp);
		static void __stdcall FreeGISWorkspace(NN<GISWebService::GISWorkspace> ws);
	public:
		GISWebHandler();
		virtual ~GISWebHandler();

		UOSInt AddAsset(NN<Map::MapDrawLayer> layer);
		Optional<GISWebService::GISWorkspace> AddWorkspace(Text::CStringNN name, Text::CStringNN uri);
		Bool AddFeature(Text::CStringNN featureName, NN<GISWebService::GISWorkspace> ws, UOSInt assetIndex);
	};
}
#endif
