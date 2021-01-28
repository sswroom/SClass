#ifndef _SM_MAP_MAPSEARCHMANAGER
#define _SM_MAP_MAPSEARCHMANAGER
#include "Data/ArrayListStrUTF8.h"
#include "Map/IMapSearchLayer.h"

namespace Map
{
	class MapSearchManager
	{
	private:
		Data::ArrayList<Map::IMapSearchLayer*> *layerArr;
		Data::ArrayListStrUTF8 *nameArr;

	public:
		MapSearchManager();
		~MapSearchManager();

		Map::IMapSearchLayer *LoadLayer(const UTF8Char *fileName);
	};
};
#endif
