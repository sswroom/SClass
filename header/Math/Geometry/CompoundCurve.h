#ifndef _SM_MATH_GEOMETRY_COMPOUNDCURVE
#define _SM_MATH_GEOMETRY_COMPOUNDCURVE
#include "Math/Geometry/LineString.h"
#include "Math/Geometry/MultiGeometry.h"

namespace Math
{
	namespace Geometry
	{
		class CompoundCurve : public MultiGeometry<LineString>
		{
		public:
			CompoundCurve(UInt32 srid, Bool hasZ, Bool hasM);
			virtual ~CompoundCurve();

			virtual void AddGeometry(LineString *geometry);
			virtual Vector2D::VectorType GetVectorType() const;
			virtual Vector2D *Clone() const;
		};
	}
}
#endif
