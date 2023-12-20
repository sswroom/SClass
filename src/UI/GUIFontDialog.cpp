#include "Stdafx.h"
#include "UI/GUIFontDialog.h"

UI::GUIFontDialog::GUIFontDialog(Optional<Text::String> fontName, Double fontSizePt, Bool isBold, Bool isItalic)
{
	this->fontName = Text::String::CopyOrNull(fontName);
	this->fontSizePt = fontSizePt;
	this->isBold = isBold;
	this->isItalic = isItalic;
}

UI::GUIFontDialog::GUIFontDialog(Text::CString fontName, Double fontSizePt, Bool isBold, Bool isItalic)
{
	this->fontName = Text::String::NewOrNull(fontName);
	this->fontSizePt = fontSizePt;
	this->isBold = isBold;
	this->isItalic = isItalic;
}

UI::GUIFontDialog::~GUIFontDialog()
{
	OPTSTR_DEL(this->fontName);
}

Optional<Text::String> UI::GUIFontDialog::GetFontName() const
{
	return this->fontName;
}

Double UI::GUIFontDialog::GetFontSizePt() const
{
	return this->fontSizePt;
}

Bool UI::GUIFontDialog::IsBold() const
{
	return this->isBold;
}

Bool UI::GUIFontDialog::IsItalic() const
{
	return this->isItalic;
}
