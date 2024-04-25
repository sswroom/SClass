#ifndef _SM_MATH_GEOMETRY_LINESTRING
#define _SM_MATH_GEOMETRY_LINESTRING
#include "Data/ArrayListA.h"
#include "Math/Coord2DDbl.h"
#include "Math/Geometry/Vector2D.h"

namespace Math
{
	namespace Geometry
	{
		class Polyline;
		class Polygon;
		class LineString : public Vector2D
		{
		protected:
			Math::Coord2DDbl *pointArr;
			UOSInt nPoint;
			Double *zArr;
			Double *mArr;
		public:
			LineString(UInt32 srid, UOSInt nPoint, Bool hasZ, Bool hasM);
			LineString(UInt32 srid, const Math::Coord2DDbl *pointArr, UOSInt nPoint, Double *zArr, Double *mArr);
			virtual ~LineString();

			virtual VectorType GetVectorType() const;
			virtual Math::Coord2DDbl GetCenter() const;
			virtual NN<Math::Geometry::Vector2D> Clone() const;
			virtual Math::RectAreaDbl GetBounds() const;
			virtual Double CalBoundarySqrDistance(Math::Coord2DDbl pt, OutParam<Math::Coord2DDbl> nearPt) const;
			virtual Double CalArea() const;
			virtual Bool JoinVector(NN<const Math::Geometry::Vector2D> vec);
			virtual Bool HasZ() const;
			virtual Bool HasM() const;
			virtual Bool GetZBounds(OutParam<Double> min, OutParam<Double> max) const;
			virtual Bool GetMBounds(OutParam<Double> min, OutParam<Double> max) const;
			virtual void Convert(NN<Math::CoordinateConverter> converter);
			virtual Bool Equals(NN<const Vector2D> vec, Bool sameTypeOnly, Bool nearlyVal) const;
			virtual UOSInt GetCoordinates(NN<Data::ArrayListA<Math::Coord2DDbl>> coordList) const;
			virtual Bool InsideOrTouch(Math::Coord2DDbl coord) const;
			virtual void SwapXY();
			virtual void MultiplyCoordinatesXY(Double v);
			virtual UOSInt GetPointCount() const;

			Math::Coord2DDbl *GetPointList(OutParam<UOSInt> nPoint) { nPoint.Set(this->nPoint); return this->pointArr; }
			const Math::Coord2DDbl *GetPointListRead(OutParam<UOSInt> nPoint) const { nPoint.Set(this->nPoint); return this->pointArr; }
			Math::Coord2DDbl GetPoint(UOSInt index) const;
			Double CalcLength() const;

			Double *GetZList(OutParam<UOSInt> nPoint) const;
			Double *GetMList(OutParam<UOSInt> nPoint) const;
			Math::Geometry::LineString *SplitByPoint(Math::Coord2DDbl pt);
			OSInt GetPointNo(Math::Coord2DDbl pt, Bool *isPoint, Math::Coord2DDbl *calPt, Double *calZ, Double *calM);

			Math::Geometry::Polygon *CreatePolygonByDist(Double dist) const;
			NN<Math::Geometry::Polyline> CreatePolyline() const;
		};
	}
}
#endif
