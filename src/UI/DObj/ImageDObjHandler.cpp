#include "Stdafx.h"
#include "Media/Resizer/LanczosResizer8_C8.h"
#include "UI/DObj/DirectObject.h"
#include "UI/DObj/ImageDObjHandler.h"

void UI::DObj::ImageDObjHandler::DrawBkg(NN<Media::DrawImage> dimg)
{
	NN<Media::DrawImage> bmpBkg;
	if (this->bmpBkg.SetTo(bmpBkg))
	{
		NN<Media::DrawImage> bmpBuff;
		Math::Size2D<UOSInt> scnSize = dimg->GetSize();
		if (this->bmpBuff.SetTo(bmpBuff))
		{
			if (bmpBuff->GetWidth() != scnSize.x && bmpBuff->GetHeight() != scnSize.y)
			{
				this->deng->DeleteImage(bmpBuff);
				this->bmpBuff = 0;
			}
		}
		if (this->bmpBuff.IsNull())
		{
			NN<Media::StaticImage> simg;
			if (bmpBkg->ToStaticImage().SetTo(simg))
			{
				Media::ColorProfile srgb(Media::ColorProfile::CPT_SRGB);
				Media::ColorProfile dispProfile(Media::ColorProfile::CPT_PDISPLAY);
				Media::Resizer::LanczosResizer8_C8 resizer(4, 3, srgb, dispProfile, colorSess, Media::AlphaType::AT_NO_ALPHA);
				resizer.SetTargetSize(scnSize);
				resizer.SetResizeAspectRatio(Media::ImageResizer::RAR_KEEPAR);
				NN<Media::StaticImage> srimg;
				if (resizer.ProcessToNew(simg).SetTo(srimg))
				{
					simg.Delete();
					this->bmpBuff = this->deng->ConvImage(srimg);
					srimg.Delete();
				}
				else
				{
					simg.Delete();
				}
			}
			else
			{
				this->bmpBuff = 0;
			}
		}
		if (this->bmpBuff.SetTo(bmpBuff))
		{
			dimg->DrawImagePt(bmpBuff, Math::Coord2DDbl(0, 0));
		}
	}
	else
	{
		NN<Media::DrawBrush> b = dimg->NewBrushARGB(this->bgColor);
		dimg->DrawRect(Math::Coord2DDbl(0, 0), dimg->GetSize().ToDouble(), 0, b);
		dimg->DelBrush(b);
	}
}

UI::DObj::ImageDObjHandler::ImageDObjHandler(NN<Media::DrawEngine> deng, Text::CStringNN fileName) : UI::DObj::DObjHandler(deng)
{
	this->bmpBkg = this->deng->LoadImage(fileName);
	this->bmpBuff = 0;
	this->bgColor = 0xff000000;
}

UI::DObj::ImageDObjHandler::~ImageDObjHandler()
{
	NN<Media::DrawImage> img;
	if (this->bmpBkg.SetTo(img))
	{
		this->deng->DeleteImage(img);
		this->bmpBkg = 0;
	}
	if (this->bmpBuff.SetTo(img))
	{
		this->deng->DeleteImage(img);
		this->bmpBuff = 0;
	}
}

void UI::DObj::ImageDObjHandler::SetBGColor(UInt32 bgColor)
{
	this->bgColor = bgColor;
}
