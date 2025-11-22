#include "Stdafx.h"
#include "Core/ByteTool_C.h"
#include "Manage/HiResClock.h"
#include "Media/VOBLPCMStreamSource.h"
#include "Sync/MutexUsage.h"
#include "Sync/SimpleThread.h"

Media::VOBLPCMStreamSource::VOBLPCMStreamSource(NN<Media::MediaStreamControl> pbc, NN<const Media::AudioFormat> fmt)
{
	this->pbc = pbc;
	this->fmt.FromAudioFormat(fmt);
	this->pbEvt = 0;

	this->buffSize = this->fmt.bitRate >> 2;
	this->dataBuff = MemAlloc(UInt8, this->buffSize);
	this->buffStart = 0;
	this->buffEnd = 0;
	this->buffSample = 0;
}

Media::VOBLPCMStreamSource::~VOBLPCMStreamSource()
{
	MemFree(this->dataBuff);
}

UnsafeArrayOpt<UTF8Char> Media::VOBLPCMStreamSource::GetSourceName(UnsafeArray<UTF8Char> buff)
{
	return this->pbc->GetMediaName(buff);
}

Bool Media::VOBLPCMStreamSource::CanSeek()
{
	return this->pbc->CanSeek();
}

Data::Duration Media::VOBLPCMStreamSource::GetStreamTime()
{
	return this->pbc->GetStreamTime();
}

Data::Duration Media::VOBLPCMStreamSource::SeekToTime(Data::Duration time)
{
	Data::Duration t = this->pbc->SeekToTime(time);
	this->buffStart = 0;
	this->buffEnd = 0;
	this->SetStreamTime(t);
	return t;
}

Bool Media::VOBLPCMStreamSource::TrimStream(UInt32 trimTimeStart, UInt32 trimTimeEnd, OptOut<Int32> syncTime)
{
	////////////////////////////////////////////////
	return false;
}

void Media::VOBLPCMStreamSource::GetFormat(NN<Media::AudioFormat> format)
{
	format->FromAudioFormat(this->fmt);
}

Bool Media::VOBLPCMStreamSource::Start(Optional<Sync::Event> evt, UOSInt blkSize)
{
	this->pbEvt = evt;
	return this->pbc->StartAudio();
}

void Media::VOBLPCMStreamSource::Stop()
{
	this->pbc->StopAudio();
	this->pbEvt = 0;
}

