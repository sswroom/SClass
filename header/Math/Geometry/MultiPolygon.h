#ifndef _SM_MATH_GEOMETRY_MULTIPOLYGON
#define _SM_MATH_GEOMETRY_MULTIPOLYGON
#include "Math/Geometry/MultiGeometry.h"
#include "Math/Geometry/Polygon.h"

namespace Math
{
	namespace Geometry
	{
		class MultiPolygon : public MultiGeometry<Polygon>
		{
		public:
			MultiPolygon(UInt32 srid, Bool hasZ, Bool hasM);
			virtual ~MultiPolygon();

			virtual Vector2D::VectorType GetVectorType() const;
			virtual Vector2D *Clone() const;
		};
	}
}
#endif