#ifndef _SM_MATH_WKTWRITER
#define _SM_MATH_WKTWRITER
#include "Math/Vector2D.h"
#include "Text/StringBuilderUTF.h"

namespace Math
{
	class WKTWriter
	{
	private:
		const UTF8Char *lastError;

		void SetLastError(const UTF8Char *lastError);
	public:
		WKTWriter();
		~WKTWriter();

		Bool GenerateWKT(Text::StringBuilderUTF *sb, Math::Vector2D *vec);
		const UTF8Char *GetLastError();
	};
}
#endif
