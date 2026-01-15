#include "Stdafx.h"
#include "Math/Math_C.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"
#include "Text/SpreadSheet/SpreadSheetStyleCtrl.h"
#include "Text/SpreadSheet/Worksheet.h"

using namespace Text::SpreadSheet;

Optional<Text::SpreadSheet::Worksheet::RowData> Text::SpreadSheet::Worksheet::CreateRow(UIntOS row)
{
	Optional<RowData> rowData;
	if (row >= 1048576)
		return nullptr;
	while (row >= this->rows.GetCount())
	{
		this->rows.Add(nullptr);
	}
	rowData = this->rows.GetItem(row);
	if (rowData.IsNull())
	{
		NN<RowData> nnrowData;
		NEW_CLASSNN(nnrowData, RowData());
		nnrowData->style = nullptr;
		nnrowData->height = -1;
		this->rows.SetItem(row, nnrowData);
		rowData = nnrowData;
	}
	return rowData;
}

Optional<Text::SpreadSheet::Worksheet::CellData> Text::SpreadSheet::Worksheet::GetCellData(UIntOS row, UIntOS col, Bool keepMerge)
{
	NN<RowData> rowData;
	NN<CellData> cell;
	if (row >= this->rows.GetCount() + 65536)
		return nullptr;
	if (col >= 65536)
		return nullptr;
	if (col > this->maxCol)
	{
		this->maxCol = col;
	}
	while (true)
	{
		if (!CreateRow(row).SetTo(rowData))
			return nullptr;
		while (col >= rowData->cells.GetCount())
		{
			rowData->cells.Add(nullptr);
		}
		if (!rowData->cells.GetItem(col).SetTo(cell))
		{
			cell = MemAllocNN(CellData);
			cell->cdt = CellDataType::String;
			cell->cellValue = nullptr;
			cell->style = nullptr;
			cell->mergeHori = 0;
			cell->mergeVert = 0;
			cell->hidden = false;
			cell->cellURL = nullptr;
			rowData->cells.SetItem(col, cell);
		}

		if (keepMerge)
			break;
		if (cell->cdt == CellDataType::MergedLeft)
		{
			col--;
		}
		else if (cell->cdt == CellDataType::MergedUp)
		{
			row--;
		}
		else
		{
			break;
		}
	}
	return cell;
}

void Text::SpreadSheet::Worksheet::FreeRowData(NN<Text::SpreadSheet::Worksheet::RowData> data)
{
	NN<CellData> cell;
	UIntOS i = data->cells.GetCount();
	while (i-- > 0)
	{
		if (data->cells.GetItem(i).SetTo(cell))
		{
			FreeCellData(cell);
		}
	}
	data.Delete();
}

void Text::SpreadSheet::Worksheet::FreeCellData(NN<Text::SpreadSheet::Worksheet::CellData> data)
{
	OPTSTR_DEL(data->cellValue);
	OPTSTR_DEL(data->cellURL);
	MemFreeNN(data);
}

NN<Text::SpreadSheet::Worksheet::RowData> Text::SpreadSheet::Worksheet::CloneRow(NN<RowData> row, NN<const SpreadSheetStyleCtrl> srcCtrl, NN<SpreadSheetStyleCtrl> newCtrl)
{
	NN<RowData> newRow;
	NN<CellData> cell;
	UIntOS i;
	UIntOS j;
	NN<CellStyle> tmpStyle;
	NEW_CLASSNN(newRow, RowData());
	if (row->style.SetTo(tmpStyle))
		newRow->style = newCtrl->GetStyle((UIntOS)srcCtrl->GetStyleIndex(tmpStyle));
	else
		newRow->style = nullptr;
	newRow->height = row->height;
	i = 0;
	j = row->cells.GetCount();
	while (i < j)
	{
		if (!row->cells.GetItem(i).SetTo(cell))
		{
			newRow->cells.Add(nullptr);
		}
		else
		{
			newRow->cells.Add(this->CloneCell(cell, srcCtrl, newCtrl));
		}
		i++;
	}
	return newRow;
}

