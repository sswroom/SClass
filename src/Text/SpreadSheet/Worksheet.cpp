#include "Stdafx.h"
#include "Math/Math.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"
#include "Text/SpreadSheet/IStyleCtrl.h"
#include "Text/SpreadSheet/Worksheet.h"

using namespace Text::SpreadSheet;

Text::SpreadSheet::Worksheet::RowData *Text::SpreadSheet::Worksheet::CreateRow(UOSInt row)
{
	RowData *rowData;
	if (row >= 1048576)
		return 0;
	while (row >= this->rows->GetCount())
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

Text::SpreadSheet::Worksheet::CellData *Text::SpreadSheet::Worksheet::GetCellData(UOSInt row, UOSInt col, Bool keepMerge)
{
	RowData *rowData;
	CellData *cell;
	if (row >= this->rows->GetCount() + 65536)
		return 0;
	if (col >= 65536)
		return 0;
	if (col > this->maxCol)
	{
		this->maxCol = col;
	}
	while (true)
	{
		rowData = CreateRow(row);
		if (rowData == 0)
			return 0;
		while (col >= rowData->cells->GetCount())
		{
			rowData->cells->Add(0);
		}
		cell = rowData->cells->GetItem(col);
		if (cell == 0)
		{
			cell = MemAlloc(CellData, 1);
			cell->cdt = CellDataType::String;
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

void Text::SpreadSheet::Worksheet::FreeRowData(Text::SpreadSheet::Worksheet::RowData *data)
{
	CellData *cell;
	if (data->cells)
	{
		UOSInt i = data->cells->GetCount();
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
	SDEL_STRING(data->cellValue);
	SDEL_STRING(data->cellURL);
	MemFree(data);
}

Text::SpreadSheet::Worksheet::RowData *Text::SpreadSheet::Worksheet::CloneRow(RowData *row, const IStyleCtrl *srcCtrl, IStyleCtrl *newCtrl)
{
	RowData *newRow;
	CellData *cell;
	UOSInt i;
	UOSInt j;
	newRow = MemAlloc(RowData, 1);
	newRow->style = newCtrl->GetStyle((UOSInt)srcCtrl->GetStyleIndex(row->style));
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

Text::SpreadSheet::Worksheet::CellData *Text::SpreadSheet::Worksheet::CloneCell(CellData *cell, const IStyleCtrl *srcCtrl, IStyleCtrl *newCtrl)
{
	CellData *newCell;
	newCell = MemAlloc(CellData, 1);
	newCell->cdt = cell->cdt;
	if (cell->cellValue)
	{
		newCell->cellValue = cell->cellValue->Clone().Ptr();
	}
	else
	{
		newCell->cellValue = 0;
	}
	newCell->style = newCtrl->GetStyle((UOSInt)srcCtrl->GetStyleIndex(cell->style));
	newCell->mergeHori = cell->mergeHori;
	newCell->mergeVert = cell->mergeVert;
	newCell->hidden = cell->hidden;
	if (cell->cellURL)
	{
		newCell->cellURL = cell->cellURL->Clone().Ptr();
	}
	else
	{
		newCell->cellURL = 0;
	}
	return newCell;
}

void Text::SpreadSheet::Worksheet::FreeDrawing(WorksheetDrawing *drawing)
{
	SDEL_CLASS(drawing->chart);
	MemFree(drawing);
}

Text::SpreadSheet::Worksheet::Worksheet(NotNullPtr<Text::String> name)
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
	NEW_CLASS(rows, Data::ArrayList<RowData*>());
	NEW_CLASS(this->colWidthsPt, Data::ArrayListDbl());
	NEW_CLASS(drawings, Data::ArrayList<WorksheetDrawing*>());
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
	NEW_CLASS(rows, Data::ArrayList<RowData*>());
	NEW_CLASS(this->colWidthsPt, Data::ArrayListDbl());
	NEW_CLASS(drawings, Data::ArrayList<WorksheetDrawing*>());
}

Text::SpreadSheet::Worksheet::~Worksheet()
{
	UOSInt i;
	RowData *row;
	this->name->Release();
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
	DEL_CLASS(this->colWidthsPt);

	LIST_FREE_FUNC(this->drawings, FreeDrawing);
	DEL_CLASS(this->drawings);	
}

NotNullPtr<Text::SpreadSheet::Worksheet> Text::SpreadSheet::Worksheet::Clone(const IStyleCtrl *srcCtrl, IStyleCtrl *newCtrl)
{
	UOSInt i;
	UOSInt j;
	NotNullPtr<Text::SpreadSheet::Worksheet> newWS;
	RowData *row;
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
	j = this->colWidthsPt->GetCount();
	while (i < j)
	{
		newWS->colWidthsPt->Add(this->colWidthsPt->GetItem(i));
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

NotNullPtr<Text::String> Text::SpreadSheet::Worksheet::GetName() const
{
	return this->name;
}

Bool Text::SpreadSheet::Worksheet::SetCellString(UOSInt row, UOSInt col, NotNullPtr<Text::String> val)
{
	return this->SetCellString(row, col, 0, val);
}

Bool Text::SpreadSheet::Worksheet::SetCellString(UOSInt row, UOSInt col, Text::CStringNN val)
{
	return this->SetCellString(row, col, 0, val);
}

Bool Text::SpreadSheet::Worksheet::SetCellTS(UOSInt row, UOSInt col, Data::Timestamp val)
{
	return this->SetCellTS(row, col, 0, val);
}

Bool Text::SpreadSheet::Worksheet::SetCellDateTime(UOSInt row, UOSInt col, NotNullPtr<Data::DateTime> val)
{
	return this->SetCellDateTime(row, col, 0, val);
}

Bool Text::SpreadSheet::Worksheet::SetCellDouble(UOSInt row, UOSInt col, Double val)
{
	return this->SetCellDouble(row, col, 0, val);
}

Bool Text::SpreadSheet::Worksheet::SetCellInt32(UOSInt row, UOSInt col, Int32 val)
{
	return this->SetCellInt32(row, col, 0, val);
}

Bool Text::SpreadSheet::Worksheet::SetCellInt64(UOSInt row, UOSInt col, Int64 val)
{
	return this->SetCellInt64(row, col, 0, val);
}

Bool Text::SpreadSheet::Worksheet::SetCellStyle(UOSInt row, UOSInt col, Text::SpreadSheet::CellStyle *style)
{
	CellData *cell;
	cell = GetCellData(row, col, false);
	if (cell == 0)
		return false;
	cell->style = style;
	return true;
}

Bool Text::SpreadSheet::Worksheet::SetCellStyleHAlign(UOSInt row, UOSInt col, IStyleCtrl *wb, HAlignment hAlign)
{
	CellData *cell;
	cell = GetCellData(row, col, false);
	if (cell == 0)
		return false;
	CellStyle *tmpStyle;
	if (cell->style == 0)
	{
		if (hAlign == Text::HAlignment::Unknown)
			return true;
		NEW_CLASS(tmpStyle, CellStyle(0));
		tmpStyle->SetHAlign(hAlign);
		cell->style = wb->FindOrCreateStyle(tmpStyle);
		DEL_CLASS(tmpStyle);
	}
	else
	{
		if (cell->style->GetHAlign() == hAlign)
			return true;
		tmpStyle = cell->style->Clone();
		tmpStyle->SetHAlign(hAlign);
		cell->style = wb->FindOrCreateStyle(tmpStyle);
		DEL_CLASS(tmpStyle);
	}
	return true;
}

Bool Text::SpreadSheet::Worksheet::SetCellStyleBorderBottom(UOSInt row, UOSInt col, IStyleCtrl *wb, UInt32 color, BorderType borderType)
{
	CellData *cell;
	cell = GetCellData(row, col, true);
	if (cell == 0)
		return false;
	CellStyle *tmpStyle;
	if (cell->style == 0)
	{
		if (borderType == Text::SpreadSheet::BorderType::None)
			return true;
		NEW_CLASS(tmpStyle, CellStyle(0));
		tmpStyle->SetBorderBottom(Text::SpreadSheet::CellStyle::BorderStyle(color, borderType));
		cell->style = wb->FindOrCreateStyle(tmpStyle);
		DEL_CLASS(tmpStyle);
	}
	else
	{
		
		if (cell->style->GetBorderBottom() == Text::SpreadSheet::CellStyle::BorderStyle(color, borderType))
			return true;
		tmpStyle = cell->style->Clone();
		tmpStyle->SetBorderBottom(Text::SpreadSheet::CellStyle::BorderStyle(color, borderType));
		cell->style = wb->FindOrCreateStyle(tmpStyle);
		DEL_CLASS(tmpStyle);
	}
	return true;
}

Bool Text::SpreadSheet::Worksheet::SetCellURL(UOSInt row, UOSInt col, Text::String *url)
{
	CellData *cell;
	cell = GetCellData(row, col, false);
	if (cell == 0)
		return false;
	SDEL_STRING(cell->cellURL);
	if (url)
	{
		cell->cellURL = url->Clone().Ptr();
	}
	return true;
}

Bool Text::SpreadSheet::Worksheet::SetCellURL(UOSInt row, UOSInt col, Text::CString url)
{
	CellData *cell;
	cell = GetCellData(row, col, false);
	if (cell == 0)
		return false;
	SDEL_STRING(cell->cellURL);
	if (url.leng > 0)
	{
		cell->cellURL = Text::String::New(url).Ptr();
	}
	return true;
}

Bool Text::SpreadSheet::Worksheet::SetCellString(UOSInt row, UOSInt col, CellStyle *style, NotNullPtr<Text::String> val)
{
	CellData *cell;
	cell = GetCellData(row, col, false);
	if (cell == 0)
		return false;
	cell->cdt = CellDataType::String;
	SDEL_STRING(cell->cellValue);
	cell->cellValue = val->Clone().Ptr();
	if (style) cell->style = style;
	return true;
}

Bool Text::SpreadSheet::Worksheet::SetCellString(UOSInt row, UOSInt col, CellStyle *style, Text::CStringNN val)
{
	CellData *cell;
	cell = GetCellData(row, col, false);
	if (cell == 0)
		return false;
	cell->cdt = CellDataType::String;
	SDEL_STRING(cell->cellValue);
	if (val.leng > 0)
	{
		cell->cellValue = Text::String::New(val).Ptr();
	}
	if (style) cell->style = style;
	return true;
}

Bool Text::SpreadSheet::Worksheet::SetCellTS(UOSInt row, UOSInt col, CellStyle *style, Data::Timestamp val)
{
	UTF8Char sbuff[32];
	UTF8Char *sptr;
	CellData *cell;
	cell = GetCellData(row, col, false);
	if (cell == 0)
		return false;
	cell->cdt = CellDataType::DateTime;
	SDEL_STRING(cell->cellValue);
	sptr = val.ToString(sbuff, "yyyy-MM-ddTHH:mm:ss.fff");
	cell->cellValue = Text::String::NewP(sbuff, sptr).Ptr();
	if (style) cell->style = style;
	return true;
}

Bool Text::SpreadSheet::Worksheet::SetCellDateTime(UOSInt row, UOSInt col, CellStyle *style, NotNullPtr<Data::DateTime> val)
{
	UTF8Char sbuff[32];
	UTF8Char *sptr;
	CellData *cell;
	cell = GetCellData(row, col, false);
	if (cell == 0)
		return false;
	cell->cdt = CellDataType::DateTime;
	SDEL_STRING(cell->cellValue);
	sptr = val->ToString(sbuff, "yyyy-MM-ddTHH:mm:ss.fff");
	cell->cellValue = Text::String::NewP(sbuff, sptr).Ptr();
	if (style) cell->style = style;
	return true;
}

Bool Text::SpreadSheet::Worksheet::SetCellDouble(UOSInt row, UOSInt col, CellStyle *style, Double val)
{
	UTF8Char sbuff[32];
	UTF8Char *sptr;
	CellData *cell;
	cell = GetCellData(row, col, false);
	if (cell == 0)
		return false;
	cell->cdt = CellDataType::Number;
	SDEL_STRING(cell->cellValue);
	sptr = Text::StrDouble(sbuff, val);
	cell->cellValue = Text::String::NewP(sbuff, sptr).Ptr();
	if (style) cell->style = style;
	return true;
}

Bool Text::SpreadSheet::Worksheet::SetCellInt32(UOSInt row, UOSInt col, CellStyle *style, Int32 val)
{
	UTF8Char sbuff[32];
	UTF8Char *sptr;
	CellData *cell;
	cell = GetCellData(row, col, false);
	if (cell == 0)
		return false;
	cell->cdt = CellDataType::Number;
	SDEL_STRING(cell->cellValue);
	sptr = Text::StrInt32(sbuff, val);
	cell->cellValue = Text::String::NewP(sbuff, sptr).Ptr();
	if (style) cell->style = style;
	return true;
}

Bool Text::SpreadSheet::Worksheet::SetCellInt64(UOSInt row, UOSInt col, CellStyle *style, Int64 val)
{
	UTF8Char sbuff[32];
	UTF8Char *sptr;
	CellData *cell;
	cell = GetCellData(row, col, false);
	if (cell == 0)
		return false;
	cell->cdt = CellDataType::Number;
	SDEL_STRING(cell->cellValue);
	sptr = Text::StrInt64(sbuff, val);
	cell->cellValue = Text::String::NewP(sbuff, sptr).Ptr();
	if (style) cell->style = style;
	return true;
}

Bool Text::SpreadSheet::Worksheet::MergeCells(UOSInt row, UOSInt col, UInt32 height, UInt32 width)
{
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
			if (cell->cdt == CellDataType::MergedLeft || cell->cdt == CellDataType::MergedUp)
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
					cell->cdt = CellDataType::MergedLeft;
				}
			}
			else
			{
				cell->cdt = CellDataType::MergedUp;
			}
			j++;
		}
		i++;
	}
	return true;
}

Bool Text::SpreadSheet::Worksheet::SetCellMergeLeft(UOSInt row, UOSInt col)
{
	if (col == 0)
		return false;

	CellData *cell;
	UInt32 width = 1;
	UInt32 height = 1;
	cell = GetCellData(row, col, true);
	cell->cdt = CellDataType::MergedLeft;
	col--;
	width++;
	while (true)
	{
		cell = GetCellData(row, col, true);
		if (cell->cdt == CellDataType::MergedUp)
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

Bool Text::SpreadSheet::Worksheet::SetCellMergeUp(UOSInt row, UOSInt col)
{
	if (row == 0)
		return false;

	CellData *cell;
	UInt32 width = 1;
	UInt32 height = 1;
	cell = GetCellData(row, col, true);
	cell->cdt = CellDataType::MergedUp;
	row--;
	height++;
	while (true)
	{
		cell = GetCellData(row, col, true);
		if (cell->cdt == CellDataType::MergedUp)
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

Bool Text::SpreadSheet::Worksheet::SetRowHidden(UOSInt row, Bool hidden)
{
	if (row >= 65536)
		return false;
	CellData *cell;
	cell = GetCellData(row, 0, true);
	cell->hidden = hidden;
	return true;
}

Bool Text::SpreadSheet::Worksheet::SetRowHeight(UOSInt row, Double height)
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

Text::SpreadSheet::Worksheet::RowData *Text::SpreadSheet::Worksheet::GetItem(UOSInt row)
{
	return this->rows->GetItem(row);
}

void Text::SpreadSheet::Worksheet::RemoveCol(UOSInt col)
{
	UOSInt i;
	RowData *row;
	CellData *cell;

	this->colWidthsPt->RemoveAt(col);
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

void Text::SpreadSheet::Worksheet::InsertCol(UOSInt col)
{
	UOSInt i;
	RowData *row;

	if (colWidthsPt->GetCount() > col)
	{
		this->colWidthsPt->Insert(col, 0);
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

UOSInt Text::SpreadSheet::Worksheet::GetMaxCol()
{
	return this->maxCol;
}

void Text::SpreadSheet::Worksheet::SetColWidth(UOSInt col, Double width, Math::Unit::Distance::DistanceUnit unit)
{
	while (col >= this->colWidthsPt->GetCount())
	{
		this->colWidthsPt->Add(-1);
	}
	if (unit == Math::Unit::Distance::DU_POINT)
	{
		this->colWidthsPt->SetItem(col, width);
	}
	else
	{
		this->colWidthsPt->SetItem(col, Math::Unit::Distance::Convert(unit, Math::Unit::Distance::DU_POINT, width));
	}
}

UOSInt Text::SpreadSheet::Worksheet::GetColWidthCount()
{
	return this->colWidthsPt->GetCount();
}

Double Text::SpreadSheet::Worksheet::GetColWidthPt(UOSInt col)
{
	if (col >= this->colWidthsPt->GetCount())
		return -1;
	return this->colWidthsPt->GetItem(col);
}

Double Text::SpreadSheet::Worksheet::GetColWidth(UOSInt col, Math::Unit::Distance::DistanceUnit unit)
{
	if (col >= this->colWidthsPt->GetCount())
		return -1;
	if (unit == Math::Unit::Distance::DU_POINT)
		return this->colWidthsPt->GetItem(col);
	return Math::Unit::Distance::Convert(Math::Unit::Distance::DU_POINT, unit, this->colWidthsPt->GetItem(col));
}

const Text::SpreadSheet::Worksheet::CellData *Text::SpreadSheet::Worksheet::GetCellDataRead(UOSInt row, UOSInt col) const
{
	RowData *rowData;
	CellData *cell;
	if (row >= this->rows->GetCount() + 65536)
		return 0;
	if (col >= 65536)
		return 0;
	if (col > this->maxCol)
	{
		return 0;
	}
	while (true)
	{
		rowData = this->rows->GetItem(row);
		if (rowData == 0)
			return 0;
		cell = rowData->cells->GetItem(col);
		if (cell == 0)
		{
			return 0;
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

Bool Text::SpreadSheet::Worksheet::GetCellString(const CellData *cell, NotNullPtr<Text::StringBuilderUTF8> sb)
{
	if (cell == 0 || cell->cellValue == 0)
	{
		return false;
	}
	if (cell->cdt == Text::SpreadSheet::CellDataType::Number)
	{
		Double v;
		Int32 iv;
		if (!cell->cellValue->ToDouble(v))
		{
			sb->Append(cell->cellValue);
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
		sb->Append(cell->cellValue);
	}
	return true;
}

UOSInt Text::SpreadSheet::Worksheet::GetDrawingCount()
{
	return this->drawings->GetCount();
}

Text::SpreadSheet::WorksheetDrawing *Text::SpreadSheet::Worksheet::GetDrawing(UOSInt index)
{
	return this->drawings->GetItem(index);
}

Text::SpreadSheet::WorksheetDrawing *Text::SpreadSheet::Worksheet::CreateDrawing(Math::Unit::Distance::DistanceUnit unit, Double x, Double y, Double w, Double h)
{
	Math::Unit::Distance::DistanceUnit inch = Math::Unit::Distance::DU_INCH;
	WorksheetDrawing *drawing = MemAlloc(WorksheetDrawing, 1);
	drawing->anchorType = AnchorType::Absolute;
	drawing->posXInch = Math::Unit::Distance::Convert(unit, inch, x);
	drawing->posYInch = Math::Unit::Distance::Convert(unit, inch, y);
	drawing->widthInch = Math::Unit::Distance::Convert(unit, inch, w);
	drawing->heightInch = Math::Unit::Distance::Convert(unit, inch, h);
	drawing->col1 = 0;
	drawing->row1 = 0;
	drawing->col2 = 0;
	drawing->row2 = 0;
	drawing->chart = 0;
	this->drawings->Add(drawing);
	return drawing;
}

Text::SpreadSheet::OfficeChart *Text::SpreadSheet::Worksheet::CreateChart(Math::Unit::Distance::DistanceUnit du, Double x, Double y, Double w, Double h, Text::CString title)
{
	WorksheetDrawing *drawing = this->CreateDrawing(du, x, y, w, h);
	drawing->chart = NEW_CLASS_D(OfficeChart(du, x, y, w, h));
	if (title.leng > 0)
	{
		drawing->chart->SetTitleText(title);
	}
	drawing->chart->SetShapeProp(NEW_CLASS_D(OfficeShapeProp(
		OfficeFill::NewSolidFill(OfficeColor::NewPreset(PresetColor::White)),
		NEW_CLASS_D(OfficeLineStyle(OfficeFill::NewSolidFill())))));
	return drawing->chart;
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
		return CSTR_NULL;
	}
}
