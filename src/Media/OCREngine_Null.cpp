#include "Stdafx.h"
#include "Media/OCREngine.h"

Media::OCREngine::OCREngine(Language lang)
{
	this->clsData = 0;
}

Media::OCREngine::~OCREngine()
{
}

void Media::OCREngine::SetCharWhiteList(UnsafeArrayOpt<const Char> whiteList)
{
}

Bool Media::OCREngine::SetParsingImage(NN<Media::StaticImage> img)
{
	return false;
}

Bool Media::OCREngine::SetOCVFrame(NN<Media::OpenCV::OCVFrame> frame)
{
	return false;
}

Optional<Text::String> Media::OCREngine::ParseInsideImage(Math::RectArea<UIntOS> area, OptOut<UIntOS> confidence)
{
	return 0;
}

void Media::OCREngine::HandleOCRResult(OCRResultFunc hdlr, AnyType userObj)
{
	this->hdlr = hdlr;
	this->hdlrObj = userObj;
}

Bool Media::OCREngine::ParseAllInImage()
{
	return false;
}