NN<Text::SpreadSheet::Worksheet::CellData> Text::SpreadSheet::Worksheet::CloneCell(NN<CellData> cell, NN<const SpreadSheetStyleCtrl> srcCtrl, NN<SpreadSheetStyleCtrl> newCtrl)
{
	NN<CellData> newCell;
	newCell = MemAllocNN(CellData);
	newCell->cdt = cell->cdt;
	NN<Text::String> s;
	if (cell->cellValue.SetTo(s))
	{
		newCell->cellValue = s->Clone();
	}
	else
	{
		newCell->cellValue = nullptr;
	}
	NN<CellStyle> tmpStyle;
	if (cell->style.SetTo(tmpStyle))
		newCell->style = newCtrl->GetStyle((UIntOS)srcCtrl->GetStyleIndex(tmpStyle));
	else
		newCell->style = nullptr;
	newCell->mergeHori = cell->mergeHori;
	newCell->mergeVert = cell->mergeVert;
	newCell->hidden = cell->hidden;
	if (cell->cellURL.SetTo(s))
	{
		newCell->cellURL = s->Clone();
	}
	else
	{
		newCell->cellURL = nullptr;
	}
	return newCell;
}

void __stdcall Text::SpreadSheet::Worksheet::FreeDrawing(NN<WorksheetDrawing> drawing)
{
	drawing->chart.Delete();
	MemFreeNN(drawing);
}

Text::SpreadSheet::Worksheet::Worksheet(NN<Text::String> name)
{
	this->name = name->Clone();
	this->freezeHori = 0;
	this->freezeVert = 0;
	this->marginLeft = 2.0;
	this->marginRight = 2.0;
	this->marginTop = 2.5;
	this->marginBottom = 2.5;
	this->marginHeader = 1.3;
	this->marginFooter = 1.3;
	this->zoom = 100;
	this->options = 0x4b6;
	this->maxCol = 0;
	this->defColWidthPt = 48.0;
	this->defRowHeightPt = 13.5;
}

Text::SpreadSheet::Worksheet::Worksheet(Text::CStringNN name)
{
	this->name = Text::String::New(name);
	this->freezeHori = 0;
	this->freezeVert = 0;
	this->marginLeft = 2.0;
	this->marginRight = 2.0;
	this->marginTop = 2.5;
	this->marginBottom = 2.5;
	this->marginHeader = 1.3;
	this->marginFooter = 1.3;
	this->zoom = 100;
	this->options = 0x4b6;
	this->maxCol = 0;
	this->defColWidthPt = 48.0;
	this->defRowHeightPt = 13.5;
}

Text::SpreadSheet::Worksheet::~Worksheet()
{
	UIntOS i;
	NN<RowData> row;
	this->name->Release();
	i = this->rows.GetCount();
	while (i-- > 0)
	{
		if (this->rows.GetItem(i).SetTo(row))
		{
			FreeRowData(row);
		}
	}
	this->drawings.FreeAll(FreeDrawing);
}

NN<Text::SpreadSheet::Worksheet> Text::SpreadSheet::Worksheet::Clone(NN<const SpreadSheetStyleCtrl> srcCtrl, NN<SpreadSheetStyleCtrl> newCtrl)
{
	UIntOS i;
	UIntOS j;
	NN<Text::SpreadSheet::Worksheet> newWS;
	NN<RowData> row;
	NEW_CLASSNN(newWS, Text::SpreadSheet::Worksheet(this->name));
	newWS->freezeHori = this->freezeHori;
	newWS->freezeVert = this->freezeVert;
	newWS->marginLeft = this->marginLeft;
	newWS->marginRight = this->marginRight;
	newWS->marginTop = this->marginTop;
	newWS->marginBottom = this->marginBottom;
	newWS->marginHeader = this->marginHeader;
	newWS->marginFooter = this->marginFooter;
	newWS->options = this->options;
	newWS->zoom = this->zoom;
	i = 0;
	j = this->colWidthsPt.GetCount();
	while (i < j)
	{
		newWS->colWidthsPt.Add(this->colWidthsPt.GetItem(i));
		i++;
	}
	i = 0;
	j = this->rows.GetCount();
	while (i < j)
	{
		if (!this->rows.GetItem(i).SetTo(row))
		{
			newWS->rows.Add(nullptr);
		}
		else
		{
			newWS->rows.Add(CloneRow(row, srcCtrl, newCtrl));
		}
		i++;
	}

	return newWS;
}

