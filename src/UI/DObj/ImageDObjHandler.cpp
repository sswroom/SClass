#include "Stdafx.h"
#include "UI/DObj/DirectObject.h"
#include "UI/DObj/ImageDObjHandler.h"

void UI::DObj::ImageDObjHandler::DrawBkg(Media::DrawImage *dimg)
{
	if (this->bmpBkg)
	{
		dimg->DrawImagePt(this->bmpBkg, 0, 0);
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
	this->bgColor = 0xff000000;
}

UI::DObj::ImageDObjHandler::~ImageDObjHandler()
{
	if (this->bmpBkg != 0)
	{
		this->deng->DeleteImage(this->bmpBkg);
		this->bmpBkg = 0;
	}
}

void UI::DObj::ImageDObjHandler::SetBGColor(UInt32 bgColor)
{
	this->bgColor = bgColor;
}
