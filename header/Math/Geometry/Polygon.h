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
			virtual NN<Math::Geometry::Vector2D> Clone() const;
			virtual Double CalBoundarySqrDistance(Math::Coord2DDbl pt, OutParam<Math::Coord2DDbl> nearPt) const;
			virtual Bool JoinVector(NN<const Math::Geometry::Vector2D> vec);
			virtual Bool InsideOrTouch(Math::Coord2DDbl coord) const;
//			Bool HasJunction() const;
//			void SplitByJunction(NN<Data::ArrayListNN<Math::Geometry::Polygon>> results);
			NN<Math::Geometry::MultiPolygon> CreateMultiPolygon() const;
			void AddFromPtOfst(UnsafeArray<UInt32> ptOfstList, UOSInt nPtOfst, UnsafeArray<Math::Coord2DDbl> pointList, UOSInt nPoint, UnsafeArrayOpt<Double> zList, UnsafeArrayOpt<Double> mList);
			UOSInt FillPointOfstList(UnsafeArray<Math::Coord2DDbl> pointList, UnsafeArray<UInt32> ptOfstList, UnsafeArrayOpt<Double> zList, UnsafeArrayOpt<Double> mList) const;
		};
	}
}
#endif
