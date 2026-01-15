#include "Stdafx.h"
#include "Text/MyString.h"
#include "Text/SpreadSheet/Workbook.h"
#include "Text/SpreadSheet/WorkbookDataSource.h"
#include "Text/SpreadSheet/Worksheet.h"


Text::SpreadSheet::WorkbookDataSource::WorkbookDataSource(NN<Worksheet> sheet, UIntOS firstRow, UIntOS lastRow, UIntOS firstCol, UIntOS lastCol)
{
	this->sheet = sheet;
	this->firstRow = firstRow;
	this->lastRow = lastRow;
	this->firstCol = firstCol;
	this->lastCol = lastCol;
}

Text::SpreadSheet::WorkbookDataSource::~WorkbookDataSource()
{

}

UnsafeArray<UTF8Char> Text::SpreadSheet::WorkbookDataSource::ToCodeRange(UnsafeArray<UTF8Char> sbuff) const
{
	sbuff = this->sheet->GetName()->ConcatTo(sbuff);
	*sbuff++ = '!';
	*sbuff++ = '$';
	sbuff = Workbook::ColCode(sbuff, this->firstCol);
	*sbuff++ = '$';
	sbuff = Text::StrUIntOS(sbuff, this->firstRow + 1);
	*sbuff++ = ':';
	*sbuff++ = '$';
	sbuff = Workbook::ColCode(sbuff, this->lastCol);
	*sbuff++ = '$';
	sbuff = Text::StrUIntOS(sbuff, this->lastRow + 1);
	return sbuff;
}

NN<Text::SpreadSheet::Worksheet> Text::SpreadSheet::WorkbookDataSource::GetSheet() const
{
	return this->sheet;
}

UIntOS Text::SpreadSheet::WorkbookDataSource::GetFirstRow() const
{
	return this->firstRow;
}

UIntOS Text::SpreadSheet::WorkbookDataSource::GetLastRow() const
{
	return this->lastRow;
}

UIntOS Text::SpreadSheet::WorkbookDataSource::GetFirstCol() const
{
	return this->firstCol;
}

UIntOS Text::SpreadSheet::WorkbookDataSource::GetLastCol() const
{
	return this->lastCol;
}
