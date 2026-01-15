#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/Path.h"
#include "Media/RealtimeVideoSource.h"
#include "Media/VideoCapturer.h"
#include "Media/CS/CSConverter.h"
#include "Media/Decoder/VideoDecoderFinder.h"
#include "Media/OpenCV/OCVFrameFeeder.h"
#include "Text/MyString.h"

#include <stdio.h>

void __stdcall Media::OpenCV::OCVFrameFeeder::OnFrame(Data::Duration frameTime, UInt32 frameNum, UnsafeArray<UnsafeArray<UInt8>> imgData, UIntOS dataSize, Media::VideoSource::FrameStruct frameStruct, AnyType userData, Media::FrameType frameType, Media::VideoSource::FrameFlag flags, Media::YCOffset ycOfst)
{
	NN<Media::OpenCV::OCVFrameFeeder> me = userData.GetNN<Media::OpenCV::OCVFrameFeeder>();
	NN<Media::OpenCV::OCVFrame> frame;
	if (Media::OpenCV::OCVFrame::CreateYFrame(imgData, dataSize, me->info.fourcc, me->info.dispSize, me->info.storeSize.x, me->info.storeBPP, me->info.pf).SetTo(frame))
	{
		if (me->thisSkip > 0)
		{
			me->thisSkip--;
		}
		else
		{
			me->frameInput->NextFrame(frame, &me->info, imgData);
			me->thisSkip = me->frameSkip;
		}
		frame.Delete();
	}
}

void __stdcall Media::OpenCV::OCVFrameFeeder::OnFrameChange(Media::VideoSource::FrameChange frChg, AnyType userData)
{
//	NN<Media::OpenCV::OCVFrameFeeder> me = userData.GetNN<Media::OpenCV::OCVFrameFeeder>();
}

Media::OpenCV::OCVFrameFeeder::OCVFrameFeeder(NN<Media::OpenCV::OCVObjectDetector> frameInput, NN<Media::VideoSource> src)
{
	this->frameInput = frameInput;
	this->src = src;
	this->decoder = nullptr;
	this->frameSkip = 0;
	this->preferedFormat = 0;
	this->preferedWidth = 0;
	this->preferedHeight = 0;
	this->info.Clear();
}

Media::OpenCV::OCVFrameFeeder::~OCVFrameFeeder()
{
	NN<Media::VideoSource> decoder;
	if (this->decoder.SetTo(decoder))
	{
		decoder->Stop();
		decoder->Init(0, 0, 0);
		this->decoder.Delete();
	}
	else
	{
		this->src->Stop();
		this->src->Init(0, 0, 0);
	}
}

