#ifndef _SM_MATH_GEOMETRY_LINESTRING
#define _SM_MATH_GEOMETRY_LINESTRING
#include "Data/ArrayListA.h"
#include "Math/Coord2DDbl.h"
#include "Math/Geometry/PointCollection.h"

namespace Math
{
	namespace Geometry
	{
		class Polyline;
		class Polygon;
		class LineString : public PointCollection
		{
		protected:
			Double *zArr;
			Double *mArr;
		public:
			LineString(UInt32 srid, UOSInt nPoint, Bool hasZ, Bool hasM);
			LineString(UInt32 srid, const Math::Coord2DDbl *pointArr, UOSInt nPoint, Bool hasZ, Bool hasM);
			virtual ~LineString();

			virtual VectorType GetVectorType() const;
			virtual NotNullPtr<Math::Geometry::Vector2D> Clone() const;
			virtual Double CalBoundarySqrDistance(Math::Coord2DDbl pt, OutParam<Math::Coord2DDbl> nearPt) const;
			virtual Bool JoinVector(NotNullPtr<const Math::Geometry::Vector2D> vec);
			virtual Bool HasZ() const;
			virtual Bool HasM() const;
			virtual void ConvCSys(NotNullPtr<const Math::CoordinateSystem> srcCSys, NotNullPtr<const Math::CoordinateSystem> destCSys);
			virtual Bool Equals(NotNullPtr<const Vector2D> vec, Bool sameTypeOnly, Bool nearlyVal) const;

			Double CalcLength() const;

			Double *GetZList(OutParam<UOSInt> nPoint) const;
			Double *GetMList(OutParam<UOSInt> nPoint) const;
			UOSInt GetPointCount() const;
			Math::Geometry::LineString *SplitByPoint(Math::Coord2DDbl pt);
			OSInt GetPointNo(Math::Coord2DDbl pt, Bool *isPoint, Math::Coord2DDbl *calPt, Double *calZ, Double *calM);

			Math::Geometry::Polygon *CreatePolygonByDist(Double dist) const;
			NotNullPtr<Math::Geometry::Polyline> CreatePolyline() const;
		};
	}
}
#endif
