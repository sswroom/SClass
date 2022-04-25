#include "Stdafx.h"
#include "Math/Math.h"
#include "Math/Quadrilateral.h"
#include "Media/ANPR.h"
#include "Media/LinearRectRemapper.h"
#include "Media/OpenCV/OCVNumPlateFinder.h"

#include "Exporter/PNGExporter.h"
#include "Media/ImageList.h"
#include <stdio.h>

struct ParseStatus
{
	Media::ANPR *me;
	Media::StaticImage *simg;
	Data::ArrayList<UOSInt> *pastPos;
};

void Media::ANPR::NumPlateArea(void *userObj, Media::OpenCV::OCVFrame *filteredFrame, Math::Coord2D<UOSInt> *rect, Double maxTiltAngle, Double pxArea, Media::OpenCV::OCVNumPlateFinder::PlateSize psize)
{
	ParseStatus *status = (ParseStatus*)userObj;
	Math::RectArea<UOSInt> area;
	Math::RectArea<UOSInt>::GetRectArea(&area, rect, 4);
	UOSInt confidence;
	UOSInt i = status->pastPos->GetCount();
	while (i > 0)
	{
		i -= 2;
		if (area.ContainPt(status->pastPos->GetItem(i), status->pastPos->GetItem(i + 1)))
		{
			return;
		}
	}
	Media::StaticImage *plainImg = CreatePlainImage(filteredFrame->GetDataPtr(), filteredFrame->GetWidth(), filteredFrame->GetHeight(), (UOSInt)filteredFrame->GetBpl(), rect, psize);
	Media::OpenCV::OCVFrame *croppedFrame = Media::OpenCV::OCVFrame::CreateYFrame(plainImg);
	if (croppedFrame)
	{
		croppedFrame->Normalize();
		status->me->ocr.SetOCVFrame(croppedFrame);
		Text::String *s = status->me->ocr.ParseInsideImage(Math::RectArea<UOSInt>(0, 0, plainImg->info.dispWidth, plainImg->info.dispHeight), &confidence);
		if (s)
		{
			s->RemoveWS();
			if (s->leng == 0 || s->leng > 10)
			{
			}
			else
			{
				s->Replace('I', '1');
				if (status->me->hdlr)
				{
					status->me->hdlr(status->me->hdlrObj, status->simg, &area, s, maxTiltAngle, pxArea, confidence, plainImg);
				}
				
/*				printf("OCR result: %s\r\n", s->v);
				UTF8Char sbuff[32];
				UTF8Char *sptr;

				Exporter::PNGExporter exporter;
				sptr = Text::StrConcatC(Text::StrUOSInt(Text::StrConcatC(sbuff, UTF8STRC("OCRRes")), status->me->parsedCnt), UTF8STRC(".png"));
				Media::ImageList imgList(CSTRP(sbuff, sptr));
				imgList.AddImage(plainImg->Clone(), 0);
				imgList.ToStaticImage(0);
				exporter.ExportNewFile(imgList.GetSourceNameObj()->ToCString(), &imgList, 0);*/

				status->me->parsedCnt++;
				status->pastPos->Add((rect[0].x + rect[1].x + rect[2].x + rect[3].x) >> 2);
				status->pastPos->Add((rect[0].y + rect[1].y + rect[2].y + rect[3].y) >> 2);
			}
			s->Release();
		}
		else
		{
			printf("OCR parsing error\r\n");
		}
		DEL_CLASS(croppedFrame);
	}
	DEL_CLASS(plainImg);
}

Media::StaticImage *Media::ANPR::CreatePlainImage(UInt8 *sptr, UOSInt swidth, UOSInt sheight, UOSInt sbpl, Math::Coord2D<UOSInt> *rect, Media::OpenCV::OCVNumPlateFinder::PlateSize psize)
{
	return CreatePlainImage(sptr, swidth, sheight, sbpl, Math::Quadrilateral::FromPolygon(rect), psize);
}

