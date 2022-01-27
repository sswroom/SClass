#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayListInt32.h"
#include "Data/ByteTool.h"
#include "Media/H265Parser.h"
#include "Media/Decoder/RHVCDecoder.h"
#include "Sync/MutexUsage.h"
#include "Sync/Thread.h"

UOSInt Media::Decoder::RHVCDecoder::CalcNALSize(const UInt8 *buff, UOSInt buffSize)
{
	UOSInt ret = buffSize + 4;
	UOSInt i = 0;
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

UInt8 *Media::Decoder::RHVCDecoder::AppendNAL(UInt8 *outBuff, const UInt8 *srcBuff, UOSInt srcBuffSize)
{
	UOSInt i;
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

void Media::Decoder::RHVCDecoder::ProcVideoFrame(UInt32 frameTime, UInt32 frameNum, UInt8 **imgData, UOSInt dataSize, Media::IVideoSource::FrameStruct frameStruct, Media::FrameType frameType, Media::IVideoSource::FrameFlag flags, Media::YCOffset ycOfst)
{
	Sync::MutexUsage mutUsage(this->frameMut);

	UInt8 *frameBuff = this->frameBuff;
	UOSInt imgOfst = 0;
	UOSInt imgSize;
	Bool frameFound = false;

	if (flags & Media::IVideoSource::FF_DISCONTTIME)
	{
		this->discontTime = true;
		this->frameSize = 0;
		this->lastIsField = false;
	}

	if ((dataSize + this->vpsSize + this->spsSize + this->ppsSize + 12) > this->maxFrameSize)
	{
		this->maxFrameSize = dataSize + this->vpsSize + this->spsSize + this->ppsSize + 12;
		MemFreeA(this->frameBuff);
		this->frameBuff = MemAllocA(UInt8, this->maxFrameSize);
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
		if (!this->finfoCb(frameTime, frameNum, (UOSInt)(frameBuff - this->frameBuff), frameStruct, frameType, this->finfoData, ycOfst))
		{
			this->sourceVideo->Stop();
		}
	}
	else
	{
		if (this->discontTime && frameStruct != Media::IVideoSource::FS_I)
		{
		}
		else
		{
			if (this->discontTime)
			{
				flags = (Media::IVideoSource::FrameFlag)(flags | Media::IVideoSource::FF_DISCONTTIME);
			}
			else
			{
				flags = (Media::IVideoSource::FrameFlag)(flags & ~Media::IVideoSource::FF_DISCONTTIME);
			}
			this->discontTime = false;
			this->frameCb(frameTime, frameNum, &this->frameBuff, (UOSInt)(frameBuff - this->frameBuff), frameStruct, this->frameCbData, frameType, flags, Media::YCOFST_C_CENTER_LEFT);
			this->frameSize = 0;
		}
	}
	mutUsage.EndUse();
}

Media::Decoder::RHVCDecoder::RHVCDecoder(IVideoSource *sourceVideo, Bool toRelease) : Media::Decoder::VDecoderBase(sourceVideo)
{
	Media::FrameInfo info;
	UOSInt size;
	UInt32 size32;
	UInt32 frameRateNorm;
	UInt32 frameRateDenorm;
	UInt8 *buff;
	this->toRelease = toRelease;
	this->sps = 0;
	this->pps = 0;
	this->vps = 0;
	this->frameBuff = 0;
	this->frameSize = 0;
	this->spsFound = false;
	this->finfoMode = false;
	this->maxFrameSize = 0;
	this->firstFrame = true;
	this->discontTime = true;

	NEW_CLASS(this->frameMut, Sync::Mutex());
	if (!sourceVideo->GetVideoInfo(&info, &frameRateNorm, &frameRateDenorm, &size))
	{
		this->sourceVideo = 0;
		return;
	}
	if (info.fourcc != *(UInt32*)"rhvc")
	{
		this->sourceVideo = 0;
		return;
	}
	this->maxFrameSize = size;

	buff = sourceVideo->GetProp(*(Int32*)"sps", &size32);
	if (buff == 0)
	{
		this->sourceVideo = 0;
		return;
	}
	this->sps = MemAlloc(UInt8, size32);
	this->spsSize = size32;
	MemCopyNO(this->sps, buff, size32);
	this->maxFrameSize += 4 + this->spsSize;

	buff = sourceVideo->GetProp(*(Int32*)"pps", &size32);
	if (buff == 0)
	{
		this->sourceVideo = 0;
		return;
	}
	this->pps = MemAlloc(UInt8, size32);
	this->ppsSize = size32;
	MemCopyNO(this->pps, buff, size32);
	this->maxFrameSize += 4 + this->ppsSize;

	buff = sourceVideo->GetProp(*(Int32*)"vps", &size32);
	if (buff == 0)
	{
		this->sourceVideo = 0;
		return;
	}
	this->vps = MemAlloc(UInt8, size32);
	this->vpsSize = size32;
	MemCopyNO(this->vps, buff, size32);
	this->maxFrameSize += 4 + this->vpsSize;

	this->frameBuff = MemAllocA(UInt8, this->maxFrameSize);
	UOSInt oriW;
	UOSInt oriH;
	oriW = info.dispWidth;
	oriH = info.dispHeight;
	Media::H265Parser::GetFrameInfoSPS(this->sps, this->spsSize, &info);
	UOSInt cropRight = 0;
	UOSInt cropBottom = 0;
	if (info.dispWidth < oriW)
	{
		cropRight = oriW - info.dispWidth;
	}
	if (info.dispHeight < oriH)
	{
		cropBottom = oriH - info.dispHeight;
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
		DEL_CLASS(this->sourceVideo);
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
	if (this->frameBuff)
	{
		MemFreeA(this->frameBuff);
	}
	DEL_CLASS(this->frameMut);
}

Text::CString Media::Decoder::RHVCDecoder::GetFilterName()
{
	return CSTR("RHVCDecoder");
}

Bool Media::Decoder::RHVCDecoder::HasFrameCount()
{
	if (this->sourceVideo)
	{
		return this->sourceVideo->HasFrameCount();
	}
	return false;
}

UOSInt Media::Decoder::RHVCDecoder::GetFrameCount()
{
	if (this->sourceVideo)
	{
		return this->sourceVideo->GetFrameCount();
	}
	return 0;
}

UInt32 Media::Decoder::RHVCDecoder::GetFrameTime(UOSInt frameIndex)
{
	if (this->sourceVideo)
	{
		return this->sourceVideo->GetFrameTime(frameIndex);
	}
	return 0;
}
void Media::Decoder::RHVCDecoder::EnumFrameInfos(FrameInfoCallback cb, void *userData)
{
	this->finfoCb = cb;
	this->finfoData = userData;
	if (this->sourceVideo)
	{
		this->finfoMode = true;
		this->sourceVideo->Stop();
		this->sourceVideo->Init(Media::Decoder::VDecoderBase::OnVideoFrame, this->fcCb, this);
		this->sourceVideo->Start();
		while (this->sourceVideo->IsRunning())
		{
			Sync::Thread::Sleep(10);
		}
		this->finfoMode = false;
	}
}

UOSInt Media::Decoder::RHVCDecoder::GetFrameSize(UOSInt frameIndex)
{
	UOSInt srcFrameSize = 0;
	if (this->sourceVideo == 0)
		return 0;
	srcFrameSize = this->sourceVideo->GetFrameSize(frameIndex);
	if (srcFrameSize == 0)
		return 0;
	return this->maxFrameSize;
}

UOSInt Media::Decoder::RHVCDecoder::ReadFrame(UOSInt frameIndex, UInt8 *buff)
{
	if (this->sourceVideo == 0)
		return 0;
	Bool frameFound = false;
	UOSInt frameSize = this->sourceVideo->ReadFrame(frameIndex, this->frameBuff);
	UOSInt outSize = 0;
	UOSInt imgOfst = 0;
	UOSInt imgSize;
	const UInt8 *imgData = this->frameBuff;
	UInt8 *frameBuff = buff;
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

Bool Media::Decoder::RHVCDecoder::GetVideoInfo(Media::FrameInfo *info, UInt32 *frameRateNorm, UInt32 *frameRateDenorm, UOSInt *maxFrameSize)
{
	if (this->pps == 0 || this->sps == 0)
		return false;

	this->sourceVideo->GetVideoInfo(info, frameRateNorm, frameRateDenorm, maxFrameSize);
	UOSInt oriW = info->dispWidth;
	UOSInt oriH = info->dispHeight;
	if (this->sps)
	{
		Media::H265Parser::GetFrameInfoSPS(this->sps, this->spsSize, info);
	}
	info->dispWidth = oriW;
	info->dispHeight = oriH;
	*maxFrameSize = this->maxFrameSize;
	info->fourcc = ReadNUInt32((const UInt8*)"HEVC");

	return true;
}
