#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math.h"
#include "Media/Resizer/LanczosResizer8_C8.h"
#include "Sync/Interlocked.h"
#include "UI/GUIClientControl.h"
#include "UI/GUIPictureList.h"
//#include <windows.h>

#define ICONPADDING 10

UI::GUIPictureList::GUIPictureList(UI::GUICore *ui, UI::GUIClientControl *parent, Media::DrawEngine *eng, Bool hasBorder, UOSInt iconWidth, UOSInt iconHeight) : UI::GUICustomDrawVScroll(ui, parent, eng)
{
	this->hasBorder = hasBorder;
	this->iconWidth = iconWidth;
	this->iconHeight = iconHeight;
	this->selectedIndex = INVALID_INDEX;
	NEW_CLASS(this->imgList, Data::ArrayList<Media::DrawImage*>());

	Media::ColorProfile rgbColor(Media::ColorProfile::CPT_SRGB);
	NEW_CLASS(this->resizer, Media::Resizer::LanczosResizer8_C8(4, 3, &rgbColor, &rgbColor, 0, Media::AT_NO_ALPHA));
	this->resizer->SetResizeAspectRatio(Media::IImgResizer::RAR_SQUAREPIXEL);
	this->resizer->SetTargetWidth(iconWidth);
	this->resizer->SetTargetHeight(iconHeight);
}

UI::GUIPictureList::~GUIPictureList()
{
	this->Clear();
	DEL_CLASS(this->imgList);
	DEL_CLASS(this->resizer);
}

void UI::GUIPictureList::OnDraw(Media::DrawImage *img)
{
	UOSInt w = img->GetWidth();
	UOSInt h = img->GetHeight();
	UOSInt i;
	UOSInt j;
	UOSInt k;
	UOSInt currY = this->GetVScrollPos();
	OSInt ofst;
	OSInt x;
	OSInt y;
	UOSInt iconPerRow;
	Media::DrawImage *gimg;
	this->ClearBackground(img);

	if (w <= this->iconWidth + ICONPADDING)
	{
		ofst = -(OSInt)currY;
		i = 0;
		j = this->imgList->GetCount();
		while (i < j)
		{
			if (ofst + (OSInt)this->iconHeight + ICONPADDING > 0)
			{
				if (i == this->selectedIndex)
				{
					Media::DrawBrush *b = img->NewBrushARGB(0xffff4040);
					img->DrawRect(0, OSInt2Double(ofst), UOSInt2Double(w), UOSInt2Double(this->iconHeight + ICONPADDING), 0, b);
					img->DelBrush(b);
				}
				gimg = this->imgList->GetItem(i);
				x = (OSInt)(w - gimg->GetWidth()) >> 1;
				y = (OSInt)((this->iconHeight + ICONPADDING - gimg->GetHeight()) >> 1) + ofst;
				gimg->SetHDPI(img->GetHDPI());
				gimg->SetVDPI(img->GetVDPI());
				img->DrawImagePt(gimg, OSInt2Double(x), OSInt2Double(y));
			}
			ofst += (OSInt)this->iconHeight + 10;
			if (ofst > (OSInt)h)
			{
				break;
			}
			i++;
		}
	}
	else
	{
		iconPerRow = w / (this->iconWidth + ICONPADDING);
		ofst = -(OSInt)currY;
		i = 0;
		j = this->imgList->GetCount();
		while (i < j)
		{
			if (ofst + (OSInt)this->iconHeight + ICONPADDING > 0)
			{
				k = 0;
				while (k < iconPerRow)
				{
					if (i + k == this->selectedIndex)
					{
						Media::DrawBrush *b = img->NewBrushARGB(0xffff4040);
						img->DrawRect(UOSInt2Double((this->iconWidth + ICONPADDING) * k), OSInt2Double(ofst), UOSInt2Double(this->iconWidth + ICONPADDING), UOSInt2Double(this->iconHeight + ICONPADDING), 0, b);
						img->DelBrush(b);
					}
					gimg = this->imgList->GetItem(i + k);
					x = (Int32)(((this->iconWidth + ICONPADDING - gimg->GetWidth()) >> 1) + (this->iconWidth + ICONPADDING) * k);
					y = (Int32)((OSInt)((this->iconHeight + ICONPADDING - gimg->GetHeight()) >> 1) + ofst);
					gimg->SetHDPI(img->GetHDPI());
					gimg->SetVDPI(img->GetVDPI());
					img->DrawImagePt(gimg, OSInt2Double(x), OSInt2Double(y));
					k++;
					if (i + k >= j)
						break;
				}
			}
			ofst += (OSInt)this->iconHeight + 10;
			if (ofst > (OSInt)h)
			{
				break;
			}
			i += iconPerRow;
		}
	}

	if (this->hasBorder)
	{
		Media::DrawPen *p = img->NewPenARGB(0xff000000, 1, 0, 0);
		img->DrawRect(0, 0, UOSInt2Double(w - 1), UOSInt2Double(h - 1), p, 0);
		img->DelPen(p);
	}
}

