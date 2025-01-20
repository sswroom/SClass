#ifndef _SM_MAP_ASSISTEDREVERSEGEOCODER
#define _SM_MAP_ASSISTEDREVERSEGEOCODER
#include "Map/ReverseGeocoder.h"

namespace Map
{
	class AssistedReverseGeocoder : public Map::ReverseGeocoder
	{
	public:
		virtual ~AssistedReverseGeocoder() {};

		virtual void AddReverseGeocoder(NN<Map::ReverseGeocoder> revGeo) = 0;
	};
}
#endif
