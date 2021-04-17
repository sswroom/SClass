#ifndef _SM_TEXT_SPREADSHEET_WORKBOOKFONT
#define _SM_TEXT_SPREADSHEET_WORKBOOKFONT
namespace Text
{
	namespace SpreadSheet
	{
		class WorkbookFont
		{
		private:
			const UTF8Char *name;
			Double size; //point
			Bool bold;
			Bool italic;
			Bool underline;
			UInt32 color;
		public:
			WorkbookFont();
			~WorkbookFont();

			WorkbookFont *SetName(const UTF8Char *name);
			WorkbookFont *SetSize(Double size);
			WorkbookFont *SetBold(Bool bold);
			WorkbookFont *SetItalic(Bool italic);
			WorkbookFont *SetUnderline(Bool underline);
			WorkbookFont *SetColor(UInt32 color);

			const UTF8Char *GetName();
			Double GetSize();
			Bool IsBold();
			Bool IsItalic();
			Bool IsUnderline();
			Int32 GetColor();

			WorkbookFont *Clone();
			Bool Equals(WorkbookFont *font);
		};
	}
}
#endif
