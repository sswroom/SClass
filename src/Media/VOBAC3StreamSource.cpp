#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "Manage/HiResClock.h"
#include "Media/VOBAC3StreamSource.h"
#include "Media/BlockParser/AC3BlockParser.h"
#include "Sync/MutexUsage.h"
#include "Sync/SimpleThread.h"

Media::VOBAC3StreamSource::VOBAC3StreamSource(NN<Media::IStreamControl> pbc)
{
	this->pbc = pbc;
	this->fmt.formatId = 0x2000;
	this->fmt.bitRate = 0;
	this->pbEvt = 0;

	this->dataBuff = 0;
	this->buffSize = 0;
	this->buffStart = 0;
	this->buffEnd = 0;
	this->buffSample = 0;
	this->lastFrameSize = 0;
}

Media::VOBAC3StreamSource::~VOBAC3StreamSource()
{
	if (this->dataBuff)
	{
		MemFree(this->dataBuff);
		MemFree(this->dataBuff2);
	}
}

Bool Media::VOBAC3StreamSource::ParseHeader(UnsafeArray<UInt8> buff, UOSInt buffSize)
{
	if (this->fmt.bitRate == 0)
	{
		Media::BlockParser::AC3BlockParser ac3Parser;
		ac3Parser.ParseStreamFormat(buff, buffSize, this->fmt);
		if (this->fmt.bitRate != 0)
		{
			this->buffSize = this->fmt.bitRate >> 2;
			this->dataBuff = MemAlloc(UInt8, this->buffSize);
			this->dataBuff2 = MemAlloc(UInt8, this->buffSize);
		}
	}
	return this->fmt.bitRate != 0;
}

Bool Media::VOBAC3StreamSource::IsReady()
{
	return this->fmt.bitRate != 0;
}

UTF8Char *Media::VOBAC3StreamSource::GetSourceName(UTF8Char *buff)
{
	return this->pbc->GetMediaName(buff);
}

Bool Media::VOBAC3StreamSource::CanSeek()
{
	return this->pbc->CanSeek();
}

Data::Duration Media::VOBAC3StreamSource::GetStreamTime()
{
	return this->pbc->GetStreamTime();
}

Data::Duration Media::VOBAC3StreamSource::SeekToTime(Data::Duration time)
{
	Data::Duration t = this->pbc->SeekToTime(time);
	this->buffStart = 0;
	this->buffEnd = 0;
	this->SetStreamTime(t);
	return t;
}

Bool Media::VOBAC3StreamSource::TrimStream(UInt32 trimTimeStart, UInt32 trimTimeEnd, Int32 *syncTime)
{
	//////////////////////////////////
	return false;
}

void Media::VOBAC3StreamSource::GetFormat(NN<Media::AudioFormat> format)
{
	format->FromAudioFormat(this->fmt);
}

Bool Media::VOBAC3StreamSource::Start(Sync::Event *evt, UOSInt blkSize)
{
	this->pbEvt = evt;
	return this->pbc->StartAudio();
}

void Media::VOBAC3StreamSource::Stop()
{
	this->pbc->StopAudio();
	this->pbEvt = 0;
}

