#ifndef _SM_MATH_WKBWRITER
#define _SM_MATH_WKBWRITER
#include "IO/Stream.h"
#include "Math/Geometry/Vector2D.h"

namespace Math
{
	class WKBWriter
	{
	public:
		WKBWriter();
		~WKBWriter();

		Bool Write(NotNullPtr<IO::Stream> stm, NotNullPtr<Math::Geometry::Vector2D> vec);
	};
}
#endif