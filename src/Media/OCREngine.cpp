#include "Stdafx.h"
#include "Media/OCREngine.h"

#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>

class Media::OCREngine::ClassData
{
public:
	tesseract::TessBaseAPI api;
	PIX *currImg;
};

Media::OCREngine::OCREngine(Language lang)
{
	NEW_CLASS(this->clsData, ClassData());
	this->clsData->currImg = 0;
	switch (lang)
	{
	default:
	case Language::English:
		this->clsData->api.Init(0, "eng");
		break;
	}
}

Media::OCREngine::~OCREngine()
{
	if (this->clsData->currImg)
	{
		pixDestroy(&this->clsData->currImg);
		this->clsData->currImg = 0;
	}
	this->clsData->api.End();
	DEL_CLASS(this->clsData);
}

void Media::OCREngine::SetCharWhiteList(const Char *whiteList)
{
	this->clsData->api.SetVariable("tessedit_char_whitelist", whiteList);
}

Bool Media::OCREngine::SetParsingImage(Media::StaticImage *img)
{
	PIX *pix;
	if (img->info.pf == Media::PF_PAL_W8)
	{
		pix = pixCreateNoInit((l_int32)img->info.dispWidth, (l_int32)img->info.dispHeight, (l_int32)img->info.storeBPP);
	}
	else
	{
		img->To32bpp();
		pix = pixCreateNoInit((l_int32)img->info.dispWidth, (l_int32)img->info.dispHeight, (l_int32)img->info.storeBPP);
	}
	if (pix == 0)
	{
		return false;
	}
	pixSetResolution(pix, Double2Int32(img->info.hdpi), Double2Int32(img->info.hdpi * img->info.par2));
	UOSInt wpl = (UOSInt)pixGetWpl(pix);
	UInt8 *data = (UInt8*)pixGetData(pix);
	img->GetImageData(data, 0, 0, img->info.dispWidth, img->info.dispHeight, wpl * 4 * img->info.storeBPP >> 3, false);
	if (img->info.pf == Media::PF_PAL_W8)
	{
		UOSInt wordCnt = wpl * img->info.dispHeight;
		while (wordCnt-- > 0)
		{
			WriteMUInt32(data, ReadUInt32(data));
			data += 4;
		}
	}
	if (this->clsData->currImg)
	{
		pixDestroy(&this->clsData->currImg);
	}
	this->clsData->currImg = pix;
	this->clsData->api.SetImage(pix);
	return true;
}

Bool Media::OCREngine::SetOCVFrame(Media::OpenCV::OCVFrame *frame)
{
	PIX *pix;
	pix = pixCreateNoInit((l_int32)frame->GetWidth(), (l_int32)frame->GetHeight(), (l_int32)8);
	if (pix == 0)
	{
		return false;
	}
	pixSetResolution(pix, 72.0, 72.0);
	UOSInt wpl = (UOSInt)pixGetWpl(pix);
	UInt8 *data = (UInt8*)pixGetData(pix);
	frame->GetImageData(data, 0, 0, frame->GetWidth(), frame->GetHeight(), wpl * 4, false);
	UOSInt wordCnt = wpl * frame->GetHeight();
	while (wordCnt-- > 0)
	{
		WriteMUInt32(data, ReadUInt32(data));
		data += 4;
	}
	if (this->clsData->currImg)
	{
		pixDestroy(&this->clsData->currImg);
	}
	this->clsData->currImg = pix;
	this->clsData->api.SetImage(pix);
	return true;
}

Text::String *Media::OCREngine::ParseInsideImage(UOSInt left, UOSInt top, UOSInt width, UOSInt height)
{
	if (this->clsData->currImg == 0)
	{
		return 0;
	}
	Text::String *s = 0;
	this->clsData->api.SetRectangle((int)left, (int)top, (int)width, (int)height);
	char *resultText = this->clsData->api.GetUTF8Text();
	if (resultText)
	{
		s = Text::String::NewNotNullSlow((const UTF8Char*)resultText);
		delete [] resultText;
	}
	return s;
}
