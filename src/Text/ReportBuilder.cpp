#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/FastStringMap.h"
#include "Data/StringUTF8Map.h"
#include "IO/Path.h"
#include "Math/Math.h"
#include "Media/PaperSize.h"
#include "Text/MyStringFloat.h"
#include "Text/ReportBuilder.h"
#include "Text/StringBuilderUTF8.h"

#include <stdio.h>

Text::CString Text::ReportBuilder::ColumnMergeLeft = CSTR("|-L");
Text::CString Text::ReportBuilder::ColumnMergeUp = CSTR("|-U");

Text::SpreadSheet::AxisType Text::ReportBuilder::FromChartDataType(Data::Chart::DataType dataType)
{
	switch (dataType)
	{
	case Data::Chart::DataType::DateTicks:
		return Text::SpreadSheet::AxisType::Date;
	case Data::Chart::DataType::None:
	case Data::Chart::DataType::DOUBLE:
	case Data::Chart::DataType::Integer:
		return Text::SpreadSheet::AxisType::Numeric;
	default:
		return Text::SpreadSheet::AxisType::Numeric;
	}
}

Text::ReportBuilder::ReportBuilder(Text::CString name, UOSInt colCount, const UTF8Char **columns)
{
	TableCell *cols;
	UOSInt i;
	this->name = Text::String::New(name);
	this->nameHAlign = Text::HAlignment::Unknown;
	this->fontName = Text::String::New(UTF8STRC("Arial"));
	this->colCount = colCount;
	this->colWidthPts = MemAlloc(Double, this->colCount);
	this->chart = 0;
	this->paperHori = false;
	this->tableBorders = false;
	this->colTypes = MemAlloc(ColType, this->colCount);
	cols = MemAlloc(TableCell, this->colCount);
	i = 0;
	while (i < this->colCount)
	{
		colWidthPts[i] = 0;
		if (columns[i])
		{
			cols[i].val = Text::String::NewNotNullSlow(columns[i]).Ptr();
		}
		else
		{
			cols[i].val = 0;
		}
		cols[i].hAlign = Text::HAlignment::Unknown;
		this->colTypes[i] = CT_STRING;
		i++;
	}
	this->tableContent.Add(cols);
	this->tableRowType.Add(RT_HEADER);
}

Text::ReportBuilder::~ReportBuilder()
{
	NN<ColURLLatLon> url;
	NN<Text::ReportBuilder::ColIcon> icon;
	UnsafeArray<TableCell> cols;
	NN<HeaderInfo> header;
	NN<Data::ArrayListNN<Text::ReportBuilder::ColIcon>> iconList;
	UOSInt i;
	UOSInt j;
	j = this->headers.GetCount();
	while (j-- > 0)
	{
		header = this->headers.GetItemNoCheck(j);
		header->name->Release();
		header->value->Release();
		MemFreeNN(header);
	}
	j = this->preheaders.GetCount();
	while (j-- > 0)
	{
		header = this->preheaders.GetItemNoCheck(j);
		header->name->Release();
		header->value->Release();
		MemFreeNN(header);
	}
	j = this->tableContent.GetCount();
	while (j-- > 0)
	{
		cols = this->tableContent.GetItemNoCheck(j);
		i = this->colCount;
		while (i-- > 0)
		{
			SDEL_STRING(cols[i].val);
		}
		MemFreeArr(cols);
	}
	i = this->urlList.GetCount();
	while (i-- > 0)
	{
		url = this->urlList.GetItemNoCheck(i);
		MemFreeNN(url);
	}
	i = this->icons.GetCount();
	while (i-- > 0)
	{
		if (this->icons.GetItem(i).SetTo(iconList))
		{
			j = iconList->GetCount();
			while (j-- > 0)
			{
				icon = iconList->GetItemNoCheck(j);
				SDEL_STRING(icon->fileName);
				SDEL_STRING(icon->name);
				MemFreeNN(icon);
			}
			iconList.Delete();
		}
	}
	SDEL_CLASS(this->chart);
	MemFree(this->colWidthPts);
	MemFree(this->colTypes);
	this->fontName->Release();
	this->name->Release();
}

void Text::ReportBuilder::SetNameHAlign(Text::HAlignment hAlign)
{
	this->nameHAlign = hAlign;
}

void Text::ReportBuilder::SetTableBorders(Bool borders)
{
	this->tableBorders = borders;
}

void Text::ReportBuilder::SetFontName(Text::String *fontName)
{
	if (fontName)
	{
		this->fontName->Release();
		this->fontName = fontName->Clone();
	}
}

void Text::ReportBuilder::SetFontName(Text::CString fontName)
{
	if (fontName.leng > 0)
	{
		this->fontName->Release();
		this->fontName = Text::String::New(fontName);
	}
}

void Text::ReportBuilder::SetPaperHori(Bool paperHori)
{
	this->paperHori = paperHori;
}

void Text::ReportBuilder::AddChart(Data::Chart *chart)
{
	SDEL_CLASS(this->chart);
	this->chart = chart;
}

void Text::ReportBuilder::AddPreHeader(Text::CString name, Text::CString val)
{
	AddPreHeader(name, 1, val, 1, false, false);
}

void Text::ReportBuilder::AddPreHeader(Text::CString name, UOSInt nameCellCnt, Text::CString val, UOSInt valCellCnt, Bool valUnderline, Bool right)
{
	NN<HeaderInfo> header = MemAllocNN(HeaderInfo);
	header->name = Text::String::New(name);
	header->nameCellCnt = nameCellCnt;
	header->value = Text::String::New(val);
	header->valueCellCnt = valCellCnt;
	header->valueUnderline = valUnderline;
	header->isRight = right;
	this->preheaders.Add(header);
}

void Text::ReportBuilder::AddHeader(Text::CString name, Text::CString val)
{
	AddHeader(name, 1, val, 1, false, false);
}

void Text::ReportBuilder::AddHeader(Text::CString name, UOSInt nameCellCnt, Text::CString val, UOSInt valCellCnt, Bool valUnderline, Bool right)
{
	NN<HeaderInfo> header = MemAllocNN(HeaderInfo);
	header->name = Text::String::New(name);
	header->nameCellCnt = nameCellCnt;
	header->value = Text::String::New(val);
	header->valueCellCnt = valCellCnt;
	header->valueUnderline = valUnderline;
	header->isRight = right;
	this->headers.Add(header);
}

