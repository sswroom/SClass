#include "Stdafx.h"
#include "Sync/MutexUsage.h"
#include "UI/DObj/LabelDObj.h"

UI::DObj::LabelDObj::LabelDObj(NN<Media::DrawEngine> deng, Text::CString txt, Text::CString fontName, Double fontSizePx, Media::DrawEngine::DrawFontStyle fontStyle, UInt32 fontColor, Math::Coord2D<IntOS> tl, UInt32 codePage) : DirectObject(tl)
{
	this->deng = deng;
	this->txtChg = true;
	this->txt = Text::String::NewOrNull(txt);
	Text::CStringNN nnfontName;
	if (fontName.SetTo(nnfontName) && nnfontName.leng > 0)
	{
		this->fontName = Text::String::New(nnfontName);
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
	OPTSTR_DEL(this->txt);
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
	if (this->txt.SetTo(s))
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

Bool UI::DObj::LabelDObj::IsObject(Math::Coord2D<IntOS> scnPos)
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
	NN<Text::String> nnthisTxt;
	if (!this->txt.SetTo(nnthisTxt) && txt.leng == 0)
		return;
	if (this->txt.SetTo(nnthisTxt) && (!txt.SetTo(nntxt) || nntxt.leng == 0))
	{
		nnthisTxt->Release();
		this->txt = nullptr;
		this->txtChg = true;
	}
	else if (this->txt.SetTo(nnthisTxt) && nnthisTxt->Equals(nntxt.v, nntxt.leng))
	{
		return;
	}
	else if (txt.SetTo(nntxt))
	{
		OPTSTR_DEL(this->txt);
		this->txt = Text::String::New(nntxt).Ptr();
		this->txtChg = true;
	}
}
