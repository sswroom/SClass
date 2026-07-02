#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayListInt32.h"
#include "Core/ByteTool_C.h"
#include "Media/H265Parser.h"
#include "Media/Decoder/RHVCDecoder.h"
#include "Sync/MutexUsage.h"
#include "Sync/SimpleThread.h"

UIntOS Media::Decoder::RHVCDecoder::CalcNALSize(const UInt8 *buff, UIntOS buffSize)
{
	UIntOS ret = buffSize + 4;
	UIntOS i = 0;
	i = 2;
	while (i < buffSize)
	{
		if (buff[i - 2] == 0 && buff[i - 1] == 0 && (buff[i] & 0xfc) == 0)
		{
			ret++;
			i++;
		}
		i++;
	}
	return ret;
}

UInt8 *Media::Decoder::RHVCDecoder::AppendNAL(UInt8 *outBuff, const UInt8 *srcBuff, UIntOS srcBuffSize)
{
	UIntOS i;
	i = 2;
	outBuff[0] = 0;
	outBuff[1] = 0;
	outBuff[2] = 0;
	outBuff[3] = 1;
	outBuff += 4;
	while (i < srcBuffSize)
	{
		if (srcBuff[i - 2] == 0 && srcBuff[i - 1] == 0 && (srcBuff[i] & 0xfc) == 0)
		{
			outBuff[0] = 0;
			outBuff[1] = 0;
			outBuff[2] = 3;
			outBuff += 3;
			i += 2;
		}
		else
		{
			*outBuff++ = srcBuff[i - 2];
			i++;
		}
	}
	i -= 2;
	while (i < srcBuffSize)
	{
		*outBuff++ = srcBuff[i++];
	}
	return outBuff;
}

void Media::Decoder::RHVCDecoder::ProcVideoFrame(Data::Duration frameTime, UInt32 frameNum, UnsafeArray<UnsafeArray<UInt8>> imgData, UIntOS dataSize, Media::VideoSource::FrameStruct frameStruct, Media::FrameType frameType, Media::VideoSource::FrameFlag flags, Media::YCOffset ycOfst)
{
	Sync::MutexUsage mutUsage(this->frameMut);

	UnsafeArray<UInt8> frameBuff = this->frameBuff;
	UIntOS imgOfst = 0;
	UIntOS imgSize;
	Bool frameFound = false;

	if (flags & Media::VideoSource::FF_DISCONTTIME)
	{
		this->discontTime = true;
		this->frameSize = 0;
		this->lastIsField = false;
	}

	if ((dataSize + this->vpsSize + this->spsSize + this->ppsSize + 12) > this->maxFrameSize)
	{
		this->maxFrameSize = dataSize + this->vpsSize + this->spsSize + this->ppsSize + 12;
		MemFreeAArr(this->frameBuff);
		this->frameBuff = MemAllocAArr(UInt8, this->maxFrameSize);
		frameBuff = this->frameBuff;
	}

	while (imgOfst < dataSize)
	{
		imgSize = ReadMUInt32(&imgData[0][imgOfst]);
		imgOfst += 4;

		if (imgSize <= 0)
		{
			break;
		}
		if (imgSize + imgOfst > dataSize)
		{
			imgSize = dataSize - imgOfst;
		}
		if ((imgData[0][imgOfst] & 0x7e) == 0x26 && !frameFound) // IDR Picture
		{
			frameBuff[0] = 0;
			frameBuff[1] = 0;
			frameBuff[2] = 0;
			frameBuff[3] = 1;
			MemCopyNO(&frameBuff[4], this->vps, this->vpsSize);
			frameBuff += 4 + this->vpsSize;

			frameBuff[0] = 0;
			frameBuff[1] = 0;
			frameBuff[2] = 0;
			frameBuff[3] = 1;
			MemCopyNO(&frameBuff[4], this->sps, this->spsSize);
			frameBuff += 4 + this->spsSize;

			frameBuff[0] = 0;
			frameBuff[1] = 0;
			frameBuff[2] = 0;
			frameBuff[3] = 1;
			MemCopyNO(&frameBuff[4], this->pps, this->ppsSize);
			frameBuff += 4 + this->ppsSize;
			frameFound = true;
		}

		frameBuff[0] = 0;
		frameBuff[1] = 0;
		frameBuff[2] = 0;
		frameBuff[3] = 1;
		MemCopyNO(&frameBuff[4], &imgData[0][imgOfst], imgSize);
		frameBuff += 4 + imgSize;

		imgOfst += imgSize;
	}

	if (this->finfoMode)
	{
		NN<Media::VideoSource> sourceVideo;
		if (!this->finfoCb.func(frameTime, frameNum, (UIntOS)(frameBuff - this->frameBuff), frameStruct, frameType, this->finfoCb.userObj, ycOfst) && this->sourceVideo.SetTo(sourceVideo))
		{
			sourceVideo->Stop();
		}
	}
	else
	{
		if (this->discontTime && frameStruct != Media::VideoSource::FS_I)
		{
		}
		else
		{
			if (this->discontTime)
			{
				flags = (Media::VideoSource::FrameFlag)(flags | Media::VideoSource::FF_DISCONTTIME);
			}
			else
			{
				flags = (Media::VideoSource::FrameFlag)(flags & ~Media::VideoSource::FF_DISCONTTIME);
			}
			this->discontTime = false;
			this->frameCb(frameTime, frameNum, &this->frameBuff, (UIntOS)(frameBuff - this->frameBuff), frameStruct, this->frameCbData, frameType, flags, Media::YCOFST_C_CENTER_LEFT);
			this->frameSize = 0;
		}
	}
	mutUsage.EndUse();
}

