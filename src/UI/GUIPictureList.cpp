#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math_C.h"
#include "Media/Resizer/LanczosResizerRGB_C8.h"
#include "Sync/Interlocked.h"
#include "UI/GUIClientControl.h"
#include "UI/GUIPictureList.h"
//#include <windows.h>

#define ICONPADDING 10

UI::GUIPictureList::GUIPictureList(NN<UI::GUICore> ui, NN<UI::GUIClientControl> parent, NN<Media::DrawEngine> eng, Bool hasBorder, Math::Size2D<UIntOS> iconSize, Optional<Media::ColorSess> colorSess) : UI::GUICustomDrawVScroll(ui, parent, eng, colorSess)
{
	this->hasBorder = hasBorder;
	this->iconSize = iconSize;
	this->selectedIndex = INVALID_INDEX;

	Media::ColorProfile rgbColor(Media::ColorProfile::CPT_SRGB);
	NEW_CLASS(this->resizer, Media::Resizer::LanczosResizerRGB_C8(4, 3, rgbColor, rgbColor, nullptr, Media::AT_ALPHA_ALL_FF));
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
	UIntOS w = img->GetWidth();
	UIntOS h = img->GetHeight();
	UIntOS i;
	UIntOS j;
	UIntOS k;
	UIntOS currY = this->GetVScrollPos();
	IntOS ofst;
	IntOS x;
	IntOS y;
	UIntOS iconPerRow;
	NN<Media::DrawImage> gimg;
	this->ClearBackground(img);

	if (w <= this->iconSize.x + ICONPADDING)
	{
		ofst = -(IntOS)currY;
		i = 0;
		j = this->imgList.GetCount();
		while (i < j)
		{
			if (ofst + (IntOS)this->iconSize.y + ICONPADDING > 0)
			{
				if (i == this->selectedIndex)
				{
					NN<Media::DrawBrush> b = img->NewBrushARGB(0xffff4040);
					img->DrawRect(Math::Coord2DDbl(0, IntOS2Double(ofst)), Math::Size2DDbl(UIntOS2Double(w), UIntOS2Double(this->iconSize.y + ICONPADDING)), nullptr, b);
					img->DelBrush(b);
				}
				if (this->imgList.GetItem(i).SetTo(gimg))
				{
					x = (IntOS)(w - gimg->GetWidth()) >> 1;
					y = (IntOS)((this->iconSize.y + ICONPADDING - gimg->GetHeight()) >> 1) + ofst;
					gimg->SetHDPI(img->GetHDPI());
					gimg->SetVDPI(img->GetVDPI());
					img->DrawImagePt(gimg, Math::Coord2DDbl(IntOS2Double(x), IntOS2Double(y)));
				}
			}
			ofst += (IntOS)this->iconSize.y + 10;
			if (ofst > (IntOS)h)
			{
				break;
			}
			i++;
		}
	}
	else
	{
		iconPerRow = w / (this->iconSize.x + ICONPADDING);
		ofst = -(IntOS)currY;
		i = 0;
		j = this->imgList.GetCount();
		while (i < j)
		{
			if (ofst + (IntOS)this->iconSize.y + ICONPADDING > 0)
			{
				k = 0;
				while (k < iconPerRow)
				{
					if (i + k == this->selectedIndex)
					{
						NN<Media::DrawBrush> b = img->NewBrushARGB(0xffff4040);
						img->DrawRect(Math::Coord2DDbl(UIntOS2Double((this->iconSize.x + ICONPADDING) * k), IntOS2Double(ofst)), this->iconSize.ToDouble() + ICONPADDING, nullptr, b);
						img->DelBrush(b);
					}
					if (this->imgList.GetItem(i + k).SetTo(gimg))
					{
						x = (IntOS)(((this->iconSize.x + ICONPADDING - gimg->GetWidth()) >> 1) + (this->iconSize.x + ICONPADDING) * k);
						y = (IntOS)((IntOS)((this->iconSize.y + ICONPADDING - gimg->GetHeight()) >> 1) + ofst);
						gimg->SetHDPI(img->GetHDPI());
						gimg->SetVDPI(img->GetVDPI());
						img->DrawImagePt(gimg, Math::Coord2DDbl(IntOS2Double(x), IntOS2Double(y)));
					}
					k++;
					if (i + k >= j)
						break;
				}
			}
			ofst += (IntOS)this->iconSize.y + 10;
			if (ofst > (IntOS)h)
			{
				break;
			}
			i += iconPerRow;
		}
	}

	if (this->hasBorder)
	{
		NN<Media::DrawPen> p = img->NewPenARGB(0xff000000, 1, nullptr, 0);
		img->DrawRect(Math::Coord2DDbl(0, 0), Math::Size2DDbl(UIntOS2Double(w - 1), UIntOS2Double(h - 1)), p, nullptr);
		img->DelPen(p);
	}
}

void UI::GUIPictureList::OnMouseDown(IntOS scrollY, Math::Coord2D<IntOS> pos, UI::GUIClientControl::MouseButton btn, KeyButton keys)
{
	UIntOS i;
	i = this->IndexFromPoint(pos);
	if (i != INVALID_INDEX && i != this->selectedIndex)
	{
		this->selectedIndex = i;
		this->Redraw();
	}
}

UIntOS UI::GUIPictureList::IndexFromPoint(Math::Coord2D<IntOS> pos)
{
	UIntOS index;
	UIntOS currY = this->GetVScrollPos();
	Math::Size2D<UIntOS> sz = this->GetSizeP();
	if (pos.x < 0 || pos.x >= (IntOS)sz.x || pos.y < 0 || pos.y >= (IntOS)sz.y)
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
		UIntOS iconPerRow = sz.x / (this->iconSize.x + ICONPADDING);
		if (pos.x >= (IntOS)(iconPerRow * (this->iconSize.x + ICONPADDING)))
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

UIntOS UI::GUIPictureList::GetCount()
{
	return this->imgList.GetCount();
}

void UI::GUIPictureList::RemoveAt(UIntOS index)
{
	NN<Media::DrawImage> img;
	if (this->imgList.RemoveAt(index).SetTo(img))
	{
		this->deng->DeleteImage(img);
	}
}

void UI::GUIPictureList::Clear()
{
	UIntOS i = this->imgList.GetCount();
	while (i-- > 0)
	{
		NN<Media::DrawImage> img;
		if(this->imgList.RemoveAt(i).SetTo(img))
			this->deng->DeleteImage(img);
	}
}

UIntOS UI::GUIPictureList::GetSelectedIndex()
{
	return this->selectedIndex;
}

void UI::GUIPictureList::SetSelectedIndex(UIntOS index)
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
