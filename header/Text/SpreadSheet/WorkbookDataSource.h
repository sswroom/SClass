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
			UIntOS firstRow;
			UIntOS lastRow;
			UIntOS firstCol;
			UIntOS lastCol;

		public:
			WorkbookDataSource(NN<Worksheet> sheet, UIntOS firstRow, UIntOS lastRow, UIntOS firstCol, UIntOS lastCol);
			~WorkbookDataSource();

			UnsafeArray<UTF8Char> ToCodeRange(UnsafeArray<UTF8Char> sbuff) const;
			NN<Worksheet> GetSheet() const;
			UIntOS GetFirstRow() const;
			UIntOS GetLastRow() const;
			UIntOS GetFirstCol() const;
			UIntOS GetLastCol() const;
		};
	}
}
#endif
