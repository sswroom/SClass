#ifndef _SM_MATH_GEOMETRY_MULTICURVE
#define _SM_MATH_GEOMETRY_MULTICURVE
#include "Math/Geometry/MultiGeometry.hpp"

namespace Math
{
	namespace Geometry
	{
		class MultiCurve : public MultiGeometry<Vector2D>
		{
		public:
			MultiCurve(UInt32 srid);
			virtual ~MultiCurve();

			virtual void AddGeometry(NN<Vector2D> geometry);
			virtual Vector2D::VectorType GetVectorType() const;
			virtual NN<Vector2D> Clone() const;
			virtual Bool HasCurve() const { return true; }
			virtual Optional<Vector2D> ToSimpleShape() const;
		};
	}
}
#endif
