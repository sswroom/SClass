#include "Stdafx.h"
#include "Math/Math.h"
#include "Media/DrawImageTool.h"
#include "Text/StringBuilder.h"
#include "UI/DObj/TextDObj.h"

UI::DObj::TextDObj::TextDObj(NotNullPtr<Media::DrawEngine> deng, Text::CString txt, Text::CString fontName, Double fontSize, Media::DrawEngine::DrawFontStyle fontStyle, UInt32 fontColor, UInt32 codePage, Math::Coord2D<OSInt> tl, Math::Size2D<UOSInt> size) : DirectObject(tl)
{
	this->deng = deng;
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
	this->fontSize = fontSize;
	this->fontStyle = fontStyle;
	this->fontColor = fontColor;
	this->size = size;
	this->currPage = 0;
	this->pageChg = false;
	this->codePage = codePage;
	this->talign = TA_LEFT;
	this->lineHeight = fontSize * 1.5;

	if (this->txt)
	{
		Media::DrawImage *dimg = this->deng->CreateImage32(this->size, Media::AT_NO_ALPHA);
		Media::DrawFont *f = dimg->NewFontPx(this->fontName->ToCString(), this->fontSize, (Media::DrawEngine::DrawFontStyle)(fontStyle | Media::DrawEngine::DFS_ANTIALIAS), codePage);
		Media::DrawImageTool::SplitString(dimg, this->txt->ToCString(), &this->lines, f, OSInt2Double(this->size.x));
		dimg->DelFont(f);
		this->deng->DeleteImage(dimg);
	}
}

UI::DObj::TextDObj::~TextDObj()
{
	UOSInt i;
	SDEL_STRING(this->txt);
	this->fontName->Release();
	i = this->lines.GetCount();
	while (i-- > 0)
	{
		this->lines.GetItem(i)->Release();
	}
}

Bool UI::DObj::TextDObj::IsChanged()
{
	return this->pageChg;
}

Bool UI::DObj::TextDObj::DoEvents()
{
	return false;
}

void UI::DObj::TextDObj::DrawObject(Media::DrawImage *dimg)
{
	this->pageChg = false;
	Math::Size2DDbl sz;
	Media::DrawFont *f;
	Media::DrawBrush *b;
	f = dimg->NewFontPx(this->fontName->ToCString(), this->fontSize, (Media::DrawEngine::DrawFontStyle)(this->fontStyle | Media::DrawEngine::DFS_ANTIALIAS), this->codePage);
	b = dimg->NewBrushARGB(this->fontColor);
	Math::Coord2D<OSInt> tl = this->GetCurrPos();
	UInt32 linePerPage = (UInt32)Double2Int32(UOSInt2Double(this->size.y) / this->lineHeight);
	UInt32 currLine = this->currPage * linePerPage;
	UOSInt j = this->lines.GetCount();
	Double currPos = OSInt2Double(tl.y);
	Double endPos = OSInt2Double(tl.y + (OSInt)this->size.y) - this->lineHeight;
	while (currPos <= endPos && currLine < j)
	{
		if (this->talign == TA_LEFT)
		{
			dimg->DrawString(Math::Coord2DDbl(OSInt2Double(tl.x), currPos), Text::String::OrEmpty(this->lines.GetItem(currLine)), f, b);
		}
		else if (this->talign == TA_CENTER)
		{
			sz = dimg->GetTextSize(f, this->lines.GetItem(currLine)->ToCString());
			dimg->DrawString(Math::Coord2DDbl(OSInt2Double(tl.x) + (UOSInt2Double(this->size.x) - sz.x) * 0.5, currPos), this->lines.GetItem(currLine)->ToCString(), f, b);
		}
		else if (this->talign == TA_RIGHT)
		{
			sz = dimg->GetTextSize(f, this->lines.GetItem(currLine)->ToCString());
			dimg->DrawString(Math::Coord2DDbl(OSInt2Double(tl.x + (OSInt)this->size.x) - sz.x, currPos), Text::String::OrEmpty(this->lines.GetItem(currLine)), f, b);
		}
		currLine++;
		currPos += this->lineHeight;
	}
	dimg->DelFont(f);
	dimg->DelBrush(b);
}

Bool UI::DObj::TextDObj::IsObject(Math::Coord2D<OSInt> scnPos)
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

UOSInt UI::DObj::TextDObj::GetPageCount()
{
	UOSInt linePerPage = (UOSInt)Double2OSInt(UOSInt2Double(this->size.y) / this->lineHeight);
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
