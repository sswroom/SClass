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
			CurvePolygon(UInt32 srid, Bool hasZ, Bool hasM);
			virtual ~CurvePolygon();

			virtual void AddGeometry(Vector2D *geometry);
			virtual Vector2D::VectorType GetVectorType() const;
			virtual Vector2D *Clone() const;
		};
	}
}
#endif