void Text::SpreadSheet::Worksheet::SetOptions(UInt16 options)
{
	this->options = options;
}

UInt16 Text::SpreadSheet::Worksheet::GetOptions()
{
	return this->options;
}

void Text::SpreadSheet::Worksheet::SetFreezeHori(UInt32 freezeHori)
{
	this->freezeHori = freezeHori;
	this->options |= 0x108;
}

UInt32 Text::SpreadSheet::Worksheet::GetFreezeHori()
{
	return this->freezeHori;
}

void Text::SpreadSheet::Worksheet::SetFreezeVert(UInt32 freezeVert)
{
	this->freezeVert = freezeVert;
	this->options |= 0x108;
}

UInt32 Text::SpreadSheet::Worksheet::GetFreezeVert()
{
	return this->freezeVert;
}

void Text::SpreadSheet::Worksheet::SetMarginLeft(Double marginLeft)
{
	this->marginLeft = marginLeft;
}

Double Text::SpreadSheet::Worksheet::GetMarginLeft()
{
	return this->marginLeft;
}

void Text::SpreadSheet::Worksheet::SetMarginRight(Double marginRight)
{
	this->marginRight = marginRight;
}

Double Text::SpreadSheet::Worksheet::GetMarginRight()
{
	return this->marginRight;
}

void Text::SpreadSheet::Worksheet::SetMarginTop(Double marginTop)
{
	this->marginTop = marginTop;
}

Double Text::SpreadSheet::Worksheet::GetMarginTop()
{
	return this->marginTop;
}

void Text::SpreadSheet::Worksheet::SetMarginBottom(Double marginBottom)
{
	this->marginBottom = marginBottom;
}

Double Text::SpreadSheet::Worksheet::GetMarginBottom()
{
	return this->marginBottom;
}

void Text::SpreadSheet::Worksheet::SetMarginHeader(Double marginHeader)
{
	this->marginHeader = marginHeader;
}

Double Text::SpreadSheet::Worksheet::GetMarginHeader()
{
	return this->marginHeader;
}

void Text::SpreadSheet::Worksheet::SetMarginFooter(Double marginFooter)
{
	this->marginFooter = marginFooter;
}

Double Text::SpreadSheet::Worksheet::GetMarginFooter()
{
	return this->marginFooter;
}

void Text::SpreadSheet::Worksheet::SetZoom(UInt32 zoom)
{
	this->zoom = zoom;
}

UInt32 Text::SpreadSheet::Worksheet::GetZoom()
{
	return this->zoom;
}

Bool Text::SpreadSheet::Worksheet::IsDefaultPageSetup()
{
	return this->marginHeader == 1.3 && this->marginFooter == 1.3 && this->marginLeft == 2.0 && this->marginRight == 2.0 && this->marginTop == 2.5 && this->marginBottom == 2.5;
}

void Text::SpreadSheet::Worksheet::SetDefColWidthPt(Double defColWidthPt)
{
	this->defColWidthPt = defColWidthPt;
}

Double Text::SpreadSheet::Worksheet::GetDefColWidthPt()
{
	return this->defColWidthPt;
}

void Text::SpreadSheet::Worksheet::SetDefRowHeightPt(Double defRowHeightPt)
{
	this->defRowHeightPt = defRowHeightPt;
}

Double Text::SpreadSheet::Worksheet::GetDefRowHeightPt()
{
	return this->defRowHeightPt;
}

NN<Text::String> Text::SpreadSheet::Worksheet::GetName() const
{
	return this->name;
}

Bool Text::SpreadSheet::Worksheet::SetCellString(UIntOS row, UIntOS col, NN<Text::String> val)
{
	return this->SetCellString(row, col, nullptr, val);
}

Bool Text::SpreadSheet::Worksheet::SetCellString(UIntOS row, UIntOS col, Text::CStringNN val)
{
	return this->SetCellString(row, col, nullptr, val);
}

Bool Text::SpreadSheet::Worksheet::SetCellTS(UIntOS row, UIntOS col, Data::Timestamp val)
{
	return this->SetCellTS(row, col, nullptr, val);
}

Bool Text::SpreadSheet::Worksheet::SetCellDateTime(UIntOS row, UIntOS col, NN<Data::DateTime> val)
{
	return this->SetCellDateTime(row, col, nullptr, val);
}