void Text::ReportBuilder::AddTableHeader(const UTF8Char **content)
{
	UnsafeArray<TableCell> cols;
	UOSInt i;
	cols = MemAlloc(TableCell, this->colCount);
	i = 0;
	while (i < this->colCount)
	{
		if (content[i])
		{
			cols[i].val = Text::String::NewNotNullSlow(content[i]).Ptr();
		}
		else
		{
			cols[i].val = 0;
		}
		cols[i].hAlign = Text::HAlignment::Unknown;
		i++;
	}
	this->tableContent.Add(cols);
	this->tableRowType.Add(RT_HEADER);
}

void Text::ReportBuilder::AddTableContent(const UTF8Char **content)
{
	TableCell *cols;
	UOSInt i;
	cols = MemAlloc(TableCell, this->colCount);
	i = 0;
	while (i < this->colCount)
	{
		if (content[i])
		{
			cols[i].val = Text::String::NewNotNullSlow(content[i]).Ptr();
		}
		else
		{
			cols[i].val = 0;
		}
		cols[i].hAlign = Text::HAlignment::Unknown;
		i++;
	}
	this->tableContent.Add(cols);
	this->tableRowType.Add(RT_CONTENT);
}

void Text::ReportBuilder::AddTableSummary(const UTF8Char **content)
{
	TableCell *cols;
	UOSInt i;
	cols = MemAlloc(TableCell, this->colCount);
	i = 0;
	while (i < this->colCount)
	{
		if (content[i])
		{
			cols[i].val = Text::String::NewNotNullSlow(content[i]).Ptr();
		}
		else
		{
			cols[i].val = 0;
		}
		cols[i].hAlign = Text::HAlignment::Unknown;
		i++;
	}
	this->tableContent.Add(cols);
	this->tableRowType.Add(RT_SUMMARY);
}

void Text::ReportBuilder::AddIcon(UOSInt index, Text::CString fileName, Text::CString name)
{
	UOSInt cnt = this->tableContent.GetCount() - 1;
	NN<Data::ArrayListNN<Text::ReportBuilder::ColIcon>> iconList;
	NN<Text::ReportBuilder::ColIcon> icon;
	while (this->icons.GetCount() < cnt)
	{
		this->icons.Add(0);
	}
	if (!this->icons.GetItem(cnt).SetTo(iconList))
	{
		NEW_CLASSNN(iconList, Data::ArrayListNN<Text::ReportBuilder::ColIcon>());
		this->icons.Add(iconList);
	}
	icon = MemAllocNN(Text::ReportBuilder::ColIcon);
	icon->col = index;
	if (fileName.v)
	{
		icon->fileName = Text::String::New(fileName.v, fileName.leng).Ptr();
		if (IO::Path::PATH_SEPERATOR != '/')
		{
			icon->fileName->Replace('/', IO::Path::PATH_SEPERATOR);
		}
	}
	else
	{
		icon->fileName = 0;
	}
	if (name.v)
	{
		icon->name = Text::String::New(name.v, name.leng).Ptr();
	}
	else
	{
		icon->name = 0;
	}
	iconList->Add(icon);
}

void Text::ReportBuilder::SetColumnWidthPts(UOSInt index, Double width)
{
	if (index >= this->colCount)
		return;
	this->colWidthPts[index] = width;
}

void Text::ReportBuilder::SetColumnType(UOSInt index, ColType colType)
{
	if (index >= this->colCount)
		return;
	this->colTypes[index] = colType;
}

void Text::ReportBuilder::SetColURLLatLon(UOSInt index, Math::Coord2DDbl pos)
{
	NN<ColURLLatLon> url;
	url = MemAllocNN(ColURLLatLon);
	url->col = index;
	url->row = this->tableContent.GetCount();
	url->lat = pos.GetLat();
	url->lon = pos.GetLon();
	this->urlList.Add(url);
}

void Text::ReportBuilder::SetColHAlign(UOSInt index, HAlignment hAlign)
{
	if (index >= this->colCount)
		return;
	UnsafeArray<TableCell> cols = this->tableContent.GetItemNoCheck(this->tableContent.GetCount() - 1);
	cols[index].hAlign = hAlign;
}

Bool Text::ReportBuilder::HasChart()
{
	return this->chart != 0;
}

