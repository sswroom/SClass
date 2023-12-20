#ifndef _SM_UI_GUIFONTDIALOG
#define _SM_UI_GUIFONTDIALOG
#include "Handles.h"
#include "Text/String.h"

namespace UI
{
	class GUIFontDialog
	{
	protected:
		Optional<Text::String> fontName;
		Double fontSizePt;
		Bool isBold;
		Bool isItalic;

	public:
		GUIFontDialog(Optional<Text::String> fontName, Double fontSizePt, Bool isBold, Bool isItalic);
		GUIFontDialog(Text::CString fontName, Double fontSizePt, Bool isBold, Bool isItalic);
		virtual ~GUIFontDialog();

		virtual Bool ShowDialog(ControlHandle *ownerHandle) = 0;

		Optional<Text::String> GetFontName() const;
		Double GetFontSizePt() const;
		Bool IsBold() const;
		Bool IsItalic() const;
	};
}
#endif
