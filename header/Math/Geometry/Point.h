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
			virtual Vector2D *Clone() const;
			virtual void GetBounds(Math::RectAreaDbl *bounds) const;
			virtual Double CalSqrDistance(Math::Coord2DDbl pt, Math::Coord2DDbl *nearPt) const;
			virtual Bool JoinVector(Vector2D *vec);
			virtual void ConvCSys(Math::CoordinateSystem *srcCSys, Math::CoordinateSystem *destCSys);
			virtual Bool Equals(Vector2D *vec) const;
		};
	}
}
#endif
