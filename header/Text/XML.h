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
		static UOSInt GetXMLTextLen(const UTF8Char *text);
		static UOSInt GetXMLTextLen(const WChar *text);
		static UOSInt GetXMLTextLiteLen(const UTF8Char *text);
		static UOSInt GetXMLTextLiteLen(const WChar *text);
		static UOSInt GetHTMLBodyTextLen(const UTF8Char *text);
		static UOSInt GetHTMLElementTextLen(const UTF8Char *text);
		static UOSInt GetHTMLBodyTextLen(const WChar *text);
		static Bool WriteUTF8Char(IO::Stream *stm, UTF32Char c);
	public:
		static UTF8Char *ToXMLText(UTF8Char *buff, const UTF8Char *text);
		static WChar *ToXMLText(WChar *buff, const WChar *text);
		static UTF8Char *ToXMLTextLite(UTF8Char *buff, const UTF8Char *text);
		static WChar *ToXMLTextLite(WChar *buff, const WChar *text);
		static UTF8Char *ToHTMLBodyText(UTF8Char *buff, const UTF8Char *text);
		static UTF8Char *ToHTMLElementText(UTF8Char *buff, const UTF8Char *text);
		static WChar *ToHTMLBodyText(WChar *buff, const WChar *text);
		static UTF8Char *ToAttrText(UTF8Char *buff, const UTF8Char *text);
		static WChar *ToAttrText(WChar *buff, const WChar *text);
		static NN<Text::String> ToNewXMLText(const UTF8Char *text);
		static const WChar *ToNewXMLText(const WChar *text);
		static NN<Text::String> ToNewXMLTextLite(const UTF8Char *text);
		static const WChar *ToNewXMLTextLite(const WChar *text);
		static NN<Text::String> ToNewHTMLBodyText(const UTF8Char *text);
		static NN<Text::String> ToNewHTMLElementText(const UTF8Char *text);
		static const WChar *ToNewHTMLBodyText(const WChar *text);
		static NN<Text::String> ToNewAttrText(const UTF8Char *text);
		static const WChar *ToNewAttrText(const WChar *text);
		static NN<Text::String> ToNewHTMLTextXMLColor(const UTF8Char *text);
		static void FreeNewText(const WChar *text);
		static void ParseStr(Text::String *out, const UTF8Char *xmlStart, const UTF8Char *xmlEnd);
		static void ParseStr(UTF8Char *out, const UTF8Char *xmlStart, const UTF8Char *xmlEnd);
		static void ParseStr(WChar *out, const WChar *xmlStart, const WChar *xmlEnd);

		static Bool HTMLAppendCharRef(const UTF8Char *chrRef, UOSInt refSize, IO::Stream *stm);
		static Bool HTMLAppendCharRef(const UTF8Char *chrRef, UOSInt refSize, NN<Text::StringBuilderUTF8> sb);
	};
}
#endif
