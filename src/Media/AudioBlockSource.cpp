#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "Media/AudioBlockSource.h"

Media::AudioBlockSource::AudioBlockSource(NN<IO::StreamData> fd, NN<const Media::AudioFormat> format, NN<Text::String> name, UInt32 samplePerBlock)
{
	this->format.FromAudioFormat(format);

	this->data = fd->GetPartialData(0, fd->GetDataSize());
	this->maxBlockSize = 0;
	this->name = name->Clone();
	this->readEvt = nullptr;
	this->readBlock = 0;

	this->blockCnt = 0;
	this->maxBlockCnt = 40;
	this->blocks = MemAlloc(AudioBlock, this->maxBlockCnt);
	this->samplePerBlock = samplePerBlock;
}

Media::AudioBlockSource::~AudioBlockSource()
{
	this->data.Delete();
	this->name->Release();
	MemFree(this->blocks);
}

UnsafeArrayOpt<UTF8Char> Media::AudioBlockSource::GetSourceName(UnsafeArray<UTF8Char> buff)
{
	return this->name->ConcatTo(buff);
}

Bool Media::AudioBlockSource::CanSeek()
{
	return true;
}

Data::Duration Media::AudioBlockSource::GetStreamTime()
{
	return Data::Duration::FromRatioU64(this->blockCnt * (UInt64)this->samplePerBlock, this->format.frequency);
}

Data::Duration Media::AudioBlockSource::SeekToTime(Data::Duration time)
{
	if (this->samplePerBlock == 0)
	{
		UInt64 byteLeft;
		UInt64 byteOfst = time.MultiplyU64((UInt64)this->format.bitRate >> 3);
		byteLeft = byteOfst;
		this->readBlock = this->blockCnt;

		UIntOS i;
		i = 0;
		while (i < this->blockCnt)
		{
			if (this->blocks[i].length > byteLeft)
			{
				this->readBlock = (UInt32)i;
				break;
			}
			byteLeft -= this->blocks[i].length;
			i++;
		}
		byteOfst -= byteLeft;
		return Data::Duration::FromRatioU64(byteOfst * 8, this->format.bitRate);
	}
	else
	{
		this->readBlock = (UInt32)time.SecsMulDivU32(this->format.frequency, this->samplePerBlock);
		return Data::Duration::FromRatioU64(this->readBlock * (UInt64)this->samplePerBlock, this->format.frequency);
	}
}

Bool Media::AudioBlockSource::TrimStream(UInt32 trimTimeStart, UInt32 trimTimeEnd, OptOut<Int32> syncTime)
{
	/////////////////////////////////////////
	return false;
}

void Media::AudioBlockSource::GetFormat(NN<AudioFormat> format)
{
	format->FromAudioFormat(this->format);
}

Bool Media::AudioBlockSource::Start(Optional<Sync::Event> evt, UIntOS blkSize)
{
	NN<Sync::Event> readEvt;
	this->readEvt = evt;
	if (this->readEvt.SetTo(readEvt))
		readEvt->Set();
	return true;
}

void Media::AudioBlockSource::Stop()
{
	this->readBlock = 0;
	this->readEvt = nullptr;
}

UIntOS Media::AudioBlockSource::ReadBlock(Data::ByteArray buff)
{
	NN<Sync::Event> readEvt;
	if (this->readBlock >= this->blockCnt)
		return 0;
	if (this->blocks[this->readBlock].length > buff.GetSize())
		return 0;
	UIntOS readSize = this->data->GetRealData(this->blocks[this->readBlock].offset, this->blocks[this->readBlock].length, buff);
	this->readBlock++;
	if (this->readEvt.SetTo(readEvt))
		readEvt->Set();
	return readSize;
}

UIntOS Media::AudioBlockSource::GetMinBlockSize()
{
	return this->maxBlockSize;
}

Data::Duration Media::AudioBlockSource::GetCurrTime()
{
	return Data::Duration::FromRatioU64(this->readBlock * (UInt64)this->samplePerBlock, this->format.frequency);
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

void Media::AudioBlockSource::UpdateBitRate(UInt32 bitRate)
{
	this->format.bitRate = bitRate;
}
