#ifndef _SM_MATH_POINTOFSTCOLLECTION
#define _SM_MATH_POINTOFSTCOLLECTION
#include "Math/PointCollection.h"

namespace Math
{
	class PointOfstCollection : public Math::PointCollection
	{
	protected:
		UInt32 *ptOfstArr;
		UOSInt nPtOfst;
	public:
		PointOfstCollection(UInt32 srid, UOSInt nPtOfst, UOSInt nPoint, Math::Coord2DDbl *pointArr);
		virtual ~PointOfstCollection();

		UInt32 *GetPtOfstList(UOSInt *nPtOfst) { *nPtOfst = this->nPtOfst; return this->ptOfstArr; }
		virtual Math::Coord2DDbl GetCenter() const;
	};
}
#endif
