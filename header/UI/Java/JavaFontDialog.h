#ifndef _SM_UI_JAVA_JAVAFONTDIALOG
#define _SM_UI_JAVA_JAVAFONTDIALOG
#include "UI/GUIFontDialog.h"

namespace UI
{
	namespace Java
	{
		class JavaFontDialog : public UI::GUIFontDialog
		{
		public:
			JavaFontDialog(Optional<Text::String> fontName, Double fontSizePt, Bool isBold, Bool isItalic);
			JavaFontDialog(Text::CString fontName, Double fontSizePt, Bool isBold, Bool isItalic);
			virtual ~JavaFontDialog();

			virtual Bool ShowDialog(Optional<ControlHandle> ownerHandle);
		};
	}
}
#endif
