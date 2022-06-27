#ifndef _SM_MATH_POLYGON
#define _SM_MATH_POLYGON
#include "Math/PointCollection.h"
#include "Data/ArrayList.h"

namespace Math
{
	class Polygon : public Math::PointCollection
	{
	private:
		Math::Coord2DDbl *pointArr;
		UOSInt nPoint;
		UInt32 *ptOfstArr;
		UOSInt nPtOfst;
	public:
		Polygon(UInt32 srid, UOSInt nPtOfst, UOSInt nPoint);
		virtual ~Polygon();

		virtual VectorType GetVectorType() const;
		virtual UInt32 *GetPtOfstList(UOSInt *nPtOfst);
		virtual Math::Coord2DDbl *GetPointList(UOSInt *nPoint);
		virtual const Math::Coord2DDbl *GetPointListRead(UOSInt *nPoint) const;
		virtual Math::Vector2D *Clone() const;
		virtual void GetBounds(Math::RectAreaDbl *bounds) const;
		virtual Double CalSqrDistance(Math::Coord2DDbl pt, Math::Coord2DDbl *nearPt) const;
		virtual Bool JoinVector(Math::Vector2D *vec);
		virtual void ConvCSys(Math::CoordinateSystem *srcCSys, Math::CoordinateSystem *destCSys);
		virtual Bool Equals(Vector2D *vec) const;
		Bool InsideVector(Math::Coord2DDbl coord) const;
		Bool HasJunction() const;
		void SplitByJunction(Data::ArrayList<Math::Polygon*> *results);
	};
}
#endif
