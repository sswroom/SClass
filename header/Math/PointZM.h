#ifndef _SM_MATH_POINTZM
#define _SM_MATH_POINTZM
#include "Math/PointZ.h"

namespace Math
{
	class PointZM : public Math::PointZ
	{
	protected:
		Double m;
	public:
		PointZM(UInt32 srid, Double x, Double y, Double z, Double m);
		virtual ~PointZM();

		virtual Math::Vector2D *Clone() const;
		Double GetM() const;
		virtual Bool HasM() const;
		virtual Bool Equals(Math::Vector2D *vec) const;
	};
}
#endif
