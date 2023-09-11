#ifndef _SM_MATH_GEOMETRY_POINTCOLLECTION
#define _SM_MATH_GEOMETRY_POINTCOLLECTION
#include "Math/RectArea.h"
#include "Math/Geometry/Vector2D.h"

namespace Math
{
	namespace Geometry
	{
		class PointCollection : public Math::Geometry::Vector2D
		{
		protected:
			Math::Coord2DDbl *pointArr;
			UOSInt nPoint;
		public:
			PointCollection(UInt32 srid, UOSInt nPoint, const Math::Coord2DDbl *pointArr);
			virtual ~PointCollection();

			Math::Coord2DDbl *GetPointList(OutParam<UOSInt> nPoint) { nPoint.Set(this->nPoint); return this->pointArr; }
			const Math::Coord2DDbl *GetPointListRead(OutParam<UOSInt> nPoint) const { nPoint.Set(this->nPoint); return this->pointArr; }

			virtual Math::Coord2DDbl GetCenter() const;
			virtual Math::RectAreaDbl GetBounds() const;
			virtual void ConvCSys(NotNullPtr<const Math::CoordinateSystem> srcCSys, NotNullPtr<const Math::CoordinateSystem> destCSys);
			virtual UOSInt GetCoordinates(NotNullPtr<Data::ArrayListA<Math::Coord2DDbl>> coordList) const;
			virtual void SwapXY();
			virtual void MultiplyCoordinatesXY(Double v);
		};
	}
}
#endif
