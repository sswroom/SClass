#ifndef _SM_MATH_MULTIPOINt
#define _SM_MATH_MULTIPOINT
#include "Math/PointCollection.h"

namespace Math
{
	class MultiPoint : public Math::PointCollection
	{
	private:
		Double *points;
		OSInt nPoints;
		Int32 part;
	public:
		MultiPoint(OSInt nPoints);
		virtual ~MultiPoint();

		virtual VectorType GetVectorType();
		virtual Int32 *GetPartList(OSInt *nParts);
		virtual Double *GetPointList(OSInt *nPoints);
	};
}
#endif
