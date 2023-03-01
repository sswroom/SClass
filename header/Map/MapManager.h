#ifndef _SM_MAP_MAPMANAGER
#define _SM_MAP_MAPMANAGER

#include "Data/ArrayList.h"
#include "Data/StringUTF8Map.h"
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
			Map::MapDrawLayer *layer;
			Data::ArrayList<Map::MapEnv *> *envList;
		} MapLayerInfo;

	private:
		Data::StringUTF8Map<MapLayerInfo*> layerArr;

	public:
		MapManager();
		~MapManager();

		Map::MapDrawLayer *LoadLayer(Text::CString fileName, Parser::ParserList *parsers, Map::MapEnv *env);
		void ClearMap(Map::MapEnv *env);
	};
}
#endif
