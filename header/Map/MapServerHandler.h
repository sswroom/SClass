#ifndef _SM_MAP_MAPSERVERHANDLER
#define _SM_MAP_MAPSERVERHANDLER
#include "Data/FastStringMap.h"
#include "Map/IMapDrawLayer.h"
#include "Net/WebServer/WebServiceHandler.h"
#include "Parser/ParserList.h"
#include "Text/CString.h"

namespace Map
{
	class MapServerHandler : public Net::WebServer::WebServiceHandler
	{
	private:
		Parser::ParserList *parsers;
		Data::ArrayList<IO::ParsedObject*> assets;
		Data::FastStringMap<Map::IMapDrawLayer*> layerMap;

		static Bool __stdcall GetLayersFunc(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, WebServiceHandler *me);
		static Bool __stdcall GetLayerDataFunc(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Text::CString subReq, WebServiceHandler *me);

		void AddLayer(Map::IMapDrawLayer *layer);
	public:
		MapServerHandler(Parser::ParserList *parsers);
		virtual ~MapServerHandler();

		Bool AddAsset(Text::CString filePath);
	};
}
#endif
