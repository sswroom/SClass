#ifndef _SM_MATH_POINTZ
#define _SM_MATH_POINTZ
#include "Math/Point.h"

namespace Math
{
	class PointZ : public Math::Point
	{
	protected:
		Double z;
	public:
		PointZ(UInt32 srid, Double x, Double y, Double z);
		virtual ~PointZ();

		virtual Math::Vector2D *Clone() const;
		Double GetZ() const;
		void GetPos3D(Double *x, Double *y, Double *z) const;
		virtual Bool HasZ() const;
		virtual void ConvCSys(Math::CoordinateSystem *srcCSys, Math::CoordinateSystem *destCSys);
		virtual Bool Equals(Math::Vector2D *vec) const;
	};
}
#endif