Media::Decoder::RHVCDecoder::RHVCDecoder(NN<VideoSource> sourceVideo, Bool toRelease) : Media::Decoder::VDecoderBase(sourceVideo)
{
	Media::FrameInfo info;
	UIntOS size;
	UInt32 size32;
	UInt32 frameRateNorm;
	UInt32 frameRateDenorm;
	UnsafeArray<UInt8> buff;
	this->toRelease = toRelease;
	this->sps = 0;
	this->pps = 0;
	this->vps = 0;
	this->frameSize = 0;
	this->spsFound = false;
	this->finfoMode = false;
	this->maxFrameSize = 0;
	this->firstFrame = true;
	this->discontTime = true;

	if (!sourceVideo->GetVideoInfo(info, frameRateNorm, frameRateDenorm, size))
	{
		this->frameBuff = MemAllocAArr(UInt8, this->maxFrameSize);
		this->sourceVideo = nullptr;
		return;
	}
	if (info.fourcc != *(UInt32*)"rhvc")
	{
		this->frameBuff = MemAllocAArr(UInt8, this->maxFrameSize);
		this->sourceVideo = nullptr;
		return;
	}
	this->maxFrameSize = size;

	if (!sourceVideo->GetProp(*(Int32*)"sps", size32).SetTo(buff))
	{
		this->frameBuff = MemAllocAArr(UInt8, this->maxFrameSize);
		this->sourceVideo = nullptr;
		return;
	}
	this->sps = MemAlloc(UInt8, size32);
	this->spsSize = size32;
	MemCopyNO(this->sps, &buff[0], size32);
	this->maxFrameSize += 4 + this->spsSize;

	if (!sourceVideo->GetProp(*(Int32*)"pps", size32).SetTo(buff))
	{
		this->frameBuff = MemAllocAArr(UInt8, this->maxFrameSize);
		this->sourceVideo = nullptr;
		return;
	}
	this->pps = MemAlloc(UInt8, size32);
	this->ppsSize = size32;
	MemCopyNO(this->pps, &buff[0], size32);
	this->maxFrameSize += 4 + this->ppsSize;

	if (!sourceVideo->GetProp(*(Int32*)"vps", size32).SetTo(buff))
	{
		this->frameBuff = MemAllocAArr(UInt8, this->maxFrameSize);
		this->sourceVideo = nullptr;
		return;
	}
	this->vps = MemAlloc(UInt8, size32);
	this->vpsSize = size32;
	MemCopyNO(this->vps, &buff[0], size32);
	this->maxFrameSize += 4 + this->vpsSize;

	this->frameBuff = MemAllocAArr(UInt8, this->maxFrameSize);
	UIntOS oriW;
	UIntOS oriH;
	oriW = info.dispSize.x;
	oriH = info.dispSize.y;
	Media::H265Parser::GetFrameInfoSPS(this->sps, this->spsSize, info);
	UIntOS cropRight = 0;
	UIntOS cropBottom = 0;
	if (info.dispSize.x < oriW)
	{
		cropRight = oriW - info.dispSize.x;
	}
	if (info.dispSize.y < oriH)
	{
		cropBottom = oriH - info.dispSize.y;
	}
	if (cropRight > 0 || cropBottom > 0)
	{
		this->SetBorderCrop(0, 0, cropRight, cropBottom);
	}
}

Media::Decoder::RHVCDecoder::~RHVCDecoder()
{
	if (this->toRelease)
	{
		this->sourceVideo.Delete();
	}
	if (this->sps)
	{
		MemFree(this->sps);
	}
	if (this->pps)
	{
		MemFree(this->pps);
	}
	if (this->vps)
	{
		MemFree(this->vps);
	}
	MemFreeAArr(this->frameBuff);
}

Text::CStringNN Media::Decoder::RHVCDecoder::GetFilterName()
{
	return CSTR("RHVCDecoder");
}

Bool Media::Decoder::RHVCDecoder::HasFrameCount()
{
	NN<Media::VideoSource> sourceVideo;
	if (!this->sourceVideo.SetTo(sourceVideo))
		return false;
	return sourceVideo->HasFrameCount();
}

