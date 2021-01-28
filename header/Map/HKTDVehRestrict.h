#ifndef _SM_MAP_HKTDVEHRESTRICT
#define _SM_MAP_HKTDVEHRESTRICT
#include "Data/Integer32Map.h"
#include "DB/DBTool.h"
#include "Map/IMapDrawLayer.h"

namespace Map
{
	class HKTDVehRestrict
	{
	private:
		typedef struct
		{
			Int32 routeId;
			Math::Polyline *pl;
		} RouteInfo;
	private:
		DB::DBTool *db;
		Math::CoordinateSystem *csys;
		Data::Integer32Map<RouteInfo*> *routeMap;

	public:
		HKTDVehRestrict(Map::IMapDrawLayer *routeLyr, DB::DBTool *db);
		~HKTDVehRestrict();

		Map::IMapDrawLayer *CreateTonnesSignLayer();
	};
}
#endif
