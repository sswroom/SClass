#ifndef _SM_MATH_GEOMETRY_MULTIPOINT
#define _SM_MATH_GEOMETRY_MULTIPOINT
#include "Math/Geometry/MultiGeometry.h"
#include "Math/Geometry/Point.h"

namespace Math
{
	namespace Geometry
	{
		class MultiPoint : public MultiGeometry<Point>
		{
		public:
			MultiPoint(UInt32 srid);
			virtual ~MultiPoint();

			virtual VectorType GetVectorType() const;
			virtual NotNullPtr<Vector2D> Clone() const;
		};
	}
}
#endif
