#include "Stdafx.h"
#include "Media/Resizer/LanczosResizer8_C8.h"
#include "UI/DObj/DirectObject.h"
#include "UI/DObj/ImageDObjHandler.h"

void UI::DObj::ImageDObjHandler::DrawBkg(Media::DrawImage *dimg)
{
	if (this->bmpBkg)
	{
		UOSInt scnW = dimg->GetWidth();
		UOSInt scnH = dimg->GetHeight();
		if (this->bmpBuff != 0)
		{
			if (this->bmpBuff->GetWidth() != scnW && this->bmpBuff->GetHeight() != scnH)
			{
				this->deng->DeleteImage(this->bmpBuff);
				this->bmpBuff = 0;
			}
		}
		if (this->bmpBuff == 0)
		{
			Media::StaticImage *simg = this->bmpBkg->ToStaticImage();
			Media::ColorProfile srgb(Media::ColorProfile::CPT_SRGB);
			Media::ColorProfile dispProfile(Media::ColorProfile::CPT_PDISPLAY);
			Media::Resizer::LanczosResizer8_C8 resizer(4, 3, &srgb, &dispProfile, colorSess, Media::AlphaType::AT_NO_ALPHA);
			resizer.SetTargetWidth(scnW);
			resizer.SetTargetHeight(scnH);
			resizer.SetResizeAspectRatio(Media::IImgResizer::RAR_KEEPAR);
			Media::StaticImage *srimg = resizer.ProcessToNew(simg);
			DEL_CLASS(simg);
			this->bmpBuff = this->deng->ConvImage(srimg);
			DEL_CLASS(srimg);
		}
		dimg->DrawImagePt(this->bmpBuff, 0, 0);
	}
	else
	{
		Media::DrawBrush *b = dimg->NewBrushARGB(this->bgColor);
		dimg->DrawRect(0, 0, UOSInt2Double(dimg->GetWidth()), UOSInt2Double(dimg->GetHeight()), 0, b);
		dimg->DelBrush(b);
	}
}

UI::DObj::ImageDObjHandler::ImageDObjHandler(Media::DrawEngine *deng, Text::CString fileName) : UI::DObj::DObjHandler(deng)
{
	this->bmpBkg = this->deng->LoadImage(fileName);
	this->bmpBuff = 0;
	this->bgColor = 0xff000000;
}

UI::DObj::ImageDObjHandler::~ImageDObjHandler()
{
	if (this->bmpBkg != 0)
	{
		this->deng->DeleteImage(this->bmpBkg);
		this->bmpBkg = 0;
	}
	if (this->bmpBuff != 0)
	{
		this->deng->DeleteImage(this->bmpBuff);
		this->bmpBuff = 0;
	}
}

void UI::DObj::ImageDObjHandler::SetBGColor(UInt32 bgColor)
{
	this->bgColor = bgColor;
}
