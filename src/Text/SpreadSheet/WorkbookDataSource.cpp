#include "Stdafx.h"
#include "Text/MyString.h"
#include "Text/SpreadSheet/Workbook.h"
#include "Text/SpreadSheet/WorkbookDataSource.h"
#include "Text/SpreadSheet/Worksheet.h"


Text::SpreadSheet::WorkbookDataSource::WorkbookDataSource(Worksheet *sheet, UOSInt firstRow, UOSInt lastRow, UOSInt firstCol, UOSInt lastCol)
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

UTF8Char *Text::SpreadSheet::WorkbookDataSource::ToCodeRange(UTF8Char *sbuff)
{
	sbuff = Text::StrConcat(sbuff, this->sheet->GetName());
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