#ifndef _SM_TEXT_JSTEXT
#define _SM_TEXT_JSTEXT
#include "Text/StringBuilderUTF.h"

namespace Text
{
	class JSText
	{
	public:
		static UTF8Char *ToJSText(UTF8Char *buff, const UTF8Char *s);
		static UTF8Char *ToJSTextDQuote(UTF8Char *buff, const UTF8Char *s);
		static void ToJSTextDQuote(Text::StringBuilderUTF *sb, const UTF8Char *s);
		static WChar *ToJSText(WChar *buff, const WChar *s);
		static WChar *ToJSTextDQuote(WChar *buff, const WChar *s);
		static const UTF8Char *ToNewJSText(Text::String *s);
		static const UTF8Char *ToNewJSText(const UTF8Char *s);
		static const UTF8Char *ToNewJSTextDQuote(const UTF8Char *s);
		static const WChar *ToNewJSText(const WChar *s);
		static const WChar *ToNewJSTextDQuote(const WChar *s);
		static const UTF8Char *FromNewJSText(const UTF8Char *s);
		static const WChar *FromNewJSText(const WChar *s);
		static void FreeNewText(const UTF8Char *s);
		static void FreeNewText(const WChar *s);

		static Bool JSONWellFormat(const UTF8Char *buff, UOSInt buffSize, UOSInt lev, Text::StringBuilderUTF *sb);
		static Bool JSWellFormat(const UTF8Char *buff, UOSInt buffSize, UOSInt lev, Text::StringBuilderUTF *sb);
	};
}
#endif
