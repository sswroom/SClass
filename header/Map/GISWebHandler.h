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
	public:
		GISWebHandler();
		virtual ~GISWebHandler();

		UOSInt AddAsset(NN<Map::MapDrawLayer> layer);
	};
}
#endif
