#include "Stdafx.h"
#include "MyMemory.h"
#include "Media/CS/CSYUV_Y8.h"

Media::CS::CSYUV_Y8::CSYUV_Y8(UInt32 srcFmt) : Media::CS::CSConverter(0)
{
	this->srcFmt = srcFmt;
}

Media::CS::CSYUV_Y8::~CSYUV_Y8()
{
}

void Media::CS::CSYUV_Y8::ConvertV2(UInt8 **srcPtr, UInt8 *destPtr, UOSInt dispWidth, UOSInt dispHeight, UOSInt srcStoreWidth, UOSInt srcStoreHeight, OSInt destRGBBpl, Media::FrameType ftype, Media::YCOffset ycOfst)
{
	if (this->srcFmt == *(UInt32*)"YUYV" || this->srcFmt == *(UInt32*)"YUY2")
	{
		UOSInt i = dispHeight;
		UOSInt j;
		OSInt sAdd = (srcStoreWidth - dispWidth) * 2;
		UInt8 *sptr = srcPtr[0];
		while (i-- > 0)
		{
			j = dispWidth;
			while (j-- > 0)
			{
				*destPtr++ = *sptr;
				sptr += 2;
			}
			sptr += sAdd;
		}
	}
	else if (this->srcFmt == *(UInt32*)"I420" ||
		this->srcFmt == FFMT_YUV420P8 ||
		this->srcFmt == *(UInt32*)"IYUV" ||
		this->srcFmt == *(UInt32*)"YV12" ||
		this->srcFmt == *(UInt32*)"YV16" ||
		this->srcFmt == *(UInt32*)"NV12" ||
		this->srcFmt == *(UInt32*)"NV21" ||
		this->srcFmt == *(UInt32*)"Y800" ||
		this->srcFmt == *(UInt32*)"Y800" ||
		this->srcFmt == *(UInt32*)"Y8  " ||
		this->srcFmt == *(UInt32*)"GREY")
	{
		if (dispWidth == srcStoreWidth)
		{
			MemCopyNO(destPtr, srcPtr[0], srcStoreWidth * dispHeight);
		}
		else
		{
			UInt8 *sptr = srcPtr[0];
			UOSInt i = dispHeight;
			while (i-- > 0)
			{
				MemCopyNO(destPtr, sptr, dispWidth);
				sptr += srcStoreWidth;
				destPtr += dispWidth;
			}
		}
	}
}

UOSInt Media::CS::CSYUV_Y8::GetSrcFrameSize(UOSInt width, UOSInt height)
{
	return 0;
}

UOSInt Media::CS::CSYUV_Y8::GetDestFrameSize(UOSInt width, UOSInt height)
{
	return width * height;
}

void Media::CS::CSYUV_Y8::YUVParamChanged(const Media::IColorHandler::YUVPARAM *yuvParam)
{

}

void Media::CS::CSYUV_Y8::RGBParamChanged(const Media::IColorHandler::RGBPARAM2 *rgbParam)
{

}

Bool Media::CS::CSYUV_Y8::IsSupported(UInt32 fmt)
{
	if (fmt == *(UInt32*)"YUYV")
	{
		return true;
	}
	else if (fmt == *(UInt32*)"YUY2")
	{
		return true;
	}
	else if (fmt == *(UInt32*)"I420")
	{
		return true;
	}
	else if (fmt == FFMT_YUV420P8)
	{
		return true;
	}
	else if (fmt == *(UInt32*)"Y800")
	{
		return true;
	}
	else if (fmt == *(UInt32*)"Y8  ")
	{
		return true;
	}
	else if (fmt == *(UInt32*)"GREY")
	{
		return true;
	}
	else if (fmt == *(UInt32*)"IYUV")
	{
		return true;
	}
	else if (fmt == *(UInt32*)"YV12")
	{
		return true;
	}
	else if (fmt == *(UInt32*)"YV16")
	{
		return true;
	}
	else if (fmt == *(UInt32*)"NV12")
	{
		return true;
	}
	else if (fmt == *(UInt32*)"NV21")
	{
		return true;
	}
	return false;
}