#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayListInt32.h"
#include "Data/ByteTool.h"
#include "Media/MPEGVideoParser.h"
#include "Media/Decoder/MP2GDecoder.h"
#include "Sync/Thread.h"

void Media::Decoder::MP2GDecoder::ProcVideoFrame(UInt32 frameTime, UInt32 frameNum, UInt8 **imgData, UOSInt dataSize, Media::IVideoSource::FrameStruct frameStruct, Media::FrameType frameType, Media::IVideoSource::FrameFlag flags, Media::YCOffset ycOfst)
{
	Int32 srch;
	OSInt endSize = dataSize - 4;
	OSInt i;
	Int32 fieldOfst;
	OSInt endOfst;
	OSInt startOfst;
	WriteMInt32((UInt8*)&srch, 0x00000100);
	Data::ArrayListInt32 frames;
	if (ReadMInt32(imgData[0]) == 0x000001b3)
	{
		Media::FrameInfo info;
		Int32 norm;
		Int32 denorm;
		UInt64 bitRate;

		if (Media::MPEGVideoParser::GetFrameInfo(imgData[0], dataSize, &info, &norm, &denorm, &bitRate, true))
		{
			if (info.par2 != this->par)
			{
				this->par = info.par2;
				if (this->fcCb)
				{
					this->fcCb(Media::IVideoSource::FC_PAR, this->frameCbData);
				}
			}
		}
	}
	i = 0;
	while (i < endSize)
	{
		if (*(Int32*)&imgData[0][i] == srch)
		{
			frames.Add((Int32)i);
			i += 3;
		}
		i++;
	}

	Int32 outFrameTime;
	Media::FrameType outFrameType;
	Media::IVideoSource::FrameFlag outFrameFlag;
	Media::IVideoSource::FrameStruct outFrameStruct;
	Int32 outFieldOfst;
	Bool outRFF;
	Int32 ftime;

	Bool lastRFF = false;
	Media::FrameType ftype = frameType;
	Media::MPEGVideoParser::MPEGFrameProp frameProp;
	i = 1;
	endSize = frames.GetCount();
	startOfst = frames.GetItem(0);
	if (Media::MPEGVideoParser::GetFrameProp(&imgData[0][startOfst], dataSize - startOfst, &frameProp))
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
	frameStruct = Media::IVideoSource::FS_I;
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
			if (frameStruct == Media::IVideoSource::FS_B)
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
			if (frameStruct == Media::IVideoSource::FS_B)
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
		if ((outFrameFlag & Media::IVideoSource::FF_DISCONTTIME) && outFieldOfst <= 1)
		{
			outFrameFlag = (Media::IVideoSource::FrameFlag)(Media::IVideoSource::FF_DISCONTTIME | Media::IVideoSource::FF_BFRAMEPROC);
		}
		else
		{
			outFrameFlag = Media::IVideoSource::FF_BFRAMEPROC;
		}
		if (outRFF)
		{
			outFrameFlag = (Media::IVideoSource::FrameFlag)(outFrameFlag | Media::IVideoSource::FF_RFF);
		}

		if (outRFF)
		{
			ftime = outFrameTime + MulDiv32(outFieldOfst * 2 + 1, this->frameRateDenorm * 250, this->frameRateNorm);
		}
		else
		{
			ftime = outFrameTime + MulDiv32(outFieldOfst, this->frameRateDenorm * 500, this->frameRateNorm);
		}
		if (i >= endSize || !this->started)
		{
			break;
		}

		endOfst = frames.GetItem(i);
		if (this->finfoMode)
		{
			this->finfoCb(ftime, frameNum, endOfst - startOfst, outFrameStruct, outFrameType, this->finfoData, ycOfst);
		}
		else
		{
			UInt8 *imgPtr = &imgData[0][startOfst];
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


		if (Media::MPEGVideoParser::GetFrameProp(&imgData[0][startOfst], dataSize - startOfst, &frameProp))
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
				frameStruct = Media::IVideoSource::FS_P;
			}
			else if (frameProp.pictureCodingType == 'B')
			{
				frameStruct = Media::IVideoSource::FS_B;
			}
			else
			{
				frameStruct = Media::IVideoSource::FS_I;
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
		this->finfoCb(ftime, frameNum, endOfst - startOfst, frameStruct, ftype, this->finfoData, ycOfst);
	}
	else
	{
		if ((outFrameFlag & Media::IVideoSource::FF_DISCONTTIME) && endSize <= 1)
		{
			outFrameFlag = (Media::IVideoSource::FrameFlag)(Media::IVideoSource::FF_DISCONTTIME | Media::IVideoSource::FF_BFRAMEPROC);
		}
		else
		{
			outFrameFlag = Media::IVideoSource::FF_BFRAMEPROC;
		}
		UInt8 *imgPtr = &imgData[0][startOfst];
		this->frameCb(ftime, frameNum, &imgPtr, endOfst - startOfst, outFrameStruct, this->frameCbData, outFrameType, outFrameFlag, ycOfst);
	}
}

Media::Decoder::MP2GDecoder::MP2GDecoder(IVideoSource *sourceVideo, Bool toRelease) : VDecoderBase(sourceVideo)
{
	Media::FrameInfo info;
	UOSInt size;
	this->toRelease = toRelease;
	this->finfoMode = false;
	if (!sourceVideo->GetVideoInfo(&info, &this->frameRateNorm, &this->frameRateDenorm, &size))
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
	this->lastFrameFlags = Media::IVideoSource::FF_NONE;
	this->lastFrameStruct = Media::IVideoSource::FS_I;
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

const UTF8Char *Media::Decoder::MP2GDecoder::GetFilterName()
{
	return (const UTF8Char*)"MP2GDecoder";
}

OSInt Media::Decoder::MP2GDecoder::GetFrameCount()
{
	return -1;
}

UInt32 Media::Decoder::MP2GDecoder::GetFrameTime(UOSInt frameIndex)
{
	return 0;
}

void Media::Decoder::MP2GDecoder::EnumFrameInfos(FrameInfoCallback cb, void *userData)
{
	this->finfoMode = true;
	this->finfoCb = cb;
	this->finfoData = userData;
	this->Init(Media::Decoder::VDecoderBase::OnVideoFrame, 0, this);
	this->Start();
	while (this->sourceVideo->IsRunning())
	{
		Sync::Thread::Sleep(10);
	}
	this->Stop();
	this->finfoMode = false;
}

Bool Media::Decoder::MP2GDecoder::GetVideoInfo(Media::FrameInfo *info, Int32 *frameRateNorm, Int32 *frameRateDenorm, UOSInt *maxFrameSize)
{
	if (this->sourceVideo == 0)
		return false;
	Bool succ = this->sourceVideo->GetVideoInfo(info, frameRateNorm, frameRateDenorm, maxFrameSize);
	if (succ)
	{
		info->fourcc = *(Int32*)"MPG2";
		if (info->color->GetRTranParam()->GetTranType() == Media::CS::TRANT_VUNKNOWN)
		{
			Media::CS::TransferType ttype;
			if (info->dispHeight > 576)
			{
				ttype = Media::CS::TRANT_BT709;
			}
			else
			{
				ttype = Media::CS::TRANT_sRGB;
			}
			info->color->GetRTranParam()->Set(ttype, 2.2);
			info->color->GetGTranParam()->Set(ttype, 2.2);
			info->color->GetBTranParam()->Set(ttype, 2.2);
		}
	}
	info->par2 = this->par;
	return succ;
}