UOSInt Media::VOBAC3StreamSource::ReadBlock(Data::ByteArray blk)
{
	UOSInt bSize;
	Sync::MutexUsage mutUsage(this->buffMut);
	if (this->buffEnd < this->buffStart)
	{
		bSize = this->buffEnd - this->buffStart + this->buffSize;
	}
	else
	{
		bSize = this->buffEnd - this->buffStart;
	}
	if (bSize == 0 || bSize < this->lastFrameSize)
	{
		mutUsage.EndUse();
		Manage::HiResClock clk;
		while (true)
		{
			if (this->buffEnd < this->buffStart)
			{
				bSize = this->buffEnd - this->buffStart + this->buffSize;
			}
			else
			{
				bSize = this->buffEnd - this->buffStart;
			}
			if (bSize > 0 && bSize >= this->lastFrameSize)
				break;
			if (!this->pbc->IsRunning())
				break;
			Sync::SimpleThread::Sleep(10);
			if (clk.GetTimeDiff() >= 5)
				break;
		}
		mutUsage.BeginUse();
		if (this->buffStart == this->buffEnd)
		{
			mutUsage.EndUse();
			return 0;
		}
	}

	UOSInt buffSize2;
	if (this->buffStart > this->buffEnd)
	{
		MemCopyNO(this->dataBuff2, &this->dataBuff[this->buffStart], this->buffSize - this->buffStart);
		MemCopyNO(&this->dataBuff2[this->buffSize - this->buffStart], this->dataBuff, this->buffEnd);
		buffSize2 = this->buffEnd - this->buffStart + this->buffSize;
	}
	else
	{
		MemCopyNO(this->dataBuff2, &this->dataBuff[this->buffStart], this->buffEnd - this->buffStart);
		buffSize2 = this->buffEnd - this->buffStart;
	}
	UOSInt i = 0;
	UOSInt frStart = (UOSInt)-1;
	while (i < buffSize2 - 1)
	{
		if (this->dataBuff2[i] == 0xb && this->dataBuff2[i + 1] == 0x77)
		{
			frStart = i;
			break;
		}
		i++;
	}
	if (frStart != (UOSInt)-1)
	{
		if (buffSize2 - frStart < 7)
		{
			buffSize2 = frStart;
			frStart = (UOSInt)-1;
		}
		else
		{
			static UInt32 frameSize32[] = {96, 96, 120, 120, 144, 144, 168, 168, 192, 192, 240, 240, 288, 288, 336, 336, 384, 384, 480, 480, 576, 576, 672, 672, 768, 768, 960, 960, 1152, 1152, 1344, 1344, 1536, 1536, 1728, 1728, 1920, 1920};
			static UInt32 frameSize44[] = {69, 70, 87,  88,  104, 105, 121, 122, 139, 140, 174, 175, 208, 209, 243, 244, 278, 279, 348, 349, 417, 418, 487, 488, 557, 558, 696, 697, 835,  836,  975,  976,  1114, 1115, 1253, 1254, 1393, 1394};
			static UInt32 frameSize48[] = {64, 64, 80,  80,  96,  96,  112, 112, 128, 128, 160, 160, 192, 192, 224, 224, 256, 256, 320, 320, 384, 384, 448, 448, 512, 512, 640, 640, 768,  768,  896,  896,  1024, 1024, 1152, 1152, 1280, 1280};

			UInt8 frmsizecod = this->dataBuff2[frStart + 4] & 0x3f;
			UInt8 fscod = (this->dataBuff2[frStart + 4] & 0xc0) >> 6;
			UInt32 frameSize = 0;
			switch (fscod)
			{
			case 0:
				frameSize = frameSize48[frmsizecod] << 1;
				break;
			case 1:
				frameSize = frameSize44[frmsizecod] << 1;
				break;
			case 2:
				frameSize = frameSize32[frmsizecod] << 1;
				break;
			}
			this->lastFrameSize = frameSize;
			if (blk.GetSize() < (UOSInt)frStart + frameSize)
			{
				buffSize2 = frStart;
				frStart = (UOSInt)-1;
			}
			else if ((UOSInt)frStart + frameSize <= (UOSInt)buffSize2)
			{
				blk.CopyFrom(Data::ByteArrayR(this->dataBuff2, (UOSInt)frStart + frameSize));
				this->buffStart += frStart + frameSize;
				if (this->buffStart >= this->buffSize)
				{
					this->buffStart -= this->buffSize;
				}
				this->buffSample += frStart + frameSize;
				mutUsage.EndUse();
				return frStart + frameSize;
			}
			else
			{
				buffSize2 = frStart;
				frStart = (UOSInt)-1;
			}
		}
	}
	if ((UOSInt)buffSize2 >= blk.GetSize())
	{
		blk.CopyFrom(Data::ByteArrayR(this->dataBuff2, blk.GetSize()));
		this->buffStart += blk.GetSize();
		if (this->buffStart >= this->buffSize)
		{
			this->buffStart -= this->buffSize;
		}
		this->buffSample += blk.GetSize();
		mutUsage.EndUse();
		return blk.GetSize();
	}
	else
	{
		blk.CopyFrom(Data::ByteArrayR(this->dataBuff2, buffSize2));
		this->buffStart += buffSize2;
		if (this->buffStart >= this->buffSize)
		{
			this->buffStart -= this->buffSize;
		}
		this->buffSample += buffSize2;
		mutUsage.EndUse();
		return buffSize2;
	}
}

UOSInt Media::VOBAC3StreamSource::GetMinBlockSize()
{
	return this->fmt.align;
}

