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
	this->hdlr = 0;
	this->hdlrObj = 0;
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
	img->GetImageData(data, 0, 0, img->info.dispWidth, img->info.dispHeight, wpl * 4, false, Media::RotateType::None);
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
	frame->GetImageData(data, 0, 0, frame->GetWidth(), frame->GetHeight(), wpl * 4, false, Media::RotateType::None);
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

Text::String *Media::OCREngine::ParseInsideImage(Math::RectArea<UOSInt> area, UOSInt *confidence)
{
	if (this->clsData->currImg == 0)
	{
		return 0;
	}
	Text::String *s = 0;
	this->clsData->api.SetRectangle((int)area.tl.x, (int)area.tl.y, (int)area.GetWidth(), (int)area.GetHeight());
	char *resultText = this->clsData->api.GetUTF8Text();
	if (resultText)
	{
		if (confidence)
		{
			*confidence = (UOSInt)this->clsData->api.MeanTextConf();
		}
		s = Text::String::NewNotNullSlow((const UTF8Char*)resultText);
		delete [] resultText;
	}
	return s;
}

void Media::OCREngine::HandleOCRResult(OCRResultFunc hdlr, void *userObj)
{
	this->hdlr = hdlr;
	this->hdlrObj = userObj;
}

Bool Media::OCREngine::ParseAllInImage()
{
	if (this->clsData->api.Recognize(0) != 0)
	{
		return false;
	}
	tesseract::PageIteratorLevel level = tesseract::RIL_WORD;
	tesseract::ResultIterator* ri = this->clsData->api.GetIterator();
	if (ri)
	{
		while (true)
		{
			int left = 0;
			int top = 0;
			int right = 0;
			int bottom = 0;
			const Char *txt = ri->GetUTF8Text(level);
			if (txt[0])
			{
				Single confidence = ri->Confidence(level);
				ri->BoundingBox(level, &left, &top, &right, &bottom);
				if (this->hdlr)
				{
					Text::String *res = Text::String::NewNotNullSlow((const UTF8Char*)txt);
					this->hdlr(this->hdlrObj, res, confidence, Math::RectArea<OSInt>(left, top, right - left, bottom - top));
					res->Release();
				}
			}
			delete []txt;
			if (!ri->Next(level))
			{
				break;
			}
		}
		delete ri;
	}
	return true;
}