Bool Text::SpreadSheet::Worksheet::SetCellDouble(UIntOS row, UIntOS col, Double val)
{
	return this->SetCellDouble(row, col, nullptr, val);
}

Bool Text::SpreadSheet::Worksheet::SetCellInt32(UIntOS row, UIntOS col, Int32 val)
{
	return this->SetCellInt32(row, col, nullptr, val);
}

Bool Text::SpreadSheet::Worksheet::SetCellInt64(UIntOS row, UIntOS col, Int64 val)
{
	return this->SetCellInt64(row, col, nullptr, val);
}

Bool Text::SpreadSheet::Worksheet::SetCellEmpty(UIntOS row, UIntOS col)
{
	return this->SetCellEmpty(row, col, nullptr);
}

Bool Text::SpreadSheet::Worksheet::SetCellStyle(UIntOS row, UIntOS col, Optional<Text::SpreadSheet::CellStyle> style)
{
	NN<CellData> cell;
	if (!GetCellData(row, col, false).SetTo(cell))
		return false;
	cell->style = style;
	return true;
}

Bool Text::SpreadSheet::Worksheet::SetCellStyleHAlign(UIntOS row, UIntOS col, NN<SpreadSheetStyleCtrl> wb, HAlignment hAlign)
{
	NN<CellData> cell;
	if (!GetCellData(row, col, false).SetTo(cell))
		return false;
	NN<CellStyle> tmpStyle;
	if (!cell->style.SetTo(tmpStyle))
	{
		if (hAlign == Text::HAlignment::Unknown)
			return true;
		NEW_CLASSNN(tmpStyle, CellStyle(0));
		tmpStyle->SetHAlign(hAlign);
		cell->style = wb->FindOrCreateStyle(tmpStyle);
		tmpStyle.Delete();
	}
	else
	{
		if (tmpStyle->GetHAlign() == hAlign)
			return true;
		tmpStyle = tmpStyle->Clone();
		tmpStyle->SetHAlign(hAlign);
		cell->style = wb->FindOrCreateStyle(tmpStyle);
		tmpStyle.Delete();
	}
	return true;
}

Bool Text::SpreadSheet::Worksheet::SetCellStyleBorderBottom(UIntOS row, UIntOS col, NN<SpreadSheetStyleCtrl> wb, UInt32 color, BorderType borderType)
{
	NN<CellData> cell;
	if (!GetCellData(row, col, true).SetTo(cell))
		return false;
	NN<CellStyle> tmpStyle;
	if (!cell->style.SetTo(tmpStyle))
	{
		if (borderType == Text::SpreadSheet::BorderType::None)
			return true;
		NEW_CLASSNN(tmpStyle, CellStyle(0));
		tmpStyle->SetBorderBottom(Text::SpreadSheet::CellStyle::BorderStyle(color, borderType));
		cell->style = wb->FindOrCreateStyle(tmpStyle);
		tmpStyle.Delete();
	}
	else
	{
		
		if (tmpStyle->GetBorderBottom() == Text::SpreadSheet::CellStyle::BorderStyle(color, borderType))
			return true;
		tmpStyle = tmpStyle->Clone();
		tmpStyle->SetBorderBottom(Text::SpreadSheet::CellStyle::BorderStyle(color, borderType));
		cell->style = wb->FindOrCreateStyle(tmpStyle);
		tmpStyle.Delete();
	}
	return true;
}

Bool Text::SpreadSheet::Worksheet::SetCellURL(UIntOS row, UIntOS col, Optional<Text::String> url)
{
	NN<CellData> cell;
	if (!GetCellData(row, col, false).SetTo(cell))
		return false;
	OPTSTR_DEL(cell->cellURL);
	NN<Text::String> nnurl;
	if (url.SetTo(nnurl))
	{
		cell->cellURL = nnurl->Clone();
	}
	return true;
}

Bool Text::SpreadSheet::Worksheet::SetCellURL(UIntOS row, UIntOS col, Text::CString url)
{
	NN<CellData> cell;
	if (!GetCellData(row, col, false).SetTo(cell))
		return false;
	OPTSTR_DEL(cell->cellURL);
	Text::CStringNN nnurl;
	if (url.SetTo(nnurl) && nnurl.leng > 0)
	{
		cell->cellURL = Text::String::New(nnurl);
	}
	return true;
}

