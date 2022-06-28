#ifndef _SM_MATH_POINTCOLLECTION
#define _SM_MATH_POINTCOLLECTION
#include "Math/RectArea.h"
#include "Math/Vector2D.h"

namespace Math
{
	class PointCollection : public Math::Vector2D
	{
	protected:
		Math::Coord2DDbl *pointArr;
		UOSInt nPoint;
	public:
		PointCollection(UInt32 srid, UOSInt nPoint, Math::Coord2DDbl *pointArr);
		virtual ~PointCollection();

		Math::Coord2DDbl *GetPointList(UOSInt *nPoint) { *nPoint = this->nPoint; return this->pointArr; }
		const Math::Coord2DDbl *GetPointListRead(UOSInt *nPoint) const { *nPoint = this->nPoint; return this->pointArr; }

		virtual Math::Coord2DDbl GetCenter() const;
		virtual void GetBounds(Math::RectAreaDbl *bounds) const;
		virtual void ConvCSys(Math::CoordinateSystem *srcCSys, Math::CoordinateSystem *destCSys);
	};
}
#endif
