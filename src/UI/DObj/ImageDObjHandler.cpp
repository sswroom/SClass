#include "Stdafx.h"
#include "UI/DObj/DirectObject.h"
#include "UI/DObj/ImageDObjHandler.h"

void UI::DObj::ImageDObjHandler::DrawBkg(Media::DrawImage *dimg)
{
	if (this->bmpBkg)
	{
		dimg->DrawImagePt(this->bmpBkg, 0, 0);
	}
}

UI::DObj::ImageDObjHandler::ImageDObjHandler(Media::DrawEngine *deng, Text::CString fileName) : UI::DObj::DObjHandler(deng)
{
	this->bmpBkg = this->deng->LoadImage(fileName);
}

UI::DObj::ImageDObjHandler::~ImageDObjHandler()
{
	if (this->bmpBkg != 0)
	{
		this->deng->DeleteImage(this->bmpBkg);
		this->bmpBkg = 0;
	}
}
