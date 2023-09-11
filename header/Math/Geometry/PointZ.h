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

			virtual NotNullPtr<Vector2D> Clone() const;
			Double GetZ() const;
			Math::Vector3 GetPos3D() const;
			virtual Bool HasZ() const;
			virtual void ConvCSys(NotNullPtr<const Math::CoordinateSystem> srcCSys, NotNullPtr<const Math::CoordinateSystem> destCSys);
			virtual Bool Equals(NotNullPtr<const Vector2D> vec) const;
			virtual Bool EqualsNearly(NotNullPtr<const Vector2D> vec) const;
		};
	}
}
#endif
