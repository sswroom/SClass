#ifndef _SM_MAP_GISWEBHANDLER
#define _SM_MAP_GISWEBHANDLER
#include "Data/ArrayListNN.h"
#include "Data/FastStringMapNN.h"
#include "Map/MapDrawLayer.h"
#include "Net/WebServer/WebServiceHandler.h"

namespace Map
{
	class GISWebHandler : public Net::WebServer::WebServiceHandler
	{
	private:
		struct GISFeature
		{
			NN<Text::String> name;
			NN<Map::MapDrawLayer> layer;
		};

		Data::ArrayListNN<Map::MapDrawLayer> assets;
		Data::FastStringMapNN<GISFeature> features;
		NN<Math::CoordinateSystem> wgs84;

		static Bool __stdcall OWSFunc(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq, NN<WebServiceHandler> svcHdlr);
		static Bool __stdcall WFSFunc(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq, NN<WebServiceHandler> svcHdlr);
		static Bool __stdcall WMTSFunc(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq, NN<WebServiceHandler> svcHdlr);
		static Bool __stdcall WMSCFunc(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq, NN<WebServiceHandler> svcHdlr);
		static Bool __stdcall TMSFunc(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq, NN<WebServiceHandler> svcHdlr);
		Bool DoWFS(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp);
		Bool ResponseException(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN exceptionCode, Text::CStringNN locator, Text::CStringNN exceptionText);
		virtual void AddHeaders(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp);

		static void __stdcall FreeFeature(NN<GISFeature> feature);
	public:
		GISWebHandler();
		virtual ~GISWebHandler();

		UOSInt AddAsset(NN<Map::MapDrawLayer> layer);
		Bool AddFeature(Text::CStringNN featureName, UOSInt assetIndex);
	};
}
#endif
