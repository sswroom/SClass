#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayListInt32.h"
#include "Data/ByteTool.h"
#include "Media/Decoder/VP09Decoder.h"

void Media::Decoder::VP09Decoder::ProcVideoFrame(UInt32 frameTime, UInt32 frameNum, UInt8 **imgData, UOSInt dataSize, Media::IVideoSource::FrameStruct frameStruct, Media::FrameType frameType, Media::IVideoSource::FrameFlag flags, Media::YCOffset ycOfst)
{
	UOSInt nextFrameNum = 0;
	VP9FrameInfo *frInfo = this->frameList->GetItem(this->frameList->GetCount() - 1);
	if (frInfo)
	{
		nextFrameNum = frInfo->srcFrameIndex + 1;
	}

	UInt32 fr1;
	UInt32 fr2;
	Bool dblFrame = false;

	if (dataSize > 8)
	{
		if (imgData[0][dataSize - 8] == 0xd1 && imgData[0][dataSize - 1] == 0xd1)
		{
			fr1 = ReadUInt24(&imgData[0][dataSize - 7]);
			fr2 = ReadUInt24(&imgData[0][dataSize - 4]);
			if (fr1 + fr2 == dataSize - 8)
			{
				dblFrame = true;
			}
			else
			{
				dblFrame = false;
			}
		}
		else if (imgData[0][dataSize - 6] == 0xc9 && imgData[0][dataSize - 1] == 0xc9)
		{
			fr1 = ReadUInt16(&imgData[0][dataSize - 5]);
			fr2 = ReadUInt16(&imgData[0][dataSize - 3]);
			if (fr1 + fr2 == dataSize - 6)
			{
				dblFrame = true;
			}
		}
		else if (imgData[0][dataSize - 4] == 0xc1 && imgData[0][dataSize - 1] == 0xc1)
		{
			fr1 = imgData[0][dataSize - 3];
			fr2 = imgData[0][dataSize - 2];
			if (fr1 + fr2 == dataSize - 4)
			{
				dblFrame = true;
			}
		}
		else if (imgData[0][dataSize - 1] == 0xd1)
		{
			dblFrame = false;
		}
	}
	if (nextFrameNum == frameNum)
	{
		if (dblFrame)
		{
			frInfo = MemAlloc(VP9FrameInfo, 1);
			frInfo->srcFrameIndex = frameNum;
			frInfo->frameOfst = 0;
			frInfo->frameSize = fr1;
			frInfo->fullFrameSize = dataSize;
			this->frameList->Add(frInfo);
			frInfo = MemAlloc(VP9FrameInfo, 1);
			frInfo->srcFrameIndex = frameNum;
			frInfo->frameOfst = fr1;
			frInfo->frameSize = fr2;
			frInfo->fullFrameSize = dataSize;
			this->frameList->Add(frInfo);
		}
		else
		{
			frInfo = MemAlloc(VP9FrameInfo, 1);
			frInfo->srcFrameIndex = frameNum;
			frInfo->frameOfst = 0;
			frInfo->frameSize = dataSize;
			frInfo->fullFrameSize = dataSize;
			this->frameList->Add(frInfo);
		}
	}

	if (dblFrame)
	{
		UInt8 *imgPtr = &imgData[0][fr1];
		this->frameCb(frameTime, frameNum, &imgData[0], fr1, Media::IVideoSource::FS_N, this->frameCbData, frameType, flags, Media::YCOFST_C_CENTER_LEFT);
		flags = (Media::IVideoSource::FrameFlag)(flags & ~Media::IVideoSource::FF_DISCONTTIME);
		this->frameCb(frameTime, frameNum, &imgPtr, fr2, frameStruct, this->frameCbData, frameType, flags, Media::YCOFST_C_CENTER_LEFT);
	}
	else
	{
		this->frameCb(frameTime, frameNum, &imgData[0], dataSize, frameStruct, this->frameCbData, frameType, flags, Media::YCOFST_C_CENTER_LEFT);
	}
}

Media::Decoder::VP09Decoder::VP09Decoder(IVideoSource *sourceVideo, Bool toRelease) : Media::Decoder::VDecoderBase(sourceVideo)
{
	this->finfoMode = false;
	NEW_CLASS(this->frameList, Data::ArrayList<VP9FrameInfo*>());
}

Media::Decoder::VP09Decoder::~VP09Decoder()
{
	UOSInt i = this->frameList->GetCount();
	while (i-- > 0)
	{
		MemFree(this->frameList->GetItem(i));
	}
	DEL_CLASS(this->frameList);
}

Text::CString Media::Decoder::VP09Decoder::GetFilterName()
{
	return CSTR("VP09Decoder");
}

