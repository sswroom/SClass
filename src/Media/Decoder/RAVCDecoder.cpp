#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayListInt32.h"
#include "Core/ByteTool_C.h"
#include "Media/H264Parser.h"
#include "Media/Decoder/RAVCDecoder.h"
#include "Sync/MutexUsage.h"
#include "Sync/SimpleThread.h"

#define ALLONE 1

void Media::Decoder::RAVCDecoder::ProcVideoFrame(Data::Duration frameTime, UInt32 frameNum, UnsafeArray<UnsafeArray<UInt8>> imgData, UIntOS dataSize, Media::VideoSource::FrameStruct frameStruct, Media::FrameType frameType, Media::VideoSource::FrameFlag flags, Media::YCOffset ycOfst)
{
	Sync::MutexUsage mutUsage(this->frameMut);
	Bool found = false;
	Media::FrameType seiFrameType = Media::FT_INTERLACED_TFF;

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


	while (imgOfst < dataSize)
	{
		if (this->firstFrame)
		{
			UInt32 size;
			this->firstFrame = false;
			size = ReadMUInt32(&imgData[0][imgOfst]);
			this->size32 = ((size & 0xffff0000) == 0) || (size + 4 == dataSize);
		}
		if (this->size32)
		{
			imgSize = ReadMUInt32(&imgData[0][imgOfst]);
			imgOfst += 4;
		}
		else
		{
			imgSize = ReadMUInt16(&imgData[0][imgOfst]);
			imgOfst += 2;
		}
		if (imgSize <= 0)
		{
			break;
		}
		if (imgSize + imgOfst > dataSize)
		{
			imgSize = dataSize - imgOfst;
			if (imgSize <= 0)
			{
				break;
			}
		}

		
		if ((imgData[0][imgOfst] & 0x1f) == 5 && !found && !this->spsFound)
		{
			UIntOS size = this->BuildIFrameHeader(frameBuff, false);
			frameBuff += size;
			this->frameSize += size;
			found = true;
		}

#if defined(ALLONE)
		WriteMInt32(&frameBuff[0], 1);
#else
		WriteMInt32(&frameBuff[0], imgSize);
#endif
		MemCopyNAC(&frameBuff[4], &imgData[0][imgOfst], imgSize);
		if ((imgData[0][imgOfst] & 0x1f) == 7) //seq_parameter_set_rbsp( )
		{
			Media::FrameInfo info;
			Media::H264Parser::GetFrameInfo(frameBuff, imgSize + 4, info, h264Flags);
			this->spsFound = true;
		}
		else if ((imgData[0][imgOfst] & 0x1f) == 8) //pic_parameter_set_rbsp( )
		{
			this->spsFound = true;
		}
		else if ((imgData[0][imgOfst] & 0x1f) == 6) //sei_rbsp( )
		{
			UIntOS i = 1;
			UInt32 payloadType = 0;
//			UInt32 payloadSize = 0;
			if (imgData[0][imgOfst + i] == 0xff)
			{
				payloadType += 255;
				i++;
			}
			payloadType += imgData[0][imgOfst + i];
			i++;
			if (imgData[0][imgOfst + i] == 0xff)
			{
//				payloadSize += 255;
				i++;
			}
//			payloadSize += imgData[0][imgOfst + i];
			i++;

			if (payloadType == 1)
			{
				UInt32 cpb_removal_delay;
				UInt32 dpb_output_delay;
				UInt32 pic_struct;
				IO::BitReaderMSB reader(&imgData[0][imgOfst + i], imgSize - i);
				if (h264Flags.nal_hrd_parameters_present_flag || h264Flags.vcl_hrd_parameters_present_flag)
				{
					reader.ReadBits(cpb_removal_delay, h264Flags.cpb_removal_delay_length_minus1 + 1);
					reader.ReadBits(dpb_output_delay, h264Flags.dpb_output_delay_length_minus1 + 1);
				}
				if (h264Flags.pic_struct_present_flag)
				{
					if (reader.ReadBits(pic_struct, 5))
					{
						if (pic_struct == 0) //(progressive) frame
						{
							seiFrameType = Media::FT_NON_INTERLACE;
						}
						else if (pic_struct == 1) //top field
						{
							seiFrameType = Media::FT_MERGED_TF;
						}
						else if (pic_struct == 2) //bottom field
						{
							seiFrameType = Media::FT_MERGED_BF;
						}
						else if (pic_struct == 3) //top field, bottom field, in that order
						{
							seiFrameType = Media::FT_INTERLACED_TFF;
						}
						else if (pic_struct == 4) //bottom field, top field, in that order
						{
							seiFrameType = Media::FT_INTERLACED_BFF;
						}
					}
				}
			}
		}
		else if ((imgData[0][imgOfst] & 0x1f) == 9) //access_unit_delimiter_rbsp( )
		{
			if (frameFound)
			{
				if (this->discontTime && frameStruct != Media::VideoSource::FS_I)
				{
					this->frameSize = 0;
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
					this->frameCb(frameTime, frameNum, &this->frameBuff, this->frameSize, frameStruct, this->frameCbData, frameType, flags, Media::YCOFST_C_CENTER_LEFT);
					this->frameSize = 0;
				}
				frameFound = false;
				frameBuff = this->frameBuff;
				frameTime = frameTime.AddMS(16);
#if defined(ALLONE)
				WriteMInt32(&frameBuff[0], 1);
#else
				WriteMInt32(&frameBuff[0], imgSize);
#endif
				MemCopyNAC(&frameBuff[4], &imgData[0][imgOfst], imgSize);
				found = false;
			}
		}
		else if ((imgData[0][imgOfst] & 0x1f) == 5 || (imgData[0][imgOfst] & 0x1f) == 1) //5: slice_layer_without_partitioning_rbsp( ), 1: slice_layer_without_partitioning_rbsp( )
		{
			UInt32 v;
			IO::BitReaderMSB reader(&imgData[0][imgOfst + 1], imgSize - 1);
			Media::H264Parser::ParseVari(reader, v);
			v = (UInt32)-1;
			Media::H264Parser::ParseVari(reader, v);
			if (!frameFound)
			{
				switch (v)
				{
				case 0:
					frameStruct = Media::VideoSource::FS_P;
					break;
				case 1:
					frameStruct = Media::VideoSource::FS_B;
					break;
				case 2:
					frameStruct = Media::VideoSource::FS_I;
					break;
				case 3:
					frameStruct = Media::VideoSource::FS_P;
					break;
				case 4:
					frameStruct = Media::VideoSource::FS_I;
					break;
				case 5:
					frameStruct = Media::VideoSource::FS_P;
					break;
				case 6:
					frameStruct = Media::VideoSource::FS_B;
					break;
				case 7:
					frameStruct = Media::VideoSource::FS_I;
					break;
				case 8:
					frameStruct = Media::VideoSource::FS_P;
					break;
				case 9:
					frameStruct = Media::VideoSource::FS_I;
					break;
				}
			}
			Media::H264Parser::ParseVari(reader, v); //pic_parameter_set_id
			if (this->h264Flags.separateColourPlane)
			{
				reader.ReadBits(v, 2); //colour_plane_id
			}
			reader.ReadBits(v, this->h264Flags.maxFrameNum_4 + 4); //frame_num
			if (!this->h264Flags.frameOnly)
			{
				reader.ReadBits(v, 1); //field_pic_flag
				if (v == 1)
				{
					if (reader.ReadBits(v, 1)) //bottom_field_flag
					{
						if (v == 0)
						{
							frameType = Media::FT_MERGED_TF;
						}
						else
						{
							frameType = Media::FT_MERGED_BF;
						}
/*						if (!frameFound)
						{
							if (lastIsField)
							{
								lastIsField = false;
								if (v == 0)
								{
									frameType = Media:FT_INTERLACED_BFF;
								}
								else
								{
									frameType = Media::FT_INTERLACED_TFF;
								}
								frameTime = lastFieldTime;
							}
							else
							{
								lastIsField = true;
								lastFieldTime = frameTime;
								frameType = Media::FT_DISCARD;
							}
						}*/
					}
				}
				else
				{
					if (!frameFound)
					{
						if (this->h264Flags.mbaff)
						{
							frameType = seiFrameType;
						}
						else
						{
							frameType = Media::FT_NON_INTERLACE;
						}
					}
				}
			}
			else
			{
				if (!frameFound)
				{
					frameType = Media::FT_NON_INTERLACE;
				}
			}
			frameFound = true;
		}

		this->frameSize += 4 + imgSize;
		frameBuff += imgSize + 4;

		imgOfst += imgSize;
	}

	if (this->finfoMode)
	{
		if (!this->finfoCb.func(frameTime, frameNum, frameSize, frameStruct, frameType, this->finfoCb.userObj, ycOfst))
		{
			this->sourceVideo->Stop();
		}
		this->frameSize = 0;
	}
	else
	{
		if (this->discontTime && frameStruct != Media::VideoSource::FS_I)
		{
			this->frameSize = 0;
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
			if (frameNum == 0x24)
			{
				frameNum = 0x24;
			}
			this->frameCb(frameTime, frameNum, &this->frameBuff, this->frameSize, frameStruct, this->frameCbData, frameType, flags, Media::YCOFST_C_CENTER_LEFT);
			this->frameSize = 0;
		}
	}
	mutUsage.EndUse();
}

