#ifndef _SM_MATH_GEOMETRY_POINTZ
#define _SM_MATH_GEOMETRY_POINTZ
#include "Math/Vector3.h"
#include "Math/Geometry/Point.h"

namespace Math
{
	namespace Geometry
	{
		class PointZ : public Point
		{
		protected:
			Double z;
		public:
			PointZ(UInt32 srid, Double x, Double y, Double z);
			virtual ~PointZ();

			virtual NN<Vector2D> Clone() const;
			Double GetZ() const;
			Math::Vector3 GetPos3D() const;
			virtual Bool HasZ() const;
			virtual Bool GetZBounds(OutParam<Double> min, OutParam<Double> max) const;
			virtual void Convert(NN<Math::CoordinateConverter> converter);
			virtual Bool Equals(NN<const Vector2D> vec, Bool sameTypeOnly, Bool nearlyVal, Bool no3DGeometry) const;
		};
	}
}
#endif
