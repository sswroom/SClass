#ifndef _SM_TEXT_CPPTEXT
#define _SM_TEXT_CPPTEXT
#include "Text/StringBuilderUTF.h"

namespace Text
{
	class CPPText
	{
	public:
		static void ToCPPString(Text::StringBuilderUTF *sb, const UTF8Char *str);
		static void FromCPPString(Text::StringBuilderUTF *sb, const UTF8Char *str);
	};
}
#endif
