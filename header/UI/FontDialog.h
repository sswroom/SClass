#ifndef _SM_UI_FONTDIALOG
#define _SM_UI_FONTDIALOG
#include "IO/Registry.h"

namespace UI
{
	class FontDialog
	{
	private:
		const UTF8Char *fontName;
		Double fontSize;
		Bool isBold;
		Bool isItalic;

	public:
		FontDialog();
		FontDialog(const UTF8Char *fontName, Double fontSize, Bool isBold, Bool isItalic);
		~FontDialog();

		Bool ShowDialog(void *ownerHandle);

		const UTF8Char *GetFontName();
		Double GetFontSize();
		Bool IsBold();
		Bool IsItalic();
	};
}
#endif
