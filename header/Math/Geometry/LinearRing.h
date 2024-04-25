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
			LinearRing(UInt32 srid, const Math::Coord2DDbl *pointArr, UOSInt nPoint, Double *zArr, Double *mArr);
			virtual ~LinearRing();

			virtual VectorType GetVectorType() const;
			virtual NN<Math::Geometry::Vector2D> Clone() const;
			virtual Bool InsideOrTouch(Math::Coord2DDbl coord) const;
			virtual Double CalArea() const;
			Bool IsOpen() const;
			Bool IsClose() const;

			static NN<LinearRing> CreateFromCircle(UInt32 srid, Math::Coord2DDbl center, Double radiusX, Double radiusY, UOSInt nPoints);
		};
	}
}
#endif
