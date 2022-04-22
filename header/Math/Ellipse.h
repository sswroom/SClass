#ifndef _SM_MATH_ELLIPSE
#define _SM_MATH_ELLIPSE
#include "Math/Vector2D.h"

namespace Math
{
	class Ellipse : public Vector2D
	{
	private:
		Double tlx;
		Double tly;
		Double w;
		Double h;
		
	public:
		Ellipse(UInt32 srid, Double tlx, Double tly, Double w, Double h);
		virtual ~Ellipse();

		virtual VectorType GetVectorType();
		virtual Math::Coord2D<Double> GetCenter();
		virtual Math::Vector2D *Clone();
		virtual void GetBounds(Double *minX, Double *minY, Double *maxX, Double *maxY);
		virtual Double CalSqrDistance(Double x, Double y, Double *nearPtX, Double *nearPtY);
		virtual Bool JoinVector(Math::Vector2D *vec);
		virtual Bool Support3D();
		virtual void ConvCSys(Math::CoordinateSystem *srcCSys, Math::CoordinateSystem *destCSys);
		virtual Bool Equals(Math::Vector2D *vec);
	};
}
#endif
