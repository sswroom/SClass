#ifndef _SM_TEXT_SPREADSHEET_WORKBOOKFONT
#define _SM_TEXT_SPREADSHEET_WORKBOOKFONT
namespace Text
{
	namespace SpreadSheet
	{
		enum class FontFamily
		{
			NA,
			Roman,
			Swiss,
			Modern,
			Script,
			Decorative
		};
		class WorkbookFont
		{
		private:
			const UTF8Char *name;
			Double size; //point
			Bool bold;
			Bool italic;
			Bool underline;
			UInt32 color;
			FontFamily family;
		public:
			WorkbookFont();
			~WorkbookFont();

			WorkbookFont *SetName(const UTF8Char *name);
			WorkbookFont *SetSize(Double size);
			WorkbookFont *SetBold(Bool bold);
			WorkbookFont *SetItalic(Bool italic);
			WorkbookFont *SetUnderline(Bool underline);
			WorkbookFont *SetColor(UInt32 color);
			WorkbookFont *SetFamily(FontFamily family);

			const UTF8Char *GetName();
			Double GetSize();
			Bool IsBold();
			Bool IsItalic();
			Bool IsUnderline();
			UInt32 GetColor();
			FontFamily GetFamily();

			WorkbookFont *Clone();
			Bool Equals(WorkbookFont *font);
		};
	}
}
#endif
