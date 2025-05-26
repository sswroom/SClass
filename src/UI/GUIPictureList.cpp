#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math.h"
#include "Media/Resizer/LanczosResizerRGB_C8.h"
#include "Sync/Interlocked.h"
#include "UI/GUIClientControl.h"
#include "UI/GUIPictureList.h"
//#include <windows.h>

#define ICONPADDING 10

UI::GUIPictureList::GUIPictureList(NN<UI::GUICore> ui, NN<UI::GUIClientControl> parent, NN<Media::DrawEngine> eng, Bool hasBorder, Math::Size2D<UOSInt> iconSize, Optional<Media::ColorSess> colorSess) : UI::GUICustomDrawVScroll(ui, parent, eng, colorSess)
{
	this->hasBorder = hasBorder;
	this->iconSize = iconSize;
	this->selectedIndex = INVALID_INDEX;

	Media::ColorProfile rgbColor(Media::ColorProfile::CPT_SRGB);
	NEW_CLASS(this->resizer, Media::Resizer::LanczosResizerRGB_C8(4, 3, rgbColor, rgbColor, 0, Media::AT_ALPHA_ALL_FF));
	this->resizer->SetResizeAspectRatio(Media::ImageResizer::RAR_SQUAREPIXEL);
	this->resizer->SetTargetSize(iconSize);
}

UI::GUIPictureList::~GUIPictureList()
{
	this->Clear();
	DEL_CLASS(this->resizer);
}

void UI::GUIPictureList::OnDraw(NN<Media::DrawImage> img)
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
	NN<Media::DrawImage> gimg;
	this->ClearBackground(img);

	if (w <= this->iconSize.x + ICONPADDING)
	{
		ofst = -(OSInt)currY;
		i = 0;
		j = this->imgList.GetCount();
		while (i < j)
		{
			if (ofst + (OSInt)this->iconSize.y + ICONPADDING > 0)
			{
				if (i == this->selectedIndex)
				{
					NN<Media::DrawBrush> b = img->NewBrushARGB(0xffff4040);
					img->DrawRect(Math::Coord2DDbl(0, OSInt2Double(ofst)), Math::Size2DDbl(UOSInt2Double(w), UOSInt2Double(this->iconSize.y + ICONPADDING)), 0, b);
					img->DelBrush(b);
				}
				if (this->imgList.GetItem(i).SetTo(gimg))
				{
					x = (OSInt)(w - gimg->GetWidth()) >> 1;
					y = (OSInt)((this->iconSize.y + ICONPADDING - gimg->GetHeight()) >> 1) + ofst;
					gimg->SetHDPI(img->GetHDPI());
					gimg->SetVDPI(img->GetVDPI());
					img->DrawImagePt(gimg, Math::Coord2DDbl(OSInt2Double(x), OSInt2Double(y)));
				}
			}
			ofst += (OSInt)this->iconSize.y + 10;
			if (ofst > (OSInt)h)
			{
				break;
			}
			i++;
		}
	}
	else
	{
		iconPerRow = w / (this->iconSize.x + ICONPADDING);
		ofst = -(OSInt)currY;
		i = 0;
		j = this->imgList.GetCount();
		while (i < j)
		{
			if (ofst + (OSInt)this->iconSize.y + ICONPADDING > 0)
			{
				k = 0;
				while (k < iconPerRow)
				{
					if (i + k == this->selectedIndex)
					{
						NN<Media::DrawBrush> b = img->NewBrushARGB(0xffff4040);
						img->DrawRect(Math::Coord2DDbl(UOSInt2Double((this->iconSize.x + ICONPADDING) * k), OSInt2Double(ofst)), this->iconSize.ToDouble() + ICONPADDING, 0, b);
						img->DelBrush(b);
					}
					if (this->imgList.GetItem(i + k).SetTo(gimg))
					{
						x = (OSInt)(((this->iconSize.x + ICONPADDING - gimg->GetWidth()) >> 1) + (this->iconSize.x + ICONPADDING) * k);
						y = (OSInt)((OSInt)((this->iconSize.y + ICONPADDING - gimg->GetHeight()) >> 1) + ofst);
						gimg->SetHDPI(img->GetHDPI());
						gimg->SetVDPI(img->GetVDPI());
						img->DrawImagePt(gimg, Math::Coord2DDbl(OSInt2Double(x), OSInt2Double(y)));
					}
					k++;
					if (i + k >= j)
						break;
				}
			}
			ofst += (OSInt)this->iconSize.y + 10;
			if (ofst > (OSInt)h)
			{
				break;
			}
			i += iconPerRow;
		}
	}

	if (this->hasBorder)
	{
		NN<Media::DrawPen> p = img->NewPenARGB(0xff000000, 1, 0, 0);
		img->DrawRect(Math::Coord2DDbl(0, 0), Math::Size2DDbl(UOSInt2Double(w - 1), UOSInt2Double(h - 1)), p, 0);
		img->DelPen(p);
	}
}

