#ifndef _SM_TEXT_HTMLUTIL
#define _SM_TEXT_HTMLUTIL
#include "IO/Stream.h"
#include "Text/EncodingFactory.h"
#include "Text/StringBuilderUTF.h"

namespace Text
{
	class HTMLUtil
	{
	public:
		static Bool HTMLWellFormat(Text::EncodingFactory *encFact, IO::Stream *stm, OSInt lev, Text::StringBuilderUTF *sb);
		static Bool CSSWellFormat(const UInt8 *buff, OSInt buffSize, OSInt lev, Text::StringBuilderUTF *sb);
		static Bool HTMLGetText(Text::EncodingFactory *encFact, const UInt8 *buff, OSInt buffSize, Bool singleLine, Text::StringBuilderUTF *sb, Data::ArrayList<const UTF8Char*> *imgList);
	};
}
#endif
