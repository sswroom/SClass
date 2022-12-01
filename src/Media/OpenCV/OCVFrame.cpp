#include "Stdafx.h"
#include "MyMemory.h"
#include "Media/FrameInfo.h"
#include "Media/ImageCopyC.h"
#include "Media/ImageUtil.h"
#include "Media/CS/CSConverter.h"
#include "Media/OpenCV/OCVFrame.h"
#include "Media/OpenCV/OCVInit.h"

#include <opencv2/imgproc.hpp>
#include <stdio.h>

#include <opencv2/highgui.hpp>

Media::OpenCV::OCVFrame::OCVFrame(void *frame)
{
	this->frame = frame;
}

Media::OpenCV::OCVFrame::~OCVFrame()
{
	cv::Mat *fr = (cv::Mat *)this->frame;
	delete fr;
}

void *Media::OpenCV::OCVFrame::GetFrame()
{
	return this->frame;
}

Media::OpenCV::OCVFrame *Media::OpenCV::OCVFrame::CropToNew(Math::RectArea<UOSInt> *area)
{
	cv::Mat *fr = (cv::Mat *)this->frame;
	cv::Mat *cimg = new cv::Mat();
	*cimg = (*fr)(cv::Range((int)area->tl.y, (int)area->br.y), cv::Range((int)area->tl.x, (int)area->br.x)).clone();
	return NEW_CLASS_D(OCVFrame(cimg));
}

void Media::OpenCV::OCVFrame::ClearOutsidePolygon(Math::Coord2D<UOSInt> *poly, UOSInt nPoints, UInt8 color)
{
	cv::Mat *fr = (cv::Mat *)this->frame;
	int nPt = (int)nPoints;
	cv::Point2i *points = MemAlloc(cv::Point2i, nPoints);
	UOSInt i = nPoints;
	while (i-- > 0)
	{
		points[i].x = (int)poly[i].x;
		points[i].y = (int)poly[i].y;
	}
	cv::Mat mask = cv::Mat::zeros(fr->size(), fr->type());
	cv::fillPoly(mask, (const cv::Point**)&points, &nPt, 1, cv::Scalar(255));
//	cv::bitwise_and(*fr, mask, *fr);
	cv::bitwise_not(mask, mask);
	cv::bitwise_or(*fr, mask, *fr);
	MemFree(points);
}

UOSInt Media::OpenCV::OCVFrame::GetWidth()
{
	cv::Mat *fr = (cv::Mat *)this->frame;
	return (UOSInt)fr->cols;
}

UOSInt Media::OpenCV::OCVFrame::GetHeight()
{
	cv::Mat *fr = (cv::Mat *)this->frame;
	return (UOSInt)fr->rows;
}

OSInt Media::OpenCV::OCVFrame::GetBpl()
{
	cv::Mat *fr = (cv::Mat *)this->frame;
	return fr->cols;
}

UInt8 *Media::OpenCV::OCVFrame::GetDataPtr()
{
	cv::Mat *fr = (cv::Mat *)this->frame;
	return fr->data;
}

void Media::OpenCV::OCVFrame::GetImageData(UInt8 *destBuff, OSInt left, OSInt top, UOSInt width, UOSInt height, UOSInt destBpl, Bool upsideDown, Media::RotateType destRotate)
{
	cv::Mat *fr = (cv::Mat *)this->frame;
	OSInt srcW = fr->cols;
	Media::ImageUtil::ImageCopyR(destBuff, (OSInt)destBpl, fr->data, srcW, left, top, width, height, ((UOSInt)srcW << 3) / width, upsideDown, Media::RotateType::None, destRotate);
}

Media::StaticImage *Media::OpenCV::OCVFrame::CreateStaticImage()
{
	Media::ColorProfile sRGB(Media::ColorProfile::CPT_SRGB);
	Media::StaticImage *simg;
	UOSInt w = this->GetWidth();
	UOSInt h = this->GetHeight();
	NEW_CLASS(simg, Media::StaticImage(w, h, 0, 8, Media::PF_PAL_W8, w * h, &sRGB, Media::ColorProfile::YUVT_BT601, Media::AT_NO_ALPHA, Media::YCOFST_C_CENTER_LEFT));
	simg->InitGrayPal();
	this->GetImageData(simg->data, 0, 0, w, h, simg->GetDataBpl(), false, Media::RotateType::None);
	cv::imshow("Clear", *(cv::Mat *)this->frame);
	return simg;
}

