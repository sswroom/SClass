#ifndef _SM_MATH_POLYGON
#define _SM_MATH_POLYGON
#include "Math/PointCollection.h"
#include "Data/ArrayList.h"

namespace Math
{
	class Polygon : public Math::PointCollection
	{
	private:
		Double *points;
		UOSInt nPoints;
		UInt32 *parts; //pointPos List
		UOSInt nParts;
	public:
		Polygon(Int32 srid, UOSInt nParts, UOSInt nPoints);
		virtual ~Polygon();

		virtual VectorType GetVectorType();
		virtual UInt32 *GetPartList(UOSInt *nParts);
		virtual Double *GetPointList(UOSInt *nPoints);
		virtual Math::Vector2D *Clone();
		virtual void GetBounds(Double *minX, Double *minY, Double *maxX, Double *maxY);
		virtual Double CalSqrDistance(Double x, Double y, Double *nearPtX, Double *nearPtY);
		virtual Bool JoinVector(Math::Vector2D *vec);
		virtual void ConvCSys(Math::CoordinateSystem *srcCSys, Math::CoordinateSystem *destCSys);
		Bool InsideVector(Double x, Double y);
		Bool HasJunction();
		void SplitByJunction(Data::ArrayList<Math::Polygon*> *results);
	};
}
#endif
