#ifndef _SM_UI_FONTDIALOG
#define _SM_UI_FONTDIALOG
#include "Handles.h"
#include "IO/Registry.h"
#include "Text/String.h"

namespace UI
{
	class FontDialog
	{
	private:
		Text::String *fontName;
		Double fontSizePt;
		Bool isBold;
		Bool isItalic;

	public:
		FontDialog();
		FontDialog(const UTF8Char *fontName, UOSInt nameLen, Double fontSizePt, Bool isBold, Bool isItalic);
		FontDialog(Text::String *fontName, Double fontSizePt, Bool isBold, Bool isItalic);
		~FontDialog();

		Bool ShowDialog(ControlHandle *ownerHandle);

		Text::String *GetFontName();
		Double GetFontSizePt();
		Bool IsBold();
		Bool IsItalic();
	};
}
#endif
