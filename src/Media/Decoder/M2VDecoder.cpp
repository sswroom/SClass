#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayListInt32.h"
#include "Data/ByteTool.h"
#include "Media/MPEGVideoParser.h"
#include "Media/Decoder/M2VDecoder.h"
#include "Sync/ThreadUtil.h"

void Media::Decoder::M2VDecoder::ProcVideoFrame(UInt32 frameTime, UInt32 frameNum, UInt8 **imgData, UOSInt dataSize, Media::IVideoSource::FrameStruct frameStruct, Media::FrameType frameType, Media::IVideoSource::FrameFlag flags, Media::YCOffset ycOfst)
{
	Int32 srch;
	WriteMInt32((UInt8*)&srch, 0x00000100);
	UInt32 startOfst = 0;
	if (ReadMInt32(imgData[0]) == 0x000001b3)
	{
		Media::FrameInfo info;
		UInt32 norm;
		UInt32 denorm;
		UInt64 bitRate;

		if (Media::MPEGVideoParser::GetFrameInfo(imgData[0], dataSize, info, norm, denorm, &bitRate, true))
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
	UOSInt endSize = dataSize - 4;
	UOSInt i = 0;
	while (i < endSize)
	{
		if (*(Int32*)&imgData[0][i] == srch)
		{
			startOfst = (UInt32)i;
			break;
		}
		i++;
	}
	Media::MPEGVideoParser::MPEGFrameProp frameProp;
	if (Media::MPEGVideoParser::GetFrameProp(&imgData[0][startOfst], dataSize - startOfst, &frameProp))
	{
		if (frameProp.pictureCodingType == 'I')
		{
			frameStruct = Media::IVideoSource::FS_I;
		}
		else if (frameProp.pictureCodingType == 'P')
		{
			frameStruct = Media::IVideoSource::FS_P;
		}
		else if (frameProp.pictureCodingType == 'B')
		{
			frameStruct = Media::IVideoSource::FS_B;
		}
		ycOfst = Media::YCOFST_C_CENTER_LEFT;
		if (frameProp.pictureStruct == Media::MPEGVideoParser::PS_TOPFIELD)
		{
			frameType = Media::FT_MERGED_TF;
		}
		else if (frameProp.pictureStruct == Media::MPEGVideoParser::PS_BOTTOMFIELD)
		{
			frameType = Media::FT_MERGED_BF;
		}
		else
		{
			if (frameProp.progressive)
			{
				frameType = Media::FT_NON_INTERLACE;
			}
			else if (frameProp.tff)
			{
				frameType = Media::FT_INTERLACED_TFF;
			}
			else
			{
				frameType = Media::FT_INTERLACED_BFF;
			}
		}
	}
	else
	{
		ycOfst = Media::YCOFST_C_CENTER_CENTER;
		frameStruct = Media::IVideoSource::FS_I;
	}
	if (flags & Media::IVideoSource::FF_DISCONTTIME)
	{
		this->discTime = true;
	}
	if (this->discTime)
	{
		if (frameStruct == Media::IVideoSource::FS_I)
		{
			this->discTime = false;
			flags = (Media::IVideoSource::FrameFlag)(flags | Media::IVideoSource::FF_DISCONTTIME);
		}
		else
		{
			return;
		}
	}
	this->frameCb(frameTime, frameNum, &imgData[0], dataSize, frameStruct, this->frameCbData, frameType, flags, ycOfst);
}

Media::Decoder::M2VDecoder::M2VDecoder(IVideoSource *sourceVideo, Bool toRelease) : Media::Decoder::VDecoderBase(sourceVideo)
{
	Media::FrameInfo info;
	UOSInt size;
	UInt32 frameRateNorm;
	UInt32 frameRateDenorm;
	this->discTime = true;
	this->par = 1;
	this->toRelease = toRelease;
	if (!sourceVideo->GetVideoInfo(info, frameRateNorm, frameRateDenorm, size))
	{
		this->sourceVideo = 0;
		return;
	}
	if (info.fourcc != *(UInt32*)"m2v1")
	{
		this->sourceVideo = 0;
		return;
	}
	this->par = info.par2;
}

Media::Decoder::M2VDecoder::~M2VDecoder()
{
	if (this->toRelease)
	{
		DEL_CLASS(this->sourceVideo);
	}
}

Text::CString Media::Decoder::M2VDecoder::GetFilterName()
{
	return CSTR("M2VDecoder");
}

Bool Media::Decoder::M2VDecoder::HasFrameCount()
{
	if (this->sourceVideo)
	{
		return this->sourceVideo->HasFrameCount();
	}
	return false;
}

UOSInt Media::Decoder::M2VDecoder::GetFrameCount()
{
	if (this->sourceVideo)
	{
		return this->sourceVideo->GetFrameCount();
	}
	return 0;
}

UInt32 Media::Decoder::M2VDecoder::GetFrameTime(UOSInt frameIndex)
{
	if (this->sourceVideo)
	{
		return this->sourceVideo->GetFrameTime(frameIndex);
	}
	return 0;
}

void Media::Decoder::M2VDecoder::EnumFrameInfos(FrameInfoCallback cb, void *userData)
{
/*	this->finfoCb = cb;
	this->finfoData = userData;
	if (this->sourceVideo)
	{
		this->finfoMode = true;
		this->sourceVideo->Stop();
		this->sourceVideo->Init(Media::Decoder::VDecoderBase::OnVideoFrame, this->fcCb, this);
		this->sourceVideo->Start();
		while (this->sourceVideo->IsRunning())
		{
			Sync::SimpleThread::Sleep(10);
		}
		this->finfoMode = false;
	}*/
}

UOSInt Media::Decoder::M2VDecoder::GetFrameSize(UOSInt frameIndex)
{
	if (this->sourceVideo)
	{
		return this->sourceVideo->GetFrameSize(frameIndex);
	}
	return 0;
}

UOSInt Media::Decoder::M2VDecoder::ReadFrame(UOSInt frameIndex, UInt8 *buff)
{
	if (this->sourceVideo)
	{
		return this->sourceVideo->ReadFrame(frameIndex, buff);
	}
	return 0;
}

Bool Media::Decoder::M2VDecoder::GetVideoInfo(NotNullPtr<Media::FrameInfo> info, OutParam<UInt32> frameRateNorm, OutParam<UInt32> frameRateDenorm, OutParam<UOSInt> maxFrameSize)
{
	if (this->sourceVideo == 0)
		return false;

	this->sourceVideo->GetVideoInfo(info, frameRateNorm, frameRateDenorm, maxFrameSize);
	info->fourcc = ReadNUInt32((const UInt8*)"MPG2");
	info->par2 = this->par;

	return true;
}
