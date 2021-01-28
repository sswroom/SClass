#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "Manage/HiResClock.h"
#include "Media/VOBLPCMStreamSource.h"
#include "Sync/Thread.h"

Media::VOBLPCMStreamSource::VOBLPCMStreamSource(Media::IStreamControl *pbc, Media::AudioFormat *fmt)
{
	this->pbc = pbc;
	NEW_CLASS(this->fmt, Media::AudioFormat());
	this->fmt->FromAudioFormat(fmt);
	this->pbEvt = 0;

	NEW_CLASS(this->buffMut, Sync::Mutex());
	this->buffSize = this->fmt->bitRate >> 2;
	this->dataBuff = MemAlloc(UInt8, this->buffSize);
	this->buffStart = 0;
	this->buffEnd = 0;
	this->buffSample = 0;
}

Media::VOBLPCMStreamSource::~VOBLPCMStreamSource()
{
	DEL_CLASS(this->fmt);
	DEL_CLASS(this->buffMut);
	MemFree(this->dataBuff);
}

UTF8Char *Media::VOBLPCMStreamSource::GetSourceName(UTF8Char *buff)
{
	return this->pbc->GetMediaName(buff);
}

Bool Media::VOBLPCMStreamSource::CanSeek()
{
	return this->pbc->CanSeek();
}

Int32 Media::VOBLPCMStreamSource::GetStreamTime()
{
	return this->pbc->GetStreamTime();
}

Int32 Media::VOBLPCMStreamSource::SeekToTime(Int32 time)
{
	Int32 t = this->pbc->SeekToTime(time);
	this->buffStart = 0;
	this->buffEnd = 0;
	this->SetStreamTime(t);
	return t;
}

Bool Media::VOBLPCMStreamSource::TrimStream(Int32 trimTimeStart, Int32 trimTimeEnd, Int32 *syncTime)
{
	////////////////////////////////////////////////
	return false;
}

void Media::VOBLPCMStreamSource::GetFormat(Media::AudioFormat *format)
{
	format->FromAudioFormat(this->fmt);
}

Bool Media::VOBLPCMStreamSource::Start(Sync::Event *evt, UOSInt blkSize)
{
	this->pbEvt = evt;
	return this->pbc->StartAudio();
}

void Media::VOBLPCMStreamSource::Stop()
{
	this->pbc->StopAudio();
	this->pbEvt = 0;
}

UOSInt Media::VOBLPCMStreamSource::ReadBlock(UInt8 *buff, UOSInt blkSize)
{
	UOSInt buffWriten;
	UOSInt byteCopied = 0;
	UOSInt v;
	if (fmt->bitpersample == 24)
	{
		v = blkSize % (this->fmt->align << 1);
	}
	else
	{
		v = blkSize % this->fmt->align;
	}
	if (v)
	{
		blkSize = blkSize - v;
	}
	this->buffMut->Lock();
	if (this->buffStart == this->buffEnd)
	{
		this->buffMut->Unlock();
		Manage::HiResClock clk;
		while (this->buffStart == this->buffEnd && this->pbc->IsRunning())
		{
			Sync::Thread::Sleep(10);
			if (clk.GetTimeDiff() >= 5)
				break;
		}
		this->buffMut->Lock();
		if (this->buffStart == this->buffEnd)
		{
			this->buffMut->Unlock();
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
	if (buffWriten > blkSize)
	{
		if (this->buffStart + blkSize >= this->buffSize)
		{
			byteCopied = this->buffSize - this->buffStart;
			MemCopyNO(buff, &this->dataBuff[this->buffStart], this->buffSize - this->buffStart);
			blkSize -= this->buffSize - this->buffStart;
			buff += this->buffSize - this->buffStart;
			this->buffStart = 0;
		}
		if (blkSize > 0)
		{
			MemCopyNO(buff, &this->dataBuff[this->buffStart], blkSize);
			this->buffStart += blkSize;
			byteCopied += blkSize;
		}
		if (this->buffStart != this->buffEnd && this->pbEvt)
		{
			this->pbEvt->Set();
		}
	}
	else
	{
		if (this->buffStart > this->buffEnd)
		{
			byteCopied = this->buffSize - this->buffStart;
			MemCopyNO(buff, &this->dataBuff[this->buffStart], this->buffSize - this->buffStart);
			blkSize -= this->buffSize - this->buffStart;
			buff += this->buffSize - this->buffStart;
			this->buffStart = 0;
		}
		if (this->buffStart < this->buffEnd)
		{
			MemCopyNO(buff, &this->dataBuff[this->buffStart], this->buffEnd - this->buffStart);
			byteCopied += this->buffEnd - this->buffStart;
			this->buffStart = this->buffEnd;
		}
	}
	this->buffSample += byteCopied;
	this->buffMut->Unlock();

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
	return this->fmt->align;
}

Int32 Media::VOBLPCMStreamSource::GetCurrTime()
{
	return (Int32)(this->buffSample * 8000LL / this->fmt->bitRate);
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
	this->buffMut->Lock();
	this->buffStart = 0;
	this->buffEnd = 0;
	this->buffSample = 0;
	this->buffMut->Unlock();
}

void Media::VOBLPCMStreamSource::SetStreamTime(Int32 time)
{
	this->buffMut->Lock();
	this->buffSample = MulDiv32(time, this->fmt->bitRate, 8000);
	this->buffMut->Unlock();
}

void Media::VOBLPCMStreamSource::WriteFrameStream(UInt8 *buff, UOSInt buffSize)
{
	UOSInt buffWriten;
//	if (buff[0] > 0)
//	{
		if (this->pbEvt)
		{
			while (true)
			{
				if (this->pbEvt == 0 || !this->pbc->IsRunning())
					break;
			
				this->buffMut->Lock();
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
					if (this->pbEvt)
						this->pbEvt->Set();
					this->buffMut->Unlock();
					break;
				}
				else
				{
					this->buffMut->Unlock();
					Sync::Thread::Sleep(10);
				}
			}
		}
		else
		{
			this->buffMut->Lock();
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
			this->buffMut->Unlock();
		}
//	}
}

Int32 Media::VOBLPCMStreamSource::GetFrameStreamTime()
{
	Int32 t;
	this->buffMut->Lock();
	OSInt buffSize = this->buffEnd - this->buffStart;
	if (buffSize < 0)
	{
		buffSize += this->buffSize;
	}
	t = (Int32)((this->buffSample + buffSize) * 8000LL / this->fmt->bitRate);
	this->buffMut->Unlock();
	return t;
}

void Media::VOBLPCMStreamSource::EndFrameStream()
{
}

Int64 Media::VOBLPCMStreamSource::GetBitRate()
{
	return this->fmt->bitRate;
}
