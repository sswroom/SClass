#ifndef _SM_TEXT_HTMLUTIL
#define _SM_TEXT_HTMLUTIL
#include "Data/ArrayListStringNN.h"
#include "IO/Stream.h"
#include "Text/EncodingFactory.h"
#include "Text/StringBuilderUTF8.h"

namespace Text
{
	class HTMLUtil
	{
	public:
		static Bool HTMLWellFormat(Optional<Text::EncodingFactory> encFact, NN<IO::Stream> stm, UOSInt lev, NN<Text::StringBuilderUTF8> sb);
		static Bool CSSWellFormat(UnsafeArray<const UInt8> buff, UOSInt buffSize, UOSInt lev, NN<Text::StringBuilderUTF8> sb);
		static Bool HTMLGetText(Optional<Text::EncodingFactory> encFact, const UInt8 *buff, UOSInt buffSize, Bool singleLine, NN<Text::StringBuilderUTF8> sb, Data::ArrayListStringNN *imgList);
		static Bool XMLWellFormat(UnsafeArray<const UInt8> buff, UOSInt buffSize, UOSInt lev, NN<Text::StringBuilderUTF8> sb);
	};
}
#endif
