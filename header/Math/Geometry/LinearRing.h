#ifndef _SM_MATH_GEOMETRY_LINEARRING
#define _SM_MATH_GEOMETRY_LINEARRING
#include "Math/Geometry/LineString.h"

namespace Math
{
	namespace Geometry
	{
		class LinearRing : public LineString
		{
		public:
			LinearRing(UInt32 srid, UOSInt nPoint, Bool hasZ, Bool hasM);
			LinearRing(UInt32 srid, UnsafeArray<const Math::Coord2DDbl> pointArr, UOSInt nPoint, UnsafeArrayOpt<Double> zArr, UnsafeArrayOpt<Double> mArr);
			virtual ~LinearRing();

			virtual VectorType GetVectorType() const;
			virtual NN<Math::Geometry::Vector2D> Clone() const;
			virtual Bool InsideOrTouch(Math::Coord2DDbl coord) const;
			virtual Double CalArea() const;
			virtual Bool HasArea() const;
			virtual Math::Coord2DDbl GetDisplayCenter() const;
			Bool IsOpen() const;
			Bool IsClose() const;

			static Double GetIntersactsCenter(NN<Data::ArrayListNative<Double>> vals);
			static NN<LinearRing> CreateFromCircle(UInt32 srid, Math::Coord2DDbl center, Double radiusX, Double radiusY, UOSInt nPoints);
		};
	}
}
#endif
