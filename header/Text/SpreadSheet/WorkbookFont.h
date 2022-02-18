#ifndef _SM_TEXT_SPREADSHEET_WORKBOOKFONT
#define _SM_TEXT_SPREADSHEET_WORKBOOKFONT
#include "Text/String.h"

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
			Text::String *name;
			Double size; //point
			Bool bold;
			Bool italic;
			Bool underline;
			UInt32 color;
			FontFamily family;
		public:
			WorkbookFont();
			~WorkbookFont();

			WorkbookFont *SetName(Text::CString name);
			WorkbookFont *SetSize(Double size);
			WorkbookFont *SetBold(Bool bold);
			WorkbookFont *SetItalic(Bool italic);
			WorkbookFont *SetUnderline(Bool underline);
			WorkbookFont *SetColor(UInt32 color);
			WorkbookFont *SetFamily(FontFamily family);

			Text::String *GetName();
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
