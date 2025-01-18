#include "Stdafx.h"
#include "MyMemory.h"
#include "Media/VideoFilter/AutoCropFilter.h"

extern "C"
{
	void AutoCropFilter_CropCalc(UInt8 *yptr, UOSInt w, UOSInt h, UOSInt ySplit, UOSInt *crops);
}

void Media::VideoFilter::AutoCropFilter::ProcessVideoFrame(Data::Duration frameTime, UInt32 frameNum, UnsafeArray<UnsafeArray<UInt8>> imgData, UOSInt dataSize, Media::IVideoSource::FrameStruct frameStruct, AnyType userData, Media::FrameType frameType, Media::IVideoSource::FrameFlag flags, Media::YCOffset ycOfst)
{
	NN<Media::IVideoSource> srcVideo;
	if (this->enabled && this->srcVideo.SetTo(srcVideo) && frameStruct == Media::IVideoSource::FS_I)
	{
		if (this->videoInfo.fourcc == *(UInt32*)"YV12")
		{
			UOSInt oriCropLeft;
			UOSInt oriCropTop;
			UOSInt oriCropRight;
			UOSInt oriCropBottom;

			srcVideo->GetBorderCrop(oriCropLeft, oriCropTop, oriCropRight, oriCropBottom);
			if (this->hasCrop && oriCropLeft == 0 && oriCropTop == 0 && oriCropRight == 0 && oriCropBottom == 0)
			{
			}
			if (!this->hasCrop)
			{
				oriCropLeft = this->videoInfo.dispSize.x >> 1;
				oriCropRight = this->videoInfo.dispSize.x >> 1;
				oriCropTop = this->videoInfo.dispSize.y >> 1;
				oriCropBottom = this->videoInfo.dispSize.y >> 1;
			}

			UOSInt w = this->videoInfo.dispSize.x;
			UOSInt h = this->videoInfo.dispSize.y;
			UnsafeArray<UInt8> yptr = imgData[0];
			UOSInt ySplit;
			UOSInt crops[4];
			crops[0] = oriCropLeft;
			crops[1] = oriCropTop;
			crops[2] = oriCropRight;
			crops[3] = oriCropBottom;
			if (this->videoInfo.fourcc == *(UInt32*)"YV12")
			{
				if (w & 3)
				{
					w = w + 4 - (w & 3);
				}
				ySplit = 1;
			}
			else if (this->videoInfo.fourcc == *(UInt32*)"YUY2")
			{
				ySplit = 2;
			}
			else
			{
				ySplit = 0;
			}

			if (ySplit > 0)
			{
				AutoCropFilter_CropCalc(yptr.Ptr(), w, h, ySplit, crops);
			}
			if (oriCropLeft != crops[0] || oriCropRight != crops[2] || oriCropTop != crops[1] || oriCropBottom != crops[3])
			{
				srcVideo->SetBorderCrop(crops[0], crops[1], crops[2], crops[3]);
				this->hasCrop = true;
				if ((crops[0] + crops[1] + crops[2] + crops[3]) <= w)
				{
					if (this->fcCb)
					{
						this->fcCb(Media::IVideoSource::FC_CROP, this->userData);
					}
				}
			}
		}
	}
	if (this->videoCb)
	{
		this->videoCb(frameTime, frameNum, imgData, dataSize, frameStruct, this->userData, frameType, flags, ycOfst);
	}
}

void Media::VideoFilter::AutoCropFilter::OnFrameChange(Media::IVideoSource::FrameChange fc)
{
	if (fc == Media::IVideoSource::FC_SRCCHG)
	{
		this->hasCrop = false;
	}
}

Media::VideoFilter::AutoCropFilter::AutoCropFilter(Media::IVideoSource *srcVideo) : Media::VideoFilter::VideoFilterBase(srcVideo)
{
	this->enabled = true;
}

Media::VideoFilter::AutoCropFilter::~AutoCropFilter()
{
}

Text::CStringNN Media::VideoFilter::AutoCropFilter::GetFilterName()
{
	return CSTR("AutoCropFilter");
}

void Media::VideoFilter::AutoCropFilter::SetEnabled(Bool enabled)
{
	this->enabled = enabled;
}

void Media::VideoFilter::AutoCropFilter::GetBorderCrop(OutParam<UOSInt> cropLeft, OutParam<UOSInt> cropTop, OutParam<UOSInt> cropRight, OutParam<UOSInt> cropBottom)
{
	NN<Media::IVideoSource> srcVideo;
	Bool cropValid = true;
	if (!this->srcVideo.SetTo(srcVideo))
	{
		cropLeft.Set(0);
		cropTop.Set(0);
		cropRight.Set(0);
		cropBottom.Set(0);
		return;
	}

	UOSInt oriCropLeft;
	UOSInt oriCropTop;
	UOSInt oriCropRight;
	UOSInt oriCropBottom;
	srcVideo->GetBorderCrop(oriCropLeft, oriCropTop, oriCropRight, oriCropBottom);
	if (!this->hasCrop || !this->enabled)
	{
		cropValid = false;
	}
	else
	{
		UOSInt w = this->videoInfo.dispSize.x;
		if ((oriCropLeft + oriCropRight + oriCropTop + oriCropBottom) > (w >> 1))
		{
			cropValid = false;
		}
	}

	if (cropValid)
	{
		cropLeft.Set(oriCropLeft);
		cropTop.Set(oriCropTop);
		cropRight.Set(oriCropRight);
		cropBottom.Set(oriCropBottom);
	}
	else
	{
		cropLeft.Set(0);
		cropTop.Set(0);
		cropRight.Set(0);
		cropBottom.Set(0);
	}
}
