#ifndef _SM_MAP_GISWEBHANDLER
#define _SM_MAP_GISWEBHANDLER
#include "Data/ArrayListNN.h"
#include "Map/MapDrawLayer.h"
#include "Net/WebServer/WebServiceHandler.h"

namespace Map
{
	class GISWebHandler : public Net::WebServer::WebServiceHandler
	{
	private:
		Data::ArrayListNN<Map::MapDrawLayer> assets;

		static Bool __stdcall OWSFunc(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq, NN<WebServiceHandler> svcHdlr);
		static Bool __stdcall WMTSFunc(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq, NN<WebServiceHandler> svcHdlr);
		static Bool __stdcall WMSCFunc(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq, NN<WebServiceHandler> svcHdlr);
		static Bool __stdcall TMSFunc(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN subReq, NN<WebServiceHandler> svcHdlr);
	public:
		GISWebHandler();
		virtual ~GISWebHandler();

		UOSInt AddAsset(NN<Map::MapDrawLayer> layer);
	};
}
#endif