void UI::GUIPictureList::OnMouseDown(OSInt scrollY, Int32 xPos, Int32 yPos, UI::GUIClientControl::MouseButton btn, KeyButton keys)
{
	UOSInt i;
	i = this->IndexFromPoint(xPos, yPos);
	if (i != INVALID_INDEX && i != this->selectedIndex)
	{
		this->selectedIndex = i;
		this->Redraw();
	}
}

UOSInt UI::GUIPictureList::IndexFromPoint(Int32 x, Int32 y)
{
	UOSInt w;
	UOSInt h;
	UOSInt index;
	UOSInt currY = this->GetVScrollPos();
	this->GetSizeP(&w, &h);
	if (x < 0 || x >= (OSInt)w || y < 0 || y >= (OSInt)h)
	{
		return INVALID_INDEX;
	}
	if (w <= this->iconWidth + ICONPADDING)
	{
		index = ((UInt32)y + currY) / (this->iconHeight + ICONPADDING);
		if (index >= this->imgList->GetCount())
			return INVALID_INDEX;
		return index;
	}
	else
	{
		UOSInt iconPerRow = w / (this->iconWidth + ICONPADDING);
		if (x >= (OSInt)(iconPerRow * (this->iconWidth + ICONPADDING)))
			return INVALID_INDEX;
		index = ((UInt32)y + currY) / (this->iconHeight + ICONPADDING) * iconPerRow + (UInt32)x / (this->iconWidth + ICONPADDING);
		if (index >= this->imgList->GetCount())
			return INVALID_INDEX;
		return index;
	}
}

void UI::GUIPictureList::Add(Media::Image *img)
{
	Media::StaticImage *simg = img->CreateStaticImage();
	Media::StaticImage *nsimg = this->resizer->ProcessToNew(simg);
	if (nsimg)
	{
		this->imgList->Add(this->deng->ConvImage(nsimg));
		DEL_CLASS(nsimg);
	}
	DEL_CLASS(simg);
}

UOSInt UI::GUIPictureList::GetCount()
{
	return this->imgList->GetCount();
}

void UI::GUIPictureList::RemoveAt(UOSInt index)
{
	Media::DrawImage *img = this->imgList->RemoveAt(index);
	if (img)
	{
		this->deng->DeleteImage(img);
	}
}

void UI::GUIPictureList::Clear()
{
	UOSInt i = this->imgList->GetCount();
	while (i-- > 0)
	{
		Media::DrawImage *img = this->imgList->RemoveAt(i);
		this->deng->DeleteImage(img);
	}
}

UOSInt UI::GUIPictureList::GetSelectedIndex()
{
	return this->selectedIndex;
}

void UI::GUIPictureList::SetSelectedIndex(UOSInt index)
{
	if (index == INVALID_INDEX || index < this->imgList->GetCount())
	{
		this->selectedIndex = index;
	}
	else
	{
		index = 0;
	}
}