Media::Decoder::RAVCDecoder::RAVCDecoder(NN<VideoSource> sourceVideo, Bool toRelease, Bool skipHeader) : Media::Decoder::VDecoderBase(sourceVideo)
{
	Media::FrameInfo info;
	UIntOS size;
	UInt32 size32;
	UInt32 frameRateNorm;
	UInt32 frameRateDenorm;
	UInt8 *buff;
	this->toRelease = toRelease;
	this->sps = 0;
	this->pps = 0;
	this->frameSize = 0;
	this->size32 = false;
	this->spsFound = false;
	this->finfoMode = false;
	this->maxFrameSize = 0;
	this->firstFrame = true;
	this->discontTime = true;
	this->skipHeader = skipHeader;
	MemClear(&this->h264Flags, sizeof(this->h264Flags));

	if (!sourceVideo->GetVideoInfo(info, frameRateNorm, frameRateDenorm, size))
	{
		this->frameBuff = MemAllocAArr(UInt8, this->maxFrameSize);
		this->sourceVideo = 0;
		return;
	}
	if (info.fourcc != *(UInt32*)"ravc")
	{
		this->frameBuff = MemAllocAArr(UInt8, this->maxFrameSize);
		this->sourceVideo = 0;
		return;
	}
	this->maxFrameSize = size;
	buff = sourceVideo->GetProp(*(Int32*)"sps", &size32);
	if (buff == 0)
	{
		this->frameBuff = MemAllocAArr(UInt8, this->maxFrameSize);
		this->sourceVideo = 0;
		return;
	}
	this->sps = MemAlloc(UInt8, size32);
	this->spsSize = size32;
	MemCopyNO(this->sps, buff, size32);
	this->maxFrameSize += 8 + size32;

	buff = sourceVideo->GetProp(*(Int32*)"pps", &size32);
	if (buff == 0)
	{
		this->frameBuff = MemAllocAArr(UInt8, this->maxFrameSize);
		this->sourceVideo = 0;
		return;
	}
	this->pps = MemAlloc(UInt8, size32);
	this->ppsSize = size32;
	MemCopyNO(this->pps, buff, size32);
	this->maxFrameSize += 4 + size32;
	this->frameBuff = MemAllocAArr(UInt8, this->maxFrameSize);
//	IntOS oriW;
//	IntOS oriH;
//	oriW = info.dispSize.x;
//	oriH = info.dispSize.y;
	size = this->BuildIFrameHeader(this->frameBuff, true);
	Media::H264Parser::GetFrameInfo(this->frameBuff, size, info, this->h264Flags);
/*	IntOS cropRight = 0;
	IntOS cropBottom = 0;
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
	}*/
}

