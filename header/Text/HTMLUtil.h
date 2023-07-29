#ifndef _SM_TEXT_HTMLUTIL
#define _SM_TEXT_HTMLUTIL
#include "Data/ArrayListNN.h"
#include "IO/Stream.h"
#include "Text/EncodingFactory.h"
#include "Text/StringBuilderUTF8.h"

namespace Text
{
	class HTMLUtil
	{
	public:
		static Bool HTMLWellFormat(Text::EncodingFactory *encFact, NotNullPtr<IO::Stream> stm, UOSInt lev, NotNullPtr<Text::StringBuilderUTF8> sb);
		static Bool CSSWellFormat(const UInt8 *buff, UOSInt buffSize, UOSInt lev, NotNullPtr<Text::StringBuilderUTF8> sb);
		static Bool HTMLGetText(Text::EncodingFactory *encFact, const UInt8 *buff, UOSInt buffSize, Bool singleLine, NotNullPtr<Text::StringBuilderUTF8> sb, Data::ArrayListNN<Text::String> *imgList);
		static Bool XMLWellFormat(const UInt8 *buff, UOSInt buffSize, UOSInt lev, NotNullPtr<Text::StringBuilderUTF8> sb);
	};
}
#endif
