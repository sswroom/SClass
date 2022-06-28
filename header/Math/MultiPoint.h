#ifndef _SM_MATH_MULTIPOINt
#define _SM_MATH_MULTIPOINT
#include "Math/PointCollection.h"

namespace Math
{
	class MultiPoint : public Math::PointCollection
	{
	public:
		MultiPoint(UInt32 srid, UOSInt nPoints);
		virtual ~MultiPoint();

		virtual VectorType GetVectorType();
	};
}
#endif
