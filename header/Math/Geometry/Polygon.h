#ifndef _SM_MATH_GEOMETRY_POLYGON
#define _SM_MATH_GEOMETRY_POLYGON
#include "Data/ArrayList.h"
#include "Math/Geometry/PointOfstCollection.h"

namespace Math
{
	namespace Geometry
	{
		class Polygon : public Math::Geometry::PointOfstCollection
		{
		public:
			Polygon(UInt32 srid, UOSInt nPtOfst, UOSInt nPoint, Bool hasZ, Bool hasM);
			virtual ~Polygon();

			virtual VectorType GetVectorType() const;
			virtual Math::Geometry::Vector2D *Clone() const;
			virtual Double CalBoundarySqrDistance(Math::Coord2DDbl pt, Math::Coord2DDbl *nearPt) const;
			virtual Double CalSqrDistance(Math::Coord2DDbl pt, Math::Coord2DDbl *nearPt) const;
			virtual Bool JoinVector(Math::Geometry::Vector2D *vec);
			Bool InsideVector(Math::Coord2DDbl coord) const;
			Bool HasJunction() const;
			void SplitByJunction(Data::ArrayList<Math::Geometry::Polygon*> *results);
		};
	}
}
#endif
