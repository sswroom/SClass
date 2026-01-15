#ifndef _SM_MATH_GEOMETRY_COMPOUNDCURVE
#define _SM_MATH_GEOMETRY_COMPOUNDCURVE
#include "Math/Geometry/LineString.h"
#include "Math/Geometry/MultiGeometry.hpp"

namespace Math
{
	namespace Geometry
	{
		class CompoundCurve : public MultiGeometry<LineString>
		{
		public:
			CompoundCurve(UInt32 srid);
			virtual ~CompoundCurve();

			virtual void AddGeometry(NN<LineString> geometry);
			virtual Vector2D::VectorType GetVectorType() const;
			virtual NN<Vector2D> Clone() const;
			UIntOS GetDrawPoints(NN<Data::ArrayListA<Math::Coord2DDbl>> ptList) const;
			virtual Bool HasCurve() const { return true; }
			virtual Optional<Vector2D> ToSimpleShape() const;
		};
	}
}
#endif
