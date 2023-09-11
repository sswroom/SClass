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
			virtual Math::Geometry::Vector2D *Clone() const;
			virtual Math::RectAreaDbl GetBounds() const;
			virtual Double CalBoundarySqrDistance(Math::Coord2DDbl pt, Math::Coord2DDbl *nearPt) const;
			virtual Bool JoinVector(Math::Geometry::Vector2D *vec);
			virtual Bool HasZ() const;
			virtual void ConvCSys(NotNullPtr<Math::CoordinateSystem> srcCSys, NotNullPtr<Math::CoordinateSystem> destCSys);
			virtual Bool Equals(Math::Geometry::Vector2D *vec) const;
			virtual Bool EqualsNearly(Math::Geometry::Vector2D *vec) const;
			virtual UOSInt GetCoordinates(Data::ArrayListA<Math::Coord2DDbl> *coordList) const;
			virtual Bool InsideVector(Math::Coord2DDbl coord) const;
			virtual void SwapXY();
			virtual void MultiplyCoordinatesXY(Double v);

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
