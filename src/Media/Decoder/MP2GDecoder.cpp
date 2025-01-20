#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayListInt32.h"
#include "Data/ByteTool.h"
#include "Media/MPEGVideoParser.h"
#include "Media/Decoder/MP2GDecoder.h"
#include "Sync/SimpleThread.h"

void Media::Decoder::MP2GDecoder::ProcVideoFrame(Data::Duration frameTime, UInt32 frameNum, UnsafeArray<UnsafeArray<UInt8>> imgData, UOSInt dataSize, Media::VideoSource::FrameStruct frameStruct, Media::FrameType frameType, Media::VideoSource::FrameFlag flags, Media::YCOffset ycOfst)
{
	Int32 srch;
	UOSInt endSize = dataSize - 4;
	UOSInt i;
	UInt32 fieldOfst;
	UOSInt endOfst;
	UOSInt startOfst;
	WriteMInt32((UInt8*)&srch, 0x00000100);
	Data::ArrayListUInt32 frames;
	if (ReadMInt32(imgData[0].Ptr()) == 0x000001b3)
	{
		Media::FrameInfo info;
		UInt32 norm;
		UInt32 denorm;
		UInt64 bitRate;

		if (Media::MPEGVideoParser::GetFrameInfo(imgData[0], dataSize, info, norm, denorm, bitRate, true))
		{
			if (info.par2 != this->par)
			{
				this->par = info.par2;
				if (this->fcCb)
				{
					this->fcCb(Media::VideoSource::FC_PAR, this->frameCbData);
				}
			}
		}
	}
	i = 0;
	while (i < endSize)
	{
		if (*(Int32*)&imgData[0][i] == srch)
		{
			frames.Add((UInt32)i);
			i += 3;
		}
		i++;
	}

	Data::Duration outFrameTime;
	Media::FrameType outFrameType;
	Media::VideoSource::FrameFlag outFrameFlag;
	Media::VideoSource::FrameStruct outFrameStruct;
	UInt32 outFieldOfst;
	Bool outRFF;
	Data::Duration ftime;

	Bool lastRFF = false;
	Media::FrameType ftype = frameType;
	Media::MPEGVideoParser::MPEGFrameProp frameProp;
	i = 1;
	endSize = frames.GetCount();
	startOfst = frames.GetItem(0);
	if (Media::MPEGVideoParser::GetFrameProp(&imgData[0][startOfst], dataSize - startOfst, frameProp))
	{
		ycOfst = Media::YCOFST_C_CENTER_LEFT;
		if (frameProp.pictureStruct == Media::MPEGVideoParser::PS_TOPFIELD)
		{
			ftype = Media::FT_MERGED_TF;
		}
		else if (frameProp.pictureStruct == Media::MPEGVideoParser::PS_BOTTOMFIELD)
		{
			ftype = Media::FT_MERGED_BF;
		}
		else
		{
			if (frameProp.progressive)
			{
				ftype = Media::FT_NON_INTERLACE;
			}
			else if (frameProp.tff)
			{
				ftype = Media::FT_INTERLACED_TFF;
			}
			else
			{
				ftype = Media::FT_INTERLACED_BFF;
			}
		}
		lastRFF = frameProp.rff;
	}
	else
	{
		ycOfst = Media::YCOFST_C_CENTER_CENTER;
	}
	frameStruct = Media::VideoSource::FS_I;
	startOfst = 0;
	fieldOfst = 0;
	while (true)
	{
/*		if (ftype == Media::FT_INTERLACED_BFF)
		{
			OSInt j = startOfst;
			Int32 srchI;
			WriteMInt32((UInt8*)&srchI, 0x000001b5);
			while (*(Int32*)&imgData[0][j] != srchI)
			{
				j++;
			}
			if ((imgData[0][j + 4] & 0xf0) == 0x80)
			{
				imgData[0][j + 7] |= 0x80;
			}
		}*/
		if (ftype == Media::FT_MERGED_TF || ftype == Media::FT_MERGED_BF)
		{
			if (frameStruct == Media::VideoSource::FS_B)
			{
				this->hasBFrame = true;
				outFrameTime = frameTime;
				outFrameType = ftype;
				outFrameFlag = flags;
				outFrameStruct = frameStruct;
				outFieldOfst = this->lastFieldOfst;
				outRFF = lastRFF;
			}
			else if (this->hasBFrame)
			{
				outFrameTime = this->lastFrameTime;
				outFrameType = ftype;
				outFrameFlag = this->lastFrameFlags;
				outFrameStruct = this->lastFrameStruct;
				outFieldOfst = this->lastFieldOfst;
				outRFF = this->lastRFF;
				this->lastFrameStruct = frameStruct;
				this->lastFrameTime = frameTime;
				this->lastFrameType = ftype;
				this->lastFrameFlags = flags;
				this->lastRFF = lastRFF;
			}
			else
			{
				outFrameTime = frameTime;
				outFrameType = ftype;
				outFrameFlag = flags;
				outFrameStruct = frameStruct;
				outFieldOfst = fieldOfst;
				outRFF = lastRFF;
				this->lastFrameStruct = frameStruct;
				this->lastFrameTime = frameTime;
				this->lastFrameType = ftype;
				this->lastFrameFlags = flags;
				this->lastRFF = lastRFF;
			}
		}
		else
		{
			if (frameStruct == Media::VideoSource::FS_B)
			{
				this->hasBFrame = true;
				outFrameTime = frameTime;
				outFrameType = ftype;
				outFrameFlag = flags;
				outFrameStruct = frameStruct;
				outFieldOfst = this->lastFieldOfst;
				outRFF = lastRFF;
			}
			else if (this->hasBFrame)
			{
				outFrameTime = this->lastFrameTime;
				outFrameType = this->lastFrameType;
				outFrameFlag = this->lastFrameFlags;
				outFrameStruct = this->lastFrameStruct;
				outFieldOfst = this->lastFieldOfst;
				outRFF = this->lastRFF;
				this->lastFrameStruct = frameStruct;
				this->lastFrameTime = frameTime;
				this->lastFrameType = ftype;
				this->lastFrameFlags = flags;
				this->lastRFF = lastRFF;
			}
			else
			{
				outFrameTime = frameTime;
				outFrameType = ftype;
				outFrameFlag = flags;
				outFrameStruct = frameStruct;
				outFieldOfst = fieldOfst;
				outRFF = lastRFF;
				this->lastFrameStruct = frameStruct;
				this->lastFrameTime = frameTime;
				this->lastFrameType = ftype;
				this->lastFrameFlags = flags;
				this->lastRFF = lastRFF;
			}
		}
		this->lastFieldOfst = fieldOfst;
		if ((outFrameFlag & Media::VideoSource::FF_DISCONTTIME) && outFieldOfst <= 1)
		{
			outFrameFlag = (Media::VideoSource::FrameFlag)(Media::VideoSource::FF_DISCONTTIME | Media::VideoSource::FF_BFRAMEPROC);
		}
		else
		{
			outFrameFlag = Media::VideoSource::FF_BFRAMEPROC;
		}
		if (outRFF)
		{
			outFrameFlag = (Media::VideoSource::FrameFlag)(outFrameFlag | Media::VideoSource::FF_RFF);
		}

		if (outRFF)
		{
			ftime = outFrameTime + Data::Duration::FromRatioU64((outFieldOfst * 2 + 1) * (UInt64)this->frameRateDenorm, this->frameRateNorm * 4);
		}
		else
		{
			ftime = outFrameTime + Data::Duration::FromRatioU64(outFieldOfst * (UInt64)this->frameRateDenorm, this->frameRateNorm * 2);
		}
		if (i >= endSize || !this->started)
		{
			break;
		}

		endOfst = frames.GetItem(i);
		if (this->finfoMode)
		{
			this->finfoCb.func(ftime, frameNum, endOfst - startOfst, outFrameStruct, outFrameType, this->finfoCb.userObj, ycOfst);
		}
		else
		{
			UnsafeArray<UInt8> imgPtr = &imgData[0][startOfst];
			this->frameCb(ftime, frameNum, &imgPtr, endOfst - startOfst, outFrameStruct, this->frameCbData, outFrameType, outFrameFlag, ycOfst);
		}
		startOfst = endOfst;
		if (outFrameType == Media::FT_MERGED_BF || outFrameType == Media::FT_MERGED_TF)
		{
			fieldOfst += 1;
		}
		else if (outRFF)
		{
			fieldOfst += 3;
			if (this->hasBFrame)
			{
				this->lastFieldOfst++;
			}
		}
		else
		{
			fieldOfst += 2;
		}


		if (Media::MPEGVideoParser::GetFrameProp(&imgData[0][startOfst], dataSize - startOfst, frameProp))
		{
			ycOfst = Media::YCOFST_C_CENTER_LEFT;
			if (frameProp.pictureStruct == Media::MPEGVideoParser::PS_TOPFIELD)
			{
				ftype = Media::FT_MERGED_TF;
			}
			else if (frameProp.pictureStruct == Media::MPEGVideoParser::PS_BOTTOMFIELD)
			{
				ftype = Media::FT_MERGED_BF;
			}
			else
			{
				if (frameProp.progressive)
				{
					ftype = Media::FT_NON_INTERLACE;
				}
				else if (frameProp.tff)
				{
					ftype = Media::FT_INTERLACED_TFF;
				}
				else
				{
					ftype = Media::FT_INTERLACED_BFF;
				}
			}

			if (frameProp.pictureCodingType == 'P')
			{
				frameStruct = Media::VideoSource::FS_P;
			}
			else if (frameProp.pictureCodingType == 'B')
			{
				frameStruct = Media::VideoSource::FS_B;
			}
			else
			{
				frameStruct = Media::VideoSource::FS_I;
			}
			lastRFF = frameProp.rff;
		}
		else
		{
			ycOfst = Media::YCOFST_C_CENTER_CENTER;
			lastRFF = false;
		}

		i++;
	}

	endOfst = dataSize;
	if (this->finfoMode)
	{
		this->finfoCb.func(ftime, frameNum, endOfst - startOfst, frameStruct, ftype, this->finfoCb.userObj, ycOfst);
	}
	else
	{
		if ((outFrameFlag & Media::VideoSource::FF_DISCONTTIME) && endSize <= 1)
		{
			outFrameFlag = (Media::VideoSource::FrameFlag)(Media::VideoSource::FF_DISCONTTIME | Media::VideoSource::FF_BFRAMEPROC);
		}
		else
		{
			outFrameFlag = Media::VideoSource::FF_BFRAMEPROC;
		}
		UnsafeArray<UInt8> imgPtr = &imgData[0][startOfst];
		this->frameCb(ftime, frameNum, &imgPtr, endOfst - startOfst, outFrameStruct, this->frameCbData, outFrameType, outFrameFlag, ycOfst);
	}
}