Media::StaticImage *Media::ANPR::CreatePlainImage(UInt8 *sptr, UOSInt swidth, UOSInt sheight, UOSInt sbpl, Math::Quadrilateral quad, Media::OpenCV::OCVNumPlateFinder::PlateSize psize)
{
	UOSInt imgW;
	UOSInt imgH;

	if (psize == Media::OpenCV::OCVNumPlateFinder::PlateSize::SingleRow) //520 x 110 or 520 x 120
	{
		imgW = 195;
		imgH = 45;
	}
	else // 331 x 170 or 331 x 180
	{
		imgW = 150;
		imgW = 75;
	}
	Media::ColorProfile color(Media::ColorProfile::CPT_SRGB);
	return Media::LinearRectRemapper::RemapW8(sptr, swidth, sheight, (OSInt)sbpl, imgW, imgH, quad, &color, Media::ColorProfile::YUVT_SMPTE170M, Media::YCOFST_C_CENTER_LEFT);
}

Media::ANPR::ANPR() : ocr(Media::OCREngine::Language::English)
{
	this->ocr.SetCharWhiteList("0123456789ABCDEFGHIJKLMNPQRSTUVWXYZ");
	this->parsedCnt = 0;
	this->hdlr = 0;
	this->hdlrObj = 0;
}

Media::ANPR::~ANPR()
{

}

void Media::ANPR::SetResultHandler(NumPlateResult hdlr, void *userObj)
{
	this->hdlr = hdlr;
	this->hdlrObj = userObj;
}

Bool Media::ANPR::ParseImage(Media::StaticImage *simg)
{
	Media::StaticImage *bwImg = (Media::StaticImage*)simg->Clone();
	Bool found = false;
	bwImg->ToW8();
	this->ocr.SetParsingImage(bwImg);
	Media::OpenCV::OCVFrame *frame = Media::OpenCV::OCVFrame::CreateYFrame(bwImg);
	if (frame)
	{
		Data::ArrayList<UOSInt> pastPos;
		ParseStatus status;
		status.me = this;
		status.simg = simg;
		status.pastPos = &pastPos;
		this->finder.Find(frame, NumPlateArea, &status);
		DEL_CLASS(frame);
		found = pastPos.GetCount() > 0;
	}
	DEL_CLASS(bwImg);
	return found;
}

Bool Media::ANPR::ParseImageQuad(Media::StaticImage *simg, Math::Quadrilateral quad)
{
	Bool found = false;
	UOSInt confidence;
	Media::StaticImage *bwImg = (Media::StaticImage*)simg->Clone();
	bwImg->ToW8();
	Media::OpenCV::OCVNumPlateFinder::PlateSize psize;
	Double ratio = quad.CalcLenTop() / quad.CalcLenLeft();
	if (ratio >= 0.4 && ratio <= 2.5)
	{
		psize = Media::OpenCV::OCVNumPlateFinder::PlateSize::DoubleRow;
	}
	else
	{
		psize = Media::OpenCV::OCVNumPlateFinder::PlateSize::SingleRow;
	}

	Media::StaticImage *plainImg = CreatePlainImage(bwImg->data, bwImg->info.dispWidth, bwImg->info.dispHeight, bwImg->GetDataBpl(), quad, psize);
	Media::OpenCV::OCVFrame *croppedFrame = Media::OpenCV::OCVFrame::CreateYFrame(plainImg);
	if (croppedFrame)
	{
		croppedFrame->Normalize();
		this->ocr.SetOCVFrame(croppedFrame);
		Text::String *s = this->ocr.ParseInsideImage(Math::RectArea<UOSInt>(0, 0, plainImg->info.dispWidth, plainImg->info.dispHeight), &confidence);
		if (s)
		{
			s->RemoveWS();
			if (s->leng == 0 || s->leng > 10)
			{
			}
			else
			{
				s->Replace('I', '1');
				if (this->hdlr)
				{
					Math::RectArea<Double> dblArea = Math::RectArea<Double>::FromQuadrilateral(quad);
					Math::RectArea<UOSInt> area = Math::RectArea<UOSInt>((UOSInt)Double2OSInt(dblArea.tl.x), (UOSInt)Double2OSInt(dblArea.tl.y), (UOSInt)Double2OSInt(dblArea.width), (UOSInt)Double2OSInt(dblArea.height));
					this->hdlr(this->hdlrObj, simg, &area, s, quad.CalcMaxTiltAngle() * 180 / Math::PI, quad.CalcArea(), confidence, plainImg);
				}
				found = true;
				
				this->parsedCnt++;
			}
			s->Release();
		}
		else
		{
			printf("OCR parsing error\r\n");
		}
		DEL_CLASS(croppedFrame);
	}
	DEL_CLASS(plainImg);
	DEL_CLASS(bwImg);
	return found;
}