UIntOS Media::Decoder::RHVCDecoder::GetFrameCount()
{
	NN<Media::VideoSource> sourceVideo;
	if (!this->sourceVideo.SetTo(sourceVideo))
		return 0;
	return sourceVideo->GetFrameCount();
}

Data::Duration Media::Decoder::RHVCDecoder::GetFrameTime(UIntOS frameIndex)
{
	NN<Media::VideoSource> sourceVideo;
	if (!this->sourceVideo.SetTo(sourceVideo))
		return 0;
	return sourceVideo->GetFrameTime(frameIndex);
}
void Media::Decoder::RHVCDecoder::EnumFrameInfos(FrameInfoCallback cb, AnyType userData)
{
	NN<Media::VideoSource> sourceVideo;
	this->finfoCb = {cb, userData};
	if (this->sourceVideo.SetTo(sourceVideo))
	{
		this->finfoMode = true;
		sourceVideo->Stop();
		sourceVideo->Init(Media::Decoder::VDecoderBase::OnVideoFrame, this->fcCb, this);
		sourceVideo->Start();
		while (sourceVideo->IsRunning())
		{
			Sync::SimpleThread::Sleep(10);
		}
		this->finfoMode = false;
	}
}

UIntOS Media::Decoder::RHVCDecoder::GetFrameSize(UIntOS frameIndex)
{
	UIntOS srcFrameSize = 0;
	NN<Media::VideoSource> sourceVideo;
	if (!this->sourceVideo.SetTo(sourceVideo))
		return 0;
	srcFrameSize = sourceVideo->GetFrameSize(frameIndex);
	if (srcFrameSize == 0)
		return 0;
	return this->maxFrameSize;
}

UIntOS Media::Decoder::RHVCDecoder::ReadFrame(UIntOS frameIndex, UnsafeArray<UInt8> buff)
{
	NN<Media::VideoSource> sourceVideo;
	if (!this->sourceVideo.SetTo(sourceVideo))
		return 0;
	Bool frameFound = false;
	UIntOS frameSize = sourceVideo->ReadFrame(frameIndex, this->frameBuff);
	UIntOS outSize = 0;
	UIntOS imgOfst = 0;
	UIntOS imgSize;
	UnsafeArray<const UInt8> imgData = this->frameBuff;
	UnsafeArray<UInt8> frameBuff = buff;
	while (imgOfst < frameSize)
	{
		imgSize = ReadMUInt32(&imgData[imgOfst]);
		imgOfst += 4;

		if (imgSize <= 0)
		{
			break;
		}
		if (imgSize + imgOfst > frameSize)
		{
			imgSize = frameSize - imgOfst;
		}
		if ((imgData[imgOfst] & 0x7e) == 0x26 && !frameFound) // IDR Picture
		{
			frameBuff[0] = 0;
			frameBuff[1] = 0;
			frameBuff[2] = 0;
			frameBuff[3] = 1;
			MemCopyNO(&frameBuff[4], this->vps, this->vpsSize);
			frameBuff += 4 + this->vpsSize;
			outSize += 4 + this->vpsSize;

			frameBuff[0] = 0;
			frameBuff[1] = 0;
			frameBuff[2] = 0;
			frameBuff[3] = 1;
			MemCopyNO(&frameBuff[4], this->sps, this->spsSize);
			frameBuff += 4 + this->spsSize;
			outSize += 4 + this->spsSize;

			frameBuff[0] = 0;
			frameBuff[1] = 0;
			frameBuff[2] = 0;
			frameBuff[3] = 1;
			MemCopyNO(&frameBuff[4], this->pps, this->ppsSize);
			frameBuff += 4 + this->ppsSize;
			outSize += 4 + this->ppsSize;
			frameFound = true;
		}

		frameBuff[0] = 0;
		frameBuff[1] = 0;
		frameBuff[2] = 0;
		frameBuff[3] = 1;
		MemCopyNO(&frameBuff[4], &imgData[imgOfst], imgSize);
		frameBuff += 4 + imgSize;
		outSize += 4 + imgSize;

		imgOfst += imgSize;
	}
	return outSize;
}

Bool Media::Decoder::RHVCDecoder::GetVideoInfo(NN<Media::FrameInfo> info, OutParam<UInt32> frameRateNorm, OutParam<UInt32> frameRateDenorm, OutParam<UIntOS> maxFrameSize)
{
	NN<Media::VideoSource> sourceVideo;
	if (this->pps == 0 || this->sps == 0 || !this->sourceVideo.SetTo(sourceVideo))
		return false;

	sourceVideo->GetVideoInfo(info, frameRateNorm, frameRateDenorm, maxFrameSize);
	Math::Size2D<UIntOS> oriSize = info->dispSize;
	if (this->sps)
	{
		Media::H265Parser::GetFrameInfoSPS(this->sps, this->spsSize, info);
	}
	info->dispSize = oriSize;
	maxFrameSize.Set(this->maxFrameSize);
	info->fourcc = ReadNUInt32((const UInt8*)"HEVC");

	return true;
}
