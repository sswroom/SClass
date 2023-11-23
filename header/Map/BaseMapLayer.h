#ifndef _SM_MAP_BASEMAPLAYER
#define _SM_MAP_BASEMAPLAYER
#include "Data/ArrayList.h"
#include "Map/MapDrawLayer.h"
#include "Net/SocketFactory.h"
#include "Net/SSLEngine.h"
#include "Parser/ParserList.h"
#include "Text/CString.h"

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
			BLT_OSM_MAPQUEST,
			BLT_GMAP_MAP,
			BLT_GMAP_TRAIN,
			BLT_GMAP_HYBRID,
			BLT_GMAP_SATELITE
		} BaseLayerType;
	public:
		static UOSInt GetLayerTypes(Data::ArrayList<BaseLayerType> *layerTypes);
		static Text::CString BaseLayerTypeGetName(BaseLayerType blt);
		static Map::MapDrawLayer *CreateLayer(BaseLayerType blt, NotNullPtr<Net::SocketFactory> sockf, Net::SSLEngine *ssl, NotNullPtr<Parser::ParserList> parsers);
	};
}
#endif
