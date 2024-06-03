#ifndef _SM_MATH_VECTORTEXTWRITER
#define _SM_MATH_VECTORTEXTWRITER
#include "Math/Geometry/Vector2D.h"
#include "Text/CString.h"
#include "Text/String.h"
#include "Text/StringBuilderUTF8.h"

namespace Math
{
	class VectorTextWriter
	{
	public:
		virtual ~VectorTextWriter() {}

		virtual Text::CStringNN GetWriterName() const = 0;
		virtual Bool ToText(NN<Text::StringBuilderUTF8> sb, NN<const Math::Geometry::Vector2D> vec) = 0;
		virtual Optional<Text::String> GetLastError() = 0;
	};
}
#endif