Media::Decoder::RAVCDecoder::~RAVCDecoder()
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
	MemFreeAArr(this->frameBuff);
}

Text::CStringNN Media::Decoder::RAVCDecoder::GetFilterName()
{
	return CSTR("RAVCDecoder");
}

Bool Media::Decoder::RAVCDecoder::HasFrameCount()
{
	if (this->sourceVideo)
	{
		return this->sourceVideo->HasFrameCount();
	}
	return false;
}

UIntOS Media::Decoder::RAVCDecoder::GetFrameCount()
{
	if (this->sourceVideo)
	{
		return this->sourceVideo->GetFrameCount();
	}
	return 0;
}

Data::Duration Media::Decoder::RAVCDecoder::GetFrameTime(UIntOS frameIndex)
{
	if (this->sourceVideo)
	{
		return this->sourceVideo->GetFrameTime(frameIndex);
	}
	return 0;
}
void Media::Decoder::RAVCDecoder::EnumFrameInfos(FrameInfoCallback cb, AnyType userData)
{
	this->finfoCb = {cb, userData};
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
	}
}

UIntOS Media::Decoder::RAVCDecoder::GetFrameSize(UIntOS frameIndex)
{
	UIntOS srcFrameSize = 0;
	if (this->sourceVideo == 0)
		return 0;
	srcFrameSize = this->sourceVideo->GetFrameSize(frameIndex);
	if (srcFrameSize == 0)
		return 0;
	return this->maxFrameSize;
}

