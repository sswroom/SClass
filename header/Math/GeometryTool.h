#ifndef _SM_MATH_GEOMETRYTOOL
#define _SM_MATH_GEOMETRYTOOL
#include "Math/Coord2D.h"
#include "Math/Geometry/Vector2D.h"
#include "Math/Unit/Distance.h"

namespace Math
{
	class GeometryTool
	{
	public:
		static const Int32 RADIUS_METER_EARTH_WGS1984 = 6378137;
	public:
		static void RotateACW(Int32 *ptOut, Int32 *ptIn, UOSInt nPoint, Double centX, Double centY, Double angleRad);
		static void RotateACW(Double *ptOut, Double *ptIn, UOSInt nPoint, Double centX, Double centY, Double angleRad);
		static UOSInt BoundPolygonY(const Int32 *points, UOSInt nPoints, Int32 *pointOut, OSInt minY, OSInt maxY, OSInt ofstX, OSInt ofstY);
		static UOSInt BoundPolygonX(const Int32 *points, UOSInt nPoints, Int32 *pointOut, OSInt minX, OSInt maxX, OSInt ofstX, OSInt ofstY);
		static UOSInt BoundPolygonY(const Math::Coord2DDbl *points, UOSInt nPoints, Math::Coord2DDbl *pointOut, Double minY, Double maxY, Math::Coord2DDbl ofst);
		static UOSInt BoundPolygonX(const Math::Coord2DDbl *points, UOSInt nPoints, Math::Coord2DDbl *pointOut, Double minX, Double maxX, Math::Coord2DDbl ofst);
		static Bool InPolygon(Int32 *points, UOSInt nPoints, Int32 ptX, Int32 ptY); /////////////////////////
		static void PtNearPline(Int32 *points, UOSInt nPoints, OSInt ptX, OSInt ptY, Int32 *nearPtX, Int32 *nearPtY);
		static Double SphereDistDeg(Double lat1, Double lon1, Double lat2, Double lon2, Double radius);
		static void GetPolygonCenter(UOSInt nParts, UOSInt nPoints, UInt32 *parts, Int32 *points, Int32 *outPtX, Int32 *outPtY);
		static Math::Coord2DDbl GetPolygonCenter(UOSInt nParts, UOSInt nPoints, UInt32 *parts, Math::Coord2DDbl *points);
		static Double CalcMaxDistanceFromPoint(Math::Coord2DDbl pt, Math::Geometry::Vector2D *vec, Math::Unit::Distance::DistanceUnit unit);
	};
}
#endif
