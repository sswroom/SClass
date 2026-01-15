#ifndef _SM_MATH_GEOMETRYTOOL
#define _SM_MATH_GEOMETRYTOOL
#include "Data/ArrayListA.hpp"
#include "Math/Coord2D.h"
#include "Math/Geometry/Polygon.h"
#include "Math/Geometry/Vector2D.h"
#include "Math/Unit/Distance.h"

namespace Math
{
	class GeometryTool
	{
	public:
		static const Int32 RADIUS_METER_EARTH_WGS1984 = 6378137;
	public:
		static void RotateACW(UnsafeArray<Int32> ptOut, UnsafeArray<const Int32> ptIn, UIntOS nPoint, Double centX, Double centY, Double angleRad);
		static void RotateACW(UnsafeArray<Double> ptOut, UnsafeArray<const Double> ptIn, UIntOS nPoint, Double centX, Double centY, Double angleRad);
		static UIntOS BoundPolygonY(UnsafeArray<const Int32> points, UIntOS nPoints, UnsafeArray<Int32> pointOut, IntOS minY, IntOS maxY, IntOS ofstX, IntOS ofstY);
		static UIntOS BoundPolygonX(UnsafeArray<const Int32> points, UIntOS nPoints, UnsafeArray<Int32> pointOut, IntOS minX, IntOS maxX, IntOS ofstX, IntOS ofstY);
		static UIntOS BoundPolygonY(UnsafeArray<const Math::Coord2DDbl> points, UIntOS nPoints, UnsafeArray<Math::Coord2DDbl> pointOut, Double minY, Double maxY, Math::Coord2DDbl ofst);
		static UIntOS BoundPolygonX(UnsafeArray<const Math::Coord2DDbl> points, UIntOS nPoints, UnsafeArray<Math::Coord2DDbl> pointOut, Double minX, Double maxX, Math::Coord2DDbl ofst);
		static Bool InPolygon(UnsafeArray<const Int32> points, UIntOS nPoints, Int32 ptX, Int32 ptY); /////////////////////////
		static void PtNearPline(UnsafeArray<const Int32> points, UIntOS nPoints, IntOS ptX, IntOS ptY, OutParam<Int32> nearPtX, OutParam<Int32> nearPtY);
		static Double SphereDistDeg(Double lat1, Double lon1, Double lat2, Double lon2, Double radius);
		static void GetPolygonCenter(UIntOS nParts, UIntOS nPoints, UnsafeArray<const UInt32> parts, UnsafeArray<const Int32> points, OutParam<Int32> outPtX, OutParam<Int32> outPtY);
		static Math::Coord2DDbl GetPolygonCenter(UIntOS nParts, UIntOS nPoints, UnsafeArray<const UInt32> parts, UnsafeArray<const Math::Coord2DDbl> points);
		static Double CalcMaxDistanceFromPoint(Math::Coord2DDbl pt, NN<const Math::Geometry::Vector2D> vec, Math::Unit::Distance::DistanceUnit unit);
		static Math::Coord2DDbl MercatorToProject(Math::Coord2DDbl pt);
		static void CalcHVAngleRad(Math::Coord2DDbl ptCurr, Math::Coord2DDbl ptNext, Double heightCurr, Double heightNext, OutParam<Double> hAngle, OutParam<Double> vAngle);
		static void CalcHVAngleDeg(Math::Coord2DDbl ptCurr, Math::Coord2DDbl ptNext, Double heightCurr, Double heightNext, OutParam<Double> hAngle, OutParam<Double> vAngle);
		static Optional<Math::Geometry::Polygon> CreateCircularPolygonWGS84(Math::Coord2DDbl pt, Double radiusMeter, UIntOS nPoints);
		static void ArcToLine(Math::Coord2DDbl center, Double radius, UIntOS nPoint, Double startAngleRad, Double endAngleRad, Bool clockwise, NN<Data::ArrayListA<Math::Coord2DDbl>> ptOut);
		static UIntOS ArcToLine(Math::Coord2DDbl pt1, Math::Coord2DDbl pt2, Math::Coord2DDbl pt3, Double minDist, NN<Data::ArrayListA<Math::Coord2DDbl>> ptOut);
		static UIntOS BezierCurveToLine(Math::Coord2DDbl pt0, Math::Coord2DDbl pt1, Math::Coord2DDbl pt2, Math::Coord2DDbl pt3, UIntOS nPoints, NN<Data::ArrayListA<Math::Coord2DDbl>> ptOut);
		static Double CalcDir(Math::Coord2DDbl startPt, Math::Coord2DDbl endPt, Math::Unit::Angle::AngleUnit aunit);
//		static UIntOS CurveToLine(Math::Coord2DDbl pt1, Math::Coord2DDbl pt2, Math::Coord2DDbl pt3, Double minDist, NN<Data::ArrayListA<Math::Coord2DDbl>> ptOut);
	};
}
#endif
