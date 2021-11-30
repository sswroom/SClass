#ifndef _SM_MATH_PIEAREA
#define _SM_MATH_PIEAREA
#include "Math/Vector2D.h"

namespace Math
{
	class PieArea : public Vector2D
	{
	private:
		Double cx;
		Double cy;
		Double r;
		Double arcAngle1;
		Double arcAngle2;
		
	public:
		PieArea(UInt32 srid, Double cx, Double cy, Double r, Double arcAngle1, Double arcAngle2);
		virtual ~PieArea();

		virtual VectorType GetVectorType();
		virtual void GetCenter(Double *x, Double *y);
		virtual Math::Vector2D *Clone();
		virtual void GetBounds(Double *minX, Double *minY, Double *maxX, Double *maxY);
		virtual Double CalSqrDistance(Double x, Double y, Double *nearPtX, Double *nearPtY);
		virtual Bool JoinVector(Math::Vector2D *vec);
		virtual Bool Support3D();
		virtual void ConvCSys(Math::CoordinateSystem *srcCSys, Math::CoordinateSystem *destCSys);
		virtual Bool Equals(Vector2D *vec);

		Double GetCX();
		Double GetCY();
		Double GetR();
		Double GetArcAngle1();
		Double GetArcAngle2();
	};
}
#endif
