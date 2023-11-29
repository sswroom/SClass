#ifndef _SM_MATH_GEOMETRY_POLYGON
#define _SM_MATH_GEOMETRY_POLYGON
#include "Data/ArrayList.h"
#include "Math/Geometry/LinearRing.h"
#include "Math/Geometry/MultiGeometry.h"

namespace Math
{
	namespace Geometry
	{
		class MultiPolygon;
		class Polygon : public Math::Geometry::MultiGeometry<LinearRing>
		{
		public:
			Polygon(UInt32 srid);
			virtual ~Polygon();

			virtual VectorType GetVectorType() const;
			virtual NotNullPtr<Math::Geometry::Vector2D> Clone() const;
			virtual Double CalBoundarySqrDistance(Math::Coord2DDbl pt, OutParam<Math::Coord2DDbl> nearPt) const;
			virtual Bool JoinVector(NotNullPtr<const Math::Geometry::Vector2D> vec);
			virtual Bool InsideOrTouch(Math::Coord2DDbl coord) const;
/*			Bool HasJunction() const;
			void SplitByJunction(Data::ArrayList<Math::Geometry::Polygon*> *results);*/
			NotNullPtr<Math::Geometry::MultiPolygon> CreateMultiPolygon() const;
			void AddFromPtOfst(UInt32 *ptOfstList, UOSInt nPtOfst, Math::Coord2DDbl *pointList, UOSInt nPoint, Double *zList, Double *mList);
			UOSInt FillPointOfstList(Math::Coord2DDbl *pointList, UInt32 *ptOfstList, Double *zList, Double *mList) const;
		};
	}
}
#endif
