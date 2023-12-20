#ifndef _SM_UI_GTK_GTKFONTDIALOG
#define _SM_UI_GTK_GTKFONTDIALOG
#include "UI/GUIFontDialog.h"

namespace UI
{
	namespace GTK
	{
		class GTKFontDialog : public UI::GUIFontDialog
		{
		public:
			GTKFontDialog(Optional<Text::String> fontName, Double fontSizePt, Bool isBold, Bool isItalic);
			GTKFontDialog(Text::CString fontName, Double fontSizePt, Bool isBold, Bool isItalic);
			virtual ~GTKFontDialog();

			virtual Bool ShowDialog(ControlHandle *ownerHandle);
		};
	}
}
#endif
