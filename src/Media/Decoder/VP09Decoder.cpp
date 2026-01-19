#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayListInt32.h"
#include "Core/ByteTool_C.h"
#include "Media/Decoder/VP09Decoder.h"

void Media::Decoder::VP09Decoder::ProcVideoFrame(Data::Duration frameTime, UInt32 frameNum, UnsafeArray<UnsafeArray<UInt8>> imgData, UIntOS dataSize, Media::VideoSource::FrameStruct frameStruct, Media::FrameType frameType, Media::VideoSource::FrameFlag flags, Media::YCOffset ycOfst)
{
	UIntOS nextFrameNum = 0;
	NN<VP9FrameInfo> frInfo;
	if (this->frameList.GetItem(this->frameList.GetCount() - 1).SetTo(frInfo))
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
			frInfo = MemAllocNN(VP9FrameInfo);
			frInfo->srcFrameIndex = frameNum;
			frInfo->frameOfst = 0;
			frInfo->frameSize = fr1;
			frInfo->fullFrameSize = dataSize;
			this->frameList.Add(frInfo);
			frInfo = MemAllocNN(VP9FrameInfo);
			frInfo->srcFrameIndex = frameNum;
			frInfo->frameOfst = fr1;
			frInfo->frameSize = fr2;
			frInfo->fullFrameSize = dataSize;
			this->frameList.Add(frInfo);
		}
		else
		{
			frInfo = MemAllocNN(VP9FrameInfo);
			frInfo->srcFrameIndex = frameNum;
			frInfo->frameOfst = 0;
			frInfo->frameSize = dataSize;
			frInfo->fullFrameSize = dataSize;
			this->frameList.Add(frInfo);
		}
	}

	if (dblFrame)
	{
		UnsafeArray<UInt8> imgPtr = &imgData[0][fr1];
		this->frameCb(frameTime, frameNum, &imgData[0], fr1, Media::VideoSource::FS_N, this->frameCbData, frameType, flags, Media::YCOFST_C_CENTER_LEFT);
		flags = (Media::VideoSource::FrameFlag)(flags & ~Media::VideoSource::FF_DISCONTTIME);
		this->frameCb(frameTime, frameNum, &imgPtr, fr2, frameStruct, this->frameCbData, frameType, flags, Media::YCOFST_C_CENTER_LEFT);
	}
	else
	{
		this->frameCb(frameTime, frameNum, &imgData[0], dataSize, frameStruct, this->frameCbData, frameType, flags, Media::YCOFST_C_CENTER_LEFT);
	}
}

Media::Decoder::VP09Decoder::VP09Decoder(NN<VideoSource> sourceVideo, Bool toRelease) : Media::Decoder::VDecoderBase(sourceVideo)
{
	this->finfoMode = false;
}

Media::Decoder::VP09Decoder::~VP09Decoder()
{
	this->frameList.MemFreeAll();
}

Text::CStringNN Media::Decoder::VP09Decoder::GetFilterName()
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

UIntOS Media::Decoder::VP09Decoder::GetFrameCount()
{
	if (this->sourceVideo && this->sourceVideo->HasFrameCount())
	{
		UIntOS srcFrameCount = this->sourceVideo->GetFrameCount();
		NN<VP9FrameInfo> frInfo;
		if (this->frameList.GetItem(this->frameList.GetCount() - 1).SetTo(frInfo) && frInfo->srcFrameIndex == srcFrameCount - 1)
		{
			return this->frameList.GetCount();
		}
		return srcFrameCount;
	}
	return 0;
}

Data::Duration Media::Decoder::VP09Decoder::GetFrameTime(UIntOS frameIndex)
{
	NN<VP9FrameInfo> frInfo;
	if (this->frameList.GetItem(frameIndex).SetTo(frInfo) && this->sourceVideo)
	{
		return this->sourceVideo->GetFrameTime(frInfo->srcFrameIndex);
	}
	if (this->sourceVideo)
	{
		return this->sourceVideo->GetFrameTime(frameIndex);
	}
	return 0;
}

void Media::Decoder::VP09Decoder::EnumFrameInfos(FrameInfoCallback cb, AnyType userData)
{
}

