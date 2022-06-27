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

		virtual VectorType GetVectorType() const;
		virtual Math::Coord2DDbl GetCenter() const;
		virtual Math::Vector2D *Clone() const;
		virtual void GetBounds(Math::RectAreaDbl *bounds) const;
		virtual Double CalSqrDistance(Math::Coord2DDbl pt, Math::Coord2DDbl *nearPt) const;
		virtual Bool JoinVector(Math::Vector2D *vec);
		virtual Bool Support3D() const;
		virtual void ConvCSys(Math::CoordinateSystem *srcCSys, Math::CoordinateSystem *destCSys);
		virtual Bool Equals(Math::Vector2D *vec) const;
	};
}
#endif
