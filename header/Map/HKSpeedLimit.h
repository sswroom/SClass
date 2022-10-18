#ifndef _SM_MAP_HKSPEEDLIMIT
#define _SM_MAP_HKSPEEDLIMIT
#include "Data/FastMap.h"
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
			Math::Geometry::Vector2D *vecOri;
		};
	private:
		DB::ReadingDB *fgdb;
		Math::CoordinateSystem *dataCsys;
		Math::CoordinateSystem *reqCsys;
		Data::FastMap<Int32, RouteInfo*> routeMap;
		Math::RectAreaDbl bounds;
		Data::FastMap<Int64, Data::ArrayList<Int32>*> indexMap;

		static void FreeRoute(RouteInfo *route);
		void FreeIndex();
		void BuildIndex();
		void AppendRouteIds(Data::ArrayList<Int32> *routeList, Int32 x, Int32 y);
	public:
		HKSpeedLimit(Text::CString fgdbPath);
		~HKSpeedLimit();

		const RouteInfo *GetNearestRoute(Math::Coord2DDbl pt);
		Int32 GetSpeedLimit(Math::Coord2DDbl pt, Double maxDistM);
		void SetReqCoordinateSystem(Math::CoordinateSystem *csys);

		static Text::CString GetDownloadURL();
		static Text::CString GetDefFileName();
	};
}
#endif
