#ifndef _SM_MAP_WFSHANDLER
#define _SM_MAP_WFSHANDLER
#include "DB/ColDef.h"
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
		Data::FastStringMapObj<RequestFunc> reqMap;

		static Bool __stdcall GetCapabilities(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, NN<WFSHandler> me, Text::CStringNN version, NN<Data::FastStringMapNN<GISFeature>> features, NN<GISWebService> svc);
		static Bool __stdcall DescribeFeatureType(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, NN<WFSHandler> me, Text::CStringNN version, NN<Data::FastStringMapNN<GISFeature>> features, NN<GISWebService> svc);
		static Bool __stdcall GetFeature(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, NN<WFSHandler> me, Text::CStringNN version, NN<Data::FastStringMapNN<GISFeature>> features, NN<GISWebService> svc);
		static Bool __stdcall GetGmlObject(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, NN<WFSHandler> me, Text::CStringNN version, NN<Data::FastStringMapNN<GISFeature>> features, NN<GISWebService> svc);
		static Bool __stdcall GetPropertyValue(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, NN<WFSHandler> me, Text::CStringNN version, NN<Data::FastStringMapNN<GISFeature>> features, NN<GISWebService> svc);
		static Bool __stdcall ListStoredQueries(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, NN<WFSHandler> me, Text::CStringNN version, NN<Data::FastStringMapNN<GISFeature>> features, NN<GISWebService> svc);
		static Bool __stdcall DescribeStoredQueries(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, NN<WFSHandler> me, Text::CStringNN version, NN<Data::FastStringMapNN<GISFeature>> features, NN<GISWebService> svc);
		static Bool __stdcall CreateStoredQuery(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, NN<WFSHandler> me, Text::CStringNN version, NN<Data::FastStringMapNN<GISFeature>> features, NN<GISWebService> svc);
		static Bool __stdcall DropStoredQuery(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, NN<WFSHandler> me, Text::CStringNN version, NN<Data::FastStringMapNN<GISFeature>> features, NN<GISWebService> svc);
		static Bool __stdcall LockFeature(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, NN<WFSHandler> me, Text::CStringNN version, NN<Data::FastStringMapNN<GISFeature>> features, NN<GISWebService> svc);
		static Bool __stdcall GetFeatureWithLock(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, NN<WFSHandler> me, Text::CStringNN version, NN<Data::FastStringMapNN<GISFeature>> features, NN<GISWebService> svc);
		static Bool __stdcall Transaction(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, NN<WFSHandler> me, Text::CStringNN version, NN<Data::FastStringMapNN<GISFeature>> features, NN<GISWebService> svc);
		static Bool __stdcall ServiceExceptionReport(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, Text::CStringNN exceptionCode, Text::CString locator, Text::CStringNN exceptionMessage, NN<GISWebService> svc);
		static Bool __stdcall ResponseGML(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, NN<GISFeature> feature, NN<Data::ArrayListInt64> idList, Optional<Map::NameArray> nameArr, Optional<Text::String> bbox, NN<GISWebService> svc);
	public:
		WFSHandler();
		~WFSHandler();

		Bool DoWFS(NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, NN<Data::FastStringMapNN<WFSHandler::GISFeature>> features, NN<GISWebService> svc);
		static void __stdcall FreeFeature(NN<GISFeature> feature);
		static Text::CStringNN ColType2XSDType(DB::DBUtil::ColType colType);
		static Text::CStringNN DrawLayerType2GMLType(Map::DrawLayerType layerType);
	};
}
#endif
