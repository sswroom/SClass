#include "Stdafx.h"
#include "Math/Math.h"
#include "Media/DrawImageTool.h"
#include "Text/StringBuilder.h"
#include "UI/DObj/TextDObj.h"

UI::DObj::TextDObj::TextDObj(Media::DrawEngine *deng, const WChar *txt, const WChar *fontName, Double fontSize, Media::DrawEngine::DrawFontStyle fontStyle, Int32 fontColor, OSInt left, OSInt top, OSInt width, OSInt height, Int32 codePage) : DirectObject(left, top)
{
	this->deng = deng;
	if (txt)
	{
		this->txt = Text::StrCopyNew(txt);
	}
	else
	{
		this->txt = 0;
	}
	if (fontName)
	{
		this->fontName = Text::StrCopyNew(fontName);
	}
	else
	{
		this->fontName = Text::StrCopyNew(L"Arial");
	}
	this->fontSize = fontSize;
	this->fontStyle = fontStyle;
	this->fontColor = fontColor;
	this->width = width;
	this->height = height;
	this->currPage = 0;
	this->pageChg = false;
	this->codePage = codePage;
	this->talign = TA_LEFT;
	NEW_CLASS(this->lines, Data::ArrayList<const WChar *>());
	this->lineHeight = fontSize * 1.5;

	if (this->txt)
	{
		Media::DrawImage *dimg = this->deng->CreateImage32(width, height, Media::AT_NO_ALPHA);
		Media::DrawFont *f = dimg->NewFontH(this->fontName, this->fontSize, (Media::DrawEngine::DrawFontStyle)(fontStyle | Media::DrawEngine::DFS_ANTIALIAS), codePage);
		Media::DrawImageTool::SplitString(dimg, this->txt, this->lines, f, Math::OSInt2Double(width));
		dimg->DelFont(f);
		this->deng->DeleteImage(dimg);
	}
}

UI::DObj::TextDObj::~TextDObj()
{
	OSInt i;
	SDEL_TEXT(this->txt);
	Text::StrDelNew(this->fontName);
	i = this->lines->GetCount();
	while (i-- > 0)
	{
		Text::StrDelNew(this->lines->GetItem(i));
	}
	DEL_CLASS(this->lines);
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
	OSInt left;
	OSInt top;
	Double sz[2];
	Media::DrawFont *f;
	Media::DrawBrush *b;
	f = dimg->NewFontH(this->fontName, this->fontSize, (Media::DrawEngine::DrawFontStyle)(this->fontStyle | Media::DrawEngine::DFS_ANTIALIAS), this->codePage);
	b = dimg->NewBrushARGB(this->fontColor);
	this->GetCurrPos(&left, &top);
	Int32 linePerPage = (Int32)(this->height / this->lineHeight);
	Int32 currLine = this->currPage * linePerPage;
	OSInt j = this->lines->GetCount();
	Double currPos = Math::OSInt2Double(top);
	Double endPos = top + this->height - this->lineHeight;
	while (currPos <= endPos && currLine < j)
	{
		if (this->talign == TA_LEFT)
		{
			dimg->DrawString(Math::OSInt2Double(left), currPos, this->lines->GetItem(currLine), f, b);
		}
		else if (this->talign == TA_CENTER)
		{
			dimg->GetTextSize(f, this->lines->GetItem(currLine), -1, sz);
			dimg->DrawString(Math::OSInt2Double(left) + (this->width - sz[0]) * 0.5, currPos, this->lines->GetItem(currLine), f, b);
		}
		else if (this->talign == TA_RIGHT)
		{
			dimg->GetTextSize(f, this->lines->GetItem(currLine), -1, sz);
			dimg->DrawString(Math::OSInt2Double(left + width) - sz[0], currPos, this->lines->GetItem(currLine), f, b);
		}
		currLine++;
		currPos += this->lineHeight;
	}
	dimg->DelFont(f);
	dimg->DelBrush(b);
}

Bool UI::DObj::TextDObj::IsObject(OSInt x, OSInt y)
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

OSInt UI::DObj::TextDObj::GetPageCount()
{
	Int32 linePerPage = (Int32)(this->height / this->lineHeight);
	return (this->lines->GetCount() - 1 + linePerPage) / linePerPage;
}

Int32 UI::DObj::TextDObj::GetCurrPage()
{
	return this->currPage;
}

void UI::DObj::TextDObj::SetCurrPage(Int32 currPage)
{
	this->currPage = currPage;
	this->pageChg = true;
}
