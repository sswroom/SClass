#include "Stdafx.h"
#include "Math/Math.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"
#include "Text/SpreadSheet/IStyleCtrl.h"
#include "Text/SpreadSheet/Worksheet.h"

Text::SpreadSheet::Worksheet::RowData *Text::SpreadSheet::Worksheet::CreateRow(UInt32 row)
{
	RowData *rowData;
	if (row >= 65536)
		return 0;
	while (row >= (UInt32)this->rows->GetCount())
	{
		this->rows->Add(0);
	}
	rowData = this->rows->GetItem(row);
	if (rowData == 0)
	{
		rowData = MemAlloc(RowData, 1);
		rowData->style = 0;
		NEW_CLASS(rowData->cells, Data::ArrayList<CellData*>());
		rowData->height = -1;
		this->rows->SetItem(row, rowData);
	}
	return rowData;
}

Text::SpreadSheet::Worksheet::CellData *Text::SpreadSheet::Worksheet::GetCellData(UInt32 row, UInt32 col, Bool keepMerge)
{
	RowData *rowData;
	CellData *cell;
	if (row >= 65536)
		return 0;
	if (col >= 65536)
		return 0;
	while (true)
	{
		rowData = CreateRow(row);
		while (col >= (UInt32)rowData->cells->GetCount())
		{
			rowData->cells->Add(0);
		}
		cell = rowData->cells->GetItem(col);
		if (cell == 0)
		{
			cell = MemAlloc(CellData, 1);
			cell->cdt = CDT_STRING;
			cell->cellValue = 0;
			cell->style = 0;
			cell->mergeHori = 0;
			cell->mergeVert = 0;
			cell->hidden = false;
			cell->cellURL = 0;
			rowData->cells->SetItem(col, cell);
		}

		if (keepMerge)
			break;
		if (cell->cdt == CDT_MERGEDLEFT)
		{
			col--;
		}
		else if (cell->cdt == CDT_MERGEDTOP)
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

void Text::SpreadSheet::Worksheet::FreeRowData(Text::SpreadSheet::Worksheet::RowData *data)
{
	CellData *cell;
	if (data->cells)
	{
		OSInt i = data->cells->GetCount();
		while (i-- > 0)
		{
			cell = data->cells->GetItem(i);
			if (cell)
			{
				FreeCellData(cell);
			}
		}
		DEL_CLASS(data->cells);
	}
	MemFree(data);
}

void Text::SpreadSheet::Worksheet::FreeCellData(Text::SpreadSheet::Worksheet::CellData *data)
{
	if (data->cellValue)
	{
		Text::StrDelNew(data->cellValue);
	}
	if (data->cellURL)
	{
		Text::StrDelNew(data->cellURL);
	}
	MemFree(data);
}

Text::SpreadSheet::Worksheet::RowData *Text::SpreadSheet::Worksheet::CloneRow(RowData *row, IStyleCtrl *srcCtrl, IStyleCtrl *newCtrl)
{
	RowData *newRow;
	CellData *cell;
	OSInt i;
	OSInt j;
	newRow = MemAlloc(RowData, 1);
	newRow->style = newCtrl->GetStyle(srcCtrl->GetStyleIndex(row->style));
	NEW_CLASS(newRow->cells, Data::ArrayList<CellData*>());
	newRow->height = row->height;
	i = 0;
	j = row->cells->GetCount();
	while (i < j)
	{
		cell = row->cells->GetItem(i);
		if (cell == 0)
		{
			newRow->cells->Add(0);
		}
		else
		{
			newRow->cells->Add(this->CloneCell(cell, srcCtrl, newCtrl));
		}
		i++;
	}
	return newRow;
}

Text::SpreadSheet::Worksheet::CellData *Text::SpreadSheet::Worksheet::CloneCell(CellData *cell, IStyleCtrl *srcCtrl, IStyleCtrl *newCtrl)
{
	CellData *newCell;
	newCell = MemAlloc(CellData, 1);
	newCell->cdt = cell->cdt;
	if (cell->cellValue)
	{
		newCell->cellValue = Text::StrCopyNew(cell->cellValue);
	}
	else
	{
		newCell->cellValue = 0;
	}
	newCell->style = newCtrl->GetStyle(srcCtrl->GetStyleIndex(cell->style));
	newCell->mergeHori = cell->mergeHori;
	newCell->mergeVert = cell->mergeVert;
	newCell->hidden = cell->hidden;
	if (cell->cellURL)
	{
		newCell->cellURL = Text::StrCopyNew(cell->cellURL);
	}
	else
	{
		newCell->cellURL = 0;
	}
	return newCell;
}

Text::SpreadSheet::Worksheet::Worksheet(const UTF8Char *name)
{
	this->name = Text::StrCopyNew(name);
	this->freezeHori = 0;
	this->freezeVert = 0;
	this->marginLeft = 2.0;
	this->marginRight = 2.0;
	this->marginTop = 2.5;
	this->marginBottom = 2.5;
	this->marginHeader = 1.3;
	this->marginFooter = 1.3;
	this->zoom = 0;
	this->options = 0x4b6;
	NEW_CLASS(rows, Data::ArrayList<RowData*>());
	NEW_CLASS(this->colWidths, Data::ArrayListDbl());
}

Text::SpreadSheet::Worksheet::~Worksheet()
{
	OSInt i;
	RowData *row;
	Text::StrDelNew(this->name);
	i = this->rows->GetCount();
	while (i-- > 0)
	{
		row = this->rows->GetItem(i);
		if (row)
		{
			FreeRowData(row);
		}
	}
	DEL_CLASS(this->rows);
	DEL_CLASS(this->colWidths);
}

Text::SpreadSheet::Worksheet *Text::SpreadSheet::Worksheet::Clone(IStyleCtrl *srcCtrl, IStyleCtrl *newCtrl)
{
	OSInt i;
	OSInt j;
	Text::SpreadSheet::Worksheet *newWS;
	RowData *row;
	NEW_CLASS(newWS, Text::SpreadSheet::Worksheet(this->name));
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
	j = this->colWidths->GetCount();
	while (i < j)
	{
		newWS->colWidths->Add(this->colWidths->GetItem(i));
		i++;
	}
	i = 0;
	j = this->rows->GetCount();
	while (i < j)
	{
		row = this->rows->GetItem(i);
		if (row == 0)
		{
			newWS->rows->Add(0);
		}
		else
		{
			newWS->rows->Add(CloneRow(row, srcCtrl, newCtrl));
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

void Text::SpreadSheet::Worksheet::SetZoom(Int32 zoom)
{
	this->zoom = zoom;
}

Int32 Text::SpreadSheet::Worksheet::GetZoom()
{
	return this->zoom;
}

Bool Text::SpreadSheet::Worksheet::IsDefaultPageSetup()
{
	return this->marginHeader == 1.3 && this->marginFooter == 1.3 && this->marginLeft == 2.0 && this->marginRight == 2.0 && this->marginTop == 2.5 && this->marginBottom == 2.5;
}

const UTF8Char *Text::SpreadSheet::Worksheet::GetName()
{
	return this->name;
}

Bool Text::SpreadSheet::Worksheet::SetCellString(UInt32 row, UInt32 col, const UTF8Char *val)
{
	CellData *cell;
	if (row == 0)
		return false;
	if (col == 0)
		return false;
	cell = GetCellData(row, col, false);
	if (cell == 0)
		return false;
	cell->cdt = CDT_STRING;
	if (cell->cellValue)
	{
		Text::StrDelNew(cell->cellValue);
		cell->cellValue = 0;
	}
	if (val)
	{
		cell->cellValue = Text::StrCopyNew(val);
	}
	return true;
}

Bool Text::SpreadSheet::Worksheet::SetCellDate(UInt32 row, UInt32 col, Data::DateTime *val)
{
	UTF8Char sbuff[32];
	CellData *cell;
	if (row == 0)
		return false;
	if (col == 0)
		return false;
	cell = GetCellData(row, col, false);
	if (cell == 0)
		return false;
	cell->cdt = CDT_DATETIME;
	if (cell->cellValue)
	{
		Text::StrDelNew(cell->cellValue);
	}
	val->ToString(sbuff, "yyyy-MM-ddTHH:mm:ss.fff");
	cell->cellValue = Text::StrCopyNew(sbuff);
	return true;
}

Bool Text::SpreadSheet::Worksheet::SetCellDouble(UInt32 row, UInt32 col, Double val)
{
	UTF8Char sbuff[32];
	CellData *cell;
	if (row == 0)
		return false;
	if (col == 0)
		return false;
	cell = GetCellData(row, col, false);
	if (cell == 0)
		return false;
	cell->cdt = CDT_NUMBER;
	if (cell->cellValue)
	{
		Text::StrDelNew(cell->cellValue);
	}
	Text::StrDouble(sbuff, val);
	cell->cellValue = Text::StrCopyNew(sbuff);
	return true;
}

Bool Text::SpreadSheet::Worksheet::SetCellInt32(UInt32 row, UInt32 col, Int32 val)
{
	UTF8Char sbuff[32];
	CellData *cell;
	if (row == 0)
		return false;
	if (col == 0)
		return false;
	cell = GetCellData(row, col, false);
	if (cell == 0)
		return false;
	cell->cdt = CDT_NUMBER;
	if (cell->cellValue)
	{
		Text::StrDelNew(cell->cellValue);
	}
	Text::StrInt32(sbuff, val);
	cell->cellValue = Text::StrCopyNew(sbuff);
	return true;
}

Bool Text::SpreadSheet::Worksheet::SetCellStyle(UInt32 row, UInt32 col, Text::SpreadSheet::CellStyle *style)
{
	CellData *cell;
	if (row == 0)
		return false;
	if (col == 0)
		return false;
	cell = GetCellData(row, col, false);
	if (cell == 0)
		return false;
	cell->style = style;
	return true;
}


Bool Text::SpreadSheet::Worksheet::SetCellURL(UInt32 row, UInt32 col, const UTF8Char *url)
{
	CellData *cell;
	if (row == 0)
		return false;
	if (col == 0)
		return false;
	cell = GetCellData(row, col, false);
	if (cell == 0)
		return false;
	SDEL_TEXT(cell->cellURL);
	if (url)
	{
		cell->cellURL = Text::StrCopyNew(url);
	}
	return true;
}

Bool Text::SpreadSheet::Worksheet::MergeCells(UInt32 row, UInt32 col, UInt32 height, UInt32 width)
{
	if (row == 0)
		return false;
	if (col == 0)
		return false;
	if (width == 0)
		return false;
	if (height == 0)
		return false;
	if (width == 0 && height == 0)
		return false;

	CellData *cell;
	UInt32 i;
	UInt32 j;
	i = 0;
	while (i < height)
	{
		j = 0;
		while (j < width)
		{
			cell = GetCellData(row + i, col + j, true);
			if (cell->cdt == CDT_MERGEDLEFT || cell->cdt == CDT_MERGEDTOP)
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
			cell = GetCellData(row + i, col + j, true);
			if (i == 0)
			{
				if (j == 0)
				{
					cell->mergeHori = width;
					cell->mergeVert = height;
				}
				else
				{
					cell->cdt = CDT_MERGEDTOP;
				}
			}
			else
			{
				cell->cdt = CDT_MERGEDLEFT;
			}
			j++;
		}
		i++;
	}
	return true;
}

Bool Text::SpreadSheet::Worksheet::SetRowHidden(UInt32 row, Bool hidden)
{
	if (row == 0)
		return false;
	if (row >= 65536)
		return false;
	CellData *cell;
	cell = GetCellData(row, 0, true);
	cell->hidden = hidden;
	return true;
}

Bool Text::SpreadSheet::Worksheet::SetRowHeight(UInt32 row, Double height)
{
	RowData *rowData = CreateRow(row);
	if (rowData)
	{
		rowData->height = height;
		return true;
	}
	else
	{
		return false;
	}
}

UOSInt Text::SpreadSheet::Worksheet::GetCount()
{
	return this->rows->GetCount();
}

Text::SpreadSheet::Worksheet::RowData *Text::SpreadSheet::Worksheet::GetItem(UOSInt Index)
{
	return this->rows->GetItem(Index);
}

void Text::SpreadSheet::Worksheet::RemoveCol(UInt32 col)
{
	OSInt i;
	RowData *row;
	CellData *cell;

	this->colWidths->RemoveAt(col);
	i = this->rows->GetCount();
	while (i-- > 0)
	{
		row = this->rows->GetItem(i);
		if (row)
		{
			cell = row->cells->RemoveAt(col);
			if (cell)
			{
				FreeCellData(cell);
			}
		}
	}
}

void Text::SpreadSheet::Worksheet::InsertCol(UInt32 col)
{
	OSInt i;
	RowData *row;

	if (colWidths->GetCount() > col)
	{
		this->colWidths->Insert(col, 0);
	}
	i = this->rows->GetCount();
	while (i-- > 0)
	{
		row = this->rows->GetItem(i);
		if (row)
		{
			if (row->cells->GetCount() > col)
			{
				row->cells->Insert(col, 0);
			}
		}
	}
}

void Text::SpreadSheet::Worksheet::SetColWidth(UOSInt Index, Double width)
{
	while (Index >= this->colWidths->GetCount())
	{
		this->colWidths->Add(-1);
	}
	this->colWidths->SetItem(Index, width);
}

UOSInt Text::SpreadSheet::Worksheet::GetColWidthCount()
{
	return this->colWidths->GetCount();
}

Double Text::SpreadSheet::Worksheet::GetColWidth(UOSInt Index)
{
	if (Index >= this->colWidths->GetCount())
		return -1;
	return this->colWidths->GetItem(Index);
}

void Text::SpreadSheet::Worksheet::Number2Time(Data::DateTime *dt, Double number)
{
	if (number < 61)
	{
		number += 1;
	}
	Int32 inum = (Int32)number;
	Int32 ms;
	Int32 s;
	Int32 m;
	dt->SetValue(1899, 12, 30, 0, 0, 0, 0);
	dt->AddDay(inum);
	number -= inum;
	inum = Math::Double2Int32(number * 86400000.0);
	ms = inum % 1000;
	inum = inum / 1000;
	s = inum % 60;
	inum = inum / 60;
	m = inum % 60;
	inum = inum / 60;
	dt->SetValue(dt->GetYear(), dt->GetMonth(), dt->GetDay(), inum, m, s, ms);
}

