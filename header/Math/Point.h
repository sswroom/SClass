#ifndef _SM_MATH_POINT
#define _SM_MATH_POINT
#include "Math/Vector2D.h"

namespace Math
{
	class Point : public Math::Vector2D
	{
	protected:
		Double x;
		Double y;
	public:
		Point(UInt32 srid, Double x, Double y);
		virtual ~Point();

		virtual VectorType GetVectorType();
		virtual Math::Coord2DDbl GetCenter();
		virtual Math::Vector2D *Clone();
		virtual void GetBounds(Double *minX, Double *minY, Double *maxX, Double *maxY);
		virtual Double CalSqrDistance(Double x, Double y, Double *nearPtX, Double *nearPtY);
		virtual Bool JoinVector(Math::Vector2D *vec);
		virtual void ConvCSys(Math::CoordinateSystem *srcCSys, Math::CoordinateSystem *destCSys);
		virtual Bool Equals(Vector2D *vec);
	};
}
#endif
