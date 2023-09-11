#ifndef _SM_MATH_GEOMETRY_POLYLINE
#define _SM_MATH_GEOMETRY_POLYLINE
#include "Math/Geometry/Polygon.h"


namespace Math
{
	namespace Geometry
	{
		class Polyline : public Math::Geometry::PointOfstCollection
		{
		protected:
			Int32 flags;
			UInt32 color;
		public:
			Polyline(UInt32 srid, const Math::Coord2DDbl *pointArr, UOSInt nPoint, Bool hasZ, Bool hasM);
			Polyline(UInt32 srid, UOSInt nPtOfst, UOSInt nPoint, Bool hasZ, Bool hasM);
			virtual ~Polyline();

			virtual VectorType GetVectorType() const;
			virtual NotNullPtr<Math::Geometry::Vector2D> Clone() const;
			virtual Double CalBoundarySqrDistance(Math::Coord2DDbl pt, OutParam<Math::Coord2DDbl> nearPt) const;
			virtual Bool JoinVector(NotNullPtr<const Math::Geometry::Vector2D> vec);

			Math::Geometry::Polyline *SplitByPoint(Math::Coord2DDbl pt);
			virtual void OptimizePolyline();
			OSInt GetPointNo(Math::Coord2DDbl pt, Bool *isPoint, Math::Coord2DDbl *calPt, Double *calZ, Double *calM);

			Math::Geometry::Polygon *CreatePolygonByDist(Double dist) const;
			Bool HasColor() const;
			UInt32 GetColor() const;
			void SetColor(UInt32 color);
		};
	}
}
#endif
