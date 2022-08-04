#ifndef _SM_MATH_POINTM
#define _SM_MATH_POINTM
#include "Math/Point.h"

namespace Math
{
	class PointM : public Math::Point
	{
	protected:
		Double m;
	public:
		PointM(UInt32 srid, Double x, Double y, Double m);
		virtual ~PointM();

		virtual Math::Vector2D *Clone() const;
		Double GetM() const;
		virtual Bool HasM() const;
		virtual Bool Equals(Math::Vector2D *vec) const;
	};
}
#endif