Data::Duration Media::VOBAC3StreamSource::GetCurrTime()
{
	return Data::Duration::FromRatioU64(this->buffSample, this->fmt.bitRate >> 3);
}

Bool Media::VOBAC3StreamSource::IsEnd()
{
	return !this->pbc->IsRunning();
}

void Media::VOBAC3StreamSource::DetectStreamInfo(UInt8 *header, UOSInt headerSize)
{
}

void Media::VOBAC3StreamSource::ClearFrameBuff()
{
	Sync::MutexUsage mutUsage(this->buffMut);
	this->buffStart = 0;
	this->buffEnd = 0;
	this->buffSample = 0;
}

void Media::VOBAC3StreamSource::SetStreamTime(Data::Duration time)
{
	Sync::MutexUsage mutUsage(this->buffMut);
	this->buffSample = time.MultiplyU64(this->fmt.bitRate >> 3);
	mutUsage.EndUse();
}

void Media::VOBAC3StreamSource::WriteFrameStream(UInt8 *buff, UOSInt buffSize)
{
	UOSInt buffWriten;
	if (buff[0] > 0)
	{
		if (this->pbEvt)
		{
			while (true)
			{
				if (this->pbEvt == 0 || !this->pbc->IsRunning())
					break;
			
				Sync::MutexUsage mutUsage(this->buffMut);
				if (this->buffStart > this->buffEnd)
				{
					buffWriten = this->buffSize - this->buffStart + this->buffEnd;
				}
				else
				{
					buffWriten = this->buffEnd - this->buffStart;
				}
				if (this->buffSize - buffWriten > buffSize - 3)
				{
					if (this->buffSize - this->buffEnd >= buffSize - 3)
					{
						MemCopyNO(&this->dataBuff[this->buffEnd], &buff[3], buffSize - 3);
						this->buffEnd += buffSize - 3;
					}
					else
					{
						MemCopyNO(&this->dataBuff[this->buffEnd], &buff[3], this->buffSize - this->buffEnd);
						MemCopyNO(this->dataBuff, &buff[3 + this->buffSize - this->buffEnd], buffSize - 3 - (this->buffSize - this->buffEnd));
						this->buffEnd = this->buffEnd + buffSize - 3 - this->buffSize;
					}
					if (this->pbEvt)
						this->pbEvt->Set();
					mutUsage.EndUse();
					break;
				}
				else
				{
					mutUsage.EndUse();
					Sync::SimpleThread::Sleep(10);
				}
			}
		}
		else
		{
			Sync::MutexUsage mutUsage(this->buffMut);
			if (this->buffStart > this->buffEnd)
			{
				buffWriten = this->buffSize - this->buffStart + this->buffEnd;
			}
			else
			{
				buffWriten = this->buffEnd - this->buffStart;
			}
			if (this->buffSize - this->buffEnd >= buffSize - 3)
			{
				MemCopyNO(&this->dataBuff[this->buffEnd], &buff[3], buffSize - 3);
				this->buffEnd += buffSize - 3;
			}
			else
			{
				MemCopyNO(&this->dataBuff[this->buffEnd], &buff[3], this->buffSize - this->buffEnd);
				MemCopyNO(this->dataBuff, &buff[3 + this->buffSize - this->buffEnd], buffSize - 3 - (this->buffSize - this->buffEnd));
				this->buffEnd = this->buffEnd + buffSize - 3 - this->buffSize;
			}
			if (buffWriten + buffSize - 3 >= this->buffSize)
			{
				this->buffStart = this->buffEnd + 1;
				if (this->buffStart >= this->buffSize)
				{
					this->buffStart -= this->buffSize;
				}
			}
		}
	}
}

Data::Duration Media::VOBAC3StreamSource::GetFrameStreamTime()
{
	Sync::MutexUsage mutUsage(this->buffMut);
	UOSInt buffSize;
	if (this->buffEnd < this->buffStart)
	{
		buffSize = this->buffEnd - this->buffStart + this->buffSize;
	}
	else
	{
		buffSize = this->buffEnd - this->buffStart;
	}
	return Data::Duration::FromRatioU64(this->buffSample + buffSize, this->fmt.bitRate >> 3);
}

void Media::VOBAC3StreamSource::EndFrameStream()
{
}

UInt64 Media::VOBAC3StreamSource::GetBitRate()
{
	return this->fmt.bitRate;
}
