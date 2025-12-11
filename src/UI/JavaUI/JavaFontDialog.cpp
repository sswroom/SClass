#include "Stdafx.h"
#include "MyMemory.h"
#include "UI/JavaUI/JavaFontDialog.h"

UI::JavaUI::JavaFontDialog::JavaFontDialog(Optional<Text::String> fontName, Double fontSizePt, Bool isBold, Bool isItalic) : GUIFontDialog(fontName, fontSizePt, isBold, isItalic)
{
}

UI::JavaUI::JavaFontDialog::JavaFontDialog(Text::CString fontName, Double fontSizePt, Bool isBold, Bool isItalic) : GUIFontDialog(fontName, fontSizePt, isBold, isItalic)
{
}


UI::JavaUI::JavaFontDialog::~JavaFontDialog()
{
}

Bool UI::JavaUI::JavaFontDialog::ShowDialog(Optional<ControlHandle> ownerHandle)
{
	return false;
}