Text::SpreadSheet::Workbook *Text::ReportBuilder::CreateWorkbook()
{
	UOSInt i;
	UOSInt j;
	UInt32 k;
	UOSInt l;
	UOSInt m;
	UOSInt urlAdd;
	Text::SpreadSheet::Workbook *wb;
	NN<Text::SpreadSheet::Worksheet> ws;
	NN<Text::SpreadSheet::Worksheet> dataSheet;
	UnsafeArray<TableCell> cols;
	NN<HeaderInfo> header;
	Text::StringBuilderUTF8 sb;
	NN<ColURLLatLon> url;
	NN<Data::ArrayListNN<Text::ReportBuilder::ColIcon>> iconList;
	NN<Text::ReportBuilder::ColIcon> icon;
	RowType lastRowType;
	RowType currRowType;
	Text::SpreadSheet::CellStyle *styleSummary = 0;
	Text::SpreadSheet::CellStyle *tableStyle = 0;
	if (this->chart == 0)
	{
		NEW_CLASS(wb, Text::SpreadSheet::Workbook());
		wb->AddDefaultStyles();
		ws = wb->AddWorksheet(this->name);

		if (this->tableBorders)
		{
			tableStyle = wb->NewCellStyle(0, Text::HAlignment::Unknown, Text::VAlignment::Unknown, CSTR_NULL).Ptr();
			tableStyle->SetBorderLeft(Text::SpreadSheet::CellStyle::BorderStyle(0xff000000, Text::SpreadSheet::BorderType::Thin));
			tableStyle->SetBorderTop(Text::SpreadSheet::CellStyle::BorderStyle(0xff000000, Text::SpreadSheet::BorderType::Thin));
			tableStyle->SetBorderRight(Text::SpreadSheet::CellStyle::BorderStyle(0xff000000, Text::SpreadSheet::BorderType::Thin));
			tableStyle->SetBorderBottom(Text::SpreadSheet::CellStyle::BorderStyle(0xff000000, Text::SpreadSheet::BorderType::Thin));
		}

		Bool lastRight = false;
		k = 0;
		i = 0;
		j = this->preheaders.GetCount();
		while (i < j)
		{
			header = this->preheaders.GetItemNoCheck(i);
			if (header->isRight)
			{
				if (!lastRight && k > 0)
					k--;

				ws->SetCellString(k, this->colCount - header->valueCellCnt, header->value);
				if (header->valueCellCnt > 1)
				{
					ws->MergeCells(k, this->colCount - header->valueCellCnt, 1, (UInt32)header->valueCellCnt);
				}
				ws->SetCellString(k, this->colCount - header->valueCellCnt - header->nameCellCnt, header->name);
				if (header->nameCellCnt > 1)
				{
					ws->MergeCells(k, this->colCount - header->valueCellCnt - header->nameCellCnt, 1, (UInt32)header->nameCellCnt);
				}
				if (header->valueUnderline)
				{
					m = header->valueCellCnt;
					while (m-- > 0)
					{
						ws->SetCellStyleBorderBottom(k, this->colCount - header->valueCellCnt + m, wb, 0xff000000, Text::SpreadSheet::BorderType::Thin);
					}
				}
			}
			else
			{
				ws->SetCellString(k, 0, header->name);
				if (header->nameCellCnt > 1)
				{
					ws->MergeCells(k, 0, 1, (UInt32)header->nameCellCnt);
				}
				ws->SetCellString(k, header->nameCellCnt, header->value);
				if (header->valueCellCnt > 1)
				{
					ws->MergeCells(k, header->nameCellCnt, 1, (UInt32)header->valueCellCnt);
				}
				if (header->valueUnderline)
				{
					m = header->valueCellCnt;
					while (m-- > 0)
					{
						ws->SetCellStyleBorderBottom(k, header->nameCellCnt + m, wb, 0xff000000, Text::SpreadSheet::BorderType::Thin);
					}
				}
			}
			lastRight = header->isRight;

			k++;
			i++;
		}

		if (this->nameHAlign != Text::HAlignment::Unknown && this->nameHAlign != Text::HAlignment::Left)
		{
			ws->MergeCells(k, 0, 1, (UInt32)this->colCount);
		}
		ws->SetCellString(k, 0, this->name);
		ws->SetCellStyleHAlign(k, 0, wb, this->nameHAlign);
		i = 0;
		j = this->colCount;
		while (i < j)
		{
			if (this->colWidthPts[i] != 0)
			{
				ws->SetColWidth(i, this->colWidthPts[i], Math::Unit::Distance::DU_POINT);
			}
			i++;
		}

		lastRight = false;
		i = 0;
		k++;
		j = this->headers.GetCount();
		while (i < j)
		{
			header = this->headers.GetItemNoCheck(i);
			if (header->isRight)
			{
				if (!lastRight && k > 0)
					k--;

				ws->SetCellString(k, this->colCount - header->valueCellCnt, header->value);
				if (header->valueCellCnt > 1)
				{
					ws->MergeCells(k, this->colCount - header->valueCellCnt, 1, (UInt32)header->valueCellCnt);
				}
				ws->SetCellString(k, this->colCount - header->valueCellCnt - header->nameCellCnt, header->name);
				if (header->nameCellCnt > 1)
				{
					ws->MergeCells(k, this->colCount - header->valueCellCnt - header->nameCellCnt, 1, (UInt32)header->nameCellCnt);
				}
				if (header->valueUnderline)
				{
					m = header->valueCellCnt;
					while (m-- > 0)
					{
						ws->SetCellStyleBorderBottom(k, this->colCount - header->valueCellCnt + m, wb, 0xff000000, Text::SpreadSheet::BorderType::Thin);
					}
				}
			}
			else
			{
				ws->SetCellString(k, 0, header->name);
				if (header->nameCellCnt > 1)
				{
					ws->MergeCells(k, 0, 1, (UInt32)header->nameCellCnt);
				}
				ws->SetCellString(k, header->nameCellCnt, header->value);
				if (header->valueCellCnt > 1)
				{
					ws->MergeCells(k, header->nameCellCnt, 1, (UInt32)header->valueCellCnt);
				}
				if (header->valueUnderline)
				{
					m = header->valueCellCnt;
					while (m-- > 0)
					{
						ws->SetCellStyleBorderBottom(k, header->nameCellCnt + m, wb, 0xff000000, Text::SpreadSheet::BorderType::Thin);
					}
				}
			}
			lastRight = header->isRight;

			k++;
			i++;
		}
		urlAdd = k;
		k++;
		lastRowType = RT_UNKNOWN;
		i = 0;
		j = this->tableContent.GetCount();
		while (i < j)
		{
			cols = this->tableContent.GetItemNoCheck(i);
			currRowType = this->tableRowType.GetItem(i);
			if (tableStyle)
			{
				l = 0;
				while (l < this->colCount)
				{
					ws->SetCellStyle(k, l, tableStyle);
					l++;
				}
			}
			else if ((lastRowType == RT_CONTENT && currRowType == RT_SUMMARY) || (lastRowType == RT_HEADER && currRowType != RT_HEADER))
			{
				if (styleSummary == 0)
				{
					styleSummary = wb->NewCellStyle().Ptr();
					styleSummary->SetBorderTop(Text::SpreadSheet::CellStyle::BorderStyle(0xff000000, Text::SpreadSheet::BorderType::Medium));
				}
				l = 0;
				while (l < this->colCount)
				{
					ws->SetCellStyle(k, l, styleSummary);
					l++;
				}
			}
			if (this->icons.GetItem(i).SetTo(iconList))
			{
				NN<Text::StringBuilderUTF8> *sbList = MemAlloc(NN<Text::StringBuilderUTF8>, this->colCount);
				l = 0;
				while (l < this->colCount)
				{
					NEW_CLASSNN(sbList[l], Text::StringBuilderUTF8());
					if (cols[l].val)
					{
						sbList[l]->Append(cols[l].val);
					}
					l++;
				}
				m = iconList->GetCount();
				l = 0;
				while (l < m)
				{
					icon = iconList->GetItemNoCheck(l);
					if (sbList[icon->col]->GetLength() > 0)
					{
						sbList[icon->col]->AppendC(UTF8STRC(", "));
					}
					sbList[icon->col]->Append(icon->name);
					l++;
				}

				l = 0;
				while (l < this->colCount)
				{
					if (sbList[l]->GetLength() > 0)
					{
						ws->SetCellString(k, l, sbList[l]->ToCString());
					}
					sbList[l].Delete();
					l++;
				}
				MemFree(sbList);
			}
			else if (currRowType == RT_HEADER)
			{
				l = 0;
				while (l < this->colCount)
				{
					if (cols[l].val)
					{
						if (cols[l].val->Equals(ColumnMergeLeft.v, ColumnMergeLeft.leng))
						{
							ws->SetCellMergeLeft(k, l);
						}
						else if (cols[l].val->Equals(ColumnMergeUp.v, ColumnMergeUp.leng))
						{
							ws->SetCellMergeUp(k, l);
						}
						else
						{
							ws->SetCellString(k, l, Text::String::OrEmpty(cols[l].val));
						}
						if (cols[l].hAlign != Text::HAlignment::Unknown)
						{
							ws->SetCellStyleHAlign(k, l, wb, cols[l].hAlign);
						}
					}
					l++;
				}
			}
			else
			{
				l = 0;
				while (l < this->colCount)
				{
					if (cols[l].val)
					{
						if (this->colTypes[l] == CT_DOUBLE)
						{
							ws->SetCellDouble(k, l, cols[l].val->ToDouble());
						}
						else if (this->colTypes[l] == CT_INT32)
						{
							ws->SetCellInt32(k, l, cols[l].val->ToInt32());
						}
						else
						{
							ws->SetCellString(k, l, Text::String::OrEmpty(cols[l].val));
						}
						if (cols[l].hAlign != Text::HAlignment::Unknown)
						{
							ws->SetCellStyleHAlign(k, l, wb, cols[l].hAlign);
						}
					}
					l++;
				}
			}
			lastRowType = currRowType;
			k++;
			i++;
		}
		i = 0;
		j = this->urlList.GetCount();
		while (i < j)
		{
			url = this->urlList.GetItemNoCheck(i);
			sb.ClearStr();
			sb.AppendC(UTF8STRC("https://www.google.com/maps/place/"));
			sb.AppendDouble(url->lat);
			sb.AppendC(UTF8STRC(","));
			sb.AppendDouble(url->lon);
			sb.AppendC(UTF8STRC("/@"));
			sb.AppendDouble(url->lat);
			sb.AppendC(UTF8STRC(","));
			sb.AppendDouble(url->lon);
			sb.AppendC(UTF8STRC(",19z"));
			ws->SetCellURL(url->row + urlAdd, url->col, sb.ToCString());
			i++;
		}
		return wb;
	}
	else
	{
		NEW_CLASS(wb, Text::SpreadSheet::Workbook());
		Text::SpreadSheet::WorkbookFont *font10 = wb->NewFont(CSTR("Arial"), 10, false);
		Text::SpreadSheet::CellStyle *strStyle = wb->NewCellStyle(font10, Text::HAlignment::Left, Text::VAlignment::Center, CSTR("General")).Ptr();
		ws = wb->AddWorksheet(this->name);
		dataSheet = wb->AddWorksheet(CSTR("ChartData"));

		Bool lastRight;
		lastRight = false;
		k = 0;
		i = 0;
		j = this->preheaders.GetCount();
		while (i < j)
		{
			header = this->preheaders.GetItemNoCheck(i);
			if (header->isRight)
			{
				if (!lastRight && k > 0)
					k--;

				ws->SetCellString(k, this->colCount - header->valueCellCnt, header->value);
				if (header->valueCellCnt > 1)
				{
					ws->MergeCells(k, this->colCount - header->valueCellCnt, 1, (UInt32)header->valueCellCnt);
				}
				ws->SetCellString(k, this->colCount - header->valueCellCnt - header->nameCellCnt, header->name);
				if (header->nameCellCnt > 1)
				{
					ws->MergeCells(k, this->colCount - header->valueCellCnt - header->nameCellCnt, 1, (UInt32)header->nameCellCnt);
				}
				if (header->valueUnderline)
				{
					m = header->valueCellCnt;
					while (m-- > 0)
					{
						ws->SetCellStyleBorderBottom(k, this->colCount - header->valueCellCnt + m, wb, 0xff000000, Text::SpreadSheet::BorderType::Thin);
					}
				}
			}
			else
			{
				ws->SetCellString(k, 0, header->name);
				if (header->nameCellCnt > 1)
				{
					ws->MergeCells(k, 0, 1, (UInt32)header->nameCellCnt);
				}
				ws->SetCellString(k, header->nameCellCnt, header->value);
				if (header->valueCellCnt > 1)
				{
					ws->MergeCells(k, header->nameCellCnt, 1, (UInt32)header->valueCellCnt);
				}
				if (header->valueUnderline)
				{
					m = header->valueCellCnt;
					while (m-- > 0)
					{
						ws->SetCellStyleBorderBottom(k, header->nameCellCnt + m, wb, 0xff000000, Text::SpreadSheet::BorderType::Thin);
					}
				}
			}
			lastRight = header->isRight;

			k++;
			i++;
		}

		ws->SetCellString(k, 0, this->name);
		i = 0;
		j = this->colCount;
		while (i < j)
		{
			if (this->colWidthPts[i] != 0)
			{
				ws->SetColWidth(i, this->colWidthPts[i], Math::Unit::Distance::DU_POINT);
			}
			i++;
		}

		lastRight = false;
		i = 0;
		k++;
		j = this->headers.GetCount();
		while (i < j)
		{
			header = this->headers.GetItemNoCheck(i);
			if (header->isRight)
			{
				if (!lastRight && k > 0)
					k--;

				ws->SetCellString(k, this->colCount - header->valueCellCnt, header->value);
				if (header->valueCellCnt > 1)
				{
					ws->MergeCells(k, this->colCount - header->valueCellCnt, 1, (UInt32)header->valueCellCnt);
				}
				ws->SetCellString(k, this->colCount - header->valueCellCnt - header->nameCellCnt, header->name);
				if (header->nameCellCnt > 1)
				{
					ws->MergeCells(k, this->colCount - header->valueCellCnt - header->nameCellCnt, 1, (UInt32)header->nameCellCnt);
				}
				if (header->valueUnderline)
				{
					m = header->valueCellCnt;
					while (m-- > 0)
					{
						ws->SetCellStyleBorderBottom(k, this->colCount - header->valueCellCnt + m, wb, 0xff000000, Text::SpreadSheet::BorderType::Thin);
					}
				}
			}
			else
			{
				ws->SetCellString(k, 0, header->name);
				if (header->nameCellCnt > 1)
				{
					ws->MergeCells(k, 0, 1, (UInt32)header->nameCellCnt);
				}
				ws->SetCellString(k, header->nameCellCnt, header->value);
				if (header->valueCellCnt > 1)
				{
					ws->MergeCells(k, header->nameCellCnt, 1, (UInt32)header->valueCellCnt);
				}
				if (header->valueUnderline)
				{
					m = header->valueCellCnt;
					while (m-- > 0)
					{
						ws->SetCellStyleBorderBottom(k, header->nameCellCnt + m, wb, 0xff000000, Text::SpreadSheet::BorderType::Thin);
					}
				}
			}
			lastRight = header->isRight;

			k++;
			i++;
		}

		Text::SpreadSheet::OfficeChart *shChart = ws->CreateChart(Math::Unit::Distance::DU_INCH, 0.64, 1.61, 13.10, 5.53, OPTSTR_CSTR(chart->GetTitle()));
		shChart->InitLineChart(Text::String::OrEmpty(chart->GetYAxisName())->ToCString(), Text::String::OrEmpty(chart->GetXAxisName())->ToCString(), FromChartDataType(chart->GetXAxisType()));
		shChart->SetDisplayBlankAs(Text::SpreadSheet::BlankAs::Gap);
		shChart->AddLegend(Text::SpreadSheet::LegendPos::Bottom);

		if (chart->GetXDataCount() == 1)
		{
			Text::SpreadSheet::CellStyle *dateStyle = 0;
			Text::SpreadSheet::CellStyle *intStyle = 0;
			Text::SpreadSheet::CellStyle *dblStyle = wb->NewCellStyle(font10, Text::HAlignment::Left, Text::VAlignment::Center, CSTR("General")).Ptr();

			UOSInt i;
			UOSInt j;
			UOSInt colCount;
			switch (chart->GetXAxisType())
			{
			case Data::Chart::DataType::DateTicks:
			{
				if (dateStyle == 0)
				{
					dateStyle = wb->NewCellStyle(font10, Text::HAlignment::Left, Text::VAlignment::Center, chart->GetTimeFormat()->ToCString()).Ptr();
				}
				Int64 *dateTicks = chart->GetXDateTicks(0, &colCount);
				i = 0;
				while (i < colCount)
				{
					dataSheet->SetCellTS(0, i + 1, dateStyle, Data::Timestamp(dateTicks[i], Data::DateTimeUtil::GetLocalTzQhr()));
					i++;
				}
				break;
			}
			case Data::Chart::DataType::DOUBLE:
			{
				if (dblStyle == 0)
				{
					dblStyle = wb->NewCellStyle(font10, Text::HAlignment::Left, Text::VAlignment::Center, chart->GetDblFormat()->ToCString()).Ptr();
				}
				Double *dblValues = chart->GetXDouble(0, &colCount);
				i = 0;
				while (i < colCount)
				{
					dataSheet->SetCellDouble(0, i + 1, dblStyle, dblValues[i]);
					i++;
				}
				break;
			}
			case Data::Chart::DataType::Integer:
			{
				if (intStyle == 0)
				{
					intStyle = wb->NewCellStyle(font10, Text::HAlignment::Left, Text::VAlignment::Center, CSTR("0")).Ptr();
				}
				Int32 *intValues = chart->GetXInt32(0, &colCount);
				i = 0;
				while (i < colCount)
				{
					dataSheet->SetCellInt32(0, i + 1, intStyle, intValues[i]);
					i++;
				}
				break;
			}
			case Data::Chart::DataType::None:
				colCount = 0;
				break;
			}
			i = 0;
			j = chart->GetYDataCount();
			while (i < j)
			{
				dataSheet->SetCellString(1 + i, 0, strStyle, Text::String::OrEmpty(chart->GetYName(i)));
				switch (chart->GetYType(i))
				{
				case Data::Chart::DataType::DateTicks:
				{
					if (dateStyle == 0)
					{
						dateStyle = wb->NewCellStyle(font10, Text::HAlignment::Left, Text::VAlignment::Center, chart->GetTimeFormat()->ToCString()).Ptr();
					}
					Int64 *dateTicks = chart->GetYDateTicks(i, &colCount);
					k = 0;
					while (k < colCount)
					{
						dataSheet->SetCellTS(i + 1, k + 1, dateStyle, Data::Timestamp(dateTicks[k], Data::DateTimeUtil::GetLocalTzQhr()));
						k++;
					}
					break;
				}
				case Data::Chart::DataType::DOUBLE:
				{
					if (dblStyle == 0)
					{
						dblStyle = wb->NewCellStyle(font10, Text::HAlignment::Left, Text::VAlignment::Center, chart->GetDblFormat()->ToCString()).Ptr();
					}
					Double *dblValues = chart->GetYDouble(i, &colCount);
					k = 0;
					while (k < colCount)
					{
						dataSheet->SetCellDouble(i + 1, k + 1, dblStyle, dblValues[k]);
						k++;
					}
					break;
				}
				case Data::Chart::DataType::Integer:
				{
					if (intStyle == 0)
					{
						intStyle = wb->NewCellStyle(font10, Text::HAlignment::Left, Text::VAlignment::Center, CSTR("0")).Ptr();
					}
					Int32 *intValues = chart->GetYInt32(i, &colCount);
					k = 0;
					while (k < colCount)
					{
						dataSheet->SetCellInt32(i + 1, k + 1, intStyle, intValues[k]);
						k++;
					}
					break;
				}
				case Data::Chart::DataType::None:
					colCount = 0;
					break;				
				}
				Text::SpreadSheet::WorkbookDataSource *catSource = NEW_CLASS_D(Text::SpreadSheet::WorkbookDataSource(dataSheet, 0, 0, 1, colCount));
				Text::SpreadSheet::WorkbookDataSource *valSource = NEW_CLASS_D(Text::SpreadSheet::WorkbookDataSource(dataSheet, i + 1, i + 1, 1, colCount));
				shChart->AddSeries(catSource, valSource, chart->GetYName(i), false);
				i++;
			}
		}
		else
		{

		}

		return wb;
	}
}

