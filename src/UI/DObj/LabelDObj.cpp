#include "Stdafx.h"
#include "Sync/MutexUsage.h"
#include "UI/DObj/LabelDObj.h"

UI::DObj::LabelDObj::LabelDObj(NN<Media::DrawEngine> deng, Text::CString txt, Text::CString fontName, Double fontSizePx, Media::DrawEngine::DrawFontStyle fontStyle, UInt32 fontColor, Math::Coord2D<OSInt> tl, UInt32 codePage) : DirectObject(tl)
{
	this->deng = deng;
	this->txtChg = true;
	if (txt.leng > 0)
	{
		this->txt = Text::String::New(txt).Ptr();
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
	this->fontSizePx = fontSizePx;
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

void UI::DObj::LabelDObj::DrawObject(NN<Media::DrawImage> dimg)
{
	this->txtChg = false;
	Sync::MutexUsage mutUsage(this->txtMut);
	NN<Text::String> s;
	if (s.Set(this->txt))
	{
		NN<Media::DrawFont> f;
		NN<Media::DrawBrush> b;
		f = dimg->NewFontPx(this->fontName->ToCString(), this->fontSizePx, (Media::DrawEngine::DrawFontStyle)(this->fontStyle | Media::DrawEngine::DFS_ANTIALIAS), this->codePage);
		b = dimg->NewBrushARGB(this->fontColor);
		Math::Coord2DDbl tl = this->GetCurrPos().ToDouble();
		dimg->DrawString(tl, s, f, b);
		dimg->DelFont(f);
		dimg->DelBrush(b);
	}
}

Bool UI::DObj::LabelDObj::IsObject(Math::Coord2D<OSInt> scnPos)
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

void UI::DObj::LabelDObj::SetFont(Text::CString fontName, Double fontSizePx)
{
	Sync::MutexUsage mutUsage(this->txtMut);
	Text::CStringNN nnfontName;
	if (!fontName.SetTo(nnfontName) || nnfontName.leng == 0)
	{
		nnfontName = CSTR("Arial");
	}
	if (!this->fontName->Equals(nnfontName.v, nnfontName.leng))
	{
		this->fontName->Release();
		this->fontName = Text::String::New(nnfontName);
		this->txtChg = true;
	}
	if (this->fontSizePx != fontSizePx)
	{
		this->fontSizePx = fontSizePx;
		this->txtChg = true;
	}
}

void UI::DObj::LabelDObj::SetText(Text::CString txt)
{
	Sync::MutexUsage mutUsage(this->txtMut);
	Text::CStringNN nntxt;
	if (this->txt == 0 && txt.leng == 0)
		return;
	if (!txt.SetTo(nntxt) || nntxt.leng == 0)
	{
		this->txt->Release();
		this->txt = 0;
		this->txtChg = true;
	}
	else if (this->txt != 0 && this->txt->Equals(nntxt.v, nntxt.leng))
	{
		return;
	}
	else
	{
		SDEL_STRING(this->txt);
		this->txt = Text::String::New(nntxt).Ptr();
		this->txtChg = true;
	}
}
