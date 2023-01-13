#include "Stdafx.h"
#include "Sync/MutexUsage.h"
#include "UI/DObj/LabelDObj.h"

UI::DObj::LabelDObj::LabelDObj(Media::DrawEngine *deng, Text::CString txt, Text::CString fontName, Double fontSize, Media::DrawEngine::DrawFontStyle fontStyle, UInt32 fontColor, OSInt left, OSInt top, UInt32 codePage) : DirectObject(left, top)
{
	this->deng = deng;
	this->txtChg = true;
	if (txt.leng > 0)
	{
		this->txt = Text::String::New(txt);
	}
	else
	{
		this->txt = 0;
	}
	if (fontName.leng > 0)
	{
		this->fontName = Text::String::New(fontName);
	}
	else
	{
		this->fontName = Text::String::New(UTF8STRC("Arial"));
	}
	this->fontSize = fontSize;
	this->fontStyle = fontStyle;
	this->fontColor = fontColor;
	this->codePage = codePage;
}

UI::DObj::LabelDObj::~LabelDObj()
{
	SDEL_STRING(this->txt);
	this->fontName->Release();
}

Bool UI::DObj::LabelDObj::IsChanged()
{
	return this->txtChg;
}

Bool UI::DObj::LabelDObj::DoEvents()
{
	return false;
}

void UI::DObj::LabelDObj::DrawObject(Media::DrawImage *dimg)
{
	this->txtChg = false;
	Sync::MutexUsage mutUsage(&this->txtMut);
	if (this->txt)
	{
		OSInt left;
		OSInt top;
		Media::DrawFont *f;
		Media::DrawBrush *b;
		f = dimg->NewFontPx(this->fontName->ToCString(), this->fontSize, (Media::DrawEngine::DrawFontStyle)(this->fontStyle | Media::DrawEngine::DFS_ANTIALIAS), this->codePage);
		b = dimg->NewBrushARGB(this->fontColor);
		this->GetCurrPos(&left, &top);
		dimg->DrawString(OSInt2Double(left), OSInt2Double(top), this->txt, f, b);
		dimg->DelFont(f);
		dimg->DelBrush(b);
	}
}

Bool UI::DObj::LabelDObj::IsObject(OSInt x, OSInt y)
{
	return false;
}

void UI::DObj::LabelDObj::OnMouseDown()
{
}

void UI::DObj::LabelDObj::OnMouseUp()
{
}

void UI::DObj::LabelDObj::OnMouseClick()
{
}

void UI::DObj::LabelDObj::SetFont(Text::CString fontName, Double fontSize)
{
	Sync::MutexUsage mutUsage(&this->txtMut);
	if (fontName.leng == 0)
	{
		fontName = CSTR("Arial");
	}
	if (!this->fontName->Equals(fontName.v, fontName.leng))
	{
		SDEL_STRING(this->fontName);
		this->fontName = Text::String::New(fontName);
		this->txtChg = true;
	}
	if (this->fontSize != fontSize)
	{
		this->fontSize = fontSize;
		this->txtChg = true;
	}
}

void UI::DObj::LabelDObj::SetText(Text::CString txt)
{
	Sync::MutexUsage mutUsage(&this->txtMut);
	if (this->txt == 0 && txt.leng == 0)
		return;
	if (txt.leng == 0)
	{
		this->txt->Release();
		this->txt = 0;
		this->txtChg = true;
	}
	else if (this->txt != 0 && this->txt->Equals(txt.v, txt.leng))
	{
		return;
	}
	else
	{
		SDEL_STRING(this->txt);
		this->txt = Text::String::New(txt);
		this->txtChg = true;
	}
}
