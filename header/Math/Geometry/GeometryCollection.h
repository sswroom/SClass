#ifndef _SM_MATH_GEOMETRY_GEOMETRYCOLLECTION
#define _SM_MATH_GEOMETRY_GEOMETRYCOLLECTION
#include "Math/Geometry/MultiGeometry.h"

namespace Math
{
	namespace Geometry
	{
		class GeometryCollection : public MultiGeometry<Vector2D>
		{
		public:
			GeometryCollection(UInt32 srid, Bool hasZ, Bool hasM);
			virtual ~GeometryCollection();

			virtual Vector2D::VectorType GetVectorType() const;
			virtual Vector2D *Clone() const;
		};
	}
}
#endif
