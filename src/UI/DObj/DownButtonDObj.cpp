#include "Stdafx.h"
#include "Math/Math_C.h"
#include "Media/DrawEngine.h"
#include "UI/DObj/DownButtonDObj.h"

UI::DObj::DownButtonDObj::DownButtonDObj(NN<Media::DrawEngine> deng, Text::CString fileNameUnclick, Text::CString fileNameClicked, Math::Coord2D<OSInt> tl, UI::UIEvent clkHdlr, AnyType clkUserObj) : DirectObject(tl)
{
	this->deng = deng;
	if (fileNameUnclick.leng == 0)
	{
		this->bmpUnclick = 0;
	}
	else
	{
		this->bmpUnclick = this->deng->LoadImage(fileNameUnclick.OrEmpty());
	}

	if (fileNameClicked.leng == 0)
	{
		this->bmpClicked = 0;
	}
	else
	{
		this->bmpClicked = this->deng->LoadImage(fileNameClicked.OrEmpty());
	}
	this->clkHdlr = clkHdlr;
	this->clkUserObj = clkUserObj;
	this->isMouseDown = false;
	this->updated = false;
}

UI::DObj::DownButtonDObj::~DownButtonDObj()
{
	NN<Media::DrawImage> img;
	if (this->bmpUnclick.SetTo(img))
	{
		this->deng->DeleteImage(img);
		this->bmpUnclick = 0;
	}
	if (this->bmpClicked.SetTo(img))
	{
		this->deng->DeleteImage(img);
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

void UI::DObj::DownButtonDObj::DrawObject(NN<Media::DrawImage> dimg)
{
	Math::Coord2D<OSInt> tl = this->GetCurrPos();
	this->dispTL = tl;
	NN<Media::DrawImage> bmpUnclick;
	NN<Media::DrawImage> bmpClicked;
	if (this->bmpUnclick.SetTo(bmpUnclick) && this->bmpClicked.SetTo(bmpClicked))
	{
		if (this->isMouseDown)
		{
			dimg->DrawImagePt(bmpClicked, tl.ToDouble());
		}
		else
		{
			dimg->DrawImagePt(bmpUnclick, tl.ToDouble());
		}
	}
	else if (this->bmpUnclick.SetTo(bmpUnclick))
	{
		dimg->DrawImagePt(bmpUnclick, tl.ToDouble());
	}
	else if (this->bmpClicked.SetTo(bmpClicked))
	{
		dimg->DrawImagePt(bmpClicked, tl.ToDouble());
	}
	this->updated = false;
}

Bool UI::DObj::DownButtonDObj::IsObject(Math::Coord2D<OSInt> scnPos)
{
	if (scnPos.x < this->dispTL.x || scnPos.y < this->dispTL.y)
		return false;
	NN<Media::DrawImage> bmpChk;
	if (!this->bmpUnclick.SetTo(bmpChk))
	{
		if (!this->bmpClicked.SetTo(bmpChk))
			return false;
	}
	if (this->dispTL.x + (OSInt)bmpChk->GetWidth() <= scnPos.x || this->dispTL.y + (OSInt)bmpChk->GetHeight() <= scnPos.y)
		return false;
	return (bmpChk->GetPixel32(scnPos.x - this->dispTL.x, scnPos.y - this->dispTL.y) & 0xff000000) != 0;
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
