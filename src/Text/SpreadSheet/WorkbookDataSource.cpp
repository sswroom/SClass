#include "Stdafx.h"
#include "Text/MyString.h"
#include "Text/SpreadSheet/Workbook.h"
#include "Text/SpreadSheet/WorkbookDataSource.h"
#include "Text/SpreadSheet/Worksheet.h"


Text::SpreadSheet::WorkbookDataSource::WorkbookDataSource(NotNullPtr<Worksheet> sheet, UOSInt firstRow, UOSInt lastRow, UOSInt firstCol, UOSInt lastCol)
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

UTF8Char *Text::SpreadSheet::WorkbookDataSource::ToCodeRange(UTF8Char *sbuff) const
{
	sbuff = this->sheet->GetName()->ConcatTo(sbuff);
	*sbuff++ = '!';
	*sbuff++ = '$';
	sbuff = Workbook::ColCode(sbuff, this->firstCol);
	*sbuff++ = '$';
	sbuff = Text::StrUOSInt(sbuff, this->firstRow + 1);
	*sbuff++ = ':';
	*sbuff++ = '$';
	sbuff = Workbook::ColCode(sbuff, this->lastCol);
	*sbuff++ = '$';
	sbuff = Text::StrUOSInt(sbuff, this->lastRow + 1);
	return sbuff;
}

NotNullPtr<Text::SpreadSheet::Worksheet> Text::SpreadSheet::WorkbookDataSource::GetSheet() const
{
	return this->sheet;
}

UOSInt Text::SpreadSheet::WorkbookDataSource::GetFirstRow() const
{
	return this->firstRow;
}

UOSInt Text::SpreadSheet::WorkbookDataSource::GetLastRow() const
{
	return this->lastRow;
}

UOSInt Text::SpreadSheet::WorkbookDataSource::GetFirstCol() const
{
	return this->firstCol;
}

UOSInt Text::SpreadSheet::WorkbookDataSource::GetLastCol() const
{
	return this->lastCol;
}
