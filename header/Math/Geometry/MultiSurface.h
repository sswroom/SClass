#ifndef _SM_MATH_GEOMETRY_MULTISURFACE
#define _SM_MATH_GEOMETRY_MULTISURFACE
#include "Math/Geometry/MultiGeometry.hpp"

namespace Math
{
	namespace Geometry
	{
		class MultiSurface : public MultiGeometry<Vector2D>
		{
		public:
			MultiSurface(UInt32 srid);
			virtual ~MultiSurface();

			virtual void AddGeometry(NN<Vector2D> geometry);
			virtual Vector2D::VectorType GetVectorType() const;
			virtual NN<Vector2D> Clone() const;
			virtual Bool HasCurve() const { return true; }
			virtual Optional<Vector2D> ToSimpleShape() const;
		};
	}
}
#endif
