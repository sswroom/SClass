#ifndef _SM_MAP_MAPSEARCHMANAGER
#define _SM_MAP_MAPSEARCHMANAGER
#include "Data/ArrayListICaseString.h"
#include "Map/MapSearchLayer.h"
#include "Text/CString.h"

namespace Map
{
	class MapSearchManager
	{
	private:
		Data::ArrayList<Map::MapSearchLayer*> layerArr;
		Data::ArrayListICaseString nameArr;

	public:
		MapSearchManager();
		~MapSearchManager();

		Map::MapSearchLayer *LoadLayer(Text::CStringNN fileName);
	};
}
#endif
