#ifndef _SM_MATH_GEOMETRY_LINEARRING
#define _SM_MATH_GEOMETRY_LINEARRING
#include "Math/Geometry/LineString.h"

namespace Math
{
	namespace Geometry
	{
		class LinearRing : public LineString
		{
		public:
			LinearRing(UInt32 srid, UOSInt nPoint, Bool hasZ, Bool hasM);
			virtual ~LinearRing();

			virtual VectorType GetVectorType() const;
			virtual NotNullPtr<Math::Geometry::Vector2D> Clone() const;
			Bool IsOpen() const;
			Bool IsClose() const;
		};
	}
}
#endif
