#include "Stdafx.h"
#include "MyMemory.h"
#include "Media/ImageFilter/BGImgFilter.h"

Media::ImageFilter::BGImgFilter::BGImgFilter()
{
	this->isEnable = false;
	this->isAbsolute = false;
	this->toCreateBG = false;

	this->bgFormat = 0;
	this->bgData = 0;
	this->bgWidth = 0;
	this->bgHeight = 0;
}

Media::ImageFilter::BGImgFilter::~BGImgFilter()
{
	if (this->bgData)
	{
		MemFreeA(this->bgData);
		this->bgData = 0;
	}
}

void Media::ImageFilter::BGImgFilter::ProcessImage(UInt8 *imgData, UInt32 imgFormat, UOSInt nBits, Media::PixelFormat pf, UOSInt imgWidth, UOSInt imgHeight, Media::FrameType frameType, Media::YCOffset ycOfst)
{
	UOSInt imgSize;
	if (this->toCreateBG)
	{
		this->toCreateBG = false;
		if (this->bgData)
		{
			MemFreeA(this->bgData);
			this->bgData = 0;
		}
		imgSize = 0;

		if (imgFormat == *(UInt32*)"NV12")
		{
			imgSize = (imgWidth * imgHeight) + (imgWidth >> 1) * (imgHeight >> 1) * 2;
		}
		else if (imgFormat == *(UInt32*)"YV12")
		{
			imgSize = (imgWidth * imgHeight) + (imgWidth >> 1) * (imgHeight >> 1) * 2;
		}
		else if (imgFormat == *(UInt32*)"I420")
		{
			imgSize = (imgWidth * imgHeight) + (imgWidth >> 1) * (imgHeight >> 1) * 2;
		}
		else if (imgFormat == *(UInt32*)"YVU9")
		{
			imgSize = (imgWidth * imgHeight) + (imgWidth >> 2) * (imgHeight >> 2) * 2;
		}
		else if (imgFormat == *(UInt32*)"YUY2")
		{
			imgSize = (imgWidth * imgHeight) * 2;
		}
		else if (imgFormat == *(UInt32*)"UYVY" || imgFormat == *(UInt32*)"Y422" || imgFormat == *(UInt32*)"UYNV" || imgFormat == *(UInt32*)"HDYC")
		{
			imgSize = (imgWidth * imgHeight) * 2;
		}
		else if (imgFormat == *(UInt32*)"AYUV")
		{
			imgSize = (imgWidth * imgHeight) * 4;
		}
		else if (imgFormat == *(UInt32*)"P010")
		{
			imgSize = ((imgWidth * imgHeight) + (imgWidth >> 1) * (imgHeight >> 1) * 2) * 2;
		}
		else if (imgFormat == *(UInt32*)"P016")
		{
			imgSize = ((imgWidth * imgHeight) + (imgWidth >> 1) * (imgHeight >> 1) * 2) * 2;
		}
		else if (imgFormat == *(UInt32*)"P210")
		{
			imgSize = ((imgWidth * imgHeight) + (imgWidth >> 1) * imgHeight * 2) * 2;
		}
		else if (imgFormat == *(UInt32*)"P216")
		{
			imgSize = ((imgWidth * imgHeight) + (imgWidth >> 1) * imgHeight * 2) * 2;
		}
		else if (imgFormat == *(UInt32*)"Y416")
		{
			imgSize = (imgWidth * imgHeight) * 8;
		}
		if (imgSize != 0)
		{
			this->bgData = MemAllocA(UInt8, imgSize);
			MemCopyANC(this->bgData, imgData, imgSize);
			this->bgFormat = imgFormat;
			this->bgWidth = imgWidth;
			this->bgHeight = imgHeight;
		}
	}
	if (!this->isEnable)
		return;

	if (this->bgData == 0 || this->bgFormat != imgFormat || this->bgWidth != imgWidth || this->bgHeight != imgHeight)
	{
		return;
	}

	if (imgFormat == *(UInt32*)"DIBS")
	{
	}
	else if (imgFormat == *(UInt32*)"NV12")
	{
		if (this->isAbsolute)
		{
		}
		else
		{
			BGImgFilter_DiffYUV8(imgData, this->bgData, imgWidth * imgHeight + (imgWidth >> 1) * (imgHeight >> 1) * 2);
		}
	}
	else if (imgFormat == *(UInt32*)"YV12")
	{
		if (this->isAbsolute)
		{
		}
		else
		{
			BGImgFilter_DiffYUV8(imgData, this->bgData, imgWidth * imgHeight + (imgWidth >> 1) * (imgHeight >> 1) * 2);
		}
	}
	else if (imgFormat == *(UInt32*)"I420")
	{
		if (this->isAbsolute)
		{
		}
		else
		{
			BGImgFilter_DiffYUV8(imgData, this->bgData, imgWidth * imgHeight + (imgWidth >> 1) * (imgHeight >> 1) * 2);
		}
	}
	else if (imgFormat == *(UInt32*)"YVU9")
	{
		if (this->isAbsolute)
		{
		}
		else
		{
			BGImgFilter_DiffYUV8(imgData, this->bgData, imgWidth * imgHeight + (imgWidth >> 2) * (imgHeight >> 2) * 2);
		}
	}
	else if (imgFormat == *(UInt32*)"YUY2")
	{
		if (this->isAbsolute)
		{
			BGImgFilter_DiffA8_YUY2(imgData, this->bgData, imgWidth * imgHeight);
		}
		else
		{
			BGImgFilter_DiffYUV8(imgData, this->bgData, imgWidth * imgHeight * 2);
		}
	}
	else if (imgFormat == *(UInt32*)"UYVY" || imgFormat == *(UInt32*)"Y422" || imgFormat == *(UInt32*)"UYNV" || imgFormat == *(UInt32*)"HDYC")
	{
		if (this->isAbsolute)
		{
			//BGImgFilter_DiffA8_UYVY(imgData, this->bgData, imgWidth * imgHeight);
		}
		else
		{
			BGImgFilter_DiffYUV8(imgData, this->bgData, imgWidth * imgHeight * 2);
		}
	}
	else if (imgFormat == *(UInt32*)"AYUV")
	{
		if (this->isAbsolute)
		{
			//BGImgFilter_DiffA8_AYUV(imgData, this->bgData, imgWidth * imgHeight);
		}
		else
		{
			BGImgFilter_DiffYUV8(imgData, this->bgData, imgWidth * imgHeight * 4);
		}
	}
	else if (imgFormat == *(UInt32*)"P010")
	{
	}
	else if (imgFormat == *(UInt32*)"P016")
	{
	}
	else if (imgFormat == *(UInt32*)"P210")
	{
	}
	else if (imgFormat == *(UInt32*)"P216")
	{
	}
	else if (imgFormat == *(UInt32*)"Y416")
	{
	}
}

Bool Media::ImageFilter::BGImgFilter::IsEnabled()
{
	return this->isEnable;
}

Bool Media::ImageFilter::BGImgFilter::IsAbsolute()
{
	return this->isAbsolute;
}

void Media::ImageFilter::BGImgFilter::SetEnabled(Bool enable)
{
	this->isEnable = enable;
}

void Media::ImageFilter::BGImgFilter::SetAbsolute(Bool absolute)
{
	this->isAbsolute = absolute;
}

void Media::ImageFilter::BGImgFilter::ToCreateBGImg()
{
	this->toCreateBG = true;
}
