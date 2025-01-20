#include "Stdafx.h"
#include "MyMemory.h"
#include "Media/ImgFilter/BWImgFilter.h"

Media::ImgFilter::BWImgFilter::BWImgFilter(Bool enable)
{
	this->isEnable = enable;
}

Media::ImgFilter::BWImgFilter::~BWImgFilter()
{
}

void Media::ImgFilter::BWImgFilter::ProcessImage(UnsafeArray<UInt8> imgData, UInt32 imgFormat, UOSInt nBits, Media::PixelFormat pf, UOSInt imgWidth, UOSInt imgHeight, Media::FrameType frameType, Media::YCOffset ycOfst)
{
	if (!this->isEnable)
		return;

	if (imgFormat == *(UInt32*)"DIBS")
	{
	}
	else if (imgFormat == *(UInt32*)"NV12")
	{
		MemFillB(imgData.Ptr() + (imgWidth * imgHeight), imgWidth * imgHeight >> 1, 0x80);
	}
	else if (imgFormat == *(UInt32*)"YV12")
	{
		MemFillB(imgData.Ptr() + (imgWidth * imgHeight), imgWidth * imgHeight >> 1, 0x80);
	}
	else if (imgFormat == *(UInt32*)"I420")
	{
		MemFillB(imgData.Ptr() + (imgWidth * imgHeight), imgWidth * imgHeight >> 1, 0x80);
	}
	else if (imgFormat == *(UInt32*)"YVU9")
	{
		MemFillB(imgData.Ptr() + (imgWidth * imgHeight), imgWidth * imgHeight >> 3, 0x80);
	}
	else if (imgFormat == *(UInt32*)"YUY2")
	{
		BWImgFilter_ProcYUY2(imgData.Ptr(), imgWidth * imgHeight);
	}
	else if (imgFormat == *(UInt32*)"UYVY" || imgFormat == *(UInt32*)"Y422" || imgFormat == *(UInt32*)"UYNV" || imgFormat == *(UInt32*)"HDYC")
	{
		BWImgFilter_ProcUYVY(imgData.Ptr(), imgWidth * imgHeight);
	}
	else if (imgFormat == *(UInt32*)"AYUV")
	{
		BWImgFilter_ProcAYUV(imgData.Ptr(), imgWidth * imgHeight);
	}
	else if (imgFormat == *(UInt32*)"P010")
	{
		MemFillW(imgData.Ptr() + (imgWidth * imgHeight * 2), imgWidth * imgHeight >> 1, 0x8000);
	}
	else if (imgFormat == *(UInt32*)"P210")
	{
		MemFillW(imgData.Ptr() + (imgWidth * imgHeight * 2), imgWidth * imgHeight, 0x8000);
	}
	else if (imgFormat == *(UInt32*)"P016")
	{
		MemFillW(imgData.Ptr() + (imgWidth * imgHeight * 2), imgWidth * imgHeight >> 1, 0x8000);
	}
	else if (imgFormat == *(UInt32*)"P216")
	{
		MemFillW(imgData.Ptr() + (imgWidth * imgHeight * 2), imgWidth * imgHeight, 0x8000);
	}
	else if (imgFormat == *(UInt32*)"Y416")
	{
		BWImgFilter_ProcY416(imgData.Ptr(), imgWidth * imgHeight);
	}
}

Bool Media::ImgFilter::BWImgFilter::IsEnabled()
{
	return isEnable;
}

void Media::ImgFilter::BWImgFilter::SetEnabled(Bool enable)
{
	this->isEnable = enable;
}
