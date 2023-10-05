#ifndef _SM_MATH_GEOMETRY_POINT
#define _SM_MATH_GEOMETRY_POINT
#include "Math/Coord2DDbl.h"
#include "Math/Geometry/Vector2D.h"

namespace Math
{
	namespace Geometry
	{
		class Point : public Vector2D
		{
		protected:
			Math::Coord2DDbl pos;
		public:
			Point(UInt32 srid, Double x, Double y);
			Point(UInt32 srid, Math::Coord2DDbl pos);
			virtual ~Point();

			virtual VectorType GetVectorType() const;
			virtual Math::Coord2DDbl GetCenter() const;
			virtual NotNullPtr<Vector2D> Clone() const;
			virtual Math::RectAreaDbl GetBounds() const;
			virtual Double CalBoundarySqrDistance(Math::Coord2DDbl pt, OutParam<Math::Coord2DDbl> nearPt) const;
			virtual Bool JoinVector(NotNullPtr<const Vector2D> vec);
			virtual void ConvCSys(NotNullPtr<const Math::CoordinateSystem> srcCSys, NotNullPtr<const Math::CoordinateSystem> destCSys);
			virtual Bool Equals(NotNullPtr<const Vector2D> vec, Bool sameTypeOnly, Bool nearlyVal) const;
			virtual UOSInt GetCoordinates(NotNullPtr<Data::ArrayListA<Math::Coord2DDbl>> coordList) const;
			virtual void SwapXY();
			virtual void MultiplyCoordinatesXY(Double v);
		};
	}
}
#endif
