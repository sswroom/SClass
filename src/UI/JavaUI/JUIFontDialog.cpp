#include "Stdafx.h"
#include "MyMemory.h"
#include "UI/JavaUI/JUIFontDialog.h"

UI::JavaUI::JUIFontDialog::JUIFontDialog(Optional<Text::String> fontName, Double fontSizePt, Bool isBold, Bool isItalic) : GUIFontDialog(fontName, fontSizePt, isBold, isItalic)
{
}

UI::JavaUI::JUIFontDialog::JUIFontDialog(Text::CString fontName, Double fontSizePt, Bool isBold, Bool isItalic) : GUIFontDialog(fontName, fontSizePt, isBold, isItalic)
{
}


UI::JavaUI::JUIFontDialog::~JUIFontDialog()
{
}

Bool UI::JavaUI::JUIFontDialog::ShowDialog(Optional<ControlHandle> ownerHandle)
{
	return false;
}
