#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "Media/AudioFrameSource.h"

Media::AudioFrameSource::AudioFrameSource(IO::StreamData *fd, Media::AudioFormat *format, NotNullPtr<Text::String> name)
{
	this->format.FromAudioFormat(format);
	if (this->format.frequency == 0)
	{
		this->format.frequency = 48000;
	}

	this->data = fd->GetPartialData(0, fd->GetDataSize());
	this->maxBlockSize = 0;
	this->name = name->Clone();
	this->readEvt = 0;
	this->readBlock = 0;
	this->readBlockOfst = 0;

	this->blockCnt = 0;
	this->maxBlockCnt = 40;
	this->totalSampleCnt = 0;
	this->totalSize = 0;
	this->blocks = MemAlloc(Media::AudioFrameSource::AudioFrame, this->maxBlockCnt);
}

Media::AudioFrameSource::AudioFrameSource(IO::StreamData *fd, Media::AudioFormat *format, Text::CString name)
{
	this->format.FromAudioFormat(format);
	if (this->format.frequency == 0)
	{
		this->format.frequency = 48000;
	}

	this->data = fd->GetPartialData(0, fd->GetDataSize());
	this->maxBlockSize = 0;
	this->name = Text::String::New(name);
	this->readEvt = 0;
	this->readBlock = 0;
	this->readBlockOfst = 0;

	this->blockCnt = 0;
	this->maxBlockCnt = 40;
	this->totalSampleCnt = 0;
	this->totalSize = 0;
	this->blocks = MemAlloc(Media::AudioFrameSource::AudioFrame, this->maxBlockCnt);
}

Media::AudioFrameSource::~AudioFrameSource()
{
	DEL_CLASS(this->data);
	this->name->Release();
	MemFree(this->blocks);
}

UTF8Char *Media::AudioFrameSource::GetSourceName(UTF8Char *buff)
{
	return this->name->ConcatTo(buff);
}

Bool Media::AudioFrameSource::CanSeek()
{
	return true;
}

Int32 Media::AudioFrameSource::GetStreamTime()
{
	return (Int32)(this->totalSampleCnt * (Int64)1000 / this->format.frequency);
}

UInt32 Media::AudioFrameSource::SeekToTime(UInt32 time)
{
	OSInt i;
	OSInt j;
	OSInt k;
	if (time < 0)
		time = 0;
	UOSInt samplePos = (UOSInt)(time * (UInt64)this->format.frequency / 1000);
	i = 0;
	j = (OSInt)this->blockCnt - 1;
	while (i <= j)
	{
		k = (i + j) >> 1;
		if (this->blocks[k].sampleOffset > samplePos)
		{
			j = k - 1;
		}
		else if (this->blocks[k].sampleOffset < samplePos)
		{
			i = k + 1;
		}
		else
		{
			this->readBlock = (UOSInt)k;
			return (UInt32)(this->blocks[this->readBlock].sampleOffset * (UInt64)1000 / this->format.frequency);
		}
	}
	this->readBlock = (UOSInt)i - 1;
	this->readBlockOfst = 0;
	return (UInt32)(this->blocks[this->readBlock].sampleOffset * (UInt64)1000 / this->format.frequency);
}

Bool Media::AudioFrameSource::TrimStream(UInt32 trimTimeStart, UInt32 trimTimeEnd, Int32 *syncTime)
{
	///////////////////////////////////
	return false;
}

void Media::AudioFrameSource::GetFormat(AudioFormat *format)
{
	if (this->format.bitRate == 0)
	{
		this->format.bitRate = (UInt32)(this->totalSize * 8 * this->format.frequency / this->totalSampleCnt);
	}

	format->FromAudioFormat(&this->format);
}

Bool Media::AudioFrameSource::Start(Sync::Event *evt, UOSInt blkSize)
{
	this->readEvt = evt;
	if (this->readEvt)
		this->readEvt->Set();
	return true;
}

void Media::AudioFrameSource::Stop()
{
	this->readBlock = 0;
	this->readEvt = 0;
}

UOSInt Media::AudioFrameSource::ReadBlock(Data::ByteArray blk)
{
	if (this->format.formatId == 1)
	{
		if (blk.GetSize() % this->format.align)
		{
			blk = blk.WithSize(blk.GetSize() - (blk.GetSize() % this->format.align));
		}
		UOSInt readSize = 0;
		UOSInt size;
		while (this->readBlock < this->blockCnt && blk.GetSize() > 0)
		{
			if ((blocks[this->readBlock].length - this->readBlockOfst) > blk.GetSize())
			{
				this->data->GetRealData(this->blocks[this->readBlock].offset + this->readBlockOfst, blk.GetSize(), blk);
				this->readBlockOfst += blk.GetSize();
				readSize += blk.GetSize();
				blk = blk.WithSize(0);
				break;
			}
			else
			{
				size = this->blocks[this->readBlock].length - this->readBlockOfst;
				this->data->GetRealData(this->blocks[this->readBlock].offset + this->readBlockOfst, size, blk);
				blk += size;
				readSize += size;
				this->readBlockOfst = 0;
				this->readBlock++;
			}
		}
		return readSize;
	}
	else
	{
		if (this->readBlock >= this->blockCnt)
			return 0;
		if (this->blocks[this->readBlock].length > blk.GetSize())
			return 0;
		UOSInt readSize = this->data->GetRealData(this->blocks[this->readBlock].offset, this->blocks[this->readBlock].length, blk);
		this->readBlock++;
		this->readBlockOfst = 0;
		if (this->readEvt)
			this->readEvt->Set();
		return readSize;
	}
}

UOSInt Media::AudioFrameSource::GetMinBlockSize()
{
	return this->maxBlockSize;
}

UInt32 Media::AudioFrameSource::GetCurrTime()
{
	if (this->readBlock >= this->blockCnt)
	{
		return (UInt32)this->GetStreamTime();
	}
	return (UInt32)(this->blocks[this->readBlock].sampleOffset * (UInt64)1000 / this->format.frequency);
}

Bool Media::AudioFrameSource::IsEnd()
{
	return this->readBlock >= this->blockCnt;
}

void Media::AudioFrameSource::AddBlock(UInt64 offset, UInt32 length, UInt32 decodedSample)
{
	if (this->blockCnt >= this->maxBlockCnt)
	{
		Media::AudioFrameSource::AudioFrame *newBlocks;
		this->maxBlockCnt = this->maxBlockCnt << 1;
		newBlocks = MemAlloc(AudioFrame, this->maxBlockCnt);
		MemCopyNO(newBlocks, this->blocks, sizeof(AudioFrame) * this->blockCnt);
		MemFree(this->blocks);
		this->blocks = newBlocks;
	}
	this->blocks[this->blockCnt].offset = offset;
	this->blocks[this->blockCnt].length = length;
	this->blocks[this->blockCnt].sampleOffset = this->totalSampleCnt;
	this->blocks[this->blockCnt].decodedSample = decodedSample;
	this->totalSampleCnt += decodedSample;
	this->totalSize += length;
	this->blockCnt++;
	if (length > this->maxBlockSize)
	{
		this->maxBlockSize = length;
	}
}
