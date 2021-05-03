#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/Path.h"
#include "Media/IRealtimeVideoSource.h"
#include "Media/IVideoCapture.h"
#include "Media/CS/CSConverter.h"
#include "Media/Decoder/VideoDecoderFinder.h"
#include "Media/OpenCV/OCVFrameFeeder.h"
#include "Text/MyString.h"

#include <stdio.h>

void __stdcall Media::OpenCV::OCVFrameFeeder::OnFrame(UInt32 frameTime, UInt32 frameNum, UInt8 **imgData, UOSInt dataSize, Media::IVideoSource::FrameStruct frameStruct, void *userData, Media::FrameType frameType, Media::IVideoSource::FrameFlag flags, Media::YCOffset ycOfst)
{
	Media::OpenCV::OCVFrameFeeder *me = (Media::OpenCV::OCVFrameFeeder*)userData;
	Media::OpenCV::OCVFrame *frame = Media::OpenCV::OCVFrame::CreateYFrame(imgData, dataSize, me->info.fourcc, me->info.dispWidth, me->info.dispHeight, me->info.storeWidth, me->info.storeBPP, me->info.pf);
	if (frame)
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
		DEL_CLASS(frame);
	}
}

void __stdcall Media::OpenCV::OCVFrameFeeder::OnFrameChange(Media::IVideoSource::FrameChange frChg, void *userData)
{
//	Media::OpenCV::OCVFrameFeeder *me = (Media::OpenCV::OCVFrameFeeder*)userData;

}

Media::OpenCV::OCVFrameFeeder::OCVFrameFeeder(Media::OpenCV::OCVObjectDetector *frameInput, Media::IVideoSource *src)
{
	this->frameInput = frameInput;
	this->src = src;
	this->decoder = 0;
	this->frameSkip = 0;
	this->preferedFormat = 0;
	this->preferedWidth = 0;
	this->preferedHeight = 0;
	this->info.Clear();
}

Media::OpenCV::OCVFrameFeeder::~OCVFrameFeeder()
{
	if (this->decoder)
	{
		this->decoder->Stop();
		this->decoder->Init(0, 0, 0);
		DEL_CLASS(this->decoder);
	}
	else
	{
		this->src->Stop();
		this->src->Init(0, 0, 0);
	}
}

