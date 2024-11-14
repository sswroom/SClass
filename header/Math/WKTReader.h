#ifndef _SM_MATH_WKTREADER
#define _SM_MATH_WKTREADER
#include "Math/Geometry/CompoundCurve.h"
#include "Math/Geometry/CurvePolygon.h"
#include "Math/Geometry/MultiPolygon.h"
#include "Math/Geometry/MultiSurface.h"
#include "Math/Geometry/Point.h"
#include "Math/Geometry/Vector2D.h"

namespace Math
{
	class WKTReader
	{
	private:
		UnsafeArrayOpt<const UTF8Char> lastError;
		UInt32 srid;

		static UnsafeArrayOpt<const UTF8Char> NextDouble(UnsafeArray<const UTF8Char> wkt, OutParam<Double> val);

		void SetLastError(UnsafeArrayOpt<const UTF8Char> lastError);
		Optional<Math::Geometry::Point> ParsePoint(UnsafeArray<const UTF8Char> wkt, OutParam<UnsafeArray<const UTF8Char>> wktEnd);
		Optional<Math::Geometry::LineString> ParseLineString(UnsafeArray<const UTF8Char> wkt, OutParam<UnsafeArray<const UTF8Char>> wktEnd, Bool curve);
		Optional<Math::Geometry::LinearRing> ParseLinearRing(UnsafeArray<const UTF8Char> wkt, OutParam<UnsafeArray<const UTF8Char>> wktEnd);
		Optional<Math::Geometry::CompoundCurve> ParseCompoundCurve(UnsafeArray<const UTF8Char> wkt, OutParam<UnsafeArray<const UTF8Char>> wktEnd);
		Optional<Math::Geometry::Polygon> ParsePolygon(UnsafeArray<const UTF8Char> wkt, OutParam<UnsafeArray<const UTF8Char>> wktEnd);
		Optional<Math::Geometry::CurvePolygon> ParseCurvePolygon(UnsafeArray<const UTF8Char> wkt, OutParam<UnsafeArray<const UTF8Char>> wktEnd);
		Optional<Math::Geometry::MultiPolygon> ParseMultiPolygon(UnsafeArray<const UTF8Char> wkt, OutParam<UnsafeArray<const UTF8Char>> wktEnd);
		Optional<Math::Geometry::MultiSurface> ParseMultiSurface(UnsafeArray<const UTF8Char> wkt, OutParam<UnsafeArray<const UTF8Char>> wktEnd);
	public:
		WKTReader(UInt32 srid);
		~WKTReader();

		Optional<Math::Geometry::Vector2D> ParseWKT(UnsafeArray<const UTF8Char> wkt);
		UnsafeArrayOpt<const UTF8Char> GetLastError();
	};
}
#endif
