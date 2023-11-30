#ifndef _SM_MATH_GEOMETRY_PIEAREA
#define _SM_MATH_GEOMETRY_PIEAREA
#include "Math/Geometry/Vector2D.h"

namespace Math
{
	namespace Geometry
	{
		class PieArea : public Vector2D
		{
		private:
			Math::Coord2DDbl center;
			Double r;
			Double arcAngle1;
			Double arcAngle2;
			
		public:
			PieArea(UInt32 srid, Math::Coord2DDbl center, Double r, Double arcAngle1, Double arcAngle2);
			virtual ~PieArea();

			virtual VectorType GetVectorType() const;
			virtual Math::Coord2DDbl GetCenter() const;
			virtual NotNullPtr<Vector2D> Clone() const;
			virtual Math::RectAreaDbl GetBounds() const;
			virtual Double CalBoundarySqrDistance(Math::Coord2DDbl pt, OutParam<Math::Coord2DDbl> nearPt) const;
			virtual Bool JoinVector(NotNullPtr<const Vector2D> vec);
			virtual Bool HasZ() const;
			virtual void Convert(NotNullPtr<Math::CoordinateConverter> converter);
			virtual Bool Equals(NotNullPtr<const Vector2D> vec, Bool sameTypeOnly, Bool nearlyVal) const;
			virtual UOSInt GetCoordinates(NotNullPtr<Data::ArrayListA<Math::Coord2DDbl>> coordList) const;
			virtual void SwapXY();
			virtual void MultiplyCoordinatesXY(Double v);

			Double GetCX() const;
			Double GetCY() const;
			Double GetR() const;
			Double GetArcAngle1() const;
			Double GetArcAngle2() const;
		};
	}
}
#endif
