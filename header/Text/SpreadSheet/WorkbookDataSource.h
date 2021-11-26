#ifndef _SM_TEXT_SPREADSHEET_WORKBOOKDATASOURCE
#define _SM_TEXT_SPREADSHEET_WORKBOOKDATASOURCE

namespace Text
{
	namespace SpreadSheet
	{
		class Worksheet;
		class WorkbookDataSource
		{
		private:
			Worksheet *sheet;
			UOSInt firstRow;
			UOSInt lastRow;
			UOSInt firstCol;
			UOSInt lastCol;

		public:
			WorkbookDataSource(Worksheet *sheet, UOSInt firstRow, UOSInt lastRow, UOSInt firstCol, UOSInt lastCol);
			~WorkbookDataSource();
		};
	}
}
#endif