Bool Media::Decoder::VP09Decoder::HasFrameCount()
{
	if (this->sourceVideo)
	{
		return this->sourceVideo->HasFrameCount();
	}
	return false;
}

UOSInt Media::Decoder::VP09Decoder::GetFrameCount()
{
	if (this->sourceVideo && this->sourceVideo->HasFrameCount())
	{
		UOSInt srcFrameCount = this->sourceVideo->GetFrameCount();
		VP9FrameInfo *frInfo = this->frameList->GetItem(this->frameList->GetCount() - 1);
		if (frInfo && frInfo->srcFrameIndex == srcFrameCount - 1)
		{
			return this->frameList->GetCount();
		}
		return srcFrameCount;
	}
	return 0;
}

UInt32 Media::Decoder::VP09Decoder::GetFrameTime(UOSInt frameIndex)
{
	VP9FrameInfo *frInfo = this->frameList->GetItem(frameIndex);
	if (frInfo && this->sourceVideo)
	{
		return this->sourceVideo->GetFrameTime(frInfo->srcFrameIndex);
	}
	if (this->sourceVideo)
	{
		return this->sourceVideo->GetFrameTime(frameIndex);
	}
	return 0;
}

void Media::Decoder::VP09Decoder::EnumFrameInfos(FrameInfoCallback cb, void *userData)
{
}

UOSInt Media::Decoder::VP09Decoder::GetFrameSize(UOSInt frameIndex)
{
	if (this->sourceVideo == 0)
		return 0;
	VP9FrameInfo *frInfo;
	if (frameIndex == this->frameList->GetCount())
	{
		UOSInt nextFrameIndex = 0;
		frInfo = this->frameList->GetItem(frameIndex - 1);
		if (frInfo)
		{
			nextFrameIndex = frInfo->srcFrameIndex + 1;
		}
		UOSInt frameSize = this->sourceVideo->GetFrameSize(nextFrameIndex);
		if (frameSize <= 0)
			return 0;

		UInt8 *frameBuff = MemAlloc(UInt8, frameSize);
		frameSize = this->sourceVideo->ReadFrame(nextFrameIndex, frameBuff);
		if (frameSize <= 0)
		{
			MemFree(frameBuff);
			return 0;
		}
		UInt32 fr1;
		UInt32 fr2;
		Bool dblFrame = false;
		if (frameSize > 8 && frameBuff[frameSize - 8] == 0xd1 && frameBuff[frameSize - 1] == 0xd1)
		{
			fr1 = ReadUInt24(&frameBuff[frameSize - 7]);
			fr2 = ReadUInt24(&frameBuff[frameSize - 4]);
			if (fr1 + fr2 == frameSize - 8)
			{
				dblFrame = true;
			}
		}
		else if (frameSize > 6 && frameBuff[frameSize - 6] == 0xc9 && frameBuff[frameSize - 1] == 0xc9)
		{
			fr1 = ReadUInt16(&frameBuff[frameSize - 5]);
			fr2 = ReadUInt16(&frameBuff[frameSize - 3]);
			if (fr1 + fr2 == frameSize - 6)
			{
				dblFrame = true;
			}
		}
		else if (frameSize > 4 && frameBuff[frameSize - 4] == 0xc1 && frameBuff[frameSize - 1] == 0xc1)
		{
			fr1 = frameBuff[frameSize - 3];
			fr2 = frameBuff[frameSize - 2];
			if (fr1 + fr2 == frameSize - 4)
			{
				dblFrame = true;
			}
		}
		if (dblFrame)
		{
			VP9FrameInfo *frInfo2;
			frInfo = MemAlloc(VP9FrameInfo, 1);
			frInfo->srcFrameIndex = nextFrameIndex;
			frInfo->frameOfst = 0;
			frInfo->frameSize = fr1;
			frInfo->fullFrameSize = frameSize;
			this->frameList->Add(frInfo);
			frInfo2 = frInfo;
			frInfo = MemAlloc(VP9FrameInfo, 1);
			frInfo->srcFrameIndex = nextFrameIndex;
			frInfo->frameOfst = fr1;
			frInfo->frameSize = fr2;
			frInfo->fullFrameSize = frameSize;
			this->frameList->Add(frInfo);
			frInfo = frInfo2;
		}
		else
		{
			frInfo = MemAlloc(VP9FrameInfo, 1);
			frInfo->srcFrameIndex = nextFrameIndex;
			frInfo->frameOfst = 0;
			frInfo->frameSize = frameSize;
			frInfo->fullFrameSize = frameSize;
			this->frameList->Add(frInfo);
		}
		MemFree(frameBuff);
		return frInfo->frameSize;
	}
	else
	{
		frInfo = this->frameList->GetItem(frameIndex);
		if (frInfo == 0)
			return 0;
		return frInfo->frameSize;
	}
}

