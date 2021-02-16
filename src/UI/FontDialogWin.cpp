#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"
#include "UI/FontDialog.h"
#include <windows.h>
#include <commdlg.h>

UI::FontDialog::FontDialog()
{
	this->fontName = 0;
	this->fontSizePt = 0;
	this->isBold = false;
	this->isItalic = false;
}

UI::FontDialog::FontDialog(const UTF8Char *fontName, Double fontSizePt, Bool isBold, Bool isItalic)
{
	this->fontName = Text::StrCopyNew(fontName);
	this->fontSizePt = fontSizePt;
	this->isBold = isBold;
	this->isItalic = isItalic;
}

UI::FontDialog::~FontDialog()
{
	if (this->fontName)
	{
		Text::StrDelNew(this->fontName);
	}
}

Bool UI::FontDialog::ShowDialog(void *ownerHandle)
{
	LOGFONTW lf;
	CHOOSEFONTW cfont;
	ZeroMemory(&cfont, sizeof(cfont));
	cfont.lStructSize = sizeof(CHOOSEFONT);
	cfont.hwndOwner = (HWND)ownerHandle;
	cfont.Flags = CF_NOSCRIPTSEL | CF_SCREENFONTS;
	cfont.lpLogFont = &lf;
	cfont.rgbColors = 0;

	ZeroMemory(&lf, sizeof(LOGFONT));
	if (this->fontName)
	{
		lf.lfHeight = -Math::Double2Int32(this->fontSizePt * 96.0 / 72.0);
		if (this->isBold)
		{
			lf.lfWeight = FW_BOLD;
		}
		else
		{
			lf.lfWeight = FW_NORMAL;
		}
		if (this->isItalic)
		{
			lf.lfItalic = TRUE;
		}
		Text::StrUTF8_WChar(lf.lfFaceName, this->fontName, -1, 0);
		cfont.Flags = cfont.Flags | CF_INITTOLOGFONTSTRUCT;
	}


	if (ChooseFontW(&cfont))
	{
		if (this->fontName)
		{
			Text::StrDelNew(this->fontName);
		}
		this->fontName = Text::StrToUTF8New(lf.lfFaceName);
		this->isBold = (lf.lfWeight == FW_BOLD);
		this->isItalic = lf.lfItalic != FALSE;
		if (lf.lfHeight < 0)
		{
			this->fontSizePt = -lf.lfHeight * 72.0 / 96.0;
		}
		else
		{
			this->fontSizePt = lf.lfHeight;
		}
		return true;
	}
	else
	{
		return false;
	}
}

const UTF8Char *UI::FontDialog::GetFontName()
{
	return this->fontName;
}

Double UI::FontDialog::GetFontSizePt()
{
	return this->fontSizePt;
}

Bool UI::FontDialog::IsBold()
{
	return this->isBold;
}

Bool UI::FontDialog::IsItalic()
{
	return this->isItalic;
}
