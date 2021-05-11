#include "Stdafx.h"
#include "MyMemory.h"
#include "Media/VideoFilter/AutoCropFilter.h"

extern "C"
{
	void AutoCropFilter_CropCalc(UInt8 *yptr, UOSInt w, UOSInt h, UOSInt ySplit, UOSInt *crops);
}

void Media::VideoFilter::AutoCropFilter::ProcessVideoFrame(UInt32 frameTime, UInt32 frameNum, UInt8 **imgData, UOSInt dataSize, Media::IVideoSource::FrameStruct frameStruct, void *userData, Media::FrameType frameType, Media::IVideoSource::FrameFlag flags, Media::YCOffset ycOfst)
{
	if (this->enabled && frameStruct == Media::IVideoSource::FS_I)
	{
		if (this->videoInfo->fourcc == *(UInt32*)"YV12")
		{
			UOSInt oriCropLeft;
			UOSInt oriCropTop;
			UOSInt oriCropRight;
			UOSInt oriCropBottom;

			this->srcVideo->GetBorderCrop(&oriCropLeft, &oriCropTop, &oriCropRight, &oriCropBottom);
			if (this->hasCrop && oriCropLeft == 0 && oriCropTop == 0 && oriCropRight == 0 && oriCropBottom == 0)
			{
			}
			if (!this->hasCrop)
			{
				oriCropLeft = this->videoInfo->dispWidth >> 1;
				oriCropRight = this->videoInfo->dispWidth >> 1;
				oriCropTop = this->videoInfo->dispHeight >> 1;
				oriCropBottom = this->videoInfo->dispHeight >> 1;
			}

			UOSInt w = this->videoInfo->dispWidth;
			UOSInt h = this->videoInfo->dispHeight;
			UInt8 *yptr = imgData[0];
			UOSInt ySplit;
			UOSInt crops[4];
			crops[0] = oriCropLeft;
			crops[1] = oriCropTop;
			crops[2] = oriCropRight;
			crops[3] = oriCropBottom;
			if (this->videoInfo->fourcc == *(UInt32*)"YV12")
			{
				if (w & 3)
				{
					w = w + 4 - (w & 3);
				}
				ySplit = 1;
			}
			else if (this->videoInfo->fourcc == *(UInt32*)"YUY2")
			{
				ySplit = 2;
			}
			else
			{
				ySplit = 0;
			}

			if (ySplit > 0)
			{
				AutoCropFilter_CropCalc(yptr, w, h, ySplit, crops);
			}
			if (oriCropLeft != crops[0] || oriCropRight != crops[2] || oriCropTop != crops[1] || oriCropBottom != crops[3])
			{
				this->srcVideo->SetBorderCrop(crops[0], crops[1], crops[2], crops[3]);
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

const UTF8Char *Media::VideoFilter::AutoCropFilter::GetFilterName()
{
	return (const UTF8Char*)"AutoCropFilter";
}

void Media::VideoFilter::AutoCropFilter::SetEnabled(Bool enabled)
{
	this->enabled = enabled;
}

void Media::VideoFilter::AutoCropFilter::GetBorderCrop(UOSInt *cropLeft, UOSInt *cropTop, UOSInt *cropRight, UOSInt *cropBottom)
{
	Bool cropValid = true;
	if (this->srcVideo == 0)
	{
		*cropLeft = 0;
		*cropTop = 0;
		*cropRight = 0;
		*cropBottom = 0;
		return;
	}

	UOSInt oriCropLeft;
	UOSInt oriCropTop;
	UOSInt oriCropRight;
	UOSInt oriCropBottom;
	this->srcVideo->GetBorderCrop(&oriCropLeft, &oriCropTop, &oriCropRight, &oriCropBottom);
	if (!this->hasCrop || !this->enabled)
	{
		cropValid = false;
	}
	else
	{
		UOSInt w = this->videoInfo->dispWidth;
		if ((oriCropLeft + oriCropRight + oriCropTop + oriCropBottom) > (w >> 1))
		{
			cropValid = false;
		}
	}

	if (cropValid)
	{
		*cropLeft = oriCropLeft;
		*cropTop = oriCropTop;
		*cropRight = oriCropRight;
		*cropBottom = oriCropBottom;
	}
	else
	{
		*cropLeft = 0;
		*cropTop = 0;
		*cropRight = 0;
		*cropBottom = 0;
	}
}