UOSInt Media::Decoder::VP09Decoder::ReadFrame(UOSInt frameIndex, UInt8 *buff)
{
	if (this->sourceVideo == 0)
		return 0;
	VP9FrameInfo *frInfo;
	if (frameIndex == this->frameList->GetCount())
	{
		UOSInt nextFrameIndex = 0;
		frInfo = this->frameList->GetItem(frameIndex - 1);
		if (frInfo)
		{
			nextFrameIndex = frInfo->srcFrameIndex + 1;
		}
		UOSInt frameSize = this->sourceVideo->GetFrameSize(nextFrameIndex);
		if (frameSize <= 0)
			return 0;

		UInt8 *frameBuff = MemAlloc(UInt8, frameSize);
		frameSize = this->sourceVideo->ReadFrame(nextFrameIndex, frameBuff);
		if (frameSize <= 0)
		{
			MemFree(frameBuff);
			return 0;
		}
		UInt32 fr1;
		UInt32 fr2;
		Bool dblFrame = false;
		if (frameSize > 8 && frameBuff[frameSize - 8] == 0xd1 && frameBuff[frameSize - 1] == 0xd1)
		{
			fr1 = ReadUInt24(&frameBuff[frameSize - 7]);
			fr2 = ReadUInt24(&frameBuff[frameSize - 4]);
			if (fr1 + fr2 == frameSize - 8)
			{
				dblFrame = true;
			}
		}
		else if (frameSize > 6 && frameBuff[frameSize - 6] == 0xc9 && frameBuff[frameSize - 1] == 0xc9)
		{
			fr1 = ReadUInt16(&frameBuff[frameSize - 5]);
			fr2 = ReadUInt16(&frameBuff[frameSize - 3]);
			if (fr1 + fr2 == frameSize - 6)
			{
				dblFrame = true;
			}
		}
		else if (frameSize > 4 && frameBuff[frameSize - 4] == 0xc1 && frameBuff[frameSize - 1] == 0xc1)
		{
			fr1 = frameBuff[frameSize - 3];
			fr2 = frameBuff[frameSize - 2];
			if (fr1 + fr2 == frameSize - 4)
			{
				dblFrame = true;
			}
		}
		if (dblFrame)
		{
			VP9FrameInfo *frInfo2;
			frInfo = MemAlloc(VP9FrameInfo, 1);
			frInfo->srcFrameIndex = nextFrameIndex;
			frInfo->frameOfst = 0;
			frInfo->frameSize = fr1;
			frInfo->fullFrameSize = frameSize;
			this->frameList->Add(frInfo);
			frInfo2 = frInfo;
			frInfo = MemAlloc(VP9FrameInfo, 1);
			frInfo->srcFrameIndex = nextFrameIndex;
			frInfo->frameOfst = fr1;
			frInfo->frameSize = fr2;
			frInfo->fullFrameSize = frameSize;
			this->frameList->Add(frInfo);
			frInfo = frInfo2;
		}
		else
		{
			frInfo = MemAlloc(VP9FrameInfo, 1);
			frInfo->srcFrameIndex = nextFrameIndex;
			frInfo->frameOfst = 0;
			frInfo->frameSize = frameSize;
			frInfo->fullFrameSize = frameSize;
			this->frameList->Add(frInfo);
		}
		MemCopyNO(buff, &frameBuff[frInfo->frameOfst], frInfo->frameSize);
		MemFree(frameBuff);
		return frInfo->frameSize;
	}
	else
	{
		frInfo = this->frameList->GetItem(frameIndex);
		if (frInfo == 0)
			return 0;
		if (frInfo->fullFrameSize == frInfo->frameSize)
		{
			return this->sourceVideo->ReadFrame(frInfo->srcFrameIndex, buff);
		}
		else
		{
			UInt8 *frameBuff = MemAlloc(UInt8, frInfo->fullFrameSize);
			UOSInt frameSize = this->sourceVideo->ReadFrame(frInfo->srcFrameIndex, frameBuff);
			if (frameSize == frInfo->fullFrameSize)
			{
				MemCopyNO(buff, &frameBuff[frInfo->frameOfst], frInfo->frameSize);
				MemFree(frameBuff);
				return frInfo->frameSize;
			}
			else
			{
				MemFree(frameBuff);
				return 0;
			}
		}
	}
}

Bool Media::Decoder::VP09Decoder::GetVideoInfo(NotNullPtr<Media::FrameInfo> info, OutParam<UInt32> frameRateNorm, OutParam<UInt32> frameRateDenorm, OutParam<UOSInt> maxFrameSize)
{
	return this->sourceVideo->GetVideoInfo(info, frameRateNorm, frameRateDenorm, maxFrameSize);
}
