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
		static UnsafeArray<WChar> ToJSTextW(UnsafeArray<WChar> buff, UnsafeArray<const WChar> s);
		static UnsafeArray<WChar> ToJSTextDQuoteW(UnsafeArray<WChar> buff, UnsafeArray<const WChar> s);
		static NN<Text::String> ToNewJSText(Optional<Text::String> s);
		static NN<Text::String> ToNewJSText(NN<Text::String> s);
		static NN<Text::String> ToNewJSText(UnsafeArrayOpt<const UTF8Char> s);
		static NN<Text::String> ToNewJSText(UnsafeArray<const UTF8Char> s);
		static NN<Text::String> ToNewJSTextDQuote(UnsafeArrayOpt<const UTF8Char> s);
		static NN<Text::String> ToNewJSTextDQuote(UnsafeArray<const UTF8Char> s);
		static UnsafeArray<const WChar> ToNewJSTextW(UnsafeArrayOpt<const WChar> s);
		static UnsafeArray<const WChar> ToNewJSTextDQuoteW(UnsafeArrayOpt<const WChar> s);
		static Optional<Text::String> FromNewJSText(UnsafeArray<const UTF8Char> s);
		static UnsafeArrayOpt<const WChar> FromNewJSTextW(UnsafeArray<const WChar> s);
		static void FreeNewText(UnsafeArray<const WChar> s);
		static UnsafeArray<UTF8Char> JSDouble(UnsafeArray<UTF8Char> buff, Double val);

		static Bool JSONWellFormat(UnsafeArray<const UTF8Char> buff, UIntOS buffSize, UIntOS lev, NN<Text::StringBuilderUTF8> sb);
		static Bool JSWellFormat(UnsafeArray<const UTF8Char> buff, UIntOS buffSize, UIntOS lev, NN<Text::StringBuilderUTF8> sb);
	};
}
#endif
