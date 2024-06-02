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
	NN<Media::StaticImage> simg;
	Data::ArrayList<UOSInt> *pastPos;
};

void Media::ANPR::NumPlateArea(AnyType userObj, Media::OpenCV::OCVFrame *filteredFrame, Math::Coord2D<UOSInt> *rect, Double maxTiltAngle, Double pxArea, Media::OpenCV::OCVNumPlateFinder::PlateSize psize)
{
	NN<ParseStatus> status = userObj.GetNN<ParseStatus>();
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
	NN<Media::StaticImage> plainImg = CreatePlainImage(filteredFrame->GetDataPtr(), filteredFrame->GetSize(), (UOSInt)filteredFrame->GetBpl(), rect, psize);
	NN<Media::OpenCV::OCVFrame> croppedFrame;
	if (Media::OpenCV::OCVFrame::CreateYFrame(plainImg).SetTo(croppedFrame))
	{
		croppedFrame->Normalize();
		status->me->ocr.SetOCVFrame(croppedFrame);
		NN<Text::String> s;
		if (status->me->ocr.ParseInsideImage(Math::RectArea<UOSInt>(0, 0, plainImg->info.dispSize.x, plainImg->info.dispSize.y), confidence).SetTo(s))
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
					status->me->hdlr(status->me->hdlrObj, status->simg, area, s, maxTiltAngle, pxArea, confidence, plainImg);
				}
				
/*				printf("OCR result: %s\r\n", s->v);
				UTF8Char sbuff[32];
				UnsafeArray<UTF8Char> sptr;

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
		croppedFrame.Delete();
	}
	plainImg.Delete();
}

NN<Media::StaticImage> Media::ANPR::CreatePlainImage(UInt8 *sptr, Math::Size2D<UOSInt> sSize, UOSInt sbpl, Math::Coord2D<UOSInt> *rect, Media::OpenCV::OCVNumPlateFinder::PlateSize psize)
{
	return CreatePlainImage(sptr, sSize, sbpl, Math::Quadrilateral::FromPolygon(rect), psize);
}

NN<Media::StaticImage> Media::ANPR::CreatePlainImage(UInt8 *sptr, Math::Size2D<UOSInt> sSize, UOSInt sbpl, Math::Quadrilateral quad, Media::OpenCV::OCVNumPlateFinder::PlateSize psize)
{
	Math::Size2D<UOSInt> imgSize;

	if (psize == Media::OpenCV::OCVNumPlateFinder::PlateSize::SingleRow) //520 x 110 or 520 x 120
	{
		imgSize.x = 195;
		imgSize.y = 45;
	}
	else // 331 x 170 or 331 x 180
	{
		imgSize.x = 165;
		imgSize.y = 90;
	}
	Media::ColorProfile color(Media::ColorProfile::CPT_SRGB);
	return Media::LinearRectRemapper::RemapW8(sptr, sSize, (OSInt)sbpl, imgSize, quad, color, Media::ColorProfile::YUVT_SMPTE170M, Media::YCOFST_C_CENTER_LEFT);
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

void Media::ANPR::SetResultHandler(NumPlateResult hdlr, AnyType userObj)
{
	this->hdlr = hdlr;
	this->hdlrObj = userObj;
}

Bool Media::ANPR::ParseImage(NN<Media::StaticImage> simg)
{
	NN<Media::StaticImage> bwImg = NN<Media::StaticImage>::ConvertFrom(simg->Clone());
	Bool found = false;
	bwImg->ToW8();
	this->ocr.SetParsingImage(bwImg);
	NN<Media::OpenCV::OCVFrame> frame;
	if (Media::OpenCV::OCVFrame::CreateYFrame(bwImg).SetTo(frame))
	{
		Data::ArrayList<UOSInt> pastPos;
		ParseStatus status;
		status.me = this;
		status.simg = simg;
		status.pastPos = &pastPos;
		this->finder.Find(frame, NumPlateArea, &status);
		frame.Delete();
		found = pastPos.GetCount() > 0;
	}
	bwImg.Delete();
	return found;
}

Bool Media::ANPR::ParseImageQuad(NN<Media::StaticImage> simg, Math::Quadrilateral quad)
{
	Bool found = false;
	UOSInt confidence;
	NN<Media::StaticImage> bwImg = NN<Media::StaticImage>::ConvertFrom(simg->Clone());
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

	NN<Media::StaticImage> plainImg = CreatePlainImage(bwImg->data, bwImg->info.dispSize, bwImg->GetDataBpl(), quad, psize);
	NN<Media::OpenCV::OCVFrame> croppedFrame;
	if (Media::OpenCV::OCVFrame::CreateYFrame(plainImg).SetTo(croppedFrame))
	{
		NN<Media::OpenCV::OCVFrame> filteredFrame = croppedFrame->BilateralFilter(11, 17, 17);
		filteredFrame->Normalize();
		this->ocr.SetOCVFrame(filteredFrame);
		if (false)//psize == Media::OpenCV::OCVNumPlateFinder::PlateSize::DoubleRow)
		{
			NN<Text::String> s1 = Text::String::OrEmpty(this->ocr.ParseInsideImage(Math::RectArea<UOSInt>(0, 0, plainImg->info.dispSize.x, plainImg->info.dispSize.y >> 1), confidence));
			NN<Text::String> s2 = Text::String::OrEmpty(this->ocr.ParseInsideImage(Math::RectArea<UOSInt>(0, plainImg->info.dispSize.y >> 1, plainImg->info.dispSize.x, (plainImg->info.dispSize.y >> 1) + (plainImg->info.dispSize.y & 1)), confidence));
			s1->RemoveWS();
			s2->RemoveWS();
			if ((s1->leng + s2->leng) == 0 || (s1->leng + s2->leng) > 10)
			{
			}
			else
			{
				s1->Replace('I', '1');
				s2->Replace('I', '1');
				printf("s1 = \"%s\", s2 = \"%s\"\r\n", s1->v.Ptr(), s2->v.Ptr());
				UTF8Char sbuff[11];
				UnsafeArray<UTF8Char> sptr;
				sptr = s2->ConcatTo(s1->ConcatTo(sbuff));
				s1->Release();
				s1 = Text::String::NewP(sbuff, sptr);
				if (this->hdlr)
				{
					Math::RectArea<Double> dblArea = Math::RectArea<Double>::FromQuadrilateral(quad);
					Math::RectArea<UOSInt> area = Math::RectArea<UOSInt>((UOSInt)Double2OSInt(dblArea.min.x), (UOSInt)Double2OSInt(dblArea.min.y), (UOSInt)Double2OSInt(dblArea.GetWidth()), (UOSInt)Double2OSInt(dblArea.GetHeight()));
					this->hdlr(this->hdlrObj, simg, area, s1, quad.CalcMaxTiltAngle() * 180 / Math::PI, quad.CalcArea(), confidence, plainImg);
				}
				found = true;
				
				this->parsedCnt++;
			}
			s1->Release();
			s2->Release();
		}
		else
		{
			NN<Text::String> s;
			if (this->ocr.ParseInsideImage(Math::RectArea<UOSInt>(0, 0, plainImg->info.dispSize.x, plainImg->info.dispSize.y), confidence).SetTo(s))
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
						Math::RectArea<UOSInt> area = Math::RectArea<UOSInt>((UOSInt)Double2OSInt(dblArea.min.x), (UOSInt)Double2OSInt(dblArea.min.y), (UOSInt)Double2OSInt(dblArea.GetWidth()), (UOSInt)Double2OSInt(dblArea.GetHeight()));
						this->hdlr(this->hdlrObj, simg, area, s, quad.CalcMaxTiltAngle() * 180 / Math::PI, quad.CalcArea(), confidence, plainImg);
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
		}
		filteredFrame.Delete();
		croppedFrame.Delete();
	}
	plainImg.Delete();
	bwImg.Delete();
	return found;
}

Bool Media::ANPR::ParseImagePlatePoint(NN<Media::StaticImage> simg, Math::Coord2D<UOSInt> coord)
{
	Int32 rate = 16;
	Math::RectArea<UOSInt> rect;
	rect = simg->CalcNearPixelRange(coord, rate);
	if (rect.GetHeight() > 100)
	{
		while (rate > 10)
		{
			rate--;
			rect = simg->CalcNearPixelRange(coord, rate);
			if (rect.GetHeight() <= 100)
			{
				break;
			}
		}
	}
	else if (rect.GetWidth() < 150)
	{
		while (rate < 32)
		{
			rate++;
			rect = simg->CalcNearPixelRange(coord, rate);
			if (rect.GetWidth() >= 150)
			{
				break;
			}
		}
	}
	/*Media::StaticImage *subimg = simg->CreateSubImage(Math::RectArea<OSInt>((OSInt)rect.tl.x, (OSInt)rect.tl.y, (OSInt)rect.width, (OSInt)rect.height));
	Media::ImageList imgList(CSTR("Temp.png"));
	subimg->To32bpp();
	imgList.AddImage(subimg, 0);
	Exporter::PNGExporter exporter;
	exporter.ExportNewFile(imgList.GetSourceNameObj()->ToCString(), &imgList, 0);*/

	return this->ParseImageQuad(simg, Math::Quadrilateral(rect.GetMin().ToDouble(), Math::Coord2DDbl((Double)rect.max.x, (Double)rect.min.y), rect.GetMax().ToDouble(), Math::Coord2DDbl((Double)rect.min.x, (Double)rect.max.y)));
}