void UI::GUIPictureList::OnMouseDown(OSInt scrollY, Math::Coord2D<OSInt> pos, UI::GUIClientControl::MouseButton btn, KeyButton keys)
{
	UOSInt i;
	i = this->IndexFromPoint(pos);
	if (i != INVALID_INDEX && i != this->selectedIndex)
	{
		this->selectedIndex = i;
		this->Redraw();
	}
}

UOSInt UI::GUIPictureList::IndexFromPoint(Math::Coord2D<OSInt> pos)
{
	UOSInt index;
	UOSInt currY = this->GetVScrollPos();
	Math::Size2D<UOSInt> sz = this->GetSizeP();
	if (pos.x < 0 || pos.x >= (OSInt)sz.x || pos.y < 0 || pos.y >= (OSInt)sz.y)
	{
		return INVALID_INDEX;
	}
	if (sz.x <= this->iconSize.x + ICONPADDING)
	{
		index = ((UInt32)pos.y + currY) / (this->iconSize.y + ICONPADDING);
		if (index >= this->imgList.GetCount())
			return INVALID_INDEX;
		return index;
	}
	else
	{
		UOSInt iconPerRow = sz.x / (this->iconSize.x + ICONPADDING);
		if (pos.x >= (OSInt)(iconPerRow * (this->iconSize.x + ICONPADDING)))
			return INVALID_INDEX;
		index = ((UInt32)pos.y + currY) / (this->iconSize.y + ICONPADDING) * iconPerRow + (UInt32)pos.x / (this->iconSize.x + ICONPADDING);
		if (index >= this->imgList.GetCount())
			return INVALID_INDEX;
		return index;
	}
}

void UI::GUIPictureList::Add(NN<Media::RasterImage> img)
{
	NN<Media::StaticImage> simg = img->CreateStaticImage();
	NN<Media::StaticImage> nsimg;
	NN<Media::DrawImage> dimg;
	if (this->resizer->ProcessToNew(simg).SetTo(nsimg))
	{
		if (this->deng->ConvImage(nsimg, this->colorSess).SetTo(dimg))
		{
			this->imgList.Add(dimg);
		}
		nsimg.Delete();
	}
	simg.Delete();
}

UOSInt UI::GUIPictureList::GetCount()
{
	return this->imgList.GetCount();
}

void UI::GUIPictureList::RemoveAt(UOSInt index)
{
	NN<Media::DrawImage> img;
	if (this->imgList.RemoveAt(index).SetTo(img))
	{
		this->deng->DeleteImage(img);
	}
}

void UI::GUIPictureList::Clear()
{
	UOSInt i = this->imgList.GetCount();
	while (i-- > 0)
	{
		NN<Media::DrawImage> img;
		if(this->imgList.RemoveAt(i).SetTo(img))
			this->deng->DeleteImage(img);
	}
}

UOSInt UI::GUIPictureList::GetSelectedIndex()
{
	return this->selectedIndex;
}

void UI::GUIPictureList::SetSelectedIndex(UOSInt index)
{
	if (index == INVALID_INDEX || index < this->imgList.GetCount())
	{
		this->selectedIndex = index;
	}
	else
	{
		index = 0;
	}
}
