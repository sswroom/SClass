#include "Stdafx.h"
#include "Text/JSText.h"
#include "Text/TextWriteUtil.h"

void Text::TextWriteUtil::WriteString(NN<Text::StyledTextWriter> writer, NN<Text::String> s)
{
	NN<Text::String> nns = Text::JSText::ToNewJSText(s->v);
	writer->SetTextColor(Text::StandardColor::Blue);
	writer->Write(nns->ToCString());
	writer->ResetTextColor();
	nns->Release();
}

void Text::TextWriteUtil::WriteArrayRange(NN<Text::StyledTextWriter> writer, NN<Data::ArrayListStringNN> arr, UOSInt startIndex, UOSInt endIndex)
{
	writer->WriteChar('[');
	if (endIndex > arr->GetCount())
	{
		endIndex = arr->GetCount();
	}
	if (startIndex < endIndex)
	{
		WriteString(writer, arr->GetItemNoCheck(startIndex));
		startIndex++;
		while (startIndex < endIndex)
		{
			writer->Write(CSTR(", "));
			WriteString(writer, arr->GetItemNoCheck(startIndex));
			startIndex++;
		}
	}
	writer->WriteChar(']');
}

void Text::TextWriteUtil::WriteArray(NN<Text::StyledTextWriter> writer, NN<Data::ArrayListStringNN> arr)
{
	WriteArrayRange(writer, arr, 0, arr->GetCount());
}
