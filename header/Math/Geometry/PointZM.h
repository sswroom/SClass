#ifndef _SM_MATH_GEOMETRY_POINTZM
#define _SM_MATH_GEOMETRY_POINTZM
#include "Math/Geometry/PointZ.h"

namespace Math
{
	namespace Geometry
	{
		class PointZM : public PointZ
		{
		protected:
			Double m;
		public:
			PointZM(UInt32 srid, Double x, Double y, Double z, Double m);
			virtual ~PointZM();

			virtual Vector2D *Clone() const;
			Double GetM() const;
			virtual Bool HasM() const;
			virtual Bool Equals(Vector2D *vec) const;
		};
	}
}
#endif
