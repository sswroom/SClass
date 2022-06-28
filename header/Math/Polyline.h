#ifndef _SM_MATH_POLYLINE
#define _SM_MATH_POLYLINE
#include "Math/Polygon.h"


namespace Math
{
	class Polyline : public Math::PointOfstCollection
	{
	protected:
		Int32 flags;
		UInt32 color;
	public:
		Polyline(UInt32 srid, Math::Coord2DDbl *pointArr, UOSInt nPoint);
		Polyline(UInt32 srid, UOSInt nPtOfst, UOSInt nPoint);
		virtual ~Polyline();

		virtual VectorType GetVectorType() const;
		virtual Math::Vector2D *Clone() const;
		virtual Double CalSqrDistance(Math::Coord2DDbl pt, Math::Coord2DDbl *nearPt) const;
		virtual Bool JoinVector(Math::Vector2D *vec);
		virtual Bool Equals(Vector2D *vec) const;

		virtual Math::Polyline *SplitByPoint(Math::Coord2DDbl pt);
		virtual void OptimizePolyline();
		OSInt GetPointNo(Math::Coord2DDbl pt, Bool *isPoint, Math::Coord2DDbl *calPt);

		Math::Polygon *CreatePolygonByDist(Double dist) const;
		Bool HasColor() const;
		UInt32 GetColor() const;
		void SetColor(UInt32 color);
	};
}
#endif
