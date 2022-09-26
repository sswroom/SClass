#ifndef _SM_MAP_HKSPEEDLIMIT
#define _SM_MAP_HKSPEEDLIMIT
#include "Data/Int32Map.h"
#include "Data/Int64Map.h"
#include "DB/ReadingDB.h"
#include "Math/Geometry/Vector2D.h"
#include "Text/CString.h"

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
			Math::Geometry::Vector2D *vec;
		};
	private:
		DB::ReadingDB *fgdb;
		Math::CoordinateSystem *csys;
		Data::Int32Map<RouteInfo*> routeMap;
		Math::RectAreaDbl bounds;
		Data::Int64Map<Data::ArrayList<Int32>*> indexMap;

		static void FreeRoute(RouteInfo *route);
		void FreeIndex();
		void BuildIndex();
		void AppendRouteIds(Data::ArrayList<Int32> *routeList, Int32 x, Int32 y);
	public:
		HKSpeedLimit(Text::CString fgdbPath);
		~HKSpeedLimit();

		const RouteInfo *GetNearestRoute(Math::Coord2DDbl pt);
		void ConvertCoordinateSystem(Math::CoordinateSystem *csys);

		static Text::CString GetDownloadURL();
		static Text::CString GetDefFileName();
	};
}
#endif
