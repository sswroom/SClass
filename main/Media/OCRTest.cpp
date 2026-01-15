#include "Stdafx.h"
#include "Core/Core.h"
#include "Exporter/PNGExporter.h"
#include "IO/Path.h"
#include "IO/StmData/FileData.h"
#include "Media/ANPR.h"
#include "Media/ImageList.h"
#include "Media/OCREngine.h"
#include "Media/OpenCV/OCVNumPlateFinder.h"
#include "Parser/FullParserList.h"

#include <stdio.h>

/*struct ParseStatus
{
	Media::OCREngine *ocr;
	Media::OpenCV::OCVFrame *frame;
};

void PossibleArea(AnyType userObj, Media::OpenCV::OCVFrame *filteredFrame, UIntOS *rect)
{
	ParseStatus *status = (ParseStatus*)userObj;
	Math::RectArea<UIntOS> area;
	Math::RectArea<UIntOS>::GetRectArea(&area, rect, 4);
	Media::OpenCV::OCVFrame *croppedFrame = filteredFrame->CropToNew(&area);
	if (croppedFrame)
	{
		UIntOS cropRect[8];
		cropRect[0] = rect[0] - area.left;
		cropRect[1] = rect[1] - area.top;
		cropRect[2] = rect[2] - area.left;
		cropRect[3] = rect[3] - area.top;
		cropRect[4] = rect[4] - area.left;
		cropRect[5] = rect[5] - area.top;
		cropRect[6] = rect[6] - area.left;
		cropRect[7] = rect[7] - area.top;
		croppedFrame->ClearOutsidePolygon(cropRect, 4, 255);

//		croppedFrame->ToBlackAndWhite(160);
		status->ocr->SetOCVFrame(croppedFrame);
		Text::String *s = status->ocr->ParseInsideImage(0, 0, area.width, area.height);
		if (s)
		{
			if (s->leng == 0)
			{
				printf("OCR empty result\r\n");
			}
			else
			{
				printf("OCR result: %s\r\n", s->v);
				Media::ImageList imgList(CSTR("Temp.png"));
				Media::StaticImage *simg = croppedFrame->CreateStaticImage();
				imgList.AddImage(simg, 0);
				simg->To32bpp();
				Exporter::PNGExporter exporter;
				exporter.ExportNewFile(imgList.GetSourceNameObj()->ToCString(), &imgList, 0);
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

void TestFile(Text::CString imgPath, Parser::ParserList *parsers, Media::OCREngine *ocr)
{
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	sptr = IO::Path::GetRealPath(sbuff, imgPath.v, imgPath.leng);
	IO::StmData::FileData fd(CSTRP(sbuff, sptr), false);
	Media::ImageList *imgList = (Media::ImageList*)parsers->ParseFileType(&fd, IO::ParserType::ImageList);
	if (imgList)
	{
		printf("Testing for %s\r\n", imgPath.v);
		Media::StaticImage *simg = (Media::StaticImage*)imgList->GetImage(0, 0);
		simg->ToW8();
		ocr->SetParsingImage(simg);
		Media::OpenCV::OCVFrame *frame = Media::OpenCV::OCVFrame::CreateYFrame(simg);
		if (frame)
		{
			ParseStatus status;
			status.ocr = ocr;
			status.frame = frame;
			Media::OpenCV::OCVNumPlateFinder::Find(frame, PossibleArea, &status);
			DEL_CLASS(frame);
		}
		DEL_CLASS(imgList);
	}
}*/

void __stdcall OnNumberPlate(AnyType userObj, NN<Media::StaticImage> simg, Math::RectArea<UIntOS> area, NN<Text::String> result, Double maxTileAngle, Double pxArea, UIntOS confidence, NN<Media::StaticImage> plateImg)
{
	printf("Parsed Number Plate: %s\r\n", result->v.Ptr());
}

void TestFile2(Text::CStringNN imgPath, Parser::ParserList *parsers, Media::ANPR *apnr)
{
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	sptr = IO::Path::GetRealPath(sbuff, imgPath.v, imgPath.leng);
	IO::StmData::FileData fd(CSTRP(sbuff, sptr), false);
	NN<Media::ImageList> imgList;
	if (Optional<Media::ImageList>::ConvertFrom(parsers->ParseFileType(fd, IO::ParserType::ImageList)).SetTo(imgList))
	{
		NN<Media::StaticImage> img;
		if (Optional<Media::StaticImage>::ConvertFrom(imgList->GetImage(0, 0)).SetTo(img))
			apnr->ParseImage(img);
		imgList.Delete();
	}
}

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
	Parser::FullParserList parsers;
/*	Media::OCREngine ocr(Media::OCREngine::Language::English);
	ocr.SetCharWhiteList("0123456789ABCDEFGHJKLMNPQRSTUVWXYZ");*/
	Media::ANPR anpr;
	anpr.SetResultHandler(OnNumberPlate, 0);
	TestFile2(CSTR("~/Progs/Temp/OCR1.jpg"), &parsers, &anpr);
	TestFile2(CSTR("~/Progs/Temp/OCR2.jpg"), &parsers, &anpr);
	TestFile2(CSTR("~/Progs/Temp/OCR3.jpg"), &parsers, &anpr);
	return 0;
}
