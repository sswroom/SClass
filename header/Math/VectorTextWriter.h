#ifndef _SM_MATH_VECTORTEXTWRITER
#define _SM_MATH_VECTORTEXTWRITER
#include "Math/Vector2D.h"
#include "Text/CString.h"
#include "Text/String.h"
#include "Text/StringBuilderUTF8.h"

namespace Math
{
	class VectorTextWriter
	{
	public:
		virtual ~VectorTextWriter() {}

		virtual Text::CString GetWriterName() = 0;
		virtual Bool ToText(Text::StringBuilderUTF8 *sb, Math::Vector2D *vec) = 0;
		virtual Text::String *GetLastError() = 0;
	};
}
#endif