Media::Decoder::MP2GDecoder::MP2GDecoder(NN<VideoSource> sourceVideo, Bool toRelease) : VDecoderBase(sourceVideo)
{
	Media::FrameInfo info;
	UOSInt size;
	this->toRelease = toRelease;
	this->finfoMode = false;
	if (!sourceVideo->GetVideoInfo(info, this->frameRateNorm, this->frameRateDenorm, size))
	{
		this->sourceVideo = 0;
	}
	if (info.fourcc != *(UInt32*)"MP2G")
	{
		this->sourceVideo = 0;
	}
	this->par = info.par2;

	this->hasBFrame = false;
	this->lastFrameTime = 0;
	this->lastFrameType = Media::FT_NON_INTERLACE;
	this->lastFrameFlags = Media::VideoSource::FF_NONE;
	this->lastFrameStruct = Media::VideoSource::FS_I;
	this->lastFieldOfst = 0;
	this->lastRFF = false;
}

Media::Decoder::MP2GDecoder::~MP2GDecoder()
{
	if (this->toRelease)
	{
		DEL_CLASS(this->sourceVideo);
	}
}

Text::CStringNN Media::Decoder::MP2GDecoder::GetFilterName()
{
	return CSTR("MP2GDecoder");
}

Bool Media::Decoder::MP2GDecoder::HasFrameCount()
{
	return false;
}

UOSInt Media::Decoder::MP2GDecoder::GetFrameCount()
{
	return 0;
}

Data::Duration Media::Decoder::MP2GDecoder::GetFrameTime(UOSInt frameIndex)
{
	return 0;
}

void Media::Decoder::MP2GDecoder::EnumFrameInfos(FrameInfoCallback cb, AnyType userData)
{
	this->finfoMode = true;
	this->finfoCb = {cb, userData};
	this->Init(Media::Decoder::VDecoderBase::OnVideoFrame, 0, this);
	this->Start();
	while (this->sourceVideo->IsRunning())
	{
		Sync::SimpleThread::Sleep(10);
	}
	this->Stop();
	this->finfoMode = false;
}

Bool Media::Decoder::MP2GDecoder::GetVideoInfo(NN<Media::FrameInfo> info, OutParam<UInt32> frameRateNorm, OutParam<UInt32> frameRateDenorm, OutParam<UOSInt> maxFrameSize)
{
	if (this->sourceVideo == 0)
		return false;
	Bool succ = this->sourceVideo->GetVideoInfo(info, frameRateNorm, frameRateDenorm, maxFrameSize);
	if (succ)
	{
		info->fourcc = *(UInt32*)"MPG2";
		if (info->color.GetRTranParam()->GetTranType() == Media::CS::TRANT_VUNKNOWN)
		{
			Media::CS::TransferType ttype;
			if (info->dispSize.y > 576)
			{
				ttype = Media::CS::TRANT_BT709;
			}
			else
			{
				ttype = Media::CS::TRANT_sRGB;
			}
			info->color.GetRTranParam()->Set(ttype, 2.2);
			info->color.GetGTranParam()->Set(ttype, 2.2);
			info->color.GetBTranParam()->Set(ttype, 2.2);
		}
	}
	info->par2 = this->par;
	return succ;
}