Bool Media::OpenCV::OCVFrameFeeder::Start()
{
	NN<Media::VideoSource> decoder;
	UIntOS i;
	UIntOS j;
	UInt32 rateNorm;
	UInt32 rateDenorm;
	UIntOS maxFrameSize;
	if (this->src->IsRunning())
	{
		return true;
	}
	if (this->src->IsRealTimeSrc())
	{
		Media::RealtimeVideoSource *realtimeVideo = (Media::RealtimeVideoSource*)src.Ptr();
		if (realtimeVideo->IsVideoCapture())
		{
			Media::VideoCapturer *capture = (Media::VideoCapturer*)realtimeVideo;
			Media::VideoCapturer::VideoFormat *formats;
			UIntOS thisSize;
			UIntOS maxSize = 0;
			UIntOS maxWidth = 0;
			UIntOS maxHeight = 0;
			UInt32 maxFmt = 0xFFFFFFFF;
			UInt32 maxBpp = 0;
			UInt32 maxRateNumer = 0;
			UInt32 maxRateDenom = 0;
			formats = MemAlloc(Media::VideoCapturer::VideoFormat, 512);
			i = 0;
			j = 512;
			while (i < j)
			{
				formats[i].info.InitFrameInfo();
				formats[i].info.Clear();
				formats[i].frameRateDenorm = 0;
				formats[i].frameRateNorm = 0;
				i++;
			}
			i = 0;
			j = capture->GetSupportedFormats(formats, 512);
			while (i < j)
			{
				thisSize = formats[i].info.dispSize.CalcArea();
				if (formats[i].info.fourcc == this->preferedFormat && formats[i].info.dispSize.x == this->preferedWidth && formats[i].info.dispSize.y == this->preferedHeight)
				{
					if (maxFmt != this->preferedFormat || maxWidth != this->preferedWidth || maxHeight != this->preferedHeight)
					{
						maxSize = thisSize;
						maxWidth = formats[i].info.dispSize.x;
						maxHeight = formats[i].info.dispSize.y;
						maxFmt = formats[i].info.fourcc;
						maxBpp = formats[i].info.storeBPP;
						maxRateNumer = formats[i].frameRateNorm;
						maxRateDenom = formats[i].frameRateDenorm;
					}
					else if (formats[i].frameRateNorm / formats[i].frameRateDenorm > maxRateNumer / maxRateDenom)
					{
						maxSize = thisSize;
						maxWidth = formats[i].info.dispSize.x;
						maxHeight = formats[i].info.dispSize.y;
						maxFmt = formats[i].info.fourcc;
						maxBpp = formats[i].info.storeBPP;
						maxRateNumer = formats[i].frameRateNorm;
						maxRateDenom = formats[i].frameRateDenorm;
					}
				}
				else if (maxFmt == this->preferedFormat)
				{
					if (formats[i].info.fourcc == this->preferedFormat && thisSize > maxSize)
					{
						maxSize = thisSize;
						maxWidth = formats[i].info.dispSize.x;
						maxHeight = formats[i].info.dispSize.y;
						maxFmt = formats[i].info.fourcc;
						maxBpp = formats[i].info.storeBPP;
						maxRateNumer = formats[i].frameRateNorm;
						maxRateDenom = formats[i].frameRateDenorm;
					}
				}
				else if (maxWidth == this->preferedWidth && maxHeight == this->preferedWidth)
				{
					if (formats[i].info.dispSize.x == this->preferedWidth && formats[i].info.dispSize.y == this->preferedHeight && (formats[i].frameRateNorm / formats[i].frameRateDenorm > maxRateNumer / maxRateDenom))
					{
						maxSize = thisSize;
						maxWidth = formats[i].info.dispSize.x;
						maxHeight = formats[i].info.dispSize.y;
						maxFmt = formats[i].info.fourcc;
						maxBpp = formats[i].info.storeBPP;
						maxRateNumer = formats[i].frameRateNorm;
						maxRateDenom = formats[i].frameRateDenorm;
					}
				}
				else if (formats[i].info.dispSize.x == this->preferedWidth && formats[i].info.dispSize.y == this->preferedHeight)
				{
					maxSize = thisSize;
					maxWidth = formats[i].info.dispSize.x;
					maxHeight = formats[i].info.dispSize.y;
					maxFmt = formats[i].info.fourcc;
					maxBpp = formats[i].info.storeBPP;
					maxRateNumer = formats[i].frameRateNorm;
					maxRateDenom = formats[i].frameRateDenorm;
				}
				else if (thisSize > maxSize)
				{
					maxSize = thisSize;
					maxWidth = formats[i].info.dispSize.x;
					maxHeight = formats[i].info.dispSize.y;
					maxFmt = formats[i].info.fourcc;
					maxBpp = formats[i].info.storeBPP;
					maxRateNumer = formats[i].frameRateNorm;
					maxRateDenom = formats[i].frameRateDenorm;
				}
				else if (thisSize == maxSize && (formats[i].frameRateNorm / formats[i].frameRateDenorm > maxRateNumer / maxRateDenom))
				{
					maxSize = thisSize;
					maxWidth = formats[i].info.dispSize.x;
					maxHeight = formats[i].info.dispSize.y;
					maxFmt = formats[i].info.fourcc;
					maxBpp = formats[i].info.storeBPP;
					maxRateNumer = formats[i].frameRateNorm;
					maxRateDenom = formats[i].frameRateDenorm;
				}

				i++;
			}
			if (maxSize > 0)
			{
				UInt32 tmp[2];
				tmp[0] = maxFmt;
				tmp[1] = 0;
				printf("OCVFrameFeeder: %s %d %d\r\n", (Char*)tmp, (Int32)maxWidth, (Int32)maxHeight);
				capture->SetPreferSize(Math::Size2D<UIntOS>(maxWidth, maxHeight), maxFmt, maxBpp, maxRateNumer, maxRateDenom);
			}

			i = 0;
			j = 512;
			while (i < j)
			{
				formats[i].info.DeinitFrameInfo();
				i++;
			}
			MemFree(formats);
		}
	}
	this->src->GetVideoInfo(this->info, rateNorm, rateDenorm, maxFrameSize);
	if (!Media::CS::CSConverter::IsSupported(this->info.fourcc))
	{
		Media::Decoder::VideoDecoderFinder decoders;
		this->decoder = decoders.DecodeVideo(this->src);
		if (this->decoder.SetTo(decoder))
		{
			decoder->GetVideoInfo(this->info, rateNorm, rateDenorm, maxFrameSize);
		}
	}
	this->thisSkip = 0;
	if (this->decoder.SetTo(decoder))
	{
		decoder->Init(OnFrame, OnFrameChange, this);
		return decoder->Start();
	}
	else
	{
		this->src->Init(OnFrame, OnFrameChange, this);
		return this->src->Start();
	}
}

void Media::OpenCV::OCVFrameFeeder::SetFrameSkip(IntOS frameSkip)
{
	this->frameSkip = frameSkip;
}

void Media::OpenCV::OCVFrameFeeder::SetPreferedFormat(UInt32 preferedFormat, UIntOS preferedWidth, UIntOS preferedHeight)
{
	this->preferedFormat = preferedFormat;
	this->preferedWidth = preferedWidth;
	this->preferedHeight = preferedHeight;
}
