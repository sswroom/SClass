#ifndef _SM_UI_FONTDIALOG
#define _SM_UI_FONTDIALOG
#include "Handles.h"
#include "IO/Registry.h"

namespace UI
{
	class FontDialog
	{
	private:
		const UTF8Char *fontName;
		Double fontSizePt;
		Bool isBold;
		Bool isItalic;

	public:
		FontDialog();
		FontDialog(const UTF8Char *fontName, Double fontSizePt, Bool isBold, Bool isItalic);
		~FontDialog();

		Bool ShowDialog(ControlHandle *ownerHandle);

		const UTF8Char *GetFontName();
		Double GetFontSizePt();
		Bool IsBold();
		Bool IsItalic();
	};
}
#endif
