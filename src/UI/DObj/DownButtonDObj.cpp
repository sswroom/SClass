#include "Stdafx.h"
#include "Math/Math.h"
#include "Media/DrawEngine.h"
#include "UI/DObj/DownButtonDObj.h"

UI::DObj::DownButtonDObj::DownButtonDObj(Media::DrawEngine *deng, Text::CString fileNameUnclick, Text::CString fileNameClicked, Math::Coord2D<OSInt> tl, UI::UIEvent clkHdlr, void *clkUserObj) : DirectObject(tl)
{
	this->deng = deng;
	if (fileNameUnclick.leng == 0)
	{
		this->bmpUnclick = 0;
	}
	else
	{
		this->bmpUnclick = this->deng->LoadImage(fileNameUnclick);
	}

	if (fileNameClicked.leng == 0)
	{
		this->bmpClicked = 0;
	}
	else
	{
		this->bmpClicked = this->deng->LoadImage(fileNameClicked);
	}
	this->clkHdlr = clkHdlr;
	this->clkUserObj = clkUserObj;
	this->isMouseDown = false;
	this->updated = false;
}

UI::DObj::DownButtonDObj::~DownButtonDObj()
{
	if (this->bmpUnclick)
	{
		this->deng->DeleteImage(this->bmpUnclick);
		this->bmpUnclick = 0;
	}
	if (this->bmpClicked)
	{
		this->deng->DeleteImage(this->bmpClicked);
		this->bmpClicked = 0;
	}
}

Bool UI::DObj::DownButtonDObj::IsChanged()
{
	if (this->IsMoving())
	{
		return true;
	}
	else
	{
		return this->updated;
	}
}

Bool UI::DObj::DownButtonDObj::DoEvents()
{
	return false;
}

void UI::DObj::DownButtonDObj::DrawObject(Media::DrawImage *dimg)
{
	Math::Coord2D<OSInt> tl = this->GetCurrPos();
	this->dispTL = tl;
	if (this->bmpUnclick && this->bmpClicked)
	{
		if (this->isMouseDown)
		{
			dimg->DrawImagePt(this->bmpClicked, OSInt2Double(tl.x), OSInt2Double(tl.y));
		}
		else
		{
			dimg->DrawImagePt(this->bmpUnclick, OSInt2Double(tl.x), OSInt2Double(tl.y));
		}
	}
	else if (this->bmpUnclick)
	{
		dimg->DrawImagePt(this->bmpUnclick, OSInt2Double(tl.x), OSInt2Double(tl.y));
	}
	else if (this->bmpClicked)
	{
		dimg->DrawImagePt(this->bmpClicked, OSInt2Double(tl.x), OSInt2Double(tl.y));
	}
	this->updated = false;
}

Bool UI::DObj::DownButtonDObj::IsObject(OSInt x, OSInt y)
{
	if (x < this->dispTL.x || y < this->dispTL.y)
		return false;
	Media::DrawImage *bmpChk = this->bmpUnclick;
	if (bmpChk == 0)
	{
		bmpChk = this->bmpClicked;
		if (bmpChk == 0)
			return false;
	}
	if (this->dispTL.x + (OSInt)bmpChk->GetWidth() <= x || this->dispTL.y + (OSInt)bmpChk->GetHeight() <= y)
		return false;
	return (bmpChk->GetPixel32(x - this->dispTL.x, y - this->dispTL.y) & 0xff000000) != 0;
}

/*System::Windows::Forms::Cursor ^UI::DObj::DownButtonDObj::GetCursor()
{
	return System::Windows::Forms::Cursors::Hand;
}*/

void UI::DObj::DownButtonDObj::OnMouseDown()
{
	this->isMouseDown = true;
	this->updated = true;
}

void UI::DObj::DownButtonDObj::OnMouseUp()
{
	this->isMouseDown = false;
	this->updated = true;
}

void UI::DObj::DownButtonDObj::OnMouseClick()
{
	this->clkHdlr(this->clkUserObj);
}
