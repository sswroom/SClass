#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "Media/AudioFixBlockSource.h"

Media::AudioFixBlockSource::AudioFixBlockSource(IO::StreamData *fd, UInt64 ofst, UInt64 length, Media::AudioFormat *format, NotNullPtr<Text::String> name)
{
	this->format.FromAudioFormat(format);
	this->data = fd->GetPartialData(ofst, length);
	this->name = name->Clone();
	this->readEvt = 0;
	this->readOfst = 0;
}

Media::AudioFixBlockSource::~AudioFixBlockSource()
{
	DEL_CLASS(this->data);
	this->name->Release();
}

UTF8Char *Media::AudioFixBlockSource::GetSourceName(UTF8Char *buff)
{
	return this->name->ConcatTo(buff);
}

Bool Media::AudioFixBlockSource::CanSeek()
{
	return true;
}

Int32 Media::AudioFixBlockSource::GetStreamTime()
{
	return (Int32)(this->data->GetDataSize() * 1000 / (this->format.bitRate >> 3));
}

UInt32 Media::AudioFixBlockSource::SeekToTime(UInt32 time)
{
	UInt32 blk = (UInt32)this->format.bitpersample >> 3;
	if (blk == 0)
	{
		blk = this->format.align;
	}
	this->readOfst = (MulDivU32(time, this->format.bitRate >> 3, 1000) / this->format.align) * blk;
	return (UInt32)(this->readOfst * 8000 / this->format.bitRate);
}

void Media::AudioFixBlockSource::GetFormat(AudioFormat *format)
{
	format->FromAudioFormat(&this->format);
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

UInt32 Media::AudioFixBlockSource::GetCurrTime()
{
	return (UInt32)(this->readOfst * 8000 / this->format.bitRate);
}

Bool Media::AudioFixBlockSource::IsEnd()
{
	return this->readOfst >= this->data->GetDataSize();
}
