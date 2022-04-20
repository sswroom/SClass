#include "Stdafx.h"
#include "Media/ANPR.h"

Media::ANPR::ANPR() : ocr(Media::OCREngine::Language::English)
{
	this->ocr.SetCharWhiteList("0123456789ABCDEFGHJKLMNPQRSTUVWXYZ");
}

Media::ANPR::~ANPR()
{

}

Text::String *Media::ANPR::ParseImage(Media::StaticImage *simg)
{

}
