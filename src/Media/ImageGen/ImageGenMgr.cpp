#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math_C.h"
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
	NN<Media::ImageGenerator> imgGen;
	NEW_CLASSNN(imgGen, Media::ImageGen::GammaImageGen());
	this->imgGenArr.Add(imgGen);
	NEW_CLASSNN(imgGen, Media::ImageGen::Gamma64ImageGen());
	this->imgGenArr.Add(imgGen);
	NEW_CLASSNN(imgGen, Media::ImageGen::ColorImageGen());
	this->imgGenArr.Add(imgGen);
	NEW_CLASSNN(imgGen, Media::ImageGen::RingsImageGen());
	this->imgGenArr.Add(imgGen);
	NEW_CLASSNN(imgGen, Media::ImageGen::LinesImageGen());
	this->imgGenArr.Add(imgGen);
	NEW_CLASSNN(imgGen, Media::ImageGen::Gradian32ImageGen());
	this->imgGenArr.Add(imgGen);
}
Media::ImageGen::ImageGenMgr::~ImageGenMgr()
{
	this->imgGenArr.DeleteAll();
}

UOSInt Media::ImageGen::ImageGenMgr::GetCount() const
{
	return this->imgGenArr.GetCount();
}

NN<Media::ImageGenerator> Media::ImageGen::ImageGenMgr::GetItemNoCheck(UOSInt index) const
{
	return this->imgGenArr.GetItemNoCheck(index);
}

Optional<Media::ImageGenerator> Media::ImageGen::ImageGenMgr::GetItem(UOSInt index) const
{
	return this->imgGenArr.GetItem(index);
}
