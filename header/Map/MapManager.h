#ifndef _SM_MAP_MAPMANAGER
#define _SM_MAP_MAPMANAGER

#include "Data/ArrayListNN.hpp"
#include "Data/StringMapNN.hpp"
#include "Map/MapDrawLayer.h"
#include "Map/MapEnv.h"
#include "Parser/ParserList.h"

namespace Map
{
	class MapManager
	{
	private:
		typedef struct
		{
			NN<Map::MapDrawLayer> layer;
			Data::ArrayListNN<Map::MapEnv> envList;
		} MapLayerInfo;

	private:
		Data::StringMapNN<MapLayerInfo> layerArr;

	public:
		MapManager();
		~MapManager();

		Optional<Map::MapDrawLayer> LoadLayer(Text::CStringNN fileName, NN<Parser::ParserList> parsers, NN<Map::MapEnv> env);
		void ClearMap(NN<Map::MapEnv> env);
	};
}
#endif
