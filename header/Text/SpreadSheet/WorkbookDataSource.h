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
			NN<Worksheet> sheet;
			UOSInt firstRow;
			UOSInt lastRow;
			UOSInt firstCol;
			UOSInt lastCol;

		public:
			WorkbookDataSource(NN<Worksheet> sheet, UOSInt firstRow, UOSInt lastRow, UOSInt firstCol, UOSInt lastCol);
			~WorkbookDataSource();

			UTF8Char *ToCodeRange(UTF8Char *sbuff) const;
			NN<Worksheet> GetSheet() const;
			UOSInt GetFirstRow() const;
			UOSInt GetLastRow() const;
			UOSInt GetFirstCol() const;
			UOSInt GetLastCol() const;
		};
	}
}
#endif
