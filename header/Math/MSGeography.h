#ifndef _SM_MATH_MSGEOGRAPHY
#define _SM_MATH_MSGEOGRAPHY
#include "Math/Vector2D.h"

namespace Math
{
	class MSGeography
	{
	public:
		static Math::Vector2D *ParseBinary(const UInt8 *buffPtr, UOSInt buffSize, UInt32 *srIdOut);
	};
}
#endif
