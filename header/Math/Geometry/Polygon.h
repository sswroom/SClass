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
			virtual NotNullPtr<Math::Geometry::Vector2D> Clone() const;
			virtual Double CalBoundarySqrDistance(Math::Coord2DDbl pt, OutParam<Math::Coord2DDbl> nearPt) const;
			virtual Bool JoinVector(NotNullPtr<const Math::Geometry::Vector2D> vec);
			virtual Bool InsideVector(Math::Coord2DDbl coord) const;
			Bool HasJunction() const;
			void SplitByJunction(Data::ArrayList<Math::Geometry::Polygon*> *results);
		};
	}
}
#endif
