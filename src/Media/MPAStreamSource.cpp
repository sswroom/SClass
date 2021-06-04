#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "Manage/HiResClock.h"
#include "Media/MPAStreamSource.h"
#include "Media/BlockParser/AC3BlockParser.h"
#include "Sync/MutexUsage.h"
#include "Sync/Thread.h"

Media::MPAStreamSource::MPAStreamSource(Media::IStreamControl *pbc)
{
	this->pbc = pbc;
	NEW_CLASS(this->fmt, Media::AudioFormat());
	this->fmt->formatId = 0;
	this->fmt->bitRate = 0;
	this->pbEvt = 0;

	NEW_CLASS(this->buffMut, Sync::Mutex());
	this->dataBuff = 0;
	this->buffSize = 0;
	this->buffStart = 0;
	this->buffEnd = 0;
	this->buffSample = 0;
	this->lastFrameSize = 0;
	this->streamStarted = false;
}

Media::MPAStreamSource::~MPAStreamSource()
{
	DEL_CLASS(this->fmt);
	DEL_CLASS(this->buffMut);
	if (this->dataBuff)
	{
		MemFree(this->dataBuff);
		MemFree(this->dataBuff2);
	}
}

Bool Media::MPAStreamSource::ParseHeader(UInt8 *buff, UOSInt buffSize)
{
	if (this->fmt->formatId == 0)
	{
		while (buffSize-- > 2)
		{
			if (buff[0] == 0xff && (buff[1] & 0xf8) == 0xf8)
			{
				static UInt32 freq[4] = {44100, 48000, 32000, 0};
				this->fmt->frequency = freq[(buff[2] >> 2) & 3];
				if ((buff[3] & 0xc0) == 3)
					this->fmt->nChannels = 1;
				else
					this->fmt->nChannels = 2;
				this->fmt->bitpersample = 16;
				this->fmt->other = 0;
				this->fmt->intType = Media::AudioFormat::IT_NORMAL;


				if ((buff[1] & 0xfe) == 0xfe) //Layer 1
				{
					static UInt32 bitrate[16] = {0, 32000, 64000, 96000, 128000, 160000, 192000, 224000, 256000, 288000, 320000, 352000, 384000, 416000, 448000, 0};
					this->fmt->formatId = 0x50;
					this->fmt->bitRate = bitrate[buff[2] >> 4];
					this->fmt->extra = MemAlloc(UInt8, 22);
					this->fmt->extraSize = 22;
					if (this->fmt->frequency == 44100)
					{
						this->fmt->align = 1;
					}
					else
					{
						this->fmt->align = 4 * (12 * this->fmt->bitRate / this->fmt->frequency);
					}
					*(UInt16*)&this->fmt->extra[0] = 1;
					*(UInt32*)&this->fmt->extra[2] = this->fmt->bitRate;
					*(UInt16*)&this->fmt->extra[6] = (UInt16)(buff[3] >> 6);
					*(UInt16*)&this->fmt->extra[8] = (UInt16)((buff[3] >> 4) & 3);
					*(UInt16*)&this->fmt->extra[10] = (UInt16)((buff[3] & 3) + 1);
					*(UInt16*)&this->fmt->extra[12] = 0;
					*(UInt32*)&this->fmt->extra[14] = 0;
					*(UInt32*)&this->fmt->extra[18] = 0;
				}
				else if ((buff[1] & 0xfe) == 0xfc) //Layer 2
				{
					static UInt32 bitrate[16] = {0, 32000, 48000, 56000, 64000, 80000, 96000, 112000, 128000, 160000, 192000, 224000, 256000, 320000, 384000, 0};
					this->fmt->formatId = 0x50;
					this->fmt->bitRate = bitrate[buff[2] >> 4];
					this->fmt->extraSize = 12;
					this->fmt->extra = MemAlloc(UInt8, 12);
					if (this->fmt->frequency == 44100)
					{
						this->fmt->align = 1;
					}
					else
					{
						this->fmt->align = 144 * this->fmt->bitRate / this->fmt->frequency;
					}
					*(Int16*)&this->fmt->extra[0] = 2;
					*(Int32*)&this->fmt->extra[2] = 211712;
					*(Int32*)&this->fmt->extra[6] = 65537;
					*(Int16*)&this->fmt->extra[10] = 1;
				}
				else if ((buff[1] & 0xfe) == 0xfa) //Layer3
				{
					static UInt32 bitrate[16] = {0, 32000, 40000, 48000, 56000, 64000, 80000, 96000, 112000, 128000, 160000, 192000, 224000, 256000, 320000, 0};
					this->fmt->formatId = 0x55;
					this->fmt->bitRate = bitrate[buff[2] >> 4];
					this->fmt->extra = MemAlloc(UInt8, 12);
					this->fmt->extraSize = 12;
					if (this->fmt->frequency == 44100)
					{
						this->fmt->align = 1;
					}
					else
					{
						this->fmt->align = 144 * this->fmt->bitRate / this->fmt->frequency;
					}
					*(Int16*)&fmt->extra[0] = 1;
					*(Int32*)&fmt->extra[2] = 2;
					*(Int32*)&fmt->extra[6] = 66236;
					*(Int16*)&fmt->extra[10] = 0;

				}

				if (this->fmt->bitRate != 0)
				{
					this->buffSize = this->fmt->bitRate >> 2;
					this->dataBuff = MemAlloc(UInt8, this->buffSize);
					this->dataBuff2 = MemAlloc(UInt8, this->buffSize);
					break;
				}
			}
			buff++;
		}
	}
	return this->fmt->formatId != 0 && this->fmt->bitRate != 0;
}