void Media::OpenCV::OCVFrame::ToBlackAndWhite(UInt8 middleC)
{
	cv::Mat *fr = (cv::Mat *)this->frame;
	UOSInt w = this->GetWidth();
	UOSInt h = this->GetHeight();
	OSInt bpl = this->GetBpl();
	OSInt lineAdd = bpl - (OSInt)w;
	UInt8 *ptr = fr->data;
	UOSInt i = h;
	UOSInt j;
	while (i-- > 0)
	{
		j = w;
		while (j-- > 0)
		{
			if (*ptr > middleC)
			{
				*ptr = 255;
			}
			else
			{
				*ptr = 0;
			}
			ptr++;
		}
		ptr += lineAdd;
	}
}

void Media::OpenCV::OCVFrame::Normalize()
{
	cv::Mat *fr = (cv::Mat *)this->frame;
	cv::normalize(*fr, *fr, 1.0, 255.0, cv::NORM_MINMAX);
}

Media::OpenCV::OCVFrame *Media::OpenCV::OCVFrame::BilateralFilter(Int32 d, Double sigmaColor, Double sigmaSpace)
{
	cv::Mat *newFr = new cv::Mat();
	cv::bilateralFilter(*(cv::Mat*)this->frame, *newFr, d, sigmaColor, sigmaSpace);
	return NEW_CLASS_D(Media::OpenCV::OCVFrame(newFr));
}

Media::OpenCV::OCVFrame *Media::OpenCV::OCVFrame::CreateYFrame(UInt8 **imgData, UOSInt dataSize, UInt32 fourcc, UOSInt dispWidth, UOSInt dispHeight, UOSInt storeWidth, UOSInt storeBPP, Media::PixelFormat pf)
{
	Media::CS::CSConverter *converter = Media::CS::CSConverter::NewConverter(fourcc, storeBPP, pf, 0, *(UInt32*)"Y800", 8, Media::PF_UNKNOWN, 0, Media::ColorProfile::YUVT_UNKNOWN, 0);
	if (converter)
	{
		cv::Mat *fr;
		fr = new cv::Mat((int)dispHeight, (int)dispWidth, CV_8UC1);
		converter->ConvertV2(imgData, fr->ptr(0), dispWidth, dispHeight, storeWidth, dispHeight, (OSInt)dispWidth, Media::FT_NON_INTERLACE, Media::YCOFST_C_CENTER_LEFT);

		DEL_CLASS(converter);
		Media::OpenCV::OCVFrame *frame;
		NEW_CLASS(frame, Media::OpenCV::OCVFrame(fr));
		return frame;
	}
	else
	{
		if (fourcc < 1024)
		{
			printf("OCVFrame: Unsupported Format: %d\r\n", fourcc);
		}
		else
		{
			UTF8Char sbuff[5];
			*(UInt32*)sbuff = fourcc;
			sbuff[4] = 0;
			printf("OCVFrame: Unsupported Format: %s\r\n", sbuff);
		}
		
		return 0;		
	}
}

Media::OpenCV::OCVFrame *Media::OpenCV::OCVFrame::CreateYFrame(Media::StaticImage *simg)
{
	cv::Mat *fr;
	if (!simg->ToW8())
	{
		return 0;
	}
	fr = new cv::Mat((int)simg->info.dispHeight, (int)simg->info.dispWidth, CV_8UC1);
	simg->GetImageData(fr->ptr(0), 0, 0, simg->info.dispWidth, simg->info.dispHeight, simg->info.dispWidth, false, Media::RotateType::None);

	Media::OpenCV::OCVFrame *frame;
	NEW_CLASS(frame, Media::OpenCV::OCVFrame(fr));
	return frame;
}