UIntOS Media::Decoder::VP09Decoder::GetFrameSize(UIntOS frameIndex)
{
	if (this->sourceVideo == 0)
		return 0;
	NN<VP9FrameInfo> frInfo;
	if (frameIndex == this->frameList.GetCount())
	{
		UIntOS nextFrameIndex = 0;
		if (this->frameList.GetItem(frameIndex - 1).SetTo(frInfo))
		{
			nextFrameIndex = frInfo->srcFrameIndex + 1;
		}
		UIntOS frameSize = this->sourceVideo->GetFrameSize(nextFrameIndex);
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
			NN<VP9FrameInfo> frInfo2;
			frInfo = MemAllocNN(VP9FrameInfo);
			frInfo->srcFrameIndex = nextFrameIndex;
			frInfo->frameOfst = 0;
			frInfo->frameSize = fr1;
			frInfo->fullFrameSize = frameSize;
			this->frameList.Add(frInfo);
			frInfo2 = frInfo;
			frInfo = MemAllocNN(VP9FrameInfo);
			frInfo->srcFrameIndex = nextFrameIndex;
			frInfo->frameOfst = fr1;
			frInfo->frameSize = fr2;
			frInfo->fullFrameSize = frameSize;
			this->frameList.Add(frInfo);
			frInfo = frInfo2;
		}
		else
		{
			frInfo = MemAllocNN(VP9FrameInfo);
			frInfo->srcFrameIndex = nextFrameIndex;
			frInfo->frameOfst = 0;
			frInfo->frameSize = frameSize;
			frInfo->fullFrameSize = frameSize;
			this->frameList.Add(frInfo);
		}
		MemFree(frameBuff);
		return frInfo->frameSize;
	}
	else
	{
		if (!this->frameList.GetItem(frameIndex).SetTo(frInfo))
			return 0;
		return frInfo->frameSize;
	}
}

UIntOS Media::Decoder::VP09Decoder::ReadFrame(UIntOS frameIndex, UnsafeArray<UInt8> buff)
{
	if (this->sourceVideo == 0)
		return 0;
	NN<VP9FrameInfo> frInfo;
	if (frameIndex == this->frameList.GetCount())
	{
		UIntOS nextFrameIndex = 0;
		if (this->frameList.GetItem(frameIndex - 1).SetTo(frInfo))
		{
			nextFrameIndex = frInfo->srcFrameIndex + 1;
		}
		UIntOS frameSize = this->sourceVideo->GetFrameSize(nextFrameIndex);
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
			NN<VP9FrameInfo> frInfo2;
			frInfo = MemAllocNN(VP9FrameInfo);
			frInfo->srcFrameIndex = nextFrameIndex;
			frInfo->frameOfst = 0;
			frInfo->frameSize = fr1;
			frInfo->fullFrameSize = frameSize;
			this->frameList.Add(frInfo);
			frInfo2 = frInfo;
			frInfo = MemAllocNN(VP9FrameInfo);
			frInfo->srcFrameIndex = nextFrameIndex;
			frInfo->frameOfst = fr1;
			frInfo->frameSize = fr2;
			frInfo->fullFrameSize = frameSize;
			this->frameList.Add(frInfo);
			frInfo = frInfo2;
		}
		else
		{
			frInfo = MemAllocNN(VP9FrameInfo);
			frInfo->srcFrameIndex = nextFrameIndex;
			frInfo->frameOfst = 0;
			frInfo->frameSize = frameSize;
			frInfo->fullFrameSize = frameSize;
			this->frameList.Add(frInfo);
		}
		MemCopyNO(buff.Ptr(), &frameBuff[frInfo->frameOfst], frInfo->frameSize);
		MemFree(frameBuff);
		return frInfo->frameSize;
	}
	else
	{
		if (!this->frameList.GetItem(frameIndex).SetTo(frInfo))
			return 0;
		if (frInfo->fullFrameSize == frInfo->frameSize)
		{
			return this->sourceVideo->ReadFrame(frInfo->srcFrameIndex, buff);
		}
		else
		{
			UInt8 *frameBuff = MemAlloc(UInt8, frInfo->fullFrameSize);
			UIntOS frameSize = this->sourceVideo->ReadFrame(frInfo->srcFrameIndex, frameBuff);
			if (frameSize == frInfo->fullFrameSize)
			{
				MemCopyNO(buff.Ptr(), &frameBuff[frInfo->frameOfst], frInfo->frameSize);
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

Bool Media::Decoder::VP09Decoder::GetVideoInfo(NN<Media::FrameInfo> info, OutParam<UInt32> frameRateNorm, OutParam<UInt32> frameRateDenorm, OutParam<UIntOS> maxFrameSize)
{
	return this->sourceVideo->GetVideoInfo(info, frameRateNorm, frameRateDenorm, maxFrameSize);
}