Bool Media::OpenCV::OCVFrameFeeder::Start()
{
	OSInt i;
	OSInt j;
	UInt32 rateNorm;
	UInt32 rateDenorm;
	UOSInt maxFrameSize;
	if (this->src->IsRunning())
	{
		return true;
	}
	if (this->src->IsRealTimeSrc())
	{
		Media::IRealtimeVideoSource *realtimeVideo = (Media::IRealtimeVideoSource*)src;
		if (realtimeVideo->IsVideoCapture())
		{
			Media::IVideoCapture *capture = (Media::IVideoCapture*)realtimeVideo;
			Media::IVideoCapture::VideoFormat *formats;
			UOSInt thisSize;
			UOSInt maxSize = 0;
			UOSInt maxWidth = -1;
			UOSInt maxHeight = -1;
			UInt32 maxFmt = 0xFFFFFFFF;
			Int32 maxBpp;
			Int32 maxRateNumer = 0;
			Int32 maxRateDenom = 0;
			formats = MemAlloc(Media::IVideoCapture::VideoFormat, 512);
			i = 0;
			j = 512;
			while (i < j)
			{
				formats[i].info.Init();
				formats[i].info.Clear();
				formats[i].frameRateDenorm = 0;
				formats[i].frameRateNorm = 0;
				i++;
			}
			i = 0;
			j = capture->GetSupportedFormats(formats, 512);
			while (i < j)
			{
				thisSize = formats[i].info.dispWidth * formats[i].info.dispHeight;
				if (formats[i].info.fourcc == this->preferedFormat && formats[i].info.dispWidth == this->preferedWidth && formats[i].info.dispHeight == this->preferedHeight)
				{
					if (maxFmt != this->preferedFormat || maxWidth != this->preferedWidth || maxHeight != this->preferedHeight)
					{
						maxSize = thisSize;
						maxWidth = formats[i].info.dispWidth;
						maxHeight = formats[i].info.dispHeight;
						maxFmt = formats[i].info.fourcc;
						maxBpp = formats[i].info.storeBPP;
						maxRateNumer = formats[i].frameRateNorm;
						maxRateDenom = formats[i].frameRateDenorm;
					}
					else if (formats[i].frameRateNorm / formats[i].frameRateDenorm > maxRateNumer / maxRateDenom)
					{
						maxSize = thisSize;
						maxWidth = formats[i].info.dispWidth;
						maxHeight = formats[i].info.dispHeight;
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
						maxWidth = formats[i].info.dispWidth;
						maxHeight = formats[i].info.dispHeight;
						maxFmt = formats[i].info.fourcc;
						maxBpp = formats[i].info.storeBPP;
						maxRateNumer = formats[i].frameRateNorm;
						maxRateDenom = formats[i].frameRateDenorm;
					}
				}
				else if (maxWidth == this->preferedWidth && maxHeight == this->preferedWidth)
				{
					if (formats[i].info.dispWidth == this->preferedWidth && formats[i].info.dispHeight == this->preferedHeight && (formats[i].frameRateNorm / formats[i].frameRateDenorm > maxRateNumer / maxRateDenom))
					{
						maxSize = thisSize;
						maxWidth = formats[i].info.dispWidth;
						maxHeight = formats[i].info.dispHeight;
						maxFmt = formats[i].info.fourcc;
						maxBpp = formats[i].info.storeBPP;
						maxRateNumer = formats[i].frameRateNorm;
						maxRateDenom = formats[i].frameRateDenorm;
					}
				}
				else if (formats[i].info.dispWidth == this->preferedWidth && formats[i].info.dispHeight == this->preferedHeight)
				{
					maxSize = thisSize;
					maxWidth = formats[i].info.dispWidth;
					maxHeight = formats[i].info.dispHeight;
					maxFmt = formats[i].info.fourcc;
					maxBpp = formats[i].info.storeBPP;
					maxRateNumer = formats[i].frameRateNorm;
					maxRateDenom = formats[i].frameRateDenorm;
				}
				else if (thisSize > maxSize)
				{
					maxSize = thisSize;
					maxWidth = formats[i].info.dispWidth;
					maxHeight = formats[i].info.dispHeight;
					maxFmt = formats[i].info.fourcc;
					maxBpp = formats[i].info.storeBPP;
					maxRateNumer = formats[i].frameRateNorm;
					maxRateDenom = formats[i].frameRateDenorm;
				}
				else if (thisSize == maxSize && (formats[i].frameRateNorm / formats[i].frameRateDenorm > maxRateNumer / maxRateDenom))
				{
					maxSize = thisSize;
					maxWidth = formats[i].info.dispWidth;
					maxHeight = formats[i].info.dispHeight;
					maxFmt = formats[i].info.fourcc;
					maxBpp = formats[i].info.storeBPP;
					maxRateNumer = formats[i].frameRateNorm;
					maxRateDenom = formats[i].frameRateDenorm;
				}

				i++;
			}
			if (maxSize > 0)
			{
				Int32 tmp[2];
				tmp[0] = maxFmt;
				tmp[1] = 0;
				printf("OCVFrameFeeder: %s %d %d\r\n", (Char*)tmp, (Int32)maxWidth, (Int32)maxHeight);
				capture->SetPreferSize(maxWidth, maxHeight, maxFmt, maxBpp, maxRateNumer, maxRateDenom);
			}

			i = 0;
			j = 512;
			while (i < j)
			{
				formats[i].info.Deinit();
				i++;
			}
			MemFree(formats);
		}
	}
	this->src->GetVideoInfo(&this->info, &rateNorm, &rateDenorm, &maxFrameSize);
	if (!Media::CS::CSConverter::IsSupported(this->info.fourcc))
	{
		Media::Decoder::VideoDecoderFinder decoders;
		this->decoder = decoders.DecodeVideo(this->src);
		if (this->decoder)
		{
			this->decoder->GetVideoInfo(&this->info, &rateNorm, &rateDenorm, &maxFrameSize);
		}
	}
	this->thisSkip = 0;
	if (this->decoder)
	{
		this->decoder->Init(OnFrame, OnFrameChange, this);
		return this->decoder->Start();
	}
	else
	{
		this->src->Init(OnFrame, OnFrameChange, this);
		return this->src->Start();
	}
}

void Media::OpenCV::OCVFrameFeeder::SetFrameSkip(OSInt frameSkip)
{
	this->frameSkip = frameSkip;
}

void Media::OpenCV::OCVFrameFeeder::SetPreferedFormat(UInt32 preferedFormat, UOSInt preferedWidth, UOSInt preferedHeight)
{
	this->preferedFormat = preferedFormat;
	this->preferedWidth = preferedWidth;
	this->preferedHeight = preferedHeight;
}
