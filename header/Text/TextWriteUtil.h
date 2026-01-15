#ifndef _SM_TEXT_TEXTWRITEUTIL
#define _SM_TEXT_TEXTWRITEUTIL
#include "Data/ArrayListStringNN.h"
#include "Data/TableData.h"
#include "Text/StyledTextWriter.h"

namespace Text
{
	class TextWriteUtil
	{
	public:
		static void WriteString(NN<Text::StyledTextWriter> writer, NN<Text::String> s);
		static void WriteArrayRange(NN<Text::StyledTextWriter> writer, NN<Data::ArrayListStringNN> arr, UOSInt startIndex, UOSInt endIndex);
		static void WriteArrayUOSRange(NN<Text::StyledTextWriter> writer, NN<Data::ArrayListNative<UOSInt>> arr, UOSInt startIndex, UOSInt endIndex);
		static void WriteArray(NN<Text::StyledTextWriter> writer, NN<Data::ArrayListStringNN> arr);
		static void WriteArrayUOS(NN<Text::StyledTextWriter> writer, NN<Data::ArrayListNative<UOSInt>> arr);
		static void WriteTableData(NN<Text::StyledTextWriter> writer, NN<Data::TableData> data);
		static void WriteTableDataPart(NN<Text::StyledTextWriter> writer, NN<Data::TableData> data, UOSInt nTop, UOSInt nBottom);
		static Bool WriteColumnLine(NN<Text::StyledTextWriter> writer, NN<Text::PString> column, UOSInt colSize); //return true if more lines exist
	};
}
#endif
