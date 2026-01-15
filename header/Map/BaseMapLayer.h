#ifndef _SM_MAP_BASEMAPLAYER
#define _SM_MAP_BASEMAPLAYER
#include "Data/ArrayListNative.hpp"
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
		static UIntOS GetLayerTypes(NN<Data::ArrayListNative<BaseLayerType>> layerTypes);
		static Text::CStringNN BaseLayerTypeGetName(BaseLayerType blt);
		static Optional<Map::MapDrawLayer> CreateLayer(BaseLayerType blt, NN<Net::TCPClientFactory> clif, Optional<Net::SSLEngine> ssl, NN<Parser::ParserList> parsers);
	};
}
#endif