Bool Text::SpreadSheet::Worksheet::SetCellString(UIntOS row, UIntOS col, Optional<CellStyle> style, NN<Text::String> val)
{
	NN<CellData> cell;
	if (!GetCellData(row, col, false).SetTo(cell))
		return false;
	cell->cdt = CellDataType::String;
	OPTSTR_DEL(cell->cellValue);
	cell->cellValue = val->Clone();
	if (!style.IsNull()) cell->style = style;
	return true;
}

Bool Text::SpreadSheet::Worksheet::SetCellString(UIntOS row, UIntOS col, Optional<CellStyle> style, Text::CStringNN val)
{
	NN<CellData> cell;
	if (!GetCellData(row, col, false).SetTo(cell))
		return false;
	cell->cdt = CellDataType::String;
	OPTSTR_DEL(cell->cellValue);
	if (val.leng > 0)
	{
		cell->cellValue = Text::String::New(val);
	}
	if (!style.IsNull()) cell->style = style;
	return true;
}

Bool Text::SpreadSheet::Worksheet::SetCellTS(UIntOS row, UIntOS col, Optional<CellStyle> style, Data::Timestamp val)
{
	UTF8Char sbuff[32];
	UnsafeArray<UTF8Char> sptr;
	NN<CellData> cell;
	if (!GetCellData(row, col, false).SetTo(cell))
		return false;
	cell->cdt = CellDataType::DateTime;
	OPTSTR_DEL(cell->cellValue);
	sptr = val.ToString(sbuff, "yyyy-MM-ddTHH:mm:ss.fffffffff");
	cell->cellValue = Text::String::NewP(sbuff, sptr);
	if (style.NotNull())
	{
		 cell->style = style;
	}
	else
	{
		CellStyle tmpStyle(0);
		UInt32 nanosec = val.inst.nanosec;
		if (nanosec == 0)
		{
			tmpStyle.SetDataFormat(CSTR("YYYY-MM-DD HH:MM:SS"));
		}
		else if (nanosec % 1000000 == 0)
		{
			tmpStyle.SetDataFormat(CSTR("YYYY-MM-DD HH:MM:SS.000"));
		}
		else
		{
			tmpStyle.SetDataFormat(CSTR("YYYY-MM-DD HH:MM:SS.00000"));
		}
	}
	return true;
}

Bool Text::SpreadSheet::Worksheet::SetCellDateTime(UIntOS row, UIntOS col, Optional<CellStyle> style, NN<Data::DateTime> val)
{
	UTF8Char sbuff[32];
	UnsafeArray<UTF8Char> sptr;
	NN<CellData> cell;
	if (!GetCellData(row, col, false).SetTo(cell))
		return false;
	cell->cdt = CellDataType::DateTime;
	OPTSTR_DEL(cell->cellValue);
	sptr = val->ToString(sbuff, "yyyy-MM-ddTHH:mm:ss.fffffffff");
	cell->cellValue = Text::String::NewP(sbuff, sptr);
	if (style.NotNull()) cell->style = style;
	return true;
}

Bool Text::SpreadSheet::Worksheet::SetCellDouble(UIntOS row, UIntOS col, Optional<CellStyle> style, Double val)
{
	UTF8Char sbuff[32];
	UnsafeArray<UTF8Char> sptr;
	NN<CellData> cell;
	if (!GetCellData(row, col, false).SetTo(cell))
		return false;
	cell->cdt = CellDataType::Number;
	OPTSTR_DEL(cell->cellValue);
	if (!Math::IsNAN(val))
	{
		sptr = Text::StrDouble(sbuff, val);
		cell->cellValue = Text::String::NewP(sbuff, sptr);
	}
	if (style.NotNull()) cell->style = style;
	return true;
}

Bool Text::SpreadSheet::Worksheet::SetCellInt32(UIntOS row, UIntOS col, Optional<CellStyle> style, Int32 val)
{
	UTF8Char sbuff[32];
	UnsafeArray<UTF8Char> sptr;
	NN<CellData> cell;
	if (!GetCellData(row, col, false).SetTo(cell))
		return false;
	cell->cdt = CellDataType::Number;
	OPTSTR_DEL(cell->cellValue);
	sptr = Text::StrInt32(sbuff, val);
	cell->cellValue = Text::String::NewP(sbuff, sptr);
	if (style.NotNull()) cell->style = style;
	return true;
}

