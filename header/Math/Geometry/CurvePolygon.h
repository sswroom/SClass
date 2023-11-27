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

			virtual void AddGeometry(NotNullPtr<Vector2D> geometry);
			virtual Vector2D::VectorType GetVectorType() const;
			virtual NotNullPtr<Vector2D> Clone() const;
			virtual NotNullPtr<Vector2D> CurveToLine() const;
			virtual Bool InsideVector(Math::Coord2DDbl coord) const;
		};
	}
}
#endif
