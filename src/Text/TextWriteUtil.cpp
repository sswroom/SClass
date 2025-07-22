#include "Stdafx.h"
#include "Text/JSText.h"
#include "Text/StringTool.h"
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

void Text::TextWriteUtil::WriteTableData(NN<Text::StyledTextWriter> writer, NN<Data::TableData> data)
{
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	NN<DB::DBReader> r;
	if (!data->GetTableData().SetTo(r))
	{
		writer->SetTextColor(Text::StandardColor::Red);
		writer->WriteLine(CSTR("Error in reading TableData"));
		writer->ResetTextColor();
		return;
	}
	Math::Size2D<UOSInt> thisSize;
	UOSInt colCnt = r->ColCount();
	UOSInt *maxW = MemAlloc(UOSInt, colCnt);
	UOSInt i;
	NN<Text::String> s;
	i = 0;
	while (i < colCnt)
	{
		if (r->GetName(i, sbuff).SetTo(sptr))
		{
			thisSize = Text::StringTool::GetMonospaceSize(sbuff);
			maxW[i] = thisSize.GetWidth();
		}
		else
		{
			maxW[i] = 6;
		}
		i++;
	}
	while (r->ReadNext())
	{
		i = 0;
		while (i < colCnt)
		{
			if (r->GetNewStr(i).SetTo(s))
			{
				thisSize = Text::StringTool::GetMonospaceSize(s->v);
				s->Release();
			}
			else
			{
				thisSize = {6, 1};
			}
			if (thisSize.GetWidth() > maxW[i])
			{
				maxW[i] = thisSize.GetWidth();
			}
			i++;
		}
	}
	data->CloseReader(r);

	if (!data->GetTableData().SetTo(r))
	{
		writer->SetTextColor(Text::StandardColor::Red);
		writer->WriteLine(CSTR("Error in reading TableData"));
		writer->ResetTextColor();
		MemFree(maxW);
		return;
	}

	Bool hasMoreLine;
	Bool thisMoreLine;
	UOSInt j;
	UnsafeArray<Optional<Text::String>> valArr = MemAllocArr(Optional<Text::String>, colCnt);
	UnsafeArray<Text::PString> sArr = MemAllocArr(Text::PString, colCnt);
	hasMoreLine = false;
	i = 0;
	while (i < colCnt)
	{
		if (i > 0)
			writer->WriteChar('|');
		if (r->GetName(i, sbuff).SetTo(sptr))
		{
			valArr[i] = s = Text::String::NewP(sbuff, sptr);
			writer->SetTextColor(Text::StandardColor::ConsoleDarkGreen);
			sArr[i] = s.Ptr()[0];
			thisMoreLine = WriteColumnLine(writer, sArr[i], maxW[i]);
			writer->ResetTextColor();
			if (thisMoreLine)
				hasMoreLine = true;
			else
			{
				s->Release();
				valArr[i] = 0;
			}
		}
		else
		{
			writer->SetTextColor(Text::StandardColor::DarkGray);
			writer->Write(CSTR("(null)"));
			j = 6;
			while (j < maxW[i])
			{
				writer->WriteChar(' ');
				j++;
			}
			writer->ResetTextColor();
			valArr[i] = 0;
		}
		i++;
	}
	writer->WriteLine();
	while (hasMoreLine)
	{
		hasMoreLine = false;
		if (i > 0)
			writer->WriteChar('|');
		if (valArr[i].SetTo(s))
		{
			writer->SetTextColor(Text::StandardColor::ConsoleDarkGreen);
			thisMoreLine = WriteColumnLine(writer, sArr[i], maxW[i]);
			writer->ResetTextColor();
			if (thisMoreLine)
				hasMoreLine = true;
			else
			{
				s->Release();
				valArr[i] = 0;
			}
		}
		else
		{
			j = 0;
			while (j < maxW[i])
			{
				writer->WriteChar(' ');
				j++;
			}
		}
	}
	i = 0;
	while (i < colCnt)
	{
		if (i > 0)
			writer->WriteChar('+');
		j = 0;
		while (j < maxW[i])
		{
			writer->WriteChar('-');
			j++;
		}
		i++;
	}
	writer->WriteLine();
	while (r->ReadNext())
	{
		hasMoreLine = false;
		i = 0;
		while (i < colCnt)
		{
			if (i > 0)
				writer->WriteChar('|');
			if (r->GetNewStr(i).SetTo(s))
			{
				valArr[i] = s;
				writer->SetTextColor(Text::StandardColor::Yellow);
				sArr[i] = s.Ptr()[0];
				thisMoreLine = WriteColumnLine(writer, sArr[i], maxW[i]);
				writer->ResetTextColor();
				if (thisMoreLine)
					hasMoreLine = true;
				else
				{
					s->Release();
					valArr[i] = 0;
				}
			}
			else
			{
				writer->SetTextColor(Text::StandardColor::DarkGray);
				writer->Write(CSTR("(null)"));
				j = 6;
				while (j < maxW[i])
				{
					writer->WriteChar(' ');
					j++;
				}
				writer->ResetTextColor();
				valArr[i] = 0;
			}
			i++;
		}
		writer->WriteLine();
		while (hasMoreLine)
		{
			hasMoreLine = false;
			if (i > 0)
				writer->WriteChar('|');
			if (valArr[i].SetTo(s))
			{
				writer->SetTextColor(Text::StandardColor::ConsoleDarkGreen);
				thisMoreLine = WriteColumnLine(writer, sArr[i], maxW[i]);
				writer->ResetTextColor();
				if (thisMoreLine)
					hasMoreLine = true;
				else
				{
					s->Release();
					valArr[i] = 0;
				}
			}
			else
			{
				j = 0;
				while (j < maxW[i])
				{
					writer->WriteChar(' ');
					j++;
				}
			}
		}		
	}
	data->CloseReader(r);
	MemFreeArr(valArr);
	MemFreeArr(sArr);
	MemFree(maxW);
}