UOSInt Media::VOBLPCMStreamSource::ReadBlock(Data::ByteArray blk)
{
	UOSInt buffWriten;
	UOSInt byteCopied = 0;
	UOSInt v;
	NN<Sync::Event> pbEvt;
	if (fmt.bitpersample == 24)
	{
		v = blk.GetSize() % (this->fmt.align << 1);
	}
	else
	{
		v = blk.GetSize() % this->fmt.align;
	}
	if (v)
	{
		blk = blk.WithSize(blk.GetSize() - v);
	}
	Sync::MutexUsage mutUsage(this->buffMut);
	if (this->buffStart == this->buffEnd)
	{
		mutUsage.EndUse();
		Manage::HiResClock clk;
		while (this->buffStart == this->buffEnd && this->pbc->IsRunning())
		{
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

	if (this->buffStart > this->buffEnd)
	{
		buffWriten = this->buffSize - this->buffStart + this->buffEnd;
	}
	else
	{
		buffWriten = this->buffEnd - this->buffStart;
	}

	byteCopied = 0;
	if (buffWriten > blk.GetSize())
	{
		if (this->buffStart + blk.GetSize() >= this->buffSize)
		{
			byteCopied = this->buffSize - this->buffStart;
			blk.CopyFrom(Data::ByteArrayR(&this->dataBuff[this->buffStart], this->buffSize - this->buffStart));
			blk += this->buffSize - this->buffStart;
			this->buffStart = 0;
		}
		if (blk.GetSize() > 0)
		{
			blk.CopyFrom(Data::ByteArrayR(&this->dataBuff[this->buffStart], blk.GetSize()));
			this->buffStart += blk.GetSize();
			byteCopied += blk.GetSize();
		}
		if (this->buffStart != this->buffEnd && this->pbEvt.SetTo(pbEvt))
		{
			pbEvt->Set();
		}
	}
	else
	{
		if (this->buffStart > this->buffEnd)
		{
			byteCopied = this->buffSize - this->buffStart;
			blk.CopyFrom(Data::ByteArrayR(&this->dataBuff[this->buffStart], this->buffSize - this->buffStart));
			blk += this->buffSize - this->buffStart;
			this->buffStart = 0;
		}
		if (this->buffStart < this->buffEnd)
		{
			blk.CopyFrom(Data::ByteArrayR(&this->dataBuff[this->buffStart], this->buffEnd - this->buffStart));
			byteCopied += this->buffEnd - this->buffStart;
			this->buffStart = this->buffEnd;
		}
	}
	this->buffSample += byteCopied;
	mutUsage.EndUse();

/*	if (fmt->bitpersample == 24)
	{
		_asm
		{
			mov ecx,byteCopied
			mov edi,buffStart
			cmp ecx,0
			jz lop24b
lop24a:
			mov ah,[edi+5]
			mov al,[edi]
			mov dh,[edi+2]
			mov dl,[edi+3]
			shl eax,16
			mov ah,[edi+1]
			mov al,[edi+4]
			mov dword ptr [edi],eax
			mov word ptr [edi+2],dx
			add edi,6
			sub ecx,6
			jnz lop24a
lop24b:
		}
	}
	else if (fmt->bitpersample == 16)
	{
		_asm
		{
			mov ecx,byteCopied
			mov edi,buffStart
			cmp ecx,0
			jz lop16b
lop16a:
			mov al,[edi+1]
			mov ah,[edi]
			stosw
			sub ecx,2
			jnz lop16a
lop16b:
		}
	}*/
	return byteCopied;
}

UOSInt Media::VOBLPCMStreamSource::GetMinBlockSize()
{
	return this->fmt.align;
}

Data::Duration Media::VOBLPCMStreamSource::GetCurrTime()
{
	return Data::Duration::FromRatioU64(this->buffSample * 8, this->fmt.bitRate);
}

Bool Media::VOBLPCMStreamSource::IsEnd()
{
	return !this->pbc->IsRunning();
}

void Media::VOBLPCMStreamSource::DetectStreamInfo(UInt8 *header, UOSInt headerSize)
{
}

void Media::VOBLPCMStreamSource::ClearFrameBuff()
{
	Sync::MutexUsage mutUsage(this->buffMut);
	this->buffStart = 0;
	this->buffEnd = 0;
	this->buffSample = 0;
}

void Media::VOBLPCMStreamSource::SetStreamTime(Data::Duration time)
{
	Sync::MutexUsage mutUsage(this->buffMut);
	this->buffSample = time.MultiplyU64(this->fmt.bitRate >> 3);
}

void Media::VOBLPCMStreamSource::WriteFrameStream(UInt8 *buff, UOSInt buffSize)
{
	UOSInt buffWriten;
	NN<Sync::Event> pbEvt;
//	if (buff[0] > 0)
//	{
		if (this->pbEvt.NotNull())
		{
			while (true)
			{
				if (this->pbEvt.IsNull() || !this->pbc->IsRunning())
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
				if (this->buffSize - buffWriten > buffSize - 6)
				{
					if (this->buffSize - this->buffEnd >= buffSize - 6)
					{
						MemCopyNO(&this->dataBuff[this->buffEnd], &buff[6], buffSize - 6);
						this->buffEnd += buffSize - 6;
					}
					else
					{
						MemCopyNO(&this->dataBuff[this->buffEnd], &buff[6], this->buffSize - this->buffEnd);
						MemCopyNO(this->dataBuff, &buff[6 + this->buffSize - this->buffEnd], buffSize - 6 - (this->buffSize - this->buffEnd));
						this->buffEnd = this->buffEnd + buffSize - 6 - this->buffSize;
					}
					if (this->pbEvt.SetTo(pbEvt))
						pbEvt->Set();
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
			if (this->buffSize - this->buffEnd >= buffSize - 6)
			{
				MemCopyNO(&this->dataBuff[this->buffEnd], &buff[6], buffSize - 6);
				this->buffEnd += buffSize - 6;
			}
			else
			{
				MemCopyNO(&this->dataBuff[this->buffEnd], &buff[6], this->buffSize - this->buffEnd);
				MemCopyNO(this->dataBuff, &buff[6 + this->buffSize - this->buffEnd], buffSize - 6 - (this->buffSize - this->buffEnd));
				this->buffEnd = this->buffEnd + buffSize - 6 - this->buffSize;
			}
			if (buffWriten + buffSize - 6 >= this->buffSize)
			{
				this->buffStart = this->buffEnd + 1;
				if (this->buffStart >= this->buffSize)
				{
					this->buffStart -= this->buffSize;
				}
			}
		}
//	}
}

Data::Duration Media::VOBLPCMStreamSource::GetFrameStreamTime()
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
	return Data::Duration::FromRatioU64((this->buffSample + buffSize) * 8,this->fmt.bitRate);
}

void Media::VOBLPCMStreamSource::EndFrameStream()
{
}

UInt64 Media::VOBLPCMStreamSource::GetBitRate()
{
	return this->fmt.bitRate;
}
