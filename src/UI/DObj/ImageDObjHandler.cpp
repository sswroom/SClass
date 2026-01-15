#include "Stdafx.h"
#include "Media/Resizer/LanczosResizerRGB_C8.h"
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
				this->bmpBuff = nullptr;
			}
		}
		if (this->bmpBuff.IsNull())
		{
			NN<Media::StaticImage> simg;
			if (bmpBkg->ToStaticImage().SetTo(simg))
			{
				Media::ColorProfile srgb(Media::ColorProfile::CPT_SRGB);
				Media::ColorProfile dispProfile(Media::ColorProfile::CPT_PDISPLAY);
				Media::Resizer::LanczosResizerRGB_C8 resizer(4, 3, srgb, dispProfile, Optional<Media::ColorManagerSess>::ConvertFrom(colorSess), Media::AlphaType::AT_ALPHA_ALL_FF);
				resizer.SetTargetSize(scnSize);
				resizer.SetResizeAspectRatio(Media::ImageResizer::RAR_KEEPAR);
				NN<Media::StaticImage> srimg;
				if (resizer.ProcessToNew(simg).SetTo(srimg))
				{
					simg.Delete();
					this->bmpBuff = this->deng->ConvImage(srimg, this->colorSess);
					srimg.Delete();
				}
				else
				{
					simg.Delete();
				}
			}
			else
			{
				this->bmpBuff = nullptr;
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
		dimg->DrawRect(Math::Coord2DDbl(0, 0), dimg->GetSize().ToDouble(), nullptr, b);
		dimg->DelBrush(b);
	}
}

UI::DObj::ImageDObjHandler::ImageDObjHandler(NN<Media::DrawEngine> deng, Text::CStringNN fileName, Optional<Media::ColorSess> colorSess) : UI::DObj::DObjHandler(deng)
{
	this->bmpBkg = this->deng->LoadImage(fileName);
	this->bmpBuff = nullptr;
	this->bgColor = 0xff000000;
	this->colorSess = colorSess;
}

UI::DObj::ImageDObjHandler::~ImageDObjHandler()
{
	NN<Media::DrawImage> img;
	if (this->bmpBkg.SetTo(img))
	{
		this->deng->DeleteImage(img);
		this->bmpBkg = nullptr;
	}
	if (this->bmpBuff.SetTo(img))
	{
		this->deng->DeleteImage(img);
		this->bmpBuff = nullptr;
	}
}

void UI::DObj::ImageDObjHandler::SetBGColor(UInt32 bgColor)
{
	this->bgColor = bgColor;
}
