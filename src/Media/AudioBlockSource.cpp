#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "Media/AudioBlockSource.h"

Media::AudioBlockSource::AudioBlockSource(IO::IStreamData *fd, Media::AudioFormat *format, const UTF8Char *name, Int32 samplePerBlock)
{
	this->format.FromAudioFormat(format);

	this->data = fd->GetPartialData(0, fd->GetDataSize());
	this->maxBlockSize = 0;

	if (name)
	{
		this->name = Text::StrCopyNew(name);
	}
	else
	{
		this->name = 0;
	}

	this->readEvt = 0;
	this->readBlock = 0;

	this->blockCnt = 0;
	this->maxBlockCnt = 40;
	this->blocks = MemAlloc(AudioBlock, this->maxBlockCnt);
	this->samplePerBlock = samplePerBlock;
}

Media::AudioBlockSource::~AudioBlockSource()
{
	DEL_CLASS(this->data);
	if (this->name)
	{
		Text::StrDelNew(this->name);
	}
	MemFree(this->blocks);
}

UTF8Char *Media::AudioBlockSource::GetSourceName(UTF8Char *buff)
{
	if (this->name == 0)
		return 0;
	return Text::StrConcat(buff, this->name);
}

Bool Media::AudioBlockSource::CanSeek()
{
	return true;
}

Int32 Media::AudioBlockSource::GetStreamTime()
{
	return (Int32)(this->blockCnt * (Int64)this->samplePerBlock * 1000 / this->format.frequency);
}

UInt32 Media::AudioBlockSource::SeekToTime(UInt32 time)
{
	if (this->samplePerBlock == 0)
	{
		UInt64 byteLeft;
		UInt64 byteOfst = time * (UInt64)this->format.bitRate / 8000;
		byteLeft = byteOfst;
		this->readBlock = this->blockCnt;

		UOSInt i;
		i = 0;
		while (i < this->blockCnt)
		{
			if (this->blocks[i].length > byteLeft)
			{
				this->readBlock = (Int32)i;
				break;
			}
			byteLeft -= this->blocks[i].length;
			i++;
		}
		byteOfst -= byteLeft;
		return (UInt32)(byteOfst * 8000 / this->format.bitRate);
	}
	else
	{
		this->readBlock = MulDiv32(time, this->format.frequency, this->samplePerBlock * 1000);
		return (UInt32)(this->readBlock * (Int64)this->samplePerBlock * 1000 / this->format.frequency);
	}
}

Bool Media::AudioBlockSource::TrimStream(UInt32 trimTimeStart, UInt32 trimTimeEnd, Int32 *syncTime)
{
	/////////////////////////////////////////
	return false;
}

void Media::AudioBlockSource::GetFormat(AudioFormat *format)
{
	format->FromAudioFormat(&this->format);
}

Bool Media::AudioBlockSource::Start(Sync::Event *evt, UOSInt blkSize)
{
	this->readEvt = evt;
	if (this->readEvt)
		this->readEvt->Set();
	return true;
}

void Media::AudioBlockSource::Stop()
{
	this->readBlock = 0;
	this->readEvt = 0;
}

UOSInt Media::AudioBlockSource::ReadBlock(UInt8 *buff, UOSInt blkSize)
{
	if (this->readBlock >= this->blockCnt)
		return 0;
	if (this->blocks[this->readBlock].length > blkSize)
		return 0;
	UOSInt readSize = this->data->GetRealData(this->blocks[this->readBlock].offset, this->blocks[this->readBlock].length, buff);
	this->readBlock++;
	if (this->readEvt)
		this->readEvt->Set();
	return readSize;
}

UOSInt Media::AudioBlockSource::GetMinBlockSize()
{
	return this->maxBlockSize;
}

UInt32 Media::AudioBlockSource::GetCurrTime()
{
	return (UInt32)(this->readBlock * (UInt64)this->samplePerBlock * 1000 / this->format.frequency);
}

Bool Media::AudioBlockSource::IsEnd()
{
	return this->readBlock >= this->blockCnt;
}

void Media::AudioBlockSource::AddBlock(UInt64 offset, UInt32 length)
{
	if (this->blockCnt >= this->maxBlockCnt)
	{
		AudioBlock *newBlocks;
		this->maxBlockCnt = this->maxBlockCnt << 1;
		newBlocks = MemAlloc(AudioBlock, this->maxBlockCnt);
		MemCopyNO(newBlocks, this->blocks, sizeof(AudioBlock) * this->blockCnt);
		MemFree(this->blocks);
		this->blocks = newBlocks;
	}
	this->blocks[this->blockCnt].offset = offset;
	this->blocks[this->blockCnt].length = length;
	this->blockCnt++;
	if (length > this->maxBlockSize)
	{
		this->maxBlockSize = length;
	}
}

void Media::AudioBlockSource::UpdateBitRate(Int32 bitRate)
{
	this->format.bitRate = bitRate;
}
