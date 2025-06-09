#ifndef _SM_UI_WIN_WINFONTDIALOG
#define _SM_UI_WIN_WINFONTDIALOG
#include "UI/GUIFontDialog.h"

namespace UI
{
	namespace Win
	{
		class WinFontDialog : public UI::GUIFontDialog
		{
		public:
			WinFontDialog(Optional<Text::String> fontName, Double fontSizePt, Bool isBold, Bool isItalic);
			WinFontDialog(Text::CString fontName, Double fontSizePt, Bool isBold, Bool isItalic);
			virtual ~WinFontDialog();

			virtual Bool ShowDialog(Optional<ControlHandle> ownerHandle);
		};
	}
}
#endif
