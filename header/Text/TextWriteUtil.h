#ifndef _SM_TEXT_TEXTWRITEUTIL
#define _SM_TEXT_TEXTWRITEUTIL
#include "Data/ArrayListStringNN.h"
#include "Text/StyledTextWriter.h"

namespace Text
{
	class TextWriteUtil
	{
	public:
		static void WriteString(NN<Text::StyledTextWriter> writer, NN<Text::String> s);
		static void WriteArrayRange(NN<Text::StyledTextWriter> writer, NN<Data::ArrayListStringNN> arr, UOSInt startIndex, UOSInt endIndex);
		static void WriteArray(NN<Text::StyledTextWriter> writer, NN<Data::ArrayListStringNN> arr);
	};
}
#endif
