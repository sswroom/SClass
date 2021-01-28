#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math.h"
#include "Media/StaticImage.h"
#include "Media/ImageGen/ColorImageGen.h"
#include "Media/ImageGen/Gamma64ImageGen.h"
#include "Media/ImageGen/GammaImageGen.h"
#include "Media/ImageGen/Gradian32ImageGen.h"
#include "Media/ImageGen/ImageGenMgr.h"
#include "Media/ImageGen/LinesImageGen.h"
#include "Media/ImageGen/RingsImageGen.h"

Media::ImageGen::ImageGenMgr::ImageGenMgr()
{
	Media::ImageGenerator *imgGen;
	NEW_CLASS(this->imgGenArr, Data::ArrayList<Media::ImageGenerator*>());

	NEW_CLASS(imgGen, Media::ImageGen::GammaImageGen());
	this->imgGenArr->Add(imgGen);
	NEW_CLASS(imgGen, Media::ImageGen::Gamma64ImageGen());
	this->imgGenArr->Add(imgGen);
	NEW_CLASS(imgGen, Media::ImageGen::ColorImageGen());
	this->imgGenArr->Add(imgGen);
	NEW_CLASS(imgGen, Media::ImageGen::RingsImageGen());
	this->imgGenArr->Add(imgGen);
	NEW_CLASS(imgGen, Media::ImageGen::LinesImageGen());
	this->imgGenArr->Add(imgGen);
	NEW_CLASS(imgGen, Media::ImageGen::Gradian32ImageGen());
	this->imgGenArr->Add(imgGen);
}
Media::ImageGen::ImageGenMgr::~ImageGenMgr()
{
	Media::ImageGenerator *imgGen;
	OSInt i = this->imgGenArr->GetCount();
	while (i-- > 0)
	{
		imgGen = this->imgGenArr->GetItem(i);
		DEL_CLASS(imgGen);
	}
	DEL_CLASS(imgGenArr);
}

OSInt Media::ImageGen::ImageGenMgr::GetCount()
{
	return this->imgGenArr->GetCount();
}

Media::ImageGenerator *Media::ImageGen::ImageGenMgr::GetGenerator(OSInt index)
{
	return this->imgGenArr->GetItem(index);
}
