#include "Stdafx.h"
#include "Media/OCREngine.h"

Media::OCREngine::OCREngine(Language lang)
{
	this->clsData = 0;
}

Media::OCREngine::~OCREngine()
{
}

void Media::OCREngine::SetCharWhiteList(const Char *whiteList)
{
}

Bool Media::OCREngine::SetParsingImage(Media::StaticImage *img)
{
	return false;
}

Bool Media::OCREngine::SetOCVFrame(Media::OpenCV::OCVFrame *frame)
{
	return false;
}

Optional<Text::String> Media::OCREngine::ParseInsideImage(Math::RectArea<UOSInt> area, OptOut<UOSInt> confidence)
{
	return 0;
}

void Media::OCREngine::HandleOCRResult(OCRResultFunc hdlr, void *userObj)
{
	this->hdlr = hdlr;
	this->hdlrObj = userObj;
}

Bool Media::OCREngine::ParseAllInImage()
{
	return false;
}
