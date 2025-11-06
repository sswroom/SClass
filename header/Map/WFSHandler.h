#ifndef _SM_MAP_WFSHANDLER
#define _SM_MAP_WFSHANDLER
#include "Map/GISWebService.h"
#include "Map/MapDrawLayer.h"
#include "Net/WebServer/WebServiceHandler.h"

namespace Map
{
	class WFSHandler
	{
	public:
		struct GISFeature
		{
			NN<GISWebService::GISWorkspace> ws;
			NN<Text::String> name;
			NN<Map::MapDrawLayer> layer;
		};

	private:
		typedef Bool (CALLBACKFUNC RequestFunc)(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, NN<WFSHandler> me, Text::CStringNN version, NN<Data::FastStringMapNN<GISFeature>> features, NN<GISWebService> svc);
		NN<Math::CoordinateSystem> wgs84;
		Data::FastStringMap<RequestFunc> reqMap;

		static Bool GetCapabilities(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, NN<WFSHandler> me, Text::CStringNN version, NN<Data::FastStringMapNN<GISFeature>> features, NN<GISWebService> svc);
		static Bool DescribeFeatureType(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, NN<WFSHandler> me, Text::CStringNN version, NN<Data::FastStringMapNN<GISFeature>> features, NN<GISWebService> svc);
		static Bool GetFeature(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, NN<WFSHandler> me, Text::CStringNN version, NN<Data::FastStringMapNN<GISFeature>> features, NN<GISWebService> svc);
		static Bool GetGmlObject(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, NN<WFSHandler> me, Text::CStringNN version, NN<Data::FastStringMapNN<GISFeature>> features, NN<GISWebService> svc);
		static Bool GetPropertyValue(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, NN<WFSHandler> me, Text::CStringNN version, NN<Data::FastStringMapNN<GISFeature>> features, NN<GISWebService> svc);
		static Bool ListStoredQueries(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, NN<WFSHandler> me, Text::CStringNN version, NN<Data::FastStringMapNN<GISFeature>> features, NN<GISWebService> svc);
		static Bool DescribeStoredQueries(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, NN<WFSHandler> me, Text::CStringNN version, NN<Data::FastStringMapNN<GISFeature>> features, NN<GISWebService> svc);
		static Bool CreateStoredQuery(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, NN<WFSHandler> me, Text::CStringNN version, NN<Data::FastStringMapNN<GISFeature>> features, NN<GISWebService> svc);
		static Bool DropStoredQuery(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, NN<WFSHandler> me, Text::CStringNN version, NN<Data::FastStringMapNN<GISFeature>> features, NN<GISWebService> svc);
		static Bool LockFeature(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, NN<WFSHandler> me, Text::CStringNN version, NN<Data::FastStringMapNN<GISFeature>> features, NN<GISWebService> svc);
		static Bool GetFeatureWithLock(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, NN<WFSHandler> me, Text::CStringNN version, NN<Data::FastStringMapNN<GISFeature>> features, NN<GISWebService> svc);
		static Bool Transaction(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, NN<WFSHandler> me, Text::CStringNN version, NN<Data::FastStringMapNN<GISFeature>> features, NN<GISWebService> svc);
	public:
		WFSHandler();
		~WFSHandler();

		Bool DoWFS(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, NN<Data::FastStringMapNN<WFSHandler::GISFeature>> features, NN<GISWebService> svc);
		static void __stdcall FreeFeature(NN<GISFeature> feature);
	};
}
#endif
