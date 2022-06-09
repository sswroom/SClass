#ifndef _SM_MATH_POLYLINE
#define _SM_MATH_POLYLINE
#include "Math/Polygon.h"


namespace Math
{
	class Polyline : public Math::PointCollection
	{
	protected:
		Math::Coord2DDbl *pointArr;
		UOSInt nPoint;
		UInt32 *ptOfstArr;
		UOSInt nPtOfst;
		Int32 flags;
		UInt32 color;
	public:
		Polyline(UInt32 srid, Math::Coord2DDbl *pointArr, UOSInt nPoint);
		Polyline(UInt32 srid, UOSInt nPtOfst, UOSInt nPoint);
		virtual ~Polyline();

		virtual VectorType GetVectorType();
		virtual UInt32 *GetPtOfstList(UOSInt *nPtOfst);
		virtual Math::Coord2DDbl *GetPointList(UOSInt *nPoint);
		virtual Math::Coord2DDbl GetCenter();
		virtual Math::Vector2D *Clone();
		virtual void GetBounds(Math::RectAreaDbl *bounds);
		virtual Double CalSqrDistance(Math::Coord2DDbl pt, Math::Coord2DDbl *nearPt);
		virtual Bool JoinVector(Math::Vector2D *vec);
		virtual void ConvCSys(Math::CoordinateSystem *srcCSys, Math::CoordinateSystem *destCSys);
		virtual Bool Equals(Vector2D *vec);

		virtual Math::Polyline *SplitByPoint(Math::Coord2DDbl pt);
		virtual void OptimizePolyline();
		OSInt GetPointNo(Math::Coord2DDbl pt, Bool *isPoint, Math::Coord2DDbl *calPt);

		Math::Polygon *CreatePolygonByDist(Double dist);
		Bool HasColor();
		UInt32 GetColor();
		void SetColor(UInt32 color);
	};
}
#endif
