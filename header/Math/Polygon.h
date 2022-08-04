#ifndef _SM_MATH_POLYGON
#define _SM_MATH_POLYGON
#include "Math/PointOfstCollection.h"
#include "Data/ArrayList.h"

namespace Math
{
	class Polygon : public Math::PointOfstCollection
	{
	public:
		Polygon(UInt32 srid, UOSInt nPtOfst, UOSInt nPoint, Bool hasZ, Bool hasM);
		virtual ~Polygon();

		virtual VectorType GetVectorType() const;
		virtual Math::Vector2D *Clone() const;
		virtual Double CalSqrDistance(Math::Coord2DDbl pt, Math::Coord2DDbl *nearPt) const;
		virtual Bool JoinVector(Math::Vector2D *vec);
		Bool InsideVector(Math::Coord2DDbl coord) const;
		Bool HasJunction() const;
		void SplitByJunction(Data::ArrayList<Math::Polygon*> *results);
	};
}
#endif
