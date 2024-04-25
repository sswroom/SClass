#ifndef _SM_MATH_WKBWRITER
#define _SM_MATH_WKBWRITER
#include "IO/Stream.h"
#include "Math/Geometry/Vector2D.h"

namespace Math
{
	class WKBWriter
	{
	private:
		Bool isoMode;
	public:
		WKBWriter(Bool isoMode);
		~WKBWriter();

		Bool Write(NN<IO::Stream> stm, NN<Math::Geometry::Vector2D> vec);
	};
}
#endif
