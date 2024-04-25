#ifndef _SM_MATH_GEOMETRY_CURVEPOLYGON
#define _SM_MATH_GEOMETRY_CURVEPOLYGON
#include "Math/Geometry/MultiGeometry.h"

namespace Math
{
	namespace Geometry
	{
		class CurvePolygon : public MultiGeometry<Vector2D>
		{
		public:
			CurvePolygon(UInt32 srid);
			virtual ~CurvePolygon();

			virtual void AddGeometry(NN<Vector2D> geometry);
			virtual Vector2D::VectorType GetVectorType() const;
			virtual NN<Vector2D> Clone() const;
			virtual NN<Vector2D> CurveToLine() const;
			virtual Bool InsideOrTouch(Math::Coord2DDbl coord) const;
		};
	}
}
#endif
