#ifndef _SM_MAP_HKSPEEDLIMIT
#define _SM_MAP_HKSPEEDLIMIT
#include "Data/FastMapNN.hpp"
#include "Map/HKRoadNetwork2.h"
#include "Math/Geometry/Vector2D.h"

namespace Map
{
	class HKSpeedLimit
	{
	public:
		struct RouteInfo
		{
			Math::RectAreaDbl bounds;
			Int32 objectId;
			Int32 routeId;
			Int32 speedLimit;
			Optional<Math::Geometry::Vector2D> vecOri;
		};
	private:
		NN<Math::CoordinateSystem> dataCsys;
		Optional<Math::CoordinateSystem> reqCsys;
		Data::FastMapNN<Int32, RouteInfo> routeMap;
		Math::RectAreaDbl bounds;
		Data::FastMapNN<Int64, Data::ArrayListNative<Int32>> indexMap;

		static void __stdcall FreeRoute(NN<RouteInfo> route);
		void FreeIndex();
		void BuildIndex();
		void AppendRouteIds(NN<Data::ArrayListNative<Int32>> routeList, Int32 x, Int32 y);
	public:
		HKSpeedLimit(NN<Map::HKRoadNetwork2> roadNetwork);
		~HKSpeedLimit();

		Optional<const RouteInfo> GetNearestRoute(Math::Coord2DDbl pt);
		Int32 GetSpeedLimit(Math::Coord2DDbl pt, Double maxDistM);
		void SetReqCoordinateSystem(NN<Math::CoordinateSystem> csys);
	};
}
#endif