Bool Text::SpreadSheet::Worksheet::SetCellInt64(UIntOS row, UIntOS col, Optional<CellStyle> style, Int64 val)
{
	UTF8Char sbuff[32];
	UnsafeArray<UTF8Char> sptr;
	NN<CellData> cell;
	if (!GetCellData(row, col, false).SetTo(cell))
		return false;
	cell->cdt = CellDataType::Number;
	OPTSTR_DEL(cell->cellValue);
	sptr = Text::StrInt64(sbuff, val);
	cell->cellValue = Text::String::NewP(sbuff, sptr);
	if (style.NotNull()) cell->style = style;
	return true;
}

Bool Text::SpreadSheet::Worksheet::SetCellEmpty(UIntOS row, UIntOS col, Optional<CellStyle> style)
{
	NN<CellData> cell;
	if (!GetCellData(row, col, false).SetTo(cell))
		return false;
	cell->cdt = CellDataType::Number;
	OPTSTR_DEL(cell->cellValue);
	if (style.NotNull()) cell->style = style;
	return true;
}

Bool Text::SpreadSheet::Worksheet::MergeCells(UIntOS row, UIntOS col, UInt32 height, UInt32 width)
{
	if (width == 0)
		return false;
	if (height == 0)
		return false;
	if (width == 0 && height == 0)
		return false;

	NN<CellData> cell;
	UInt32 i;
	UInt32 j;
	i = 0;
	while (i < height)
	{
		j = 0;
		while (j < width)
		{
			if (GetCellData(row + i, col + j, true).SetTo(cell) && (cell->cdt == CellDataType::MergedLeft || cell->cdt == CellDataType::MergedUp))
				return false;
			j++;
		}
		i++;
	}

	i = 0;
	while (i < height)
	{
		j = 0;
		while (j < width)
		{
			if (GetCellData(row + i, col + j, true).SetTo(cell))
			{
				if (i == 0)
				{
					if (j == 0)
					{
						cell->mergeHori = width;
						cell->mergeVert = height;
					}
					else
					{
						cell->cdt = CellDataType::MergedLeft;
					}
				}
				else
				{
					cell->cdt = CellDataType::MergedUp;
				}
			}
			j++;
		}
		i++;
	}
	return true;
}

Bool Text::SpreadSheet::Worksheet::SetCellMergeLeft(UIntOS row, UIntOS col)
{
	if (col == 0)
		return false;

	NN<CellData> cell;
	UInt32 width = 1;
	UInt32 height = 1;
	if (GetCellData(row, col, true).SetTo(cell))
		cell->cdt = CellDataType::MergedLeft;
	col--;
	width++;
	while (true)
	{
		if (!GetCellData(row, col, true).SetTo(cell))
		{
			return false;
		}
		else if (cell->cdt == CellDataType::MergedUp)
		{
			row--;
			height++;
		}
		else if (cell->cdt == CellDataType::MergedLeft)
		{
			col--;
			width++;
		}
		else
		{
			if (cell->mergeHori < width)
			{
				cell->mergeHori = width;
			}
			if (cell->mergeVert < height)
			{
				cell->mergeVert = height;
			}
			break;
		}
	}
	return true;
}

Bool Text::SpreadSheet::Worksheet::SetCellMergeUp(UIntOS row, UIntOS col)
{
	if (row == 0)
		return false;

	NN<CellData> cell;
	UInt32 width = 1;
	UInt32 height = 1;
	if (GetCellData(row, col, true).SetTo(cell))
		cell->cdt = CellDataType::MergedUp;
	row--;
	height++;
	while (true)
	{
		if (!GetCellData(row, col, true).SetTo(cell))
		{
			return false;
		}
		else if (cell->cdt == CellDataType::MergedUp)
		{
			row--;
			height++;
		}
		else if (cell->cdt == CellDataType::MergedLeft)
		{
			col--;
			width++;
		}
		else
		{
			if (cell->mergeHori < width)
			{
				cell->mergeHori = width;
			}
			if (cell->mergeVert < height)
			{
				cell->mergeVert = height;
			}
			break;
		}
	}
	return true;
}

