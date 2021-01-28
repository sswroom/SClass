#ifndef _SM_MAP_MAPMANAGER
#define _SM_MAP_MAPMANAGER

#include "Data/ArrayList.h"
#include "Data/StringUTF8Map.h"
#include "Map/IMapDrawLayer.h"
#include "Map/MapEnv.h"
#include "Parser/ParserList.h"

namespace Map
{
	class MapManager
	{
	private:
		typedef struct
		{
			Map::IMapDrawLayer *layer;
			Data::ArrayList<Map::MapEnv *> *envList;
		} MapLayerInfo;

	private:
		Data::StringUTF8Map<MapLayerInfo*> *layerArr;

	public:
		MapManager();
		~MapManager();

		Map::IMapDrawLayer *LoadLayer(const UTF8Char *fileName, Parser::ParserList *parsers, Map::MapEnv *env);
		void ClearMap(Map::MapEnv *env);
	};
}
#endif
