#include "Stdafx.h"
#include "Math/Math.h"
#include "Media/DrawEngine.h"
#include "UI/DObj/DownButtonDObj.h"

UI::DObj::DownButtonDObj::DownButtonDObj(NotNullPtr<Media::DrawEngine> deng, Text::CString fileNameUnclick, Text::CString fileNameClicked, Math::Coord2D<OSInt> tl, UI::UIEvent clkHdlr, AnyType clkUserObj) : DirectObject(tl)
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
	NotNullPtr<Media::DrawImage> img;
	if (img.Set(this->bmpUnclick))
	{
		this->deng->DeleteImage(img);
		this->bmpUnclick = 0;
	}
	if (img.Set(this->bmpClicked))
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

void UI::DObj::DownButtonDObj::DrawObject(NotNullPtr<Media::DrawImage> dimg)
{
	Math::Coord2D<OSInt> tl = this->GetCurrPos();
	this->dispTL = tl;
	NotNullPtr<Media::DrawImage> bmpUnclick;
	NotNullPtr<Media::DrawImage> bmpClicked;
	if (bmpUnclick.Set(this->bmpUnclick) && bmpClicked.Set(this->bmpClicked))
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
	else if (bmpUnclick.Set(this->bmpUnclick))
	{
		dimg->DrawImagePt(bmpUnclick, tl.ToDouble());
	}
	else if (bmpClicked.Set(this->bmpClicked))
	{
		dimg->DrawImagePt(bmpClicked, tl.ToDouble());
	}
	this->updated = false;
}

Bool UI::DObj::DownButtonDObj::IsObject(Math::Coord2D<OSInt> scnPos)
{
	if (scnPos.x < this->dispTL.x || scnPos.y < this->dispTL.y)
		return false;
	Media::DrawImage *bmpChk = this->bmpUnclick;
	if (bmpChk == 0)
	{
		bmpChk = this->bmpClicked;
		if (bmpChk == 0)
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