Bool Text::SpreadSheet::Worksheet::SetRowHidden(UIntOS row, Bool hidden)
{
	NN<CellData> cell;
	if (row >= 65536 || !GetCellData(row, 0, true).SetTo(cell))
		return false;
	cell->hidden = hidden;
	return true;
}

Bool Text::SpreadSheet::Worksheet::SetRowHeight(UIntOS row, Double height)
{
	NN<RowData> rowData;
	if (CreateRow(row).SetTo(rowData))
	{
		rowData->height = height;
		return true;
	}
	else
	{
		return false;
	}
}

UIntOS Text::SpreadSheet::Worksheet::GetCount()
{
	return this->rows.GetCount();
}

Optional<Text::SpreadSheet::Worksheet::RowData> Text::SpreadSheet::Worksheet::GetItem(UIntOS row)
{
	return this->rows.GetItem(row);
}

void Text::SpreadSheet::Worksheet::RemoveCol(UIntOS col)
{
	UIntOS i;
	NN<RowData> row;
	NN<CellData> cell;

	this->colWidthsPt.RemoveAt(col);
	i = this->rows.GetCount();
	while (i-- > 0)
	{
		if (this->rows.GetItem(i).SetTo(row))
		{
			if (row->cells.RemoveAt(col).SetTo(cell))
			{
				FreeCellData(cell);
			}
		}
	}
}

void Text::SpreadSheet::Worksheet::InsertCol(UIntOS col)
{
	UIntOS i;
	NN<RowData> row;

	if (this->colWidthsPt.GetCount() > col)
	{
		this->colWidthsPt.Insert(col, 0);
	}
	i = this->rows.GetCount();
	while (i-- > 0)
	{
		if (this->rows.GetItem(i).SetTo(row))
		{
			if (row->cells.GetCount() > col)
			{
				row->cells.Insert(col, nullptr);
			}
		}
	}
}

UIntOS Text::SpreadSheet::Worksheet::GetMaxCol()
{
	return this->maxCol;
}

void Text::SpreadSheet::Worksheet::SetColWidth(UIntOS col, Double width, Math::Unit::Distance::DistanceUnit unit)
{
	while (col >= this->colWidthsPt.GetCount())
	{
		this->colWidthsPt.Add(-1);
	}
	if (unit == Math::Unit::Distance::DU_POINT)
	{
		this->colWidthsPt.SetItem(col, width);
	}
	else
	{
		this->colWidthsPt.SetItem(col, Math::Unit::Distance::Convert(unit, Math::Unit::Distance::DU_POINT, width));
	}
}

UIntOS Text::SpreadSheet::Worksheet::GetColWidthCount()
{
	return this->colWidthsPt.GetCount();
}

Double Text::SpreadSheet::Worksheet::GetColWidthPt(UIntOS col)
{
	if (col >= this->colWidthsPt.GetCount())
		return -1;
	return this->colWidthsPt.GetItem(col);
}

Double Text::SpreadSheet::Worksheet::GetColWidth(UIntOS col, Math::Unit::Distance::DistanceUnit unit)
{
	if (col >= this->colWidthsPt.GetCount())
		return -1;
	if (unit == Math::Unit::Distance::DU_POINT)
		return this->colWidthsPt.GetItem(col);
	return Math::Unit::Distance::Convert(Math::Unit::Distance::DU_POINT, unit, this->colWidthsPt.GetItem(col));
}

Optional<const Text::SpreadSheet::Worksheet::CellData> Text::SpreadSheet::Worksheet::GetCellDataRead(UIntOS row, UIntOS col) const
{
	NN<RowData> rowData;
	NN<CellData> cell;
	if (row >= this->rows.GetCount() + 65536)
		return nullptr;
	if (col >= 65536)
		return nullptr;
	if (col > this->maxCol)
	{
		return nullptr;
	}
	while (true)
	{
		if (!this->rows.GetItem(row).SetTo(rowData))
			return nullptr;
		if (!rowData->cells.GetItem(col).SetTo(cell))
		{
			return nullptr;
		}
		if (cell->cdt == CellDataType::MergedLeft)
		{
			col--;
		}
		else if (cell->cdt == CellDataType::MergedUp)
		{
			row--;
		}
		else
		{
			break;
		}
	}
	return cell;
}

