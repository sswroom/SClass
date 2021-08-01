#ifndef _SM_MAP_BASEMAPLAYER
#define _SM_MAP_BASEMAPLAYER
#include "Data/ArrayList.h"
#include "Map/IMapDrawLayer.h"
#include "Net/SocketFactory.h"
#include "Net/SSLEngine.h"
#include "Parser/ParserList.h"

namespace Map
{
	class BaseMapLayer
	{
	public:
		typedef enum
		{
			BLT_OSM_TILE,
			BLT_OSM_CYCLE,
			BLT_OSM_TRANSP,
			BLT_OSM_LANDSCAPE,
			BLT_OSM_OUTDOORS,
			BLT_OSM_TRANSP_DARK,
			BLT_OSM_SPINAL,
			BLT_OSM_MAPQUEST
		} BaseLayerType;
	public:
		static UOSInt GetLayerTypes(Data::ArrayList<BaseLayerType> *layerTypes);
		static const UTF8Char *BaseLayerTypeGetName(BaseLayerType blt);
		static Map::IMapDrawLayer *CreateLayer(BaseLayerType blt, Net::SocketFactory *sockf, Net::SSLEngine *ssl, Parser::ParserList *parsers);
	};
}
#endif
