#ifndef _SM_MATH_POINTCOLLECTION
#define _SM_MATH_POINTCOLLECTION
#include "Math/RectArea.h"
#include "Math/Vector2D.h"

namespace Math
{
	class PointCollection : public Math::Vector2D
	{
	public:
		PointCollection(UInt32 srid);
		virtual ~PointCollection();

		virtual UInt32 *GetPtOfstList(UOSInt *nPtOfst) = 0;
		virtual Math::Coord2D<Double> *GetPointList(UOSInt *nPoint) = 0;
		virtual Math::Coord2D<Double> GetCenter();
	};
}
#endif
