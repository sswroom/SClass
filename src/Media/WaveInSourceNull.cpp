#include "Stdafx.h"
#include "MyMemory.h"
#include "Media/WaveInSource.h"
#include "Sync/Event.h"
#include "Text/MyString.h"

void Media::WaveInSource::OpenAudio()
{
	this->hWaveIn = 0;
}

void __stdcall Media::WaveInSource::AudioBlock(void *hwi, UInt32 uMsg, UInt32 *dwInstance, UInt32 *dwParam1, UInt32 *dwParam2)
{
//	Media::WaveInSource *me = (Media::WaveInSource*)dwInstance;
}

UInt32 Media::WaveInSource::GetDeviceCount()
{
	return 0;
}

UTF8Char *Media::WaveInSource::GetDeviceName(UTF8Char *sbuff, UInt32 devNo)
{
	return 0;
}

Media::WaveInSource::WaveInSource(const UTF8Char *devName, UInt32 freq, UInt16 nbits, UInt16 nChannels)
{
	this->devId = (UInt32)-1;
	this->freq = freq;
	this->nbits = nbits;
	this->nChannels = nChannels;
	this->hWaveIn = 0;
	this->hdrs = 0;
	this->hdrsCnt = 0;
	NEW_CLASS(this->dataEvt, Sync::Event(true));
}

Media::WaveInSource::WaveInSource(UInt32 devId, UInt32 freq, UInt16 nbits, UInt16 nChannels)
{
	this->devId = devId;
	this->freq = freq;
	this->nbits = nbits;
	this->nChannels = nChannels;
	this->hWaveIn = 0;
	this->hdrs = 0;
	this->hdrsCnt = 0;
	NEW_CLASS(this->dataEvt, Sync::Event(true));
}

Media::WaveInSource::~WaveInSource()
{
	DEL_CLASS(this->dataEvt);
}

Bool Media::WaveInSource::IsError()
{
	return hWaveIn == 0;
}

UTF8Char *Media::WaveInSource::GetSourceName(UTF8Char *buff)
{
	return 0;
}

Bool Media::WaveInSource::CanSeek()
{
	return false;
}

Data::Duration Media::WaveInSource::GetStreamTime()
{
	return Data::Duration::Infinity();
}

Data::Duration Media::WaveInSource::SeekToTime(Data::Duration time)
{
	return 0;
}

Bool Media::WaveInSource::TrimStream(UInt32 trimTimeStart, UInt32 trimTimeEnd, Int32 *syncTime)
{
	return false;
}

void Media::WaveInSource::GetFormat(NotNullPtr<AudioFormat> format)
{
	format->formatId = 1;
	format->bitpersample = this->nbits;
	format->frequency = this->freq;
	format->nChannels = this->nChannels;
	format->bitRate = this->freq * this->nChannels * this->nbits;
	format->align = (UInt32)this->nChannels * this->nbits >> 3;
}

Bool Media::WaveInSource::Start(Sync::Event *evt, UOSInt blkSize)
{
	this->evt = evt;
	return false;
}

void Media::WaveInSource::Stop()
{
	this->started = false;
}

UOSInt Media::WaveInSource::ReadBlock(Data::ByteArray blk)
{
	return 0;
}

UOSInt Media::WaveInSource::GetMinBlockSize()
{
	return (UInt32)(this->nChannels * this->nbits >> 3);
}

Data::Duration Media::WaveInSource::GetCurrTime()
{
	return 0;
}

Bool Media::WaveInSource::IsEnd()
{
	return true;
}