UIntOS Media::Decoder::RAVCDecoder::ReadFrame(UIntOS frameIndex, UnsafeArray<UInt8> buff)
{
	if (this->sourceVideo == 0)
		return 0;
	Bool found = false;
	UIntOS frameSize = this->sourceVideo->ReadFrame(frameIndex, this->frameBuff);
	UIntOS outSize = 0;
	UIntOS imgOfst = 0;
	UIntOS imgSize;
	UnsafeArray<const UInt8> imgData = this->frameBuff;
	UnsafeArray<UInt8> frameBuff = buff;
	while (imgOfst < frameSize)
	{
		if (this->firstFrame)
		{
			UInt32 size;
			this->firstFrame = false;
			size = ReadMUInt32(&imgData[imgOfst]);
			this->size32 = ((size & 0xffff0000) == 0) || (size + 4 == frameSize);
		}
		if (this->size32)
		{
			imgSize = ReadMUInt32(&imgData[imgOfst]);
			imgOfst += 4;
		}
		else
		{
			imgSize = ReadMUInt16(&imgData[imgOfst]);
			imgOfst += 2;
		}
		if (imgSize <= 0)
		{
			break;
		}
		if (imgSize + imgOfst > frameSize)
		{
			imgSize = frameSize - imgOfst;
			if (imgSize <= 0)
			{
				break;
			}
		}

		
		if ((imgData[imgOfst] & 0x1f) == 5 && !found && !this->spsFound)
		{
			UIntOS size = this->BuildIFrameHeader(frameBuff, false);
			frameBuff += size;
			outSize += size;
			found = true;
		}

#if defined(ALLONE)
		WriteMInt32(&frameBuff[0], 1);
#else
		WriteMInt32(&frameBuff[0], imgSize);
#endif
		MemCopyNAC(&frameBuff[4], &imgData[imgOfst], imgSize);
		if ((imgData[imgOfst] & 0x1f) == 7) //seq_parameter_set_rbsp( )
		{
			this->spsFound = true;
		}
		else if ((imgData[imgOfst] & 0x1f) == 8) //pic_parameter_set_rbsp( )
		{
			this->spsFound = true;
		}
		else if ((imgData[imgOfst] & 0x1f) == 6) //sei_rbsp( )
		{
		}
		else if ((imgData[imgOfst] & 0x1f) == 9) //access_unit_delimiter_rbsp( )
		{
		}
		else if ((imgData[imgOfst] & 0x1f) == 5 || (imgData[imgOfst] & 0x1f) == 1) //5: slice_layer_without_partitioning_rbsp( ), 1: slice_layer_without_partitioning_rbsp( )
		{
		}

		outSize += 4 + imgSize;
		frameBuff += imgSize + 4;

		imgOfst += imgSize;
	}
	return outSize;
}

Bool Media::Decoder::RAVCDecoder::GetVideoInfo(NN<Media::FrameInfo> info, OutParam<UInt32> frameRateNorm, OutParam<UInt32> frameRateDenorm, OutParam<UIntOS> maxFrameSize)
{
	if (this->pps == 0 || this->sps == 0)
		return false;

	UIntOS size = this->BuildIFrameHeader(this->frameBuff, true);
	this->sourceVideo->GetVideoInfo(info, frameRateNorm, frameRateDenorm, maxFrameSize);
	Math::Size2D<UIntOS> oriSize = info->dispSize;
	Media::H264Parser::GetFrameInfo(this->frameBuff, size, info, nullptr);
	info->dispSize = oriSize;
	maxFrameSize.Set(this->maxFrameSize);
	info->fourcc = ReadNUInt32((const UInt8*)"h264");

	return true;
}

UIntOS Media::Decoder::RAVCDecoder::BuildIFrameHeader(UnsafeArray<UInt8> buff, Bool forceBuild)
{
	if (this->skipHeader && !forceBuild)
	{
		return 0;
	}
	WriteMInt32(&buff[0], 1);
	MemCopyNO(&buff[4], this->sps, this->spsSize);
	WriteMInt32(&buff[this->spsSize + 4], 1);
	MemCopyNO(&buff[this->spsSize + 8], this->pps, this->ppsSize);
	return this->spsSize + 8 + this->ppsSize;
}
