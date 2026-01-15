#ifndef _SM_MATH_MSGEOGRAPHY
#define _SM_MATH_MSGEOGRAPHY
#include "Math/Geometry/Vector2D.h"

namespace Math
{
	class MSGeography
	{
	public:
		static Optional<Math::Geometry::Vector2D> ParseBinary(const UInt8 *buffPtr, UIntOS buffSize, OptOut<UInt32> srIdOut);
	};
}
#endif
