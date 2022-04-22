#include "Stdafx.h"
#include "Math/Math.h"
#include "Media/ANPR.h"
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

void Media::ANPR::NumPlateArea(void *userObj, Media::OpenCV::OCVFrame *filteredFrame, Math::Coord2D<UOSInt> *rect, Double maxTileAngle, Double pxArea, Media::OpenCV::OCVNumPlateFinder::PlateSize psize)
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
				if (status->me->hdlr)
				{
					status->me->hdlr(status->me->hdlrObj, status->simg, &area, s, maxTileAngle, pxArea, confidence, plainImg);
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
	Double xPos;
	Double yPos;
	Double pt1x;
	Double pt1y;
	Double pt2x;
	Double pt2y;
	Double pt3x;
	Double pt3y;
	Double pt4x;
	Double pt4y;
	Double x;
	Double y;
	Double xRate;
	Double yRate;
	UOSInt ix;
	UOSInt iy;
	Media::StaticImage *ret;
	UOSInt imgW;
	UOSInt imgH;
	UOSInt ofst;
	UOSInt i;
	UOSInt j;
	Double lRate;
	Double rRate;
	Double tRate;
	Double bRate;
	Double c;
	
	UOSInt minX = rect[3].x;
	UOSInt minY = rect[3].y;
	UOSInt maxX = minX;
	UOSInt maxY = minY;
	i = 3;
	while (i-- > 0)
	{
		if (rect[i].x < minX) minX = rect[i].x;
		if (rect[i].x > maxX) maxX = rect[i].x;
		if (rect[i].y < minY) minY = rect[i].y;
		if (rect[i].y > maxY) maxY = rect[i].y;
	}

	UOSInt diff;
	UOSInt tldiff = rect[3].x - minX + rect[3].y - minY;
	UOSInt tlIndex = 3;
	i = 3;
	while (i-- > 0)
	{
		diff = rect[i].x - minX + rect[i].y - minY;
		if (diff < tldiff)
		{
			tldiff = diff;
			tlIndex = i;
		}
	}
	switch (tlIndex)
	{
	case 0:
		pt1x = UOSInt2Double(rect[0].x);
		pt1y = UOSInt2Double(rect[0].y);
		pt3x = UOSInt2Double(rect[2].x);
		pt3y = UOSInt2Double(rect[2].y);
		if (rect[1].x > rect[3].x)
		{
			pt2x = UOSInt2Double(rect[1].x);
			pt2y = UOSInt2Double(rect[1].y);
			pt4x = UOSInt2Double(rect[3].x);
			pt4y = UOSInt2Double(rect[3].y);
		}
		else
		{
			pt2x = UOSInt2Double(rect[3].x);
			pt2y = UOSInt2Double(rect[3].y);
			pt4x = UOSInt2Double(rect[1].x);
			pt4y = UOSInt2Double(rect[1].y);
		}
		break;
	case 1:
		pt1x = UOSInt2Double(rect[1].x);
		pt1y = UOSInt2Double(rect[1].y);
		pt3x = UOSInt2Double(rect[3].x);
		pt3y = UOSInt2Double(rect[3].y);
		if (rect[0].x > rect[2].x)
		{
			pt2x = UOSInt2Double(rect[0].x);
			pt2y = UOSInt2Double(rect[0].y);
			pt4x = UOSInt2Double(rect[2].x);
			pt4y = UOSInt2Double(rect[2].y);
		}
		else
		{
			pt2x = UOSInt2Double(rect[2].x);
			pt2y = UOSInt2Double(rect[2].y);
			pt4x = UOSInt2Double(rect[0].x);
			pt4y = UOSInt2Double(rect[0].y);
		}
		break;
	case 2:
		pt1x = UOSInt2Double(rect[2].x);
		pt1y = UOSInt2Double(rect[2].y);
		pt3x = UOSInt2Double(rect[0].x);
		pt3y = UOSInt2Double(rect[0].y);
		if (rect[1].x > rect[3].x)
		{
			pt2x = UOSInt2Double(rect[1].x);
			pt2y = UOSInt2Double(rect[1].y);
			pt4x = UOSInt2Double(rect[3].x);
			pt4y = UOSInt2Double(rect[3].y);
		}
		else
		{
			pt2x = UOSInt2Double(rect[3].x);
			pt2y = UOSInt2Double(rect[3].y);
			pt4x = UOSInt2Double(rect[1].x);
			pt4y = UOSInt2Double(rect[1].y);
		}
		break;
	default:
		pt1x = UOSInt2Double(rect[3].x);
		pt1y = UOSInt2Double(rect[3].y);
		pt3x = UOSInt2Double(rect[1].x);
		pt3y = UOSInt2Double(rect[1].y);
		if (rect[0].x > rect[2].x)
		{
			pt2x = UOSInt2Double(rect[0].x);
			pt2y = UOSInt2Double(rect[0].y);
			pt4x = UOSInt2Double(rect[2].x);
			pt4y = UOSInt2Double(rect[2].y);
		}
		else
		{
			pt2x = UOSInt2Double(rect[2].x);
			pt2y = UOSInt2Double(rect[2].y);
			pt4x = UOSInt2Double(rect[0].x);
			pt4y = UOSInt2Double(rect[0].y);
		}
		break;
	}

	if (psize == Media::OpenCV::OCVNumPlateFinder::PlateSize::SingleRow)
	{
		imgW = 150;
		imgH = 48;
	}
	else
	{
		imgW = 150;
		imgW = 75;
	}
	Media::ColorProfile color(Media::ColorProfile::CPT_SRGB);
	NEW_CLASS(ret, Media::StaticImage(imgW, imgH, 0, 8, Media::PF_PAL_W8, imgW * imgH, &color, Media::ColorProfile::YUVT_SMPTE170M, Media::AT_NO_ALPHA, Media::YCOFST_C_CENTER_LEFT));
	ret->InitGrayPal();
	xPos = UOSInt2Double(imgW - 1);
	yPos = UOSInt2Double(imgH - 1);
	UInt8 *dptr = ret->data;
	i = 0;
	while (i < imgH)
	{
		yRate = UOSInt2Double(i) / yPos;

		j = 0;
		while (j < imgW)
		{
			xRate = UOSInt2Double(j) / xPos;
			if (xRate + yRate <= 1)
			{
				x = pt1x + (pt2x - pt1x) * xRate + (pt4x - pt1x) * yRate;
				y = pt1y + (pt2y - pt1y) * xRate + (pt4y - pt1y) * yRate;
			}
			else
			{
				lRate = 1 - xRate;
				rRate = 1 - yRate;
				x = pt3x + (pt2x - pt3x) * rRate + (pt4x - pt3x) * lRate;
				y = pt3y + (pt2y - pt3y) * rRate + (pt4y - pt3y) * lRate;
			}
			ix = (UOSInt)x;
			iy = (UOSInt)y;
			ofst = iy * sbpl + ix;
			rRate = x - UOSInt2Double(ix);
			lRate = 1 - rRate;
			bRate = y - UOSInt2Double(iy);
			tRate = 1 - bRate;
			if (ix + 1 < swidth)
			{
				if (iy + 1 < sheight)
				{
					c = (sptr[ofst] * lRate + sptr[ofst + 1] * rRate) * tRate;
					c += (sptr[ofst + sbpl] * lRate + sptr[ofst + sbpl + 1] * rRate) * bRate;
				}
				else
				{
					c = (sptr[ofst] * lRate + sptr[ofst + 1] * rRate);
				}
			}
			else
			{
				if (iy + 1 < sheight)
				{
					c = (sptr[ofst] * tRate + sptr[ofst + sbpl] * bRate);
				}
				else
				{
					c = sptr[ofst];
				}
			}
			*dptr = (UInt8)(UInt32)(c + 0.5);
			dptr++;
			j++;
		}
		i++;
	}
	return ret;
}

Media::ANPR::ANPR() : ocr(Media::OCREngine::Language::English)
{
	this->ocr.SetCharWhiteList("0123456789ABCDEFGHJKLMNPQRSTUVWXYZ");
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
/*	UOSInt imgLeft = 321;
	UOSInt imgTop = 104;
	UOSInt imgWidth = 153;
	UOSInt imgHeight = 47;
	Text::String *s = ocr.ParseInsideImage(imgLeft, imgTop, imgWidth, imgHeight);
	if (s)
	{
		printf("OCR result: %s\r\n", s->v);
		s->Release();
	}
	else
	{
		printf("OCR parsing error\r\n");
	}*/	
	return found;
}