Bool Media::MPAStreamSource::IsReady()
{
	return this->fmt->formatId != 0 && this->fmt->bitRate != 0;
}

UTF8Char *Media::MPAStreamSource::GetSourceName(UTF8Char *buff)
{
	return this->pbc->GetMediaName(buff);
}

Bool Media::MPAStreamSource::CanSeek()
{
	return this->pbc->CanSeek();
}

Int32 Media::MPAStreamSource::GetStreamTime()
{
	return this->pbc->GetStreamTime();
}

UInt32 Media::MPAStreamSource::SeekToTime(UInt32 time)
{
	UInt32 t = this->pbc->SeekToTime(time);
	this->buffStart = 0;
	this->buffEnd = 0;
	this->SetStreamTime(t);
	return t;
}

Bool Media::MPAStreamSource::TrimStream(UInt32 trimTimeStart, UInt32 trimTimeEnd, Int32 *syncTime)
{
	/////////////////////////////////
	return false;
}

void Media::MPAStreamSource::GetFormat(Media::AudioFormat *format)
{
	format->FromAudioFormat(this->fmt);
}

Bool Media::MPAStreamSource::Start(Sync::Event *evt, UOSInt blkSize)
{
	this->pbEvt = evt;
	this->streamStarted = true;
//	return this->pbc->StartPlay();
	return true;
}

void Media::MPAStreamSource::Stop()
{
//	this->pbc->StopPlay();
	this->streamStarted = false;
	this->pbEvt = 0;
}

