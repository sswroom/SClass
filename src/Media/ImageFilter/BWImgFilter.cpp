#include "Stdafx.h"
#include "MyMemory.h"
#include "Media/ImageFilter/BWImgFilter.h"

Media::ImageFilter::BWImgFilter::BWImgFilter(Bool enable)
{
	this->isEnable = enable;
}

Media::ImageFilter::BWImgFilter::~BWImgFilter()
{
}

void Media::ImageFilter::BWImgFilter::ProcessImage(UInt8 *imgData, UInt32 imgFormat, UOSInt nBits, Media::PixelFormat pf, UOSInt imgWidth, UOSInt imgHeight, Media::FrameType frameType, Media::YCOffset ycOfst)
{
	if (!this->isEnable)
		return;

	if (imgFormat == *(Int32*)"DIBS")
	{
	}
	else if (imgFormat == *(Int32*)"NV12")
	{
		MemFillB(imgData + (imgWidth * imgHeight), imgWidth * imgHeight >> 1, 0x80);
	}
	else if (imgFormat == *(Int32*)"YV12")
	{
		MemFillB(imgData + (imgWidth * imgHeight), imgWidth * imgHeight >> 1, 0x80);
	}
	else if (imgFormat == *(Int32*)"I420")
	{
		MemFillB(imgData + (imgWidth * imgHeight), imgWidth * imgHeight >> 1, 0x80);
	}
	else if (imgFormat == *(Int32*)"YVU9")
	{
		MemFillB(imgData + (imgWidth * imgHeight), imgWidth * imgHeight >> 3, 0x80);
	}
	else if (imgFormat == *(Int32*)"YUY2")
	{
		BWImgFilter_ProcYUY2(imgData, imgWidth * imgHeight);
	}
	else if (imgFormat == *(Int32*)"UYVY" || imgFormat == *(Int32*)"Y422" || imgFormat == *(Int32*)"UYNV" || imgFormat == *(Int32*)"HDYC")
	{
		BWImgFilter_ProcUYVY(imgData, imgWidth * imgHeight);
	}
	else if (imgFormat == *(Int32*)"AYUV")
	{
		BWImgFilter_ProcAYUV(imgData, imgWidth * imgHeight);
	}
	else if (imgFormat == *(Int32*)"P010")
	{
		MemFillW(imgData + (imgWidth * imgHeight * 2), imgWidth * imgHeight >> 1, 0x8000);
	}
	else if (imgFormat == *(Int32*)"P210")
	{
		MemFillW(imgData + (imgWidth * imgHeight * 2), imgWidth * imgHeight, 0x8000);
	}
	else if (imgFormat == *(Int32*)"P016")
	{
		MemFillW(imgData + (imgWidth * imgHeight * 2), imgWidth * imgHeight >> 1, 0x8000);
	}
	else if (imgFormat == *(Int32*)"P216")
	{
		MemFillW(imgData + (imgWidth * imgHeight * 2), imgWidth * imgHeight, 0x8000);
	}
	else if (imgFormat == *(Int32*)"Y416")
	{
		BWImgFilter_ProcY416(imgData, imgWidth * imgHeight);
	}
}

Bool Media::ImageFilter::BWImgFilter::IsEnabled()
{
	return isEnable;
}

void Media::ImageFilter::BWImgFilter::SetEnabled(Bool enable)
{
	this->isEnable = enable;
}
