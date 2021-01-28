#ifndef _SM_MATH_DYNAMICPOLYLINE
#define _SM_MATH_DYNAMICPOLYLINE
#include "Math/Polyline.h"

namespace Math
{
	class DynamicPolyline : public Math::Polyline
	{
	protected:
		OSInt pointCapacity;

	public:
		DynamicPolyline(Double x, Double y);
		virtual ~DynamicPolyline();

		void AddPoint(Double x, Double y);
		void ChangeLastPoint(Double x, Double y);
		void GetLastPoint(Double *x, Double *y);
		Bool RemoveLastPoint();
	};
}
#endif
