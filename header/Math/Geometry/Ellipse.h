#ifndef _SM_MATH_GEOMETRY_ELLIPSE
#define _SM_MATH_GEOMETRY_ELLIPSE
#include "Math/Geometry/Vector2D.h"

namespace Math
{
	namespace Geometry
	{
		class Ellipse : public Vector2D
		{
		private:
			Math::Coord2DDbl tl;
			Math::Size2DDbl size;
			
		public:
			Ellipse(UInt32 srid, Math::Coord2DDbl tl, Math::Size2DDbl size);
			virtual ~Ellipse();

			virtual VectorType GetVectorType() const;
			virtual Math::Coord2DDbl GetCenter() const;
			virtual NN<Math::Geometry::Vector2D> Clone() const;
			virtual Math::RectAreaDbl GetBounds() const;
			virtual Double CalBoundarySqrDistance(Math::Coord2DDbl pt, OutParam<Math::Coord2DDbl> nearPt) const;
			virtual Double CalArea() const;
			virtual Bool JoinVector(NN<const Math::Geometry::Vector2D> vec);
			virtual Bool HasZ() const;
			virtual Bool GetZBounds(OutParam<Double> min, OutParam<Double> max) const;
			virtual Bool GetMBounds(OutParam<Double> min, OutParam<Double> max) const;
			virtual void Convert(NN<Math::CoordinateConverter> converter);
			virtual Bool Equals(NN<const Math::Geometry::Vector2D> vec, Bool sameTypeOnly, Bool nearlyVal, Bool no3DGeometry) const;
			virtual UOSInt GetCoordinates(NN<Data::ArrayListA<Math::Coord2DDbl>> coordList) const;
			virtual Bool InsideOrTouch(Math::Coord2DDbl coord) const;
			virtual void SwapXY();
			virtual void MultiplyCoordinatesXY(Double v);
			virtual UOSInt GetPointCount() const;
			virtual Bool HasArea() const;
			virtual UOSInt CalcHIntersacts(Double y, NN<Data::ArrayList<Double>> xList) const;
			virtual Math::Coord2DDbl GetDisplayCenter() const;

			Math::Coord2DDbl GetTL();
			Math::Coord2DDbl GetBR();
			Double GetLeft();
			Double GetTop();
			Double GetWidth();
			Double GetHeight();
		};
	}
}
#endif
