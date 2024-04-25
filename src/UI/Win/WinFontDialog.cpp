#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"
#include "UI/Win/WinFontDialog.h"
#include <windows.h>
#include <commdlg.h>

UI::Win::WinFontDialog::WinFontDialog(Optional<Text::String> fontName, Double fontSizePt, Bool isBold, Bool isItalic) : GUIFontDialog(fontName, fontSizePt, isBold, isItalic)
{
}

UI::Win::WinFontDialog::WinFontDialog(Text::CString fontName, Double fontSizePt, Bool isBold, Bool isItalic) : GUIFontDialog(fontName, fontSizePt, isBold, isItalic)
{
}


UI::Win::WinFontDialog::~WinFontDialog()
{
}

Bool UI::Win::WinFontDialog::ShowDialog(ControlHandle *ownerHandle)
{
	LOGFONTW lf;
	CHOOSEFONTW cfont;
	NN<Text::String> s;
	ZeroMemory(&cfont, sizeof(cfont));
	cfont.lStructSize = sizeof(CHOOSEFONT);
	cfont.hwndOwner = (HWND)ownerHandle;
	cfont.Flags = CF_NOSCRIPTSEL | CF_SCREENFONTS;
	cfont.lpLogFont = &lf;
	cfont.rgbColors = 0;

	ZeroMemory(&lf, sizeof(LOGFONT));
	if (this->fontName.SetTo(s))
	{
		lf.lfHeight = -Double2Int32(this->fontSizePt * 96.0 / 72.0);
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
		Text::StrUTF8_WChar(lf.lfFaceName, s->v, 0);
		cfont.Flags = cfont.Flags | CF_INITTOLOGFONTSTRUCT;
	}


	if (ChooseFontW(&cfont))
	{
		OPTSTR_DEL(this->fontName);;
		this->fontName = Text::String::NewNotNull(lf.lfFaceName);
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