NN<Media::VectorDocument> Text::ReportBuilder::CreateVDoc(Int32 id, NN<Media::DrawEngine> deng)
{
	NN<Media::VectorDocument> doc;
	NN<Media::VectorGraph> g;
	UTF8Char sbuff[32];
	UTF8Char *sptr;
	Media::PaperSize paperSize(Media::PaperSize::PT_A4);
	Double border = 10.0;
	Double fontHeightMM = 3.0;
	Double fontHeightPt = Math::Unit::Distance::Convert(Math::Unit::Distance::DU_MILLIMETER, Math::Unit::Distance::DU_POINT, fontHeightMM);
	NN<Media::DrawFont> f;
	NN<Media::DrawBrush> b;
	NN<Media::DrawPen> p;
	Double headerW1;
	Double headerW2;
	Double headerW3;
	Double headerW4;
	Math::Size2DDbl sz;
	Double currY;
	Double nextY;
	UnsafeArray<TableCell> cols;
	NN<HeaderInfo> header;
	Bool lastRight;
	UOSInt i;
	UOSInt j;
	UOSInt k;
	UOSInt l;
	UOSInt m;
	UOSInt n;
	Double *colMinWidth;
	Double *colTotalWidth;
	Double *colPos;
	Double *colSize;
	Double *colCurrX;
	Int32 pageId = 0;
	RowType lastRowType;
	RowType currRowType;

	colMinWidth = MemAlloc(Double, this->colCount);
	colTotalWidth = MemAlloc(Double, this->colCount);
	colPos = MemAlloc(Double, this->colCount);
	colSize = MemAlloc(Double, this->colCount);
	colCurrX = MemAlloc(Double, this->colCount);

	NN<Data::ArrayListNN<Text::ReportBuilder::ColIcon>> iconList;
	Data::FastStringMap<IconStatus *> iconStatus;
	IconStatus *iconSt;
	NN<Text::ReportBuilder::ColIcon> icon;

	lastRowType = RT_UNKNOWN;
	sptr = Text::StrInt32(sbuff, id);
	NEW_CLASSNN(doc, Media::VectorDocument(0, CSTRP(sbuff, sptr), deng));
	if (this->paperHori)
	{
		g = doc->AddGraph(paperSize.GetHeightMM(), paperSize.GetWidthMM(), Math::Unit::Distance::DU_MILLIMETER);
	}
	else
	{
		g = doc->AddGraph(paperSize.GetWidthMM(), paperSize.GetHeightMM(), Math::Unit::Distance::DU_MILLIMETER);
	}
	f = g->NewFontPt(this->fontName->ToCString(), fontHeightPt, Media::DrawEngine::DFS_NORMAL, 0);
	headerW1 = 0;
	headerW2 = 0;
	headerW3 = 0;
	headerW4 = 0;
	i = this->headers.GetCount();
	while (i-- > 0)
	{
		header = this->headers.GetItemNoCheck(i);
		if (header->isRight)
		{
			sz = g->GetTextSize(f, header->name->ToCString());
			if (sz.x > headerW3)
				headerW3 = sz.x;
			sz = g->GetTextSize(f, header->value->ToCString());
			if (sz.x > headerW4)
				headerW4 = sz.x;
		}
		else
		{
			sz = g->GetTextSize(f, header->name->ToCString());
			if (sz.x > headerW1)
				headerW1 = sz.x;
			sz = g->GetTextSize(f, header->value->ToCString());
			if (sz.x > headerW2)
				headerW2 = sz.x;
		}
	}
	i = this->preheaders.GetCount();
	while (i-- > 0)
	{
		header = this->preheaders.GetItemNoCheck(i);
		if (header->isRight)
		{
			sz = g->GetTextSize(f, header->name->ToCString());
			if (sz.x > headerW3)
				headerW3 = sz.x;
			sz = g->GetTextSize(f, header->value->ToCString());
			if (sz.x > headerW4)
				headerW4 = sz.x;
		}
		else
		{
			sz = g->GetTextSize(f, header->name->ToCString());
			if (sz.x > headerW1)
				headerW1 = sz.x;
			sz = g->GetTextSize(f, header->value->ToCString());
			if (sz.x > headerW2)
				headerW2 = sz.x;
		}
	}

	i = this->colCount;
	while (i-- > 0)
	{
		colMinWidth[i] = 0;
		colTotalWidth[i] = 0;
		colPos[i] = 0;
		colSize[i] = 0;
	}

	i = this->tableContent.GetCount();
	while (i-- > 0)
	{
		cols = this->tableContent.GetItemNoCheck(i);
		j = this->colCount;
		while (j-- > 0)
		{
			colCurrX[j] = 0;
			if (cols[j].val)
			{
				sz = g->GetTextSize(f, cols[j].val->ToCString());
				colCurrX[j] = sz.x;
			}
		}

		if (this->icons.GetItem(i).SetTo(iconList))
		{
			j = iconList->GetCount();
			while (j-- > 0)
			{
				icon = iconList->GetItemNoCheck(j);
				if (icon->fileName)
				{
					iconSt = iconStatus.Get(icon->fileName);
					if (iconSt == 0)
					{
						iconSt = MemAlloc(IconStatus, 1);
						iconSt->dimg = deng->LoadImage(icon->fileName->ToCString());
						iconStatus.Put(icon->fileName, iconSt);
					}

					if (iconSt->dimg)
					{
						colCurrX[icon->col] += fontHeightPt / UOSInt2Double(iconSt->dimg->GetHeight()) * UOSInt2Double(iconSt->dimg->GetWidth());
					}
				}
			}
		}

		j = this->colCount;
		while (j-- > 0)
		{
			if (colTotalWidth[j] < colCurrX[j])
				colTotalWidth[j] = colCurrX[j];
		}
	}

	Double endY;
	Double totalColWidth;
	Double drawWidth;
	totalColWidth = 0;
	if (this->paperHori)
	{
		endY = paperSize.GetWidthMM() - border - fontHeightMM;
		drawWidth = paperSize.GetHeightMM() - border * 2.0;
	}
	else
	{
		endY = paperSize.GetHeightMM() - border - fontHeightMM;
		drawWidth = paperSize.GetWidthMM() - border * 2.0;
	}

	i = this->colCount;
	while (i-- > 0)
	{
		totalColWidth += colTotalWidth[i];
	}
	i = this->colCount;
	while (i-- > 0)
	{
		///////////////////////////
		colSize[i] = colTotalWidth[i] * drawWidth / totalColWidth;
	}

	colPos[0] = border;
	i = 1;
	j = this->colCount;
	while (i < j)
	{
		colPos[i] = colPos[i - 1] + colSize[i - 1];
		i++;
	}
	g->DelFont(f);

	k = 0;
	l = this->tableContent.GetCount();
	while (k < l)
	{
		if (this->tableRowType.GetItem(k) != RT_HEADER)
			break;
		k++;
	}
	while (true)
	{
		f = g->NewFontPt(this->fontName->ToCString(), fontHeightPt, Media::DrawEngine::DFS_NORMAL, 0);
		b = g->NewBrushARGB(0xff000000);
		p = g->NewPenARGB(0xff000000, 0.2, 0, 0);

		lastRight = false;
		currY = border;
		i = 0;
		j = this->preheaders.GetCount();
		while (i < j)
		{
			header = this->preheaders.GetItemNoCheck(i);
			if (header->isRight)
			{
				if (!lastRight && i > 0)
					currY -= fontHeightMM * 1.5;
				g->DrawString(Math::Coord2DDbl(border + drawWidth - headerW4 - headerW3 - 0.5, currY), header->name, f, b);
				g->DrawString(Math::Coord2DDbl(border + drawWidth - headerW4, currY), header->value, f, b);
				if (header->valueUnderline)
				{
					g->DrawLine(border + drawWidth - headerW4, currY + fontHeightMM, border + drawWidth, currY + fontHeightMM, p);
				}
			}
			else
			{
				g->DrawString(Math::Coord2DDbl(border, currY), header->name, f, b);
				g->DrawString(Math::Coord2DDbl(border + headerW1 + 0.5, currY), header->value, f, b);
				if (header->valueUnderline)
				{
					g->DrawLine(border + headerW1 + 0.5, currY + fontHeightMM, border + headerW1 + 0.5 + headerW2, currY + fontHeightMM, p);
				}
			}
			lastRight = header->isRight;

			currY += fontHeightMM * 1.5;
			i++;
		}

		g->DrawStringHAlign(Math::Coord2DDbl(border, currY), border + drawWidth, this->name->ToCString(), f, b, this->nameHAlign);
		currY += fontHeightMM * 2;
		lastRight = false;
		i = 0;
		j = this->headers.GetCount();
		while (i < j)
		{
			header = this->headers.GetItemNoCheck(i);
			if (header->isRight)
			{
				if (!lastRight && i > 0)
					currY -= fontHeightMM * 1.5;
				g->DrawString(Math::Coord2DDbl(border + drawWidth - headerW4 - headerW3 - 0.5, currY), header->name, f, b);
				g->DrawString(Math::Coord2DDbl(border + drawWidth - headerW4, currY), header->value, f, b);
				if (header->valueUnderline)
				{
					g->DrawLine(border + drawWidth - headerW4, currY + fontHeightMM, border + drawWidth, currY + fontHeightMM, p);
				}
			}
			else
			{
				g->DrawString(Math::Coord2DDbl(border, currY), header->name, f, b);
				g->DrawString(Math::Coord2DDbl(border + headerW1 + 0.5, currY), header->value, f, b);
				if (header->valueUnderline)
				{
					g->DrawLine(border + headerW1 + 0.5, currY + fontHeightMM, border + headerW1 + 0.5 + headerW2, currY + fontHeightMM, p);
				}
			}
			lastRight = header->isRight;

			currY += fontHeightMM * 1.5;
			i++;
		}
		currY += fontHeightMM;

		if (l == 1 && this->chart)
		{

		}
		else
		{
			Double nextX;
			m = 0;
			while (m < l)
			{
				if (this->tableRowType.GetItem(m) != RT_HEADER)
					break;
				nextY = currY + fontHeightMM * 1.5;
				cols = this->tableContent.GetItemNoCheck(m);
				if (this->tableBorders)
				{
					g->DrawLine(border + drawWidth, currY, border + drawWidth, nextY, p);
				}
				j = this->colCount;
				i = j;
				while (i-- > 0)
				{
					if (cols[i].val->Equals(this->ColumnMergeUp.v, this->ColumnMergeUp.leng))
					{
						if (this->tableBorders)
						{
							UnsafeArray<TableCell> cols2;
							n = m - 1;
							while (true)
							{
								cols2 = this->tableContent.GetItemNoCheck(n);
								if (cols2[i].val && cols2[i].val->Equals(this->ColumnMergeUp.v, this->ColumnMergeUp.leng))
									n--;
								else
									break;
							}
							if (cols2[i].val == 0 || !cols2[i].val->Equals(this->ColumnMergeLeft.v, this->ColumnMergeLeft.leng))
							{
								g->DrawLine(colPos[i], currY, colPos[i], nextY, p);
							}
						}
					}
					else
					{
						if (i + 1 < j)
						{
							nextX = colPos[i + 1];
						}
						else
						{
							nextX = border + drawWidth;
						}
						if (cols[i].val->Equals(this->ColumnMergeLeft.v, this->ColumnMergeLeft.leng))
						{
							while (i-- > 0)
							{
								if (!cols[i].val->Equals(this->ColumnMergeLeft.v, this->ColumnMergeLeft.leng))
								{
									break;
								}
							}
						}
						if (this->tableBorders)
						{
							g->DrawLine(colPos[i], currY, nextX, currY, p);
							g->DrawLine(colPos[i], currY, colPos[i], nextY, p);
						}
						g->DrawStringHAlign(Math::Coord2DDbl(colPos[i], currY), nextX, cols[i].val->ToCString(), f, b, cols[i].hAlign);
					}
				}
				m++;
				currY = nextY;
			}
			if (!this->tableBorders)
			{
				currY += 0.2;
				g->DrawLine(border, currY, border + drawWidth, currY, p);
			}
			if (currY > endY)
			{
				k = l;
			}
			while (k < l)
			{
				cols = this->tableContent.GetItemNoCheck(k);
				nextY = currY + fontHeightMM * 1.5;
				currRowType = this->tableRowType.GetItem(k);
				if (this->tableBorders)
				{
					g->DrawLine(border + drawWidth, currY, border + drawWidth, nextY, p);
				}
				else if (lastRowType == RT_CONTENT && currRowType == RT_SUMMARY)
				{
					g->DrawLine(border, currY, border + drawWidth, currY, p);
				}

				j = this->colCount;
				i = j;
				while (i-- > 0)
				{
					if (cols[i].val)
					{
						if (cols[i].val->Equals(this->ColumnMergeUp.v, this->ColumnMergeUp.leng))
						{
							if (this->tableBorders)
							{
								UnsafeArray<TableCell> cols2;
								n = m - 1;
								while (true)
								{
									cols2 = this->tableContent.GetItemNoCheck(n);
									if (cols2[i].val && cols2[i].val->Equals(this->ColumnMergeUp.v, this->ColumnMergeUp.leng))
										n--;
									else
										break;
								}
								if (cols2[i].val == 0 || !cols2[i].val->Equals(this->ColumnMergeLeft.v, this->ColumnMergeLeft.leng))
								{
									g->DrawLine(colPos[i], currY, colPos[i], nextY, p);
								}
							}
						}
						else
						{
							if (i + 1 < j)
							{
								nextX = colPos[i + 1];
							}
							else
							{
								nextX = border + drawWidth;
							}
							if (cols[i].val->Equals(this->ColumnMergeLeft.v, this->ColumnMergeLeft.leng))
							{
								while (i-- > 0)
								{
									if (!cols[i].val->Equals(this->ColumnMergeLeft.v, this->ColumnMergeLeft.leng))
									{
										break;
									}
								}
							}
							if (this->tableBorders)
							{
								g->DrawLine(colPos[i], currY, nextX, currY, p);
								g->DrawLine(colPos[i], currY, colPos[i], nextY, p);
							}
							g->DrawStringHAlign(Math::Coord2DDbl(colPos[i], currY), nextX, cols[i].val->ToCString(), f, b, cols[i].hAlign);
						}
					}
				}

				if (this->icons.GetItem(k).SetTo(iconList))
				{
					i = 0;
					j = this->colCount;
					while (i < j)
					{
						if (cols[i].val)
						{
							sz = g->GetTextSize(f, cols[i].val->ToCString());
							colCurrX[i] = colPos[i] + sz.x;
						}
						else
						{
							colCurrX[i] = colPos[i];
						}
						i++;
					}

					i = 0;
					j = iconList->GetCount();
					while (i < j)
					{
						icon = iconList->GetItemNoCheck(i);
						if (icon->fileName)
						{
							iconSt = iconStatus.Get(icon->fileName);
							NN<Media::DrawImage> dimg;
							if (iconSt && dimg.Set(iconSt->dimg))
							{
								Double w = fontHeightMM * UOSInt2Double(dimg->GetWidth()) / UOSInt2Double(dimg->GetHeight());
								Double dpi = UOSInt2Double(dimg->GetHeight()) / Math::Unit::Distance::Convert(Math::Unit::Distance::DU_MILLIMETER, Math::Unit::Distance::DU_INCH, fontHeightMM);
								dimg->SetHDPI(dpi);
								dimg->SetVDPI(dpi);
								g->DrawImagePt(dimg, Math::Coord2DDbl(colCurrX[icon->col], currY));
								colCurrX[icon->col] += w;
							}
						}
						i++;
					}
				}

				currY = nextY;
				lastRowType = currRowType;
				k++;

				if (currY >= endY)
				{
					break;
				}
			}
			if (this->tableBorders)
			{
				g->DrawLine(border, currY, border + drawWidth, currY, p);
			}
			pageId++;
			sptr = Text::StrInt32(sbuff, pageId);
			sz = g->GetTextSize(f, CSTRP(sbuff, sptr));
			g->DrawString(Math::Coord2DDbl(border + (drawWidth - sz.x) * 0.5, paperSize.GetHeightMM() - border), CSTRP(sbuff, sptr), f, b);
		}

		g->DelFont(f);
		g->DelBrush(b);
		g->DelPen(p);

		Double paperWidthMM;
		Double paperHeightMM;
		if (this->paperHori)
		{
			paperWidthMM = paperSize.GetHeightMM();
			paperHeightMM = paperSize.GetWidthMM();
		}
		else
		{
			paperWidthMM = paperSize.GetWidthMM();
			paperHeightMM = paperSize.GetHeightMM();
		}
		if (k >= l)
		{
			if (this->chart)
			{
				this->chart->Plot(g, border, currY, drawWidth, paperHeightMM - border - currY);
			}
			break;
		}
		g = doc->AddGraph(paperWidthMM, paperHeightMM, Math::Unit::Distance::DU_MILLIMETER);
		lastRowType = RT_UNKNOWN;
	}
	MemFree(colMinWidth);
	MemFree(colTotalWidth);
	MemFree(colPos);
	MemFree(colSize);
	MemFree(colCurrX);

	NN<Media::DrawImage> img;
	i = iconStatus.GetCount();
	while (i-- > 0)
	{
		iconSt = iconStatus.GetItem(i);
		if (img.Set(iconSt->dimg))
		{
			deng->DeleteImage(img);
		}
		MemFree(iconSt);
	}
	return doc;
}
