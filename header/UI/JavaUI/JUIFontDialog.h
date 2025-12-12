#ifndef _SM_UI_JAVAUI_JUIFONTDIALOG
#define _SM_UI_JAVAUI_JUIFONTDIALOG
#include "UI/GUIFontDialog.h"

namespace UI
{
	namespace JavaUI
	{
		class JUIFontDialog : public UI::GUIFontDialog
		{
		public:
			JUIFontDialog(Optional<Text::String> fontName, Double fontSizePt, Bool isBold, Bool isItalic);
			JUIFontDialog(Text::CString fontName, Double fontSizePt, Bool isBold, Bool isItalic);
			virtual ~JUIFontDialog();

			virtual Bool ShowDialog(Optional<ControlHandle> ownerHandle);
		};
	}
}
#endif
