#ifndef _SM_MATH_POLYLINE
#define _SM_MATH_POLYLINE
#include "Math/Polygon.h"


namespace Math
{
	class Polyline : public Math::PointCollection
	{
	protected:
		Double *points;
		UOSInt nPoints;
		UInt32 *parts;
		UOSInt nParts;
		Int32 flags;
		Int32 color;
	public:
		Polyline(Int32 srid, Double *points, UOSInt nPoints);
		Polyline(Int32 srid, UOSInt nParts, UOSInt nPoints);
		virtual ~Polyline();

		virtual VectorType GetVectorType();
		virtual UInt32 *GetPartList(UOSInt *nParts);
		virtual Double *GetPointList(UOSInt *nPoints);
		virtual void GetCenter(Double *x, Double *y);
		virtual Math::Vector2D *Clone();
		virtual void GetBounds(Double *minX, Double *minY, Double *maxX, Double *maxY);
		virtual Double CalSqrDistance(Double x, Double y, Double *nearPtX, Double *nearPtY);
		virtual Bool JoinVector(Math::Vector2D *vec);
		virtual void ConvCSys(Math::CoordinateSystem *srcCSys, Math::CoordinateSystem *destCSys);

		virtual Math::Polyline *SplitByPoint(Double x, Double y);
		virtual void OptimizePolyline();
		OSInt GetPointNo(Double x, Double y, Bool *isPoint, Double *calPtX, Double *calPtY);

		Math::Polygon *CreatePolygonByDist(Double dist);
		Bool HasColor();
		Int32 GetColor();
		void SetColor(Int32 color);
	};
}
#endif
