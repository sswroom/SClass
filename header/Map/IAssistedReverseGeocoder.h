#ifndef _SM_MAP_IASSISTEDREVERSEGEOCODER
#define _SM_MAP_IASSISTEDREVERSEGEOCODER
#include "Map/IReverseGeocoder.h"

namespace Map
{
	class IAssistedReverseGeocoder : public Map::IReverseGeocoder
	{
	public:
		virtual ~IAssistedReverseGeocoder() {};

		virtual void AddReverseGeocoder(NN<Map::IReverseGeocoder> revGeo) = 0;
	};
}
#endif
