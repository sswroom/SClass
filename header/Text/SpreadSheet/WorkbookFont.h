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
			Optional<Text::String> name;
			Double size; //point
			Bool bold;
			Bool italic;
			Bool underline;
			UInt32 color;
			FontFamily family;
		public:
			WorkbookFont();
			~WorkbookFont();

			NN<WorkbookFont> SetName(Text::CString name);
			NN<WorkbookFont> SetSize(Double size);
			NN<WorkbookFont> SetBold(Bool bold);
			NN<WorkbookFont> SetItalic(Bool italic);
			NN<WorkbookFont> SetUnderline(Bool underline);
			NN<WorkbookFont> SetColor(UInt32 color);
			NN<WorkbookFont> SetFamily(FontFamily family);

			Optional<Text::String> GetName() const;
			Double GetSize() const;
			Bool IsBold() const;
			Bool IsItalic() const;
			Bool IsUnderline() const;
			UInt32 GetColor() const;
			FontFamily GetFamily() const;

			NN<WorkbookFont> Clone() const;
			Bool Equals(NN<WorkbookFont> font) const;
		};
	}
}
#endif
