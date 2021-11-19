#ifndef _SM_MATH_POINT3D
#define _SM_MATH_POINT3D
#include "Math/Point.h"

namespace Math
{
	class Point3D : public Math::Point
	{
	private:
		Double z;
	public:
		Point3D(UInt32 srid, Double x, Double y, Double z);
		virtual ~Point3D();

		virtual Math::Vector2D *Clone();
		virtual void GetCenter3D(Double *x, Double *y, Double *z);
		virtual Bool Support3D();
		virtual void ConvCSys(Math::CoordinateSystem *srcCSys, Math::CoordinateSystem *destCSys);
		virtual Bool Equals(Math::Vector2D *vec);
	};
}
#endif
