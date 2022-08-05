#ifndef _SM_MATH_GEOMETRY_POINTZ
#define _SM_MATH_GEOMETRY_POINTZ
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

			virtual Vector2D *Clone() const;
			Double GetZ() const;
			void GetPos3D(Double *x, Double *y, Double *z) const;
			virtual Bool HasZ() const;
			virtual void ConvCSys(Math::CoordinateSystem *srcCSys, Math::CoordinateSystem *destCSys);
			virtual Bool Equals(Vector2D *vec) const;
		};
	}
}
#endif
