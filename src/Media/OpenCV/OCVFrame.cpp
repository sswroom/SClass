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
	*cimg = (*fr)(cv::Range((int)area->min.y, (int)area->max.y), cv::Range((int)area->min.x, (int)area->max.x)).clone();
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

UOSInt Media::OpenCV::OCVFrame::GetWidth() const
{
	cv::Mat *fr = (cv::Mat *)this->frame;
	return (UOSInt)fr->cols;
}

UOSInt Media::OpenCV::OCVFrame::GetHeight() const
{
	cv::Mat *fr = (cv::Mat *)this->frame;
	return (UOSInt)fr->rows;
}

Math::Size2D<UOSInt> Media::OpenCV::OCVFrame::GetSize() const
{
	cv::Mat *fr = (cv::Mat *)this->frame;
	return Math::Size2D<UOSInt>((UOSInt)fr->cols, (UOSInt)fr->rows);
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

void Media::OpenCV::OCVFrame::GetImageData(UnsafeArray<UInt8> destBuff, OSInt left, OSInt top, UOSInt width, UOSInt height, UOSInt destBpl, Bool upsideDown, Media::RotateType destRotate)
{
	cv::Mat *fr = (cv::Mat *)this->frame;
	OSInt srcW = fr->cols;
	Media::ImageUtil::ImageCopyR(destBuff, (OSInt)destBpl, fr->data, srcW, left, top, width, height, ((UOSInt)srcW << 3) / width, upsideDown, Media::RotateType::None, destRotate);
}

Media::StaticImage *Media::OpenCV::OCVFrame::CreateStaticImage()
{
	Media::ColorProfile sRGB(Media::ColorProfile::CPT_SRGB);
	Media::StaticImage *simg;
	Math::Size2D<UOSInt> size = this->GetSize();
	NEW_CLASS(simg, Media::StaticImage(size, 0, 8, Media::PF_PAL_W8, size.CalcArea(), sRGB, Media::ColorProfile::YUVT_BT601, Media::AT_NO_ALPHA, Media::YCOFST_C_CENTER_LEFT));
	simg->InitGrayPal();
	this->GetImageData(simg->data, 0, 0, size.x, size.y, simg->GetDataBpl(), false, Media::RotateType::None);
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

NN<Media::OpenCV::OCVFrame> Media::OpenCV::OCVFrame::BilateralFilter(Int32 d, Double sigmaColor, Double sigmaSpace)
{
	cv::Mat *newFr = new cv::Mat();
	cv::bilateralFilter(*(cv::Mat*)this->frame, *newFr, d, sigmaColor, sigmaSpace);
	NN<Media::OpenCV::OCVFrame> ret;
	NEW_CLASSNN(ret, Media::OpenCV::OCVFrame(newFr));
	return ret;
}

Optional<Media::OpenCV::OCVFrame> Media::OpenCV::OCVFrame::CreateYFrame(UnsafeArray<UnsafeArray<UInt8>> imgData, UOSInt dataSize, UInt32 fourcc, Math::Size2D<UOSInt> dispSize, UOSInt storeWidth, UOSInt storeBPP, Media::PixelFormat pf)
{
	NN<Media::CS::CSConverter> converter;
	if (Media::CS::CSConverter::NewConverter(fourcc, storeBPP, pf, Media::ColorProfile(), *(UInt32*)"Y800", 8, Media::PF_UNKNOWN, Media::ColorProfile(), Media::ColorProfile::YUVT_UNKNOWN, 0).SetTo(converter))
	{
		cv::Mat *fr;
		fr = new cv::Mat((int)dispSize.y, (int)dispSize.x, CV_8UC1);
		converter->ConvertV2(imgData, fr->ptr(0), dispSize.x, dispSize.y, storeWidth, dispSize.y, (OSInt)dispSize.x, Media::FT_NON_INTERLACE, Media::YCOFST_C_CENTER_LEFT);

		converter.Delete();
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

Optional<Media::OpenCV::OCVFrame> Media::OpenCV::OCVFrame::CreateYFrame(NN<Media::StaticImage> simg)
{
	cv::Mat *fr;
	if (!simg->ToW8())
	{
		return 0;
	}
	fr = new cv::Mat((int)simg->info.dispSize.y, (int)simg->info.dispSize.x, CV_8UC1);
	simg->GetRasterData(fr->ptr(0), 0, 0, simg->info.dispSize.x, simg->info.dispSize.y, simg->info.dispSize.x, false, Media::RotateType::None);

	NN<Media::OpenCV::OCVFrame> frame;
	NEW_CLASSNN(frame, Media::OpenCV::OCVFrame(fr));
	return frame;
}
