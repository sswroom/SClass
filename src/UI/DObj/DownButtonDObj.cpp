#include "Stdafx.h"
#include "Math/Math.h"
#include "Media/GDIEngine.h"
#include "UI/DObj/DownButtonDObj.h"

UI::DObj::DownButtonDObj::DownButtonDObj(Media::DrawEngine *deng, const WChar *fileNameUnclick, const WChar *fileNameClicked, OSInt left, OSInt top, UI::UIEvent clkHdlr, void *clkUserObj) : DirectObject(left, top)
{
	this->deng = deng;
	if (fileNameUnclick == 0)
	{
		this->bmpUnclick = 0;
	}
	else
	{
		this->bmpUnclick = this->deng->LoadImageW(fileNameUnclick);
	}

	if (fileNameClicked == 0)
	{
		this->bmpClicked = 0;
	}
	else
	{
		this->bmpClicked = this->deng->LoadImageW(fileNameClicked);
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
	OSInt left;
	OSInt top;
	this->GetCurrPos(&left, &top);
	this->dispLeft = left;
	this->dispTop = top;
	if (this->bmpUnclick && this->bmpClicked)
	{
		if (this->isMouseDown)
		{
			dimg->DrawImagePt(this->bmpClicked, Math::OSInt2Double(left), Math::OSInt2Double(top));
		}
		else
		{
			dimg->DrawImagePt(this->bmpUnclick, Math::OSInt2Double(left), Math::OSInt2Double(top));
		}
	}
	else if (this->bmpUnclick)
	{
		dimg->DrawImagePt(this->bmpUnclick, Math::OSInt2Double(left), Math::OSInt2Double(top));
	}
	else if (this->bmpClicked)
	{
		dimg->DrawImagePt(this->bmpClicked, Math::OSInt2Double(left), Math::OSInt2Double(top));
	}
	this->updated = false;
}

Bool UI::DObj::DownButtonDObj::IsObject(OSInt x, OSInt y)
{
	if (x < this->dispLeft || y < this->dispTop)
		return false;
	Media::DrawImage *bmpChk = this->bmpUnclick;
	if (bmpChk == 0)
	{
		bmpChk = this->bmpClicked;
		if (bmpChk == 0)
			return false;
	}
	if (this->dispLeft + (OSInt)bmpChk->GetWidth() <= x || this->dispTop + (OSInt)bmpChk->GetHeight() <= y)
		return false;
	return (((Media::GDIImage*)bmpChk)->GetPixel32(x - this->dispLeft, y - this->dispTop) & 0xff000000) != 0;
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
