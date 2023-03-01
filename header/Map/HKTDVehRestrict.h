#ifndef _SM_MAP_HKTDVEHRESTRICT
#define _SM_MAP_HKTDVEHRESTRICT
#include "Data/FastMap.h"
#include "DB/DBTool.h"
#include "Map/MapDrawLayer.h"

namespace Map
{
	class HKTDVehRestrict
	{
	private:
		typedef struct
		{
			Int32 routeId;
			Math::Geometry::Polyline *pl;
		} RouteInfo;
	private:
		DB::DBTool *db;
		Math::CoordinateSystem *csys;
		Data::FastMap<Int32, RouteInfo*> routeMap;

	public:
		HKTDVehRestrict(Map::MapDrawLayer *routeLyr, DB::DBTool *db);
		~HKTDVehRestrict();

		Map::MapDrawLayer *CreateTonnesSignLayer();
	};
}
#endif
