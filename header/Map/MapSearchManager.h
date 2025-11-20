#ifndef _SM_MAP_MAPSEARCHMANAGER
#define _SM_MAP_MAPSEARCHMANAGER
#include "Data/ArrayListNN.hpp"
#include "Data/ArrayListICaseStringNN.h"
#include "Map/MapSearchLayer.h"
#include "Text/CString.h"

namespace Map
{
	class MapSearchManager
	{
	private:
		Data::ArrayListNN<Map::MapSearchLayer> layerArr;
		Data::ArrayListICaseStringNN nameArr;

	public:
		MapSearchManager();
		~MapSearchManager();

		NN<Map::MapSearchLayer> LoadLayer(Text::CStringNN fileName);
	};
}
#endif
