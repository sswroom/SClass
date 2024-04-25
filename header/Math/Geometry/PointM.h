#ifndef _SM_MATH_GEOMETRY_POINTM
#define _SM_MATH_GEOMETRY_POINTM
#include "Math/Geometry/Point.h"

namespace Math
{
	namespace Geometry
	{
		class PointM : public Point
		{
		protected:
			Double m;
		public:
			PointM(UInt32 srid, Double x, Double y, Double m);
			virtual ~PointM();

			virtual NN<Vector2D> Clone() const;
			Double GetM() const;
			virtual Bool HasM() const;
			virtual Bool GetMBounds(OutParam<Double> min, OutParam<Double> max) const;
			virtual Bool Equals(NN<const Vector2D> vec, Bool sameTypeOnly, Bool nearlyVal) const;
		};
	}
}
#endif