UOSInt Media::MPAStreamSource::ReadBlock(UInt8 *buff, UOSInt blkSize)
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
			if (!this->pbc->IsRunning() && !this->streamStarted)
				break;
			Sync::Thread::Sleep(10);
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
		buffSize2 = (UOSInt)(this->buffEnd - this->buffStart) + this->buffSize;
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
		if (this->dataBuff2[i] == 0xff && (this->dataBuff2[i + 1] & 0xf8) == 0xf8)
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
			UInt32 frameSize = 0;
			Int32 bitrateIndex = (this->dataBuff2[i + 2] & 0xF0) >> 4;
			Int32 samplingFrequency = ((this->dataBuff2[i + 2] & 0xC) >> 2);
			UInt32 paddingBit = (this->dataBuff2[i + 2] & 2) >> 1;
			if ((this->dataBuff2[i + 1] & 0xfe) == 0xfe) //Layer 1
			{
				static UInt32 bitrate[16] = {0, 32000, 64000, 96000, 128000, 160000, 192000, 224000, 256000, 288000, 320000, 352000, 384000, 416000, 448000, 0};
				if (samplingFrequency == 0)
				{
					frameSize =  4 * (12 * bitrate[bitrateIndex] / 44100) + paddingBit;
				}
				else if (samplingFrequency == 1)
				{
					frameSize = 4 * (12 * bitrate[bitrateIndex] / 48000);
				}
				else if (samplingFrequency == 2)
				{
					frameSize = 4 * (12 * bitrate[bitrateIndex] / 32000);
				}
			}
			else if ((this->dataBuff2[i + 1] & 0xfe) == 0xfc) //Layer 2
			{
				static UInt32 bitrate[16] = {0, 32000, 48000, 56000, 64000, 80000, 96000, 112000, 128000, 160000, 192000, 224000, 256000, 320000, 384000, 0};
				if (samplingFrequency == 0)
				{
					frameSize = (144 * bitrate[bitrateIndex] / 44100) + paddingBit;
				}
				else if (samplingFrequency == 1)
				{
					frameSize = 144 * bitrate[bitrateIndex] / 48000;
				}
				else if (samplingFrequency == 2)
				{
					frameSize = 144 * bitrate[bitrateIndex] / 32000;
				}
			}
			else if ((this->dataBuff2[i + 1] & 0xfe) == 0xfa) //Layer3
			{
				static UInt32 bitrate[16] = {0, 32000, 40000, 48000, 56000, 64000, 80000, 96000, 112000, 128000, 160000, 192000, 224000, 256000, 320000, 0};
				if (samplingFrequency == 0)
				{
					frameSize = (144 * bitrate[bitrateIndex] / 44100) + paddingBit;
				}
				else if (samplingFrequency == 1)
				{
					frameSize = 144 * bitrate[bitrateIndex] / 48000;
				}
				else if (samplingFrequency == 2)
				{
					frameSize = 144 * bitrate[bitrateIndex] / 32000;
				}
			}
			if (frameSize == 0)
			{
				frameSize = 4;
			}

			this->lastFrameSize = frameSize;
			if (blkSize < frStart + frameSize)
			{
				buffSize2 = frStart;
				frStart = (UOSInt)-1;
			}
			else if (frStart + frameSize <= buffSize2)
			{
				MemCopyNO(buff, this->dataBuff2, frStart + frameSize);
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
	if (buffSize2 >= blkSize)
	{
		MemCopyNO(buff, this->dataBuff2, blkSize);
		this->buffStart += blkSize;
		if (this->buffStart >= this->buffSize)
		{
			this->buffStart -= this->buffSize;
		}
		this->buffSample += blkSize;
		mutUsage.EndUse();
		return blkSize;
	}
	else
	{
		MemCopyNO(buff, this->dataBuff2, buffSize2);
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

UOSInt Media::MPAStreamSource::GetMinBlockSize()
{
	UOSInt size = this->fmt->align;
	if (size == 1)
	{
		size = 144 * this->fmt->bitRate / this->fmt->frequency + 1;
	}
	return size;
}

UInt32 Media::MPAStreamSource::GetCurrTime()
{
	return (UInt32)(this->buffSample * 8000LL / this->fmt->bitRate);
}

Bool Media::MPAStreamSource::IsEnd()
{
	return !this->pbc->IsRunning();
}

void Media::MPAStreamSource::DetectStreamInfo(UInt8 *header, UOSInt headerSize)
{
}

void Media::MPAStreamSource::ClearFrameBuff()
{
	Sync::MutexUsage mutUsage(this->buffMut);
	this->buffStart = 0;
	this->buffEnd = 0;
	this->buffSample = 0;
	mutUsage.EndUse();
}

void Media::MPAStreamSource::SetStreamTime(UInt32 time)
{
	Sync::MutexUsage mutUsage(this->buffMut);
	this->buffSample = (UInt64)time * this->fmt->bitRate / 8000;
	mutUsage.EndUse();
}

void Media::MPAStreamSource::WriteFrameStream(UInt8 *buff, UOSInt buffSize)
{
	UOSInt buffWriten;
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
			if (this->buffSize - buffWriten > buffSize)
			{
				if (this->buffSize - this->buffEnd >= buffSize)
				{
					MemCopyNO(&this->dataBuff[this->buffEnd], buff, buffSize);
					this->buffEnd += buffSize;
				}
				else
				{
					MemCopyNO(&this->dataBuff[this->buffEnd], buff, this->buffSize - this->buffEnd);
					MemCopyNO(this->dataBuff, &buff[this->buffSize - this->buffEnd], buffSize - (this->buffSize - this->buffEnd));
					this->buffEnd = this->buffEnd + buffSize - this->buffSize;
				}
				if (this->pbEvt)
					this->pbEvt->Set();
				mutUsage.EndUse();
				break;
			}
			else
			{
				mutUsage.EndUse();
				Sync::Thread::Sleep(10);
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
		if (this->buffSize - this->buffEnd >= buffSize)
		{
			MemCopyNO(&this->dataBuff[this->buffEnd], buff, buffSize);
			this->buffEnd += buffSize;
		}
		else
		{
			MemCopyNO(&this->dataBuff[this->buffEnd], buff, this->buffSize - this->buffEnd);
			MemCopyNO(this->dataBuff, &buff[this->buffSize - this->buffEnd], buffSize - (this->buffSize - this->buffEnd));
			this->buffEnd = this->buffEnd + buffSize - this->buffSize;
		}
		if (buffWriten + buffSize >= this->buffSize)
		{
			this->buffStart = this->buffEnd + 1;
			this->buffSample += buffWriten + buffSize - this->buffSize + 1;
			if (this->buffStart >= this->buffSize)
			{
				this->buffStart -= this->buffSize;
			}
		}
		mutUsage.EndUse();
	}
}

Int32 Media::MPAStreamSource::GetFrameStreamTime()
{
	Int32 t;
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
	t = (Int32)((this->buffSample + buffSize) * 8000LL / this->fmt->bitRate);
	mutUsage.EndUse();
	return t;
}

void Media::MPAStreamSource::EndFrameStream()
{
}

UInt64 Media::MPAStreamSource::GetBitRate()
{
	return this->fmt->bitRate;
}
