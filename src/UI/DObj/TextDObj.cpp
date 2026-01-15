#include "Stdafx.h"
#include "Math/Math_C.h"
#include "Media/DrawImageTool.h"
#include "Text/StringBuilder.hpp"
#include "UI/DObj/TextDObj.h"

UI::DObj::TextDObj::TextDObj(NN<Media::DrawEngine> deng, Text::CString txt, Text::CString fontName, Double fontSize, Media::DrawEngine::DrawFontStyle fontStyle, UInt32 fontColor, UInt32 codePage, Math::Coord2D<IntOS> tl, Math::Size2D<UIntOS> size) : DirectObject(tl)
{
	this->deng = deng;
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
	this->fontSize = fontSize;
	this->fontStyle = fontStyle;
	this->fontColor = fontColor;
	this->size = size;
	this->currPage = 0;
	this->pageChg = false;
	this->codePage = codePage;
	this->talign = TA_LEFT;
	this->lineHeight = fontSize * 1.5;

	NN<Text::String> nntxt;
	if (this->txt.SetTo(nntxt))
	{
		NN<Media::DrawImage> dimg;
		if (this->deng->CreateImage32(this->size, Media::AT_ALPHA_ALL_FF).SetTo(dimg))
		{
			NN<Media::DrawFont> f = dimg->NewFontPx(this->fontName->ToCString(), this->fontSize, (Media::DrawEngine::DrawFontStyle)(fontStyle | Media::DrawEngine::DFS_ANTIALIAS), codePage);
			Media::DrawImageTool::SplitString(dimg, nntxt->ToCString(), this->lines, f, IntOS2Double(this->size.x));
			dimg->DelFont(f);
			this->deng->DeleteImage(dimg);
		}
	}
}

UI::DObj::TextDObj::~TextDObj()
{
	OPTSTR_DEL(this->txt);
	this->fontName->Release();
	this->lines.FreeAll();
}

Bool UI::DObj::TextDObj::IsChanged()
{
	return this->pageChg;
}

Bool UI::DObj::TextDObj::DoEvents()
{
	return false;
}

void UI::DObj::TextDObj::DrawObject(NN<Media::DrawImage> dimg)
{
	this->pageChg = false;
	Math::Size2DDbl sz;
	NN<Media::DrawFont> f;
	NN<Media::DrawBrush> b;
	f = dimg->NewFontPx(this->fontName->ToCString(), this->fontSize, (Media::DrawEngine::DrawFontStyle)(this->fontStyle | Media::DrawEngine::DFS_ANTIALIAS), this->codePage);
	b = dimg->NewBrushARGB(this->fontColor);
	Math::Coord2D<IntOS> tl = this->GetCurrPos();
	UInt32 linePerPage = (UInt32)Double2Int32(UIntOS2Double(this->size.y) / this->lineHeight);
	UInt32 currLine = this->currPage * linePerPage;
	UIntOS j = this->lines.GetCount();
	Double currPos = IntOS2Double(tl.y);
	Double endPos = IntOS2Double(tl.y + (IntOS)this->size.y) - this->lineHeight;
	while (currPos <= endPos && currLine < j)
	{
		if (this->talign == TA_LEFT)
		{
			dimg->DrawString(Math::Coord2DDbl(IntOS2Double(tl.x), currPos), Text::String::OrEmpty(this->lines.GetItem(currLine)), f, b);
		}
		else if (this->talign == TA_CENTER)
		{
			sz = dimg->GetTextSize(f, Text::String::OrEmpty(this->lines.GetItem(currLine))->ToCString());
			dimg->DrawString(Math::Coord2DDbl(IntOS2Double(tl.x) + (UIntOS2Double(this->size.x) - sz.x) * 0.5, currPos), Text::String::OrEmpty(this->lines.GetItem(currLine))->ToCString(), f, b);
		}
		else if (this->talign == TA_RIGHT)
		{
			sz = dimg->GetTextSize(f, Text::String::OrEmpty(this->lines.GetItem(currLine))->ToCString());
			dimg->DrawString(Math::Coord2DDbl(IntOS2Double(tl.x + (IntOS)this->size.x) - sz.x, currPos), Text::String::OrEmpty(this->lines.GetItem(currLine)), f, b);
		}
		currLine++;
		currPos += this->lineHeight;
	}
	dimg->DelFont(f);
	dimg->DelBrush(b);
}

Bool UI::DObj::TextDObj::IsObject(Math::Coord2D<IntOS> scnPos)
{
	return false;
}

/*System::Windows::Forms::Cursor ^UI::DObj::TextDObj::GetCursor()
{
	return System::Windows::Forms::Cursors::Arrow;
}*/

void UI::DObj::TextDObj::OnMouseDown()
{
}

void UI::DObj::TextDObj::OnMouseUp()
{
}

void UI::DObj::TextDObj::OnMouseClick()
{
}

void UI::DObj::TextDObj::SetTextAlign(TextAlign talign)
{
	this->talign = talign;
}

UIntOS UI::DObj::TextDObj::GetPageCount()
{
	UIntOS linePerPage = (UIntOS)Double2IntOS(UIntOS2Double(this->size.y) / this->lineHeight);
	return (this->lines.GetCount() - 1 + linePerPage) / linePerPage;
}

UInt32 UI::DObj::TextDObj::GetCurrPage()
{
	return this->currPage;
}

void UI::DObj::TextDObj::SetCurrPage(UInt32 currPage)
{
	this->currPage = currPage;
	this->pageChg = true;
}
