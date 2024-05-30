#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "Media/AudioFixBlockSource.h"

Media::AudioFixBlockSource::AudioFixBlockSource(NN<IO::StreamData> fd, UInt64 ofst, UInt64 length, NN<const Media::AudioFormat> format, NN<Text::String> name)
{
	this->format.FromAudioFormat(format);
	this->data = fd->GetPartialData(ofst, length);
	this->name = name->Clone();
	this->readEvt = 0;
	this->readOfst = 0;
}

Media::AudioFixBlockSource::~AudioFixBlockSource()
{
	this->data.Delete();
	this->name->Release();
}

UnsafeArrayOpt<UTF8Char> Media::AudioFixBlockSource::GetSourceName(UnsafeArray<UTF8Char> buff)
{
	return this->name->ConcatTo(buff);
}

Bool Media::AudioFixBlockSource::CanSeek()
{
	return true;
}

Data::Duration Media::AudioFixBlockSource::GetStreamTime()
{
	return Data::Duration::FromRatioU64(this->data->GetDataSize(), (this->format.bitRate >> 3));
}

Data::Duration Media::AudioFixBlockSource::SeekToTime(Data::Duration time)
{
	UInt32 blk = (UInt32)this->format.bitpersample >> 3;
	if (blk == 0)
	{
		blk = this->format.align;
	}
	this->readOfst = time.MultiplyU64(this->format.bitRate >> 3) / this->format.align * blk;
	return Data::Duration::FromRatioU64(this->readOfst, this->format.bitRate >> 3);
}

void Media::AudioFixBlockSource::GetFormat(NN<AudioFormat> format)
{
	format->FromAudioFormat(this->format);
}

Bool Media::AudioFixBlockSource::TrimStream(UInt32 trimTimeStart, UInt32 trimTimeEnd, Int32 *syncTime)
{
	//////////////////////////////////////////
	return false;
}

Bool Media::AudioFixBlockSource::Start(Sync::Event *evt, UOSInt blkSize)
{
	this->readEvt = evt;
	if (this->readEvt)
		this->readEvt->Set();
	return true;
}

void Media::AudioFixBlockSource::Stop()
{
	this->readEvt = 0;
	this->readOfst = 0;
}

UOSInt Media::AudioFixBlockSource::ReadBlock(Data::ByteArray blk)
{
	UInt32 blkSize = this->format.align;
	UOSInt readSize = blk.GetSize() / blkSize;
	readSize = this->data->GetRealData(this->readOfst, readSize * blkSize, blk);
	this->readOfst += readSize;
	if (this->readEvt)
		this->readEvt->Set();
	return readSize;
}

UOSInt Media::AudioFixBlockSource::GetMinBlockSize()
{
	return this->format.align;
}

Data::Duration Media::AudioFixBlockSource::GetCurrTime()
{
	return Data::Duration::FromRatioU64(this->readOfst, (this->format.bitRate >> 3));
}

Bool Media::AudioFixBlockSource::IsEnd()
{
	return this->readOfst >= this->data->GetDataSize();
}
