#ifndef _SM_MAP_MAPSEARCHMANAGER
#define _SM_MAP_MAPSEARCHMANAGER
#include "Data/ArrayListStrUTF8.h"
#include "Map/IMapSearchLayer.h"
#include "Text/CString.h"

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

		Map::IMapSearchLayer *LoadLayer(Text::CString fileName);
	};
}
#endif
