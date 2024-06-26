#include "Stdafx.h"
#include "MyMemory.h"
#include "Media/VideoFilter/UVOffsetFilter.h"

void Media::VideoFilter::UVOffsetFilter::ProcessVideoFrame(Data::Duration frameTime, UInt32 frameNum, UnsafeArray<UnsafeArray<UInt8>> imgData, UOSInt dataSize, Media::IVideoSource::FrameStruct frameStruct, AnyType userData, Media::FrameType frameType, Media::IVideoSource::FrameFlag flags, Media::YCOffset ycOfst)
{
	if (this->videoInfo.fourcc == *(UInt32*)"YV12")
	{
		Int32 uOfst = this->uOfst;
		Int32 vOfst = this->vOfst;
		UOSInt w = this->videoInfo.storeSize.x;
		UOSInt h = this->videoInfo.storeSize.y;
		UOSInt hh = h >> 1;
		UOSInt wh = w >> 1;
		UnsafeArray<UInt8> imgPtr = imgData[0] + w * h;
		if (vOfst > 0)
		{
			UOSInt moveSize = wh - (UOSInt)(OSInt)vOfst;
			UOSInt hLeft = hh;
			UOSInt wLeft;
			UInt8 v;
			while (hLeft-- > 0)
			{
				MemCopyO(imgPtr.Ptr() + vOfst, imgPtr.Ptr(), moveSize);
				v = *imgPtr;
				wLeft = (UOSInt)vOfst;
				while (wLeft-- > 1)
				{
					imgPtr[wLeft] = v;
				}
				imgPtr += wh;
			}
		}
		else if (vOfst < 0)
		{
			UOSInt moveSize = wh + (UOSInt)(OSInt)vOfst;
			UOSInt hLeft = hh;
			UOSInt wLeft;
			UInt8 v;
			while (hLeft-- > 0)
			{
				MemCopyO(imgPtr.Ptr(), imgPtr.Ptr() - vOfst, moveSize);
				imgPtr += wh;
				v = imgPtr[-1];
				wLeft = (UOSInt)-vOfst;
				while (wLeft-- > 1)
				{
					imgPtr[-1 - (OSInt)wLeft] = v;
				}
			}
		}
		else
		{
			imgPtr = imgPtr + wh * hh;
		}
		if (uOfst > 0)
		{
			UOSInt moveSize = wh - (UOSInt)(OSInt)uOfst;
			UOSInt hLeft = hh;
			UOSInt wLeft;
			UInt8 v;
			while (hLeft-- > 0)
			{
				MemCopyO(imgPtr.Ptr() + uOfst, imgPtr.Ptr(), moveSize);
				v = *imgPtr;
				wLeft = (UOSInt)uOfst;
				while (wLeft-- > 1)
				{
					imgPtr[wLeft] = v;
				}
				imgPtr += wh;
			}
		}
		else if (uOfst < 0)
		{
			UOSInt moveSize = wh + (UOSInt)(OSInt)uOfst;
			UOSInt hLeft = hh;
			UOSInt wLeft;
			UInt8 v;
			while (hLeft-- > 0)
			{
				MemCopyO(imgPtr.Ptr(), imgPtr.Ptr() - uOfst, moveSize);
				imgPtr += wh;
				v = imgPtr[-1];
				wLeft = (UOSInt)-uOfst;
				while (wLeft-- > 1)
				{
					imgPtr[-1 - (OSInt)wLeft] = v;
				}
			}
		}
	}
	if (this->videoCb)
	{
		this->videoCb(frameTime, frameNum, imgData, dataSize, frameStruct, this->userData, frameType, flags, ycOfst);
	}
}

Media::VideoFilter::UVOffsetFilter::UVOffsetFilter(Media::IVideoSource *srcVideo) : Media::VideoFilter::VideoFilterBase(srcVideo)
{
	this->uOfst = 0;
	this->vOfst = 0;
}

Media::VideoFilter::UVOffsetFilter::~UVOffsetFilter()
{
}

Text::CStringNN Media::VideoFilter::UVOffsetFilter::GetFilterName()
{
	return CSTR("UVOffsetFilter");
}

void Media::VideoFilter::UVOffsetFilter::SetOffset(Int32 uOfst, Int32 vOfst)
{
	this->uOfst = uOfst;
	this->vOfst = vOfst;
}
