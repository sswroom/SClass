#include "Stdafx.h"
#include "IO/Path.h"
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
	NEW_CLASSNN(this->clsData, ClassData());
	this->clsData->currImg = 0;
	switch (lang)
	{
	default:
	case Language::English:
		this->lang = Text::String::New(CSTR("eng"));
		break;
	}
	this->clsData->api.Init(0, (const Char*)this->lang->v.Ptr());
}

Media::OCREngine::OCREngine(Text::CStringNN lang)
{
	this->hdlr = 0;
	this->hdlrObj = 0;
	this->lang = Text::String::New(lang);
	NEW_CLASSNN(this->clsData, ClassData());
	this->clsData->currImg = 0;
	this->clsData->api.Init(0, (const Char*)this->lang->v.Ptr());
}

Media::OCREngine::~OCREngine()
{
	if (this->clsData->currImg)
	{
		pixDestroy(&this->clsData->currImg);
		this->clsData->currImg = 0;
	}
	this->clsData->api.End();
	this->clsData.Delete();
	this->lang->Release();
}

void Media::OCREngine::ChangeLanguage(Text::CStringNN lang)
{
	this->lang->Release();
	this->lang = Text::String::New(lang);
	this->clsData->api.Init(0, (const Char*)lang.v.Ptr());
	if (this->clsData->currImg)
	{
		this->clsData->api.SetImage(this->clsData->currImg);
	}
}

void Media::OCREngine::SetCharWhiteList(UnsafeArrayOpt<const Char> whiteList)
{
	UnsafeArray<const Char> nnlist;
	if (whiteList.SetTo(nnlist))
	{
		this->clsData->api.SetVariable("tessedit_char_whitelist", nnlist.Ptr());
	}
	else
	{
		this->clsData->api.End();
		this->clsData->api.Init(0, (const Char*)this->lang->v.Ptr());
		if (this->clsData->currImg)
		{
			this->clsData->api.SetImage(this->clsData->currImg);
		}
	}
}

Bool Media::OCREngine::SetParsingImage(NN<Media::StaticImage> img)
{
	PIX *pix;
	if (img->info.pf == Media::PF_PAL_W8)
	{
		pix = pixCreateNoInit((l_int32)img->info.dispSize.x, (l_int32)img->info.dispSize.y, (l_int32)img->info.storeBPP);
	}
	else
	{
		img->ToB8G8R8A8();
		pix = pixCreateNoInit((l_int32)img->info.dispSize.x, (l_int32)img->info.dispSize.y, (l_int32)img->info.storeBPP);
	}
	if (pix == 0)
	{
		return false;
	}
	pixSetResolution(pix, Double2Int32(img->info.hdpi), Double2Int32(img->info.hdpi * img->info.par2));
	UIntOS wpl = (UIntOS)pixGetWpl(pix);
	UInt8 *data = (UInt8*)pixGetData(pix);
	img->GetRasterData(data, 0, 0, img->info.dispSize.x, img->info.dispSize.y, wpl * 4, false, Media::RotateType::None);
	if (img->info.pf == Media::PF_PAL_W8)
	{
		UIntOS wordCnt = wpl * img->info.dispSize.y;
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

Bool Media::OCREngine::SetOCVFrame(NN<Media::OpenCV::OCVFrame> frame)
{
	PIX *pix;
	pix = pixCreateNoInit((l_int32)frame->GetWidth(), (l_int32)frame->GetHeight(), (l_int32)8);
	if (pix == 0)
	{
		return false;
	}
	pixSetResolution(pix, 72.0, 72.0);
	UIntOS wpl = (UIntOS)pixGetWpl(pix);
	UInt8 *data = (UInt8*)pixGetData(pix);
	frame->GetImageData(data, 0, 0, frame->GetWidth(), frame->GetHeight(), wpl * 4, false, Media::RotateType::None);
	UIntOS wordCnt = wpl * frame->GetHeight();
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

Optional<Text::String> Media::OCREngine::ParseInsideImage(Math::RectArea<UIntOS> area, OptOut<UIntOS> confidence)
{
	if (this->clsData->currImg == 0 || area.min.x >= area.max.x || area.min.y >= area.max.y || area.max.x > (UIntOS)pixGetWidth(this->clsData->currImg) || area.max.y > (UIntOS)pixGetHeight(this->clsData->currImg))
	{
		return nullptr;
	}
	Optional<Text::String> s = nullptr;
	this->clsData->api.SetRectangle((int)area.min.x, (int)area.min.y, (int)area.GetWidth(), (int)area.GetHeight());
	char *resultText = this->clsData->api.GetUTF8Text();
	if (resultText)
	{
		if (confidence.IsNotNull())
		{
			confidence.SetNoCheck((UIntOS)this->clsData->api.MeanTextConf());
		}
		s = Text::String::NewNotNullSlow((const UTF8Char*)resultText);
		delete [] resultText;
	}
	return s;
}

void Media::OCREngine::HandleOCRResult(OCRResultFunc hdlr, AnyType userObj)
{
	this->hdlr = hdlr;
	this->hdlrObj = userObj;
}

Bool Media::OCREngine::ParseAllInImage()
{
	this->clsData->api.SetRectangle(0, 0, (int)pixGetWidth(this->clsData->currImg), (int)pixGetHeight(this->clsData->currImg));
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
					NN<Text::String> res = Text::String::NewNotNullSlow((const UTF8Char*)txt);
					this->hdlr(this->hdlrObj, res, confidence, Math::RectArea<IntOS>(left, top, right - left, bottom - top));
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

void Media::OCREngine::GetAvailableLanguages(NN<Data::ArrayListStringNN> langs) const
{
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	UnsafeArray<UTF8Char> sptr2;
	sptr = Text::StrConcatASCII(sbuff, this->clsData->api.GetDatapath());
	if (sptr[-1] != IO::Path::PATH_SEPERATOR)
	{
		*sptr++ = IO::Path::PATH_SEPERATOR;
	}
	sptr2 = Text::StrConcatC(sptr, UTF8STRC("*.traineddata"));
	NN<IO::Path::FindFileSession> sess;
	if (IO::Path::FindFile(CSTRP(sbuff, sptr2)).SetTo(sess))
	{
		while (IO::Path::FindNextFile(sptr, sess, 0, 0, 0).SetTo(sptr2))
		{
			langs->Add(Text::String::NewP(sptr, UnsafeArray<const UTF8Char>(&sptr2[-12])));
		}
		IO::Path::FindFileClose(sess);
	}
}