void Text::TextWriteUtil::WriteTableDataPart(NN<Text::StyledTextWriter> writer, NN<Data::TableData> data, UOSInt nTop, UOSInt nBottom)
{
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	NN<DB::DBReader> r;
	UOSInt rowCnt = 0;
	UOSInt currRow;
	if (!data->GetTableData().SetTo(r))
	{
		writer->SetTextColor(Text::StandardColor::Red);
		writer->WriteLine(CSTR("Error in reading TableData"));
		writer->ResetTextColor();
		return;
	}
	while (r->ReadNext())
	{
		rowCnt++;
	}	
	data->CloseReader(r);
	if (rowCnt <= nTop + nBottom)
	{
		WriteTableData(writer, data);
		return;
	}

	if (!data->GetTableData().SetTo(r))
	{
		writer->SetTextColor(Text::StandardColor::Red);
		writer->WriteLine(CSTR("Error in reading TableData"));
		writer->ResetTextColor();
		return;
	}
	Math::Size2D<UOSInt> thisSize;
	UOSInt colCnt = r->ColCount();
	UOSInt *maxW = MemAlloc(UOSInt, colCnt);
	UOSInt i;
	NN<Text::String> s;
	i = 0;
	while (i < colCnt)
	{
		if (r->GetName(i, sbuff).SetTo(sptr))
		{
			thisSize = Text::StringTool::GetMonospaceSize(sbuff);
			maxW[i] = thisSize.GetWidth();
		}
		else
		{
			maxW[i] = 6;
		}
		i++;
	}
	currRow = 0;
	while (r->ReadNext())
	{
		if (currRow < nTop || currRow >= rowCnt - nBottom)
		{
			i = 0;
			while (i < colCnt)
			{
				if (r->GetNewStr(i).SetTo(s))
				{
					thisSize = Text::StringTool::GetMonospaceSize(s->v);
					s->Release();
				}
				else
				{
					thisSize = {6, 1};
				}
				if (thisSize.GetWidth() > maxW[i])
				{
					maxW[i] = thisSize.GetWidth();
				}
				i++;
			}
		}
		currRow++;
	}
	data->CloseReader(r);

	if (!data->GetTableData().SetTo(r))
	{
		writer->SetTextColor(Text::StandardColor::Red);
		writer->WriteLine(CSTR("Error in reading TableData"));
		writer->ResetTextColor();
		MemFree(maxW);
		return;
	}

	Bool hasMoreLine;
	Bool thisMoreLine;
	UOSInt j;
	UnsafeArray<Optional<Text::String>> valArr = MemAllocArr(Optional<Text::String>, colCnt);
	UnsafeArray<Text::PString> sArr = MemAllocArr(Text::PString, colCnt);
	hasMoreLine = false;
	i = 0;
	while (i < colCnt)
	{
		if (i > 0)
			writer->WriteChar('|');
		if (r->GetName(i, sbuff).SetTo(sptr))
		{
			valArr[i] = s = Text::String::NewP(sbuff, sptr);
			writer->SetTextColor(Text::StandardColor::ConsoleDarkGreen);
			sArr[i] = s.Ptr()[0];
			thisMoreLine = WriteColumnLine(writer, sArr[i], maxW[i]);
			writer->ResetTextColor();
			if (thisMoreLine)
				hasMoreLine = true;
			else
			{
				s->Release();
				valArr[i] = 0;
			}
		}
		else
		{
			writer->SetTextColor(Text::StandardColor::DarkGray);
			writer->Write(CSTR("(null)"));
			j = 6;
			while (j < maxW[i])
			{
				writer->WriteChar(' ');
				j++;
			}
			writer->ResetTextColor();
			valArr[i] = 0;
		}
		i++;
	}
	writer->WriteLine();
	while (hasMoreLine)
	{
		hasMoreLine = false;
		if (i > 0)
			writer->WriteChar('|');
		if (valArr[i].SetTo(s))
		{
			writer->SetTextColor(Text::StandardColor::ConsoleDarkGreen);
			thisMoreLine = WriteColumnLine(writer, sArr[i], maxW[i]);
			writer->ResetTextColor();
			if (thisMoreLine)
				hasMoreLine = true;
			else
			{
				s->Release();
				valArr[i] = 0;
			}
		}
		else
		{
			j = 0;
			while (j < maxW[i])
			{
				writer->WriteChar(' ');
				j++;
			}
		}
	}
	i = 0;
	while (i < colCnt)
	{
		if (i > 0)
			writer->WriteChar('+');
		j = 0;
		while (j < maxW[i])
		{
			writer->WriteChar('-');
			j++;
		}
		i++;
	}
	writer->WriteLine();
	currRow = 0;
	while (r->ReadNext())
	{
		if (currRow < nTop || currRow >= rowCnt - nBottom)
		{
			hasMoreLine = false;
			i = 0;
			while (i < colCnt)
			{
				if (i > 0)
					writer->WriteChar('|');
				if (r->GetNewStr(i).SetTo(s))
				{
					valArr[i] = s;
					writer->SetTextColor(Text::StandardColor::Yellow);
					sArr[i] = s.Ptr()[0];
					thisMoreLine = WriteColumnLine(writer, sArr[i], maxW[i]);
					writer->ResetTextColor();
					if (thisMoreLine)
						hasMoreLine = true;
					else
					{
						s->Release();
						valArr[i] = 0;
					}
				}
				else
				{
					writer->SetTextColor(Text::StandardColor::DarkGray);
					writer->Write(CSTR("(null)"));
					j = 6;
					while (j < maxW[i])
					{
						writer->WriteChar(' ');
						j++;
					}
					writer->ResetTextColor();
					valArr[i] = 0;
				}
				i++;
			}
			writer->WriteLine();
			while (hasMoreLine)
			{
				hasMoreLine = false;
				if (i > 0)
					writer->WriteChar('|');
				if (valArr[i].SetTo(s))
				{
					writer->SetTextColor(Text::StandardColor::ConsoleDarkGreen);
					thisMoreLine = WriteColumnLine(writer, sArr[i], maxW[i]);
					writer->ResetTextColor();
					if (thisMoreLine)
						hasMoreLine = true;
					else
					{
						s->Release();
						valArr[i] = 0;
					}
				}
				else
				{
					j = 0;
					while (j < maxW[i])
					{
						writer->WriteChar(' ');
						j++;
					}
				}
			}
		}
		currRow++;
		if (currRow == nTop || (nTop == 0 && currRow == 1))
		{
			i = 0;
			while (i < colCnt)
			{
				if (i > 0)
					writer->WriteChar(' ');
				if (maxW[i] < 3)
				{
					j = 0;
					while (j < maxW[i])
					{
						writer->WriteChar(' ');
						j++;
					}
				}
				else
				{
					j = (maxW[i] - 3) >> 1;
					while (j-- > 0)
					{
						writer->WriteChar(' ');
					}
					writer->Write(CSTR("..."));
					j = ((maxW[i] - 3) >> 1) + 3;
					while (j < maxW[i])
					{
						writer->WriteChar(' ');
						j++;
					}
				}
				i++;
			}
			writer->WriteLine();
		}
	}
	data->CloseReader(r);
	MemFreeArr(valArr);
	MemFreeArr(sArr);
	MemFree(maxW);
}

Bool Text::TextWriteUtil::WriteColumnLine(NN<Text::StyledTextWriter> writer, NN<Text::PString> column, UOSInt colSize)
{
	Text::PString sarr[2];
	UOSInt n = Text::StrSplitLineP(sarr, 2, column.Ptr()[0]);
	Math::Size2D<UOSInt> size = Text::StringTool::GetMonospaceSize(sarr[0].v);
	writer->Write(sarr[0].ToCString());
	UOSInt i = size.GetWidth();
	while (i < colSize)
	{
		writer->WriteChar(' ');
		i++;
	}
	if (n == 2)
	{
		column->v = sarr[1].v;
		column->leng = sarr[1].leng;
		return true;
	}
	return false;
}
