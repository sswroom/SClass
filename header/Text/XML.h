#ifndef _SM_TEXT_XML
#define _SM_TEXT_XML
#include "IO/Stream.h"
#include "Text/String.h"

namespace Text
{
	class XML
	{
	private:
		static UOSInt GetXMLTextLen(const UTF8Char *text);
		static UOSInt GetXMLTextLen(const WChar *text);
		static UOSInt GetXMLTextLiteLen(const UTF8Char *text);
		static UOSInt GetXMLTextLiteLen(const WChar *text);
		static UOSInt GetHTMLTextLen(const UTF8Char *text);
		static UOSInt GetHTMLTextLen(const WChar *text);
		static Bool WriteUTF8Char(IO::Stream *stm, UTF32Char c);
	public:
		static UTF8Char *ToXMLText(UTF8Char *buff, const UTF8Char *text);
		static WChar *ToXMLText(WChar *buff, const WChar *text);
		static UTF8Char *ToXMLTextLite(UTF8Char *buff, const UTF8Char *text);
		static WChar *ToXMLTextLite(WChar *buff, const WChar *text);
		static UTF8Char *ToHTMLText(UTF8Char *buff, const UTF8Char *text);
		static WChar *ToHTMLText(WChar *buff, const WChar *text);
		static UTF8Char *ToAttrText(UTF8Char *buff, const UTF8Char *text);
		static WChar *ToAttrText(WChar *buff, const WChar *text);
		static Text::String *ToNewXMLText(const UTF8Char *text);
		static const WChar *ToNewXMLText(const WChar *text);
		static Text::String *ToNewXMLTextLite(const UTF8Char *text);
		static const WChar *ToNewXMLTextLite(const WChar *text);
		static Text::String *ToNewHTMLText(const UTF8Char *text);
		static const WChar *ToNewHTMLText(const WChar *text);
		static Text::String *ToNewAttrText(const UTF8Char *text);
		static const WChar *ToNewAttrText(const WChar *text);
		static void FreeNewText(const WChar *text);
		static void ParseStr(Text::String *out, const UTF8Char *xmlStart, const UTF8Char *xmlEnd);
		static void ParseStr(UTF8Char *out, const UTF8Char *xmlStart, const UTF8Char *xmlEnd);
		static void ParseStr(WChar *out, const WChar *xmlStart, const WChar *xmlEnd);

		static Bool HTMLAppendCharRef(const UTF8Char *chrRef, OSInt refSize, IO::Stream *stm);
	};
}
#endif
