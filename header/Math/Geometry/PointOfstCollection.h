#ifndef _SM_MATH_GEOMETRY_POINTOFSTCOLLECTION
#define _SM_MATH_GEOMETRY_POINTOFSTCOLLECTION
#include "Math/Geometry/PointCollection.h"

namespace Math
{
	namespace Geometry
	{
		class PointOfstCollection : public Math::Geometry::PointCollection
		{
		protected:
			UInt32 *ptOfstArr;
			UOSInt nPtOfst;
			Double *zArr;
			Double *mArr;
		public:
			PointOfstCollection(UInt32 srid, UOSInt nPtOfst, UOSInt nPoint, Math::Coord2DDbl *pointArr, Bool hasZ, Bool hasM);
			virtual ~PointOfstCollection();

			UInt32 *GetPtOfstList(UOSInt *nPtOfst) { *nPtOfst = this->nPtOfst; return this->ptOfstArr; }
			virtual Math::Coord2DDbl GetCenter() const;
			virtual void ConvCSys(Math::CoordinateSystem *srcCSys, Math::CoordinateSystem *destCSys);
			virtual Bool Equals(Vector2D *vec) const;
			virtual Bool HasZ() const;
			Double *GetZList(UOSInt *nPoint);
			virtual Bool HasM() const;
			Double *GetMList(UOSInt *nPoint);
		};
	}
}
#endif