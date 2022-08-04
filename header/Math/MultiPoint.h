#ifndef _SM_MATH_MULTIPOINT
#define _SM_MATH_MULTIPOINT
#include "Math/MultiGeometry.h"
#include "Math/Point.h"

namespace Math
{
	class MultiPoint : public Math::MultiGeometry<Math::Point>
	{
	public:
		MultiPoint(UInt32 srid);
		virtual ~MultiPoint();

		virtual VectorType GetVectorType() const;
		virtual Math::Vector2D *Clone() const;
	};
}
#endif