Bool Text::SpreadSheet::Worksheet::GetCellString(const CellData *cell, NN<Text::StringBuilderUTF8> sb)
{
	NN<Text::String> cellValue;
	if (cell == 0 || !cell->cellValue.SetTo(cellValue))
	{
		return false;
	}
	if (cell->cdt == Text::SpreadSheet::CellDataType::Number)
	{
		Double v;
		Int32 iv;
		if (!cellValue->ToDouble(v))
		{
			sb->Append(cellValue);
		}
		else
		{
			iv = Double2Int32(v);
			if (iv == v)
			{
				sb->AppendI32(iv);
			}
			else
			{
				sb->AppendDouble(v);
			}
/*			Text::String *fmt;
			if (cell->style && (fmt = cell->style->GetDataFormat()) != 0)
			{
				printf("Style: %s\r\n", fmt->v);
			}
			else
			{
				printf("Style: null\r\n");
			}*/
		}
	}
	else
	{
		sb->Append(cellValue);
	}
	return true;
}

UIntOS Text::SpreadSheet::Worksheet::GetDrawingCount()
{
	return this->drawings.GetCount();
}

Optional<Text::SpreadSheet::WorksheetDrawing> Text::SpreadSheet::Worksheet::GetDrawing(UIntOS index)
{
	return this->drawings.GetItem(index);
}

NN<Text::SpreadSheet::WorksheetDrawing> Text::SpreadSheet::Worksheet::GetDrawingNoCheck(UIntOS index)
{
	return this->drawings.GetItemNoCheck(index);
}

NN<Text::SpreadSheet::WorksheetDrawing> Text::SpreadSheet::Worksheet::CreateDrawing(Math::Unit::Distance::DistanceUnit unit, Double x, Double y, Double w, Double h)
{
	Math::Unit::Distance::DistanceUnit inch = Math::Unit::Distance::DU_INCH;
	NN<WorksheetDrawing> drawing = MemAllocNN(WorksheetDrawing);
	drawing->anchorType = AnchorType::Absolute;
	drawing->posXInch = Math::Unit::Distance::Convert(unit, inch, x);
	drawing->posYInch = Math::Unit::Distance::Convert(unit, inch, y);
	drawing->widthInch = Math::Unit::Distance::Convert(unit, inch, w);
	drawing->heightInch = Math::Unit::Distance::Convert(unit, inch, h);
	drawing->col1 = 0;
	drawing->row1 = 0;
	drawing->col2 = 0;
	drawing->row2 = 0;
	drawing->chart = nullptr;
	this->drawings.Add(drawing);
	return drawing;
}

NN<Text::SpreadSheet::OfficeChart> Text::SpreadSheet::Worksheet::CreateChart(Math::Unit::Distance::DistanceUnit du, Double x, Double y, Double w, Double h, Text::CString title)
{
	NN<WorksheetDrawing> drawing = this->CreateDrawing(du, x, y, w, h);
	NN<OfficeChart> chart;
	NEW_CLASSNN(chart, OfficeChart(du, x, y, w, h));
	drawing->chart = chart;
	if (title.leng > 0)
	{
		chart->SetTitleText(title);
	}
	chart->SetShapeProp(NEW_CLASS_D(OfficeShapeProp(
		OfficeFill::NewSolidFill(OfficeColor::NewPreset(PresetColor::White)),
		NEW_CLASS_D(OfficeLineStyle(OfficeFill::NewSolidFill())))));
	return chart;
}

Text::CString Text::SpreadSheet::CellDataTypeGetName(Text::SpreadSheet::CellDataType val)
{
	switch (val)
	{
	case Text::SpreadSheet::CellDataType::String:
		return CSTR("String");
	case Text::SpreadSheet::CellDataType::Number:
		return CSTR("Number");
	case Text::SpreadSheet::CellDataType::DateTime:
		return CSTR("DateTime");
	case Text::SpreadSheet::CellDataType::MergedLeft:
		return CSTR("MergedLeft");
	case Text::SpreadSheet::CellDataType::MergedUp:
		return CSTR("MergedUp");
	default:
		return nullptr;
	}
}
