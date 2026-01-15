#include "Stdafx.h"
#include "MyMemory.h"
#include "Media/ImgFilter/BGImgFilter.h"

Media::ImgFilter::BGImgFilter::BGImgFilter()
{
	this->isEnable = false;
	this->isAbsolute = false;
	this->toCreateBG = false;

	this->bgFormat = 0;
	this->bgData = nullptr;
	this->bgWidth = 0;
	this->bgHeight = 0;
}

Media::ImgFilter::BGImgFilter::~BGImgFilter()
{
	UnsafeArray<UInt8> bgData;
	if (this->bgData.SetTo(bgData))
	{
		MemFreeAArr(bgData);
		this->bgData = nullptr;
	}
}

void Media::ImgFilter::BGImgFilter::ProcessImage(UnsafeArray<UInt8> imgData, UInt32 imgFormat, UIntOS nBits, Media::PixelFormat pf, UIntOS imgWidth, UIntOS imgHeight, Media::FrameType frameType, Media::YCOffset ycOfst)
{
	UIntOS imgSize;
	UnsafeArray<UInt8> bgData;
	if (this->toCreateBG)
	{
		this->toCreateBG = false;
		if (this->bgData.SetTo(bgData))
		{
			MemFreeAArr(bgData);
			this->bgData = nullptr;
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
			this->bgData = bgData = MemAllocAArr(UInt8, imgSize);
			MemCopyANC(bgData.Ptr(), imgData.Ptr(), imgSize);
			this->bgFormat = imgFormat;
			this->bgWidth = imgWidth;
			this->bgHeight = imgHeight;
		}
	}
	if (!this->isEnable)
		return;

	if (!this->bgData.SetTo(bgData) || this->bgFormat != imgFormat || this->bgWidth != imgWidth || this->bgHeight != imgHeight)
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
			BGImgFilter_DiffYUV8(imgData.Ptr(), bgData.Ptr(), imgWidth * imgHeight + (imgWidth >> 1) * (imgHeight >> 1) * 2);
		}
	}
	else if (imgFormat == *(UInt32*)"YV12")
	{
		if (this->isAbsolute)
		{
		}
		else
		{
			BGImgFilter_DiffYUV8(imgData.Ptr(), bgData.Ptr(), imgWidth * imgHeight + (imgWidth >> 1) * (imgHeight >> 1) * 2);
		}
	}
	else if (imgFormat == *(UInt32*)"I420")
	{
		if (this->isAbsolute)
		{
		}
		else
		{
			BGImgFilter_DiffYUV8(imgData.Ptr(), bgData.Ptr(), imgWidth * imgHeight + (imgWidth >> 1) * (imgHeight >> 1) * 2);
		}
	}
	else if (imgFormat == *(UInt32*)"YVU9")
	{
		if (this->isAbsolute)
		{
		}
		else
		{
			BGImgFilter_DiffYUV8(imgData.Ptr(), bgData.Ptr(), imgWidth * imgHeight + (imgWidth >> 2) * (imgHeight >> 2) * 2);
		}
	}
	else if (imgFormat == *(UInt32*)"YUY2")
	{
		if (this->isAbsolute)
		{
			BGImgFilter_DiffA8_YUY2(imgData.Ptr(), bgData.Ptr(), imgWidth * imgHeight);
		}
		else
		{
			BGImgFilter_DiffYUV8(imgData.Ptr(), bgData.Ptr(), imgWidth * imgHeight * 2);
		}
	}
	else if (imgFormat == *(UInt32*)"UYVY" || imgFormat == *(UInt32*)"Y422" || imgFormat == *(UInt32*)"UYNV" || imgFormat == *(UInt32*)"HDYC")
	{
		if (this->isAbsolute)
		{
			//BGImgFilter_DiffA8_UYVY(imgData, bgData.Ptr(), imgWidth * imgHeight);
		}
		else
		{
			BGImgFilter_DiffYUV8(imgData.Ptr(), bgData.Ptr(), imgWidth * imgHeight * 2);
		}
	}
	else if (imgFormat == *(UInt32*)"AYUV")
	{
		if (this->isAbsolute)
		{
			//BGImgFilter_DiffA8_AYUV(imgData, bgData.Ptr(), imgWidth * imgHeight);
		}
		else
		{
			BGImgFilter_DiffYUV8(imgData.Ptr(), bgData.Ptr(), imgWidth * imgHeight * 4);
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

Bool Media::ImgFilter::BGImgFilter::IsEnabled()
{
	return this->isEnable;
}

Bool Media::ImgFilter::BGImgFilter::IsAbsolute()
{
	return this->isAbsolute;
}

void Media::ImgFilter::BGImgFilter::SetEnabled(Bool enable)
{
	this->isEnable = enable;
}

void Media::ImgFilter::BGImgFilter::SetAbsolute(Bool absolute)
{
	this->isAbsolute = absolute;
}

void Media::ImgFilter::BGImgFilter::ToCreateBGImg()
{
	this->toCreateBG = true;
}
