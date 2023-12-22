#ifndef _SM_MAP_MAPSERVERHANDLER
#define _SM_MAP_MAPSERVERHANDLER
#include "Data/FastStringMap.h"
#include "Map/MapDrawLayer.h"
#include "Math/GeographicCoordinateSystem.h"
#include "Net/WebServer/WebServiceHandler.h"
#include "Parser/ParserList.h"
#include "Text/CString.h"
#include "Text/JSON.h"

namespace Map
{
	class MapServerHandler : public Net::WebServer::WebServiceHandler
	{
	private:
		NotNullPtr<Parser::ParserList> parsers;
		Data::ArrayListNN<IO::ParsedObject> assets;
		Data::FastStringMap<Map::MapDrawLayer*> layerMap;
		Text::String *cesiumScenePath;
		Double cesiumMinError;
		Math::GeographicCoordinateSystem *wgs84;

		static Bool __stdcall GetLayersFunc(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, WebServiceHandler *me);
		static Bool __stdcall GetLayerDataFunc(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, WebServiceHandler *me);
		static Bool __stdcall CesiumDataFunc(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, WebServiceHandler *me);
		static Bool __stdcall CesiumB3DMFunc(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, WebServiceHandler *me);

		void CheckObject(Text::JSONBase *obj, Double x1, Double y1, Double x2, Double y2, Double minErr, NotNullPtr<Text::String> fileName, NotNullPtr<Text::StringBuilderUTF8> tmpSb);
		Bool InObjectRange(Text::JSONBase *obj, Double x1, Double y1, Double x2, Double y2);
		Bool InSphereRange(Text::JSONBase *sphere, Double x1, Double y1, Double x2, Double y2);
		void AddLayer(NotNullPtr<Map::MapDrawLayer> layer);
	public:
		MapServerHandler(NotNullPtr<Parser::ParserList> parsers);
		virtual ~MapServerHandler();

		Bool AddAsset(Text::CStringNN filePath);
		void SetCesiumScenePath(Text::CString cesiumScenePath);
		void SetCesiumMinError(Double minError);
	};
}
#endif
