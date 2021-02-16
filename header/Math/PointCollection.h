#ifndef _SM_MATH_POINTCOLLECTION
#define _SM_MATH_POINTCOLLECTION
#include "Math/Vector2D.h"

namespace Math
{
	class PointCollection : public Math::Vector2D
	{
	public:
		PointCollection(Int32 srid);
		virtual ~PointCollection();

		virtual UInt32 *GetPtOfstList(UOSInt *nPtOfst) = 0;
		virtual Double *GetPointList(UOSInt *nPoint) = 0;
		virtual void GetCenter(Double *x, Double *y);
	};
}
#endif
