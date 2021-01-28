#ifndef _SM_TEXT_STRINGBUILDERW
#define _SM_TEXT_STRINGBUILDERW
#include "Text/StringBuilderUTF16.h"
#include "Text/StringBuilderUTF32.h"

namespace Text
{
#if _WCHAR_SIZE == 4
	typedef StringBuilderUTF32 StringBuilderW;
#else
	typedef StringBuilderUTF16 StringBuilderW;
#endif
};
#endif
