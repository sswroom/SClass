#ifndef _SM_TEXT_JSTEXT
#define _SM_TEXT_JSTEXT
#include "Text/String.h"
#include "Text/StringBuilderUTF8.h"

namespace Text
{
	class JSText
	{
	public:
		static UTF8Char *ToJSText(UTF8Char *buff, const UTF8Char *s);
		static UTF8Char *ToJSTextDQuote(UTF8Char *buff, const UTF8Char *s);
		static void ToJSTextDQuote(NotNullPtr<Text::StringBuilderUTF8> sb, const UTF8Char *s);
		static WChar *ToJSText(WChar *buff, const WChar *s);
		static WChar *ToJSTextDQuote(WChar *buff, const WChar *s);
		static NotNullPtr<Text::String> ToNewJSText(Text::String *s);
		static NotNullPtr<Text::String> ToNewJSText(Optional<Text::String> s);
		static NotNullPtr<Text::String> ToNewJSText(NotNullPtr<Text::String> s);
		static NotNullPtr<Text::String> ToNewJSText(const UTF8Char *s);
		static NotNullPtr<Text::String> ToNewJSTextDQuote(const UTF8Char *s);
		static const WChar *ToNewJSText(const WChar *s);
		static const WChar *ToNewJSTextDQuote(const WChar *s);
		static Text::String *FromNewJSText(const UTF8Char *s);
		static const WChar *FromNewJSText(const WChar *s);
		static void FreeNewText(const WChar *s);

		static Bool JSONWellFormat(const UTF8Char *buff, UOSInt buffSize, UOSInt lev, NotNullPtr<Text::StringBuilderUTF8> sb);
		static Bool JSWellFormat(const UTF8Char *buff, UOSInt buffSize, UOSInt lev, NotNullPtr<Text::StringBuilderUTF8> sb);
	};
}
#endif
