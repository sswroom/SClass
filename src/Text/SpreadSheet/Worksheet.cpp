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
	if (row >= 65536)
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
		else if (cell->cdt == CellDataType::MergedTop)
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

Text::SpreadSheet::Worksheet::RowData *Text::SpreadSheet::Worksheet::CloneRow(RowData *row, IStyleCtrl *srcCtrl, IStyleCtrl *newCtrl)
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

Text::SpreadSheet::Worksheet::CellData *Text::SpreadSheet::Worksheet::CloneCell(CellData *cell, IStyleCtrl *srcCtrl, IStyleCtrl *newCtrl)
{
	CellData *newCell;
	newCell = MemAlloc(CellData, 1);
	newCell->cdt = cell->cdt;
	if (cell->cellValue)
	{
		newCell->cellValue = cell->cellValue->Clone();
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
		newCell->cellURL = cell->cellURL->Clone();
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
	DEL_CLASS(this->colWidthsPt);

	LIST_FREE_FUNC(this->drawings, FreeDrawing);
	DEL_CLASS(this->drawings);	
}

Text::SpreadSheet::Worksheet *Text::SpreadSheet::Worksheet::Clone(IStyleCtrl *srcCtrl, IStyleCtrl *newCtrl)
{
	UOSInt i;
	UOSInt j;
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

const UTF8Char *Text::SpreadSheet::Worksheet::GetName()
{
	return this->name;
}

Bool Text::SpreadSheet::Worksheet::SetCellString(UOSInt row, UOSInt col, Text::String *val)
{
	return this->SetCellString(row, col, 0, val);
}

Bool Text::SpreadSheet::Worksheet::SetCellString(UOSInt row, UOSInt col, const UTF8Char *val)
{
	return this->SetCellString(row, col, 0, val);
}

Bool Text::SpreadSheet::Worksheet::SetCellDate(UOSInt row, UOSInt col, Data::DateTime *val)
{
	return this->SetCellDate(row, col, 0, val);
}

Bool Text::SpreadSheet::Worksheet::SetCellDouble(UOSInt row, UOSInt col, Double val)
{
	return this->SetCellDouble(row, col, 0, val);
}

Bool Text::SpreadSheet::Worksheet::SetCellInt32(UOSInt row, UOSInt col, Int32 val)
{
	return this->SetCellInt32(row, col, 0, val);
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

Bool Text::SpreadSheet::Worksheet::SetCellURL(UOSInt row, UOSInt col, Text::String *url)
{
	CellData *cell;
	cell = GetCellData(row, col, false);
	if (cell == 0)
		return false;
	SDEL_STRING(cell->cellURL);
	if (url)
	{
		cell->cellURL = url->Clone();
	}
	return true;
}

Bool Text::SpreadSheet::Worksheet::SetCellURL(UOSInt row, UOSInt col, const UTF8Char *url)
{
	CellData *cell;
	cell = GetCellData(row, col, false);
	if (cell == 0)
		return false;
	SDEL_STRING(cell->cellURL);
	if (url)
	{
		cell->cellURL = Text::String::New(url);
	}
	return true;
}

Bool Text::SpreadSheet::Worksheet::SetCellString(UOSInt row, UOSInt col, CellStyle *style, Text::String *val)
{
	CellData *cell;
	cell = GetCellData(row, col, false);
	if (cell == 0)
		return false;
	cell->cdt = CellDataType::String;
	SDEL_STRING(cell->cellValue);
	if (val)
	{
		cell->cellValue = val->Clone();
	}
	if (style) cell->style = style;
	return true;
}

Bool Text::SpreadSheet::Worksheet::SetCellString(UOSInt row, UOSInt col, CellStyle *style, const UTF8Char *val)
{
	CellData *cell;
	cell = GetCellData(row, col, false);
	if (cell == 0)
		return false;
	cell->cdt = CellDataType::String;
	SDEL_STRING(cell->cellValue);
	if (val)
	{
		cell->cellValue = Text::String::New(val);
	}
	if (style) cell->style = style;
	return true;
}

Bool Text::SpreadSheet::Worksheet::SetCellDate(UOSInt row, UOSInt col, CellStyle *style, Data::DateTime *val)
{
	UTF8Char sbuff[32];
	CellData *cell;
	cell = GetCellData(row, col, false);
	if (cell == 0)
		return false;
	cell->cdt = CellDataType::DateTime;
	SDEL_STRING(cell->cellValue);
	val->ToString(sbuff, "yyyy-MM-ddTHH:mm:ss.fff");
	cell->cellValue = Text::String::New(sbuff);
	if (style) cell->style = style;
	return true;
}

Bool Text::SpreadSheet::Worksheet::SetCellDouble(UOSInt row, UOSInt col, CellStyle *style, Double val)
{
	UTF8Char sbuff[32];
	CellData *cell;
	cell = GetCellData(row, col, false);
	if (cell == 0)
		return false;
	cell->cdt = CellDataType::Number;
	SDEL_STRING(cell->cellValue);
	Text::StrDouble(sbuff, val);
	cell->cellValue = Text::String::New(sbuff);
	if (style) cell->style = style;
	return true;
}

Bool Text::SpreadSheet::Worksheet::SetCellInt32(UOSInt row, UOSInt col, CellStyle *style, Int32 val)
{
	UTF8Char sbuff[32];
	CellData *cell;
	cell = GetCellData(row, col, false);
	if (cell == 0)
		return false;
	cell->cdt = CellDataType::Number;
	SDEL_STRING(cell->cellValue);
	Text::StrInt32(sbuff, val);
	cell->cellValue = Text::String::New(sbuff);
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
			if (cell->cdt == CellDataType::MergedLeft || cell->cdt == CellDataType::MergedTop)
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
					cell->cdt = CellDataType::MergedTop;
				}
			}
			else
			{
				cell->cdt = CellDataType::MergedLeft;
			}
			j++;
		}
		i++;
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

Text::SpreadSheet::OfficeChart *Text::SpreadSheet::Worksheet::CreateChart(Math::Unit::Distance::DistanceUnit du, Double x, Double y, Double w, Double h, const UTF8Char *title)
{
	WorksheetDrawing *drawing = this->CreateDrawing(du, x, y, w, h);
	drawing->chart = NEW_CLASS_D(OfficeChart(du, x, y, w, h));
	if (title)
	{
		drawing->chart->SetTitleText(title);
	}
	drawing->chart->SetShapeProp(NEW_CLASS_D(OfficeShapeProp(
		OfficeFill::NewSolidFill(OfficeColor::NewPreset(PresetColor::White)),
		NEW_CLASS_D(OfficeLineStyle(OfficeFill::NewSolidFill())))));
	return drawing->chart;
}
