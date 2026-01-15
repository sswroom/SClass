#ifndef _SM_TEXT_XML
#define _SM_TEXT_XML
#include "IO/Stream.h"
#include "Text/String.h"
#include "Text/StringBuilderUTF8.h"

namespace Text
{
	class XML
	{
	private:
		static UIntOS GetXMLTextLen(UnsafeArray<const UTF8Char> text);
		static UIntOS GetXMLTextLen(UnsafeArray<const WChar> text);
		static UIntOS GetXMLTextLiteLen(UnsafeArray<const UTF8Char> text);
		static UIntOS GetXMLTextLiteLen(UnsafeArray<const WChar> text);
		static UIntOS GetHTMLBodyTextLen(UnsafeArray<const UTF8Char> text);
		static UIntOS GetHTMLElementTextLen(UnsafeArray<const UTF8Char> text);
		static UIntOS GetHTMLBodyTextLen(UnsafeArray<const WChar> text);
		static Bool WriteUTF8Char(NN<IO::Stream> stm, UTF32Char c);
	public:
		static UnsafeArray<UTF8Char> ToXMLText(UnsafeArray<UTF8Char> buff, UnsafeArray<const UTF8Char> text);
		static UnsafeArray<WChar> ToXMLTextW(UnsafeArray<WChar> buff, UnsafeArray<const WChar> text);
		static UnsafeArray<UTF8Char> ToXMLTextLite(UnsafeArray<UTF8Char> buff, UnsafeArray<const UTF8Char> text);
		static UnsafeArray<WChar> ToXMLTextLite(UnsafeArray<WChar> buff, UnsafeArray<const WChar> text);
		static UnsafeArray<UTF8Char> ToHTMLBodyText(UnsafeArray<UTF8Char> buff, UnsafeArray<const UTF8Char> text);
		static UnsafeArray<UTF8Char> ToHTMLElementText(UnsafeArray<UTF8Char> buff, UnsafeArray<const UTF8Char> text);
		static UnsafeArray<WChar> ToHTMLBodyText(UnsafeArray<WChar> buff, UnsafeArray<const WChar> text);
		static UnsafeArray<UTF8Char> ToAttrText(UnsafeArray<UTF8Char> buff, UnsafeArray<const UTF8Char> text);
		static UnsafeArray<WChar> ToAttrText(UnsafeArray<WChar> buff, UnsafeArray<const WChar> text);
		static NN<Text::String> ToNewXMLText(UnsafeArray<const UTF8Char> text);
		static UnsafeArray<const WChar> ToNewXMLTextW(UnsafeArray<const WChar> text);
		static NN<Text::String> ToNewXMLTextLite(UnsafeArray<const UTF8Char> text);
		static UnsafeArray<const WChar> ToNewXMLTextLite(UnsafeArray<const WChar> text);
		static NN<Text::String> ToNewHTMLBodyText(UnsafeArray<const UTF8Char> text);
		static NN<Text::String> ToNewHTMLElementText(UnsafeArray<const UTF8Char> text);
		static UnsafeArray<const WChar> ToNewHTMLBodyTextW(UnsafeArray<const WChar> text);
		static NN<Text::String> ToNewAttrText(UnsafeArrayOpt<UTF8Char> text);
		static NN<Text::String> ToNewAttrText(UnsafeArrayOpt<const UTF8Char> text);
		static UnsafeArray<const WChar> ToNewAttrTextW(UnsafeArrayOpt<const WChar> text);
		static NN<Text::String> ToNewHTMLTextXMLColor(UnsafeArray<const UTF8Char> text);
		static void FreeNewText(UnsafeArray<const WChar> text);
		static void ParseStr(NN<Text::String> out, UnsafeArray<const UTF8Char> xmlStart, UnsafeArray<const UTF8Char> xmlEnd);
		static void ParseStr(UnsafeArray<UTF8Char> out, UnsafeArray<const UTF8Char> xmlStart, UnsafeArray<const UTF8Char> xmlEnd);
		static void ParseStr(UnsafeArray<WChar> out, UnsafeArray<const WChar> xmlStart, UnsafeArray<const WChar> xmlEnd);

		static Bool HTMLAppendCharRef(UnsafeArray<const UTF8Char> chrRef, UIntOS refSize, NN<IO::Stream> stm);
		static Bool HTMLAppendCharRef(UnsafeArray<const UTF8Char> chrRef, UIntOS refSize, NN<Text::StringBuilderUTF8> sb);
	};
}
#endif
