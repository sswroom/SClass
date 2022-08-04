#ifndef _SM_MATH_MULTIPOLYGON
#define _SM_MATH_MULTIPOLYGON
#include "Math/MultiGeometry.h"
#include "Math/Polygon.h"

namespace Math
{
	class MultiPolygon : public Math::MultiGeometry<Math::Polygon>
	{
	public:
		MultiPolygon(UInt32 srid);
		virtual ~MultiPolygon();

		virtual VectorType GetVectorType() const;
		virtual Math::Vector2D *Clone() const;
	};
}
#endif
