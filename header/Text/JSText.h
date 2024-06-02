#ifndef _SM_TEXT_JSTEXT
#define _SM_TEXT_JSTEXT
#include "Text/String.h"
#include "Text/StringBuilderUTF8.h"

namespace Text
{
	class JSText
	{
	public:
		static UnsafeArray<UTF8Char> ToJSText(UnsafeArray<UTF8Char> buff, UnsafeArray<const UTF8Char> s);
		static UnsafeArray<UTF8Char> ToJSTextDQuote(UnsafeArray<UTF8Char> buff, UnsafeArray<const UTF8Char> s);
		static void ToJSTextDQuote(NN<Text::StringBuilderUTF8> sb, UnsafeArray<const UTF8Char> s);
		static WChar *ToJSTextW(WChar *buff, const WChar *s);
		static WChar *ToJSTextDQuoteW(WChar *buff, const WChar *s);
		static NN<Text::String> ToNewJSText(Text::String *s);
		static NN<Text::String> ToNewJSText(Optional<Text::String> s);
		static NN<Text::String> ToNewJSText(NN<Text::String> s);
		static NN<Text::String> ToNewJSText(UnsafeArrayOpt<const UTF8Char> s);
		static NN<Text::String> ToNewJSText(UnsafeArray<const UTF8Char> s);
		static NN<Text::String> ToNewJSTextDQuote(UnsafeArrayOpt<const UTF8Char> s);
		static NN<Text::String> ToNewJSTextDQuote(UnsafeArray<const UTF8Char> s);
		static const WChar *ToNewJSTextW(const WChar *s);
		static const WChar *ToNewJSTextDQuoteW(const WChar *s);
		static Text::String *FromNewJSText(UnsafeArray<const UTF8Char> s);
		static const WChar *FromNewJSTextW(const WChar *s);
		static void FreeNewText(const WChar *s);

		static Bool JSONWellFormat(UnsafeArray<const UTF8Char> buff, UOSInt buffSize, UOSInt lev, NN<Text::StringBuilderUTF8> sb);
		static Bool JSWellFormat(UnsafeArray<const UTF8Char> buff, UOSInt buffSize, UOSInt lev, NN<Text::StringBuilderUTF8> sb);
	};
}
#endif
