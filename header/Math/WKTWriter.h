#ifndef _SM_MATH_WKTWRITER
#define _SM_MATH_WKTWRITER
#include "Math/VectorTextWriter.h"

namespace Math
{
	class WKTWriter : public VectorTextWriter
	{
	private:
		Text::String *lastError;

		void SetLastError(Text::CString lastError);
	public:
		WKTWriter();
		virtual ~WKTWriter();

		virtual Text::CString GetWriterName();
		virtual Bool ToText(Text::StringBuilderUTF8 *sb, Math::Vector2D *vec);
		virtual Text::String *GetLastError();
	};
}
#endif
