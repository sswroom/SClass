#ifndef _SM_MATH_POLYGON
#define _SM_MATH_POLYGON
#include "Math/PointCollection.h"
#include "Data/ArrayList.h"

namespace Math
{
	class Polygon : public Math::PointCollection
	{
	private:
		Double *pointArr;
		UOSInt nPoint;
		UInt32 *ptOfstArr;
		UOSInt nPtOfst;
	public:
		Polygon(UInt32 srid, UOSInt nPtOfst, UOSInt nPoint);
		virtual ~Polygon();

		virtual VectorType GetVectorType();
		virtual UInt32 *GetPtOfstList(UOSInt *nPtOfst);
		virtual Double *GetPointList(UOSInt *nPoint);
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
