#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "Media/AudioFixBlockSource.h"

Media::AudioFixBlockSource::AudioFixBlockSource(IO::IStreamData *fd, Int64 ofst, Int64 length, Media::AudioFormat *format, const UTF8Char *name)
{
	this->format.FromAudioFormat(format);
	this->data = fd->GetPartialData(ofst, length);

	if (name)
	{
		this->name = Text::StrCopyNew(name);
	}
	else
	{
		this->name = 0;
	}

	this->readEvt = 0;
	this->readOfst = 0;
}

Media::AudioFixBlockSource::~AudioFixBlockSource()
{
	DEL_CLASS(this->data);
	if (this->name)
	{
		Text::StrDelNew(this->name);
	}
}

UTF8Char *Media::AudioFixBlockSource::GetSourceName(UTF8Char *buff)
{
	if (this->name == 0)
		return 0;
	return Text::StrConcat(buff, this->name);
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
	this->readOfst = (MulDiv32(time, this->format.bitRate >> 3, 1000) / this->format.align) * blk;
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

UOSInt Media::AudioFixBlockSource::ReadBlock(UInt8 *buff, UOSInt blkSize)
{
	UInt32 blk = this->format.align;
	UOSInt readSize = blkSize / blk;
	readSize = this->data->GetRealData(this->readOfst, readSize * blk, buff);
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
