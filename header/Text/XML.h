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
		static UOSInt GetXMLTextLen(UnsafeArray<const UTF8Char> text);
		static UOSInt GetXMLTextLen(const WChar *text);
		static UOSInt GetXMLTextLiteLen(UnsafeArray<const UTF8Char> text);
		static UOSInt GetXMLTextLiteLen(const WChar *text);
		static UOSInt GetHTMLBodyTextLen(UnsafeArray<const UTF8Char> text);
		static UOSInt GetHTMLElementTextLen(UnsafeArray<const UTF8Char> text);
		static UOSInt GetHTMLBodyTextLen(const WChar *text);
		static Bool WriteUTF8Char(IO::Stream *stm, UTF32Char c);
	public:
		static UnsafeArray<UTF8Char> ToXMLText(UnsafeArray<UTF8Char> buff, UnsafeArray<const UTF8Char> text);
		static WChar *ToXMLText(WChar *buff, const WChar *text);
		static UnsafeArray<UTF8Char> ToXMLTextLite(UnsafeArray<UTF8Char> buff, UnsafeArray<const UTF8Char> text);
		static WChar *ToXMLTextLite(WChar *buff, const WChar *text);
		static UnsafeArray<UTF8Char> ToHTMLBodyText(UnsafeArray<UTF8Char> buff, UnsafeArray<const UTF8Char> text);
		static UnsafeArray<UTF8Char> ToHTMLElementText(UnsafeArray<UTF8Char> buff, UnsafeArray<const UTF8Char> text);
		static WChar *ToHTMLBodyText(WChar *buff, const WChar *text);
		static UnsafeArray<UTF8Char> ToAttrText(UnsafeArray<UTF8Char> buff, UnsafeArray<const UTF8Char> text);
		static WChar *ToAttrText(WChar *buff, const WChar *text);
		static NN<Text::String> ToNewXMLText(UnsafeArray<const UTF8Char> text);
		static const WChar *ToNewXMLText(const WChar *text);
		static NN<Text::String> ToNewXMLTextLite(UnsafeArray<const UTF8Char> text);
		static const WChar *ToNewXMLTextLite(const WChar *text);
		static NN<Text::String> ToNewHTMLBodyText(UnsafeArray<const UTF8Char> text);
		static NN<Text::String> ToNewHTMLElementText(UnsafeArray<const UTF8Char> text);
		static const WChar *ToNewHTMLBodyText(const WChar *text);
		static NN<Text::String> ToNewAttrText(UnsafeArrayOpt<const UTF8Char> text);
		static const WChar *ToNewAttrText(const WChar *text);
		static NN<Text::String> ToNewHTMLTextXMLColor(UnsafeArray<const UTF8Char> text);
		static void FreeNewText(const WChar *text);
		static void ParseStr(Text::String *out, UnsafeArray<const UTF8Char> xmlStart, UnsafeArray<const UTF8Char> xmlEnd);
		static void ParseStr(UnsafeArray<UTF8Char> out, UnsafeArray<const UTF8Char> xmlStart, UnsafeArray<const UTF8Char> xmlEnd);
		static void ParseStr(WChar *out, const WChar *xmlStart, const WChar *xmlEnd);

		static Bool HTMLAppendCharRef(UnsafeArray<const UTF8Char> chrRef, UOSInt refSize, IO::Stream *stm);
		static Bool HTMLAppendCharRef(UnsafeArray<const UTF8Char> chrRef, UOSInt refSize, NN<Text::StringBuilderUTF8> sb);
	};
}
#endif
