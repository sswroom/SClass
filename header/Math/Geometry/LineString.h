#ifndef _SM_MATH_GEOMETRY_LINESTRING
#define _SM_MATH_GEOMETRY_LINESTRING
#include "Math/Coord2DDbl.h"
#include "Math/Geometry/Polygon.h"
#include "Math/Geometry/Polyline.h"
#include "Math/Geometry/PointCollection.h"

namespace Math
{
	namespace Geometry
	{
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
			virtual Math::Geometry::Vector2D *Clone() const;
			virtual Double CalSqrDistance(Math::Coord2DDbl pt, Math::Coord2DDbl *nearPt) const;
			virtual Bool JoinVector(Math::Geometry::Vector2D *vec);
			virtual Bool HasZ() const;
			virtual Bool HasM() const;
			virtual void ConvCSys(Math::CoordinateSystem *srcCSys, Math::CoordinateSystem *destCSys);
			virtual Bool Equals(Vector2D *vec) const;

			Double *GetZList(UOSInt *nPoint) const;
			Double *GetMList(UOSInt *nPoint) const;
			Math::Geometry::LineString *SplitByPoint(Math::Coord2DDbl pt);
			OSInt GetPointNo(Math::Coord2DDbl pt, Bool *isPoint, Math::Coord2DDbl *calPt, Double *calZ, Double *calM);

			Math::Geometry::Polygon *CreatePolygonByDist(Double dist) const;
			Math::Geometry::Polyline *CreatePolyline() const;
		};
	}
}
#endif
