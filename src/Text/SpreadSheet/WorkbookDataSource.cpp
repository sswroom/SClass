#include "Stdafx.h"
#include "Text/SpreadSheet/WorkbookDataSource.h"

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
