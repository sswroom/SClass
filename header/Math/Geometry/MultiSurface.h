#ifndef _SM_MATH_GEOMETRY_MULTISURFACE
#define _SM_MATH_GEOMETRY_MULTISURFACE
#include "Math/Geometry/MultiGeometry.h"

namespace Math
{
	namespace Geometry
	{
		class MultiSurface : public MultiGeometry<Vector2D>
		{
		public:
			MultiSurface(UInt32 srid, Bool hasZ, Bool hasM);
			virtual ~MultiSurface();

			virtual void AddGeometry(NotNullPtr<Vector2D> geometry);
			virtual Vector2D::VectorType GetVectorType() const;
			virtual NotNullPtr<Vector2D> Clone() const;
		};
	}
}
#endif
