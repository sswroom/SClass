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
			virtual Vector2D *Clone() const;
			virtual void GetBounds(Math::RectAreaDbl* bounds) const;
			virtual Double CalBoundarySqrDistance(Math::Coord2DDbl pt, Math::Coord2DDbl* nearPt) const;
			virtual Bool JoinVector(Vector2D *vec);
			virtual Bool HasZ() const;
			virtual void ConvCSys(Math::CoordinateSystem *srcCSys, Math::CoordinateSystem *destCSys);
			virtual Bool Equals(Vector2D *vec) const;

			Double GetCX() const;
			Double GetCY() const;
			Double GetR() const;
			Double GetArcAngle1() const;
			Double GetArcAngle2() const;
		};
	}
}
#endif
