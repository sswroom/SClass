#ifndef _SM_MAP_HKTDVEHRESTRICT
#define _SM_MAP_HKTDVEHRESTRICT
#include "Data/FastMapNN.h"
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
		NN<DB::DBTool> db;
		NN<Math::CoordinateSystem> csys;
		Data::FastMapNN<Int32, RouteInfo> routeMap;

	public:
		HKTDVehRestrict(NN<Map::MapDrawLayer> routeLyr, NN<DB::DBTool> db);
		~HKTDVehRestrict();

		NN<Map::MapDrawLayer> CreateTonnesSignLayer();
	};
}
#endif
