#ifndef _SM_MATH_GEOMETRY_CIRCULARSTRING
#define _SM_MATH_GEOMETRY_CIRCULARSTRING
#include "Math/Geometry/LineString.h"

namespace Math
{
	namespace Geometry
	{
		class CircularString : public LineString
		{
		public:
			CircularString(UInt32 srid, UOSInt nPoint, Bool hasZ, Bool hasM);
			CircularString(UInt32 srid, UnsafeArray<const Math::Coord2DDbl> pointArr, UOSInt nPoint, UnsafeArrayOpt<Double> zArr, UnsafeArrayOpt<Double> mArr);
			virtual ~CircularString();

			virtual VectorType GetVectorType() const;
			virtual NN<Math::Geometry::Vector2D> Clone() const;
			virtual Bool HasCurve() const { return true; }
			virtual Optional<Vector2D> ToSimpleShape() const;
		};
	}
}
#endif
