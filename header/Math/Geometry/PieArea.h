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
			virtual NN<Vector2D> Clone() const;
			virtual Math::RectAreaDbl GetBounds() const;
			virtual Double CalBoundarySqrDistance(Math::Coord2DDbl pt, OutParam<Math::Coord2DDbl> nearPt) const;
			virtual Double CalArea() const;
			virtual Bool JoinVector(NN<const Vector2D> vec);
			virtual Bool HasZ() const;
			virtual Bool GetZBounds(OutParam<Double> min, OutParam<Double> max) const;
			virtual Bool GetMBounds(OutParam<Double> min, OutParam<Double> max) const;
			virtual void Convert(NN<Math::CoordinateConverter> converter);
			virtual Bool Equals(NN<const Vector2D> vec, Bool sameTypeOnly, Bool nearlyVal, Bool no3DGeometry) const;
			virtual UIntOS GetCoordinates(NN<Data::ArrayListA<Math::Coord2DDbl>> coordList) const;
			virtual Bool InsideOrTouch(Math::Coord2DDbl coord) const;
			virtual void SwapXY();
			virtual void MultiplyCoordinatesXY(Double v);
			virtual UIntOS GetPointCount() const;
			virtual Bool HasArea() const;
			virtual UIntOS CalcHIntersacts(Double y, NN<Data::ArrayListNative<Double>> xList) const;
			virtual Math::Coord2DDbl GetDisplayCenter() const;

			Double GetCX() const;
			Double GetCY() const;
			Double GetR() const;
			Double GetArcAngle1() const;
			Double GetArcAngle2() const;
		};
	}
}
#endif
