#include "Stdafx.h"
#include "MyMemory.h"
#include "UI/Java/JavaFontDialog.h"

UI::Java::JavaFontDialog::JavaFontDialog(Optional<Text::String> fontName, Double fontSizePt, Bool isBold, Bool isItalic) : GUIFontDialog(fontName, fontSizePt, isBold, isItalic)
{
}

UI::Java::JavaFontDialog::JavaFontDialog(Text::CString fontName, Double fontSizePt, Bool isBold, Bool isItalic) : GUIFontDialog(fontName, fontSizePt, isBold, isItalic)
{
}


UI::Java::JavaFontDialog::~JavaFontDialog()
{
}

Bool UI::Java::JavaFontDialog::ShowDialog(ControlHandle *ownerHandle)
{
	return false;
}
