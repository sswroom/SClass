#include "Stdafx.h"
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

void Media::ANPR::NumPlateArea(void *userObj, Media::OpenCV::OCVFrame *filteredFrame, UOSInt *rect)
{
	ParseStatus *status = (ParseStatus*)userObj;
	UTF8Char sbuff[32];
	UTF8Char *sptr;
	Math::RectArea<UOSInt> area;
	Math::RectArea<UOSInt>::GetRectArea(&area, rect, 4);
	UOSInt i = status->pastPos->GetCount();
	while (i > 0)
	{
		i -= 2;
		if (area.ContainPt(status->pastPos->GetItem(i), status->pastPos->GetItem(i + 1)))
		{
			return;
		}
	}
	Media::OpenCV::OCVFrame *croppedFrame = filteredFrame->CropToNew(&area);
	if (croppedFrame)
	{
		croppedFrame->Normalize();
		UOSInt cropRect[8];
		cropRect[0] = rect[0] - area.left;
		cropRect[1] = rect[1] - area.top;
		cropRect[2] = rect[2] - area.left;
		cropRect[3] = rect[3] - area.top;
		cropRect[4] = rect[4] - area.left;
		cropRect[5] = rect[5] - area.top;
		cropRect[6] = rect[6] - area.left;
		cropRect[7] = rect[7] - area.top;
		croppedFrame->ClearOutsidePolygon(cropRect, 4, 255);

		status->me->ocr.SetOCVFrame(croppedFrame);
		Text::String *s = status->me->ocr.ParseInsideImage(0, 0, area.width, area.height);
		if (s)
		{
			if (s->leng == 0)
			{
			}
			else
			{
				if (status->me->hdlr)
				{
					status->me->hdlr(status->me->hdlrObj, status->simg, &area, s);
				}
/*				printf("OCR result: %s\r\n", s->v);
				sptr = Text::StrConcatC(Text::StrUOSInt(Text::StrConcatC(sbuff, UTF8STRC("OCRRes")), status->me->parsedCnt), UTF8STRC(".png"));
				Media::ImageList imgList(CSTRP(sbuff, sptr));
				Media::StaticImage *simg = croppedFrame->CreateStaticImage();
				imgList.AddImage(simg, 0);
				simg->To32bpp();
				Exporter::PNGExporter exporter;
				exporter.ExportNewFile(imgList.GetSourceNameObj()->ToCString(), &imgList, 0);*/
				
				status->me->parsedCnt++;
				status->pastPos->Add((rect[0] + rect[2] + rect[4] + rect[6]) >> 2);
				status->pastPos->Add((rect[1] + rect[3] + rect[5] + rect[7]) >> 2);
			}
			s->Release();
		}
		else
		{
			printf("OCR parsing error\r\n");
		}
		DEL_CLASS(croppedFrame);
	}
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
		Media::OpenCV::OCVNumPlateFinder::Find(frame, NumPlateArea, &status);
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
