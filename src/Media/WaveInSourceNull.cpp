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

Int32 Media::WaveInSource::GetDeviceCount()
{
	return 0;
}

UTF8Char *Media::WaveInSource::GetDeviceName(UTF8Char *u8buff, Int32 devNo)
{
	return 0;
}

Media::WaveInSource::WaveInSource(const UTF8Char *devName, Int32 freq, Int16 nbits, Int16 nChannels)
{
	this->devId = -1;
	this->freq = freq;
	this->nbits = nbits;
	this->nChannels = nChannels;
	this->hWaveIn = 0;
	this->hdrs = 0;
	this->hdrsCnt = 0;
	NEW_CLASS(this->dataEvt, Sync::Event(true, (const UTF8Char*)"Media.WaveInSource.dataEvt"));
}

Media::WaveInSource::WaveInSource(Int32 devId, Int32 freq, Int16 nbits, Int16 nChannels)
{
	this->devId = devId;
	this->freq = freq;
	this->nbits = nbits;
	this->nChannels = nChannels;
	this->hWaveIn = 0;
	this->hdrs = 0;
	this->hdrsCnt = 0;
	NEW_CLASS(this->dataEvt, Sync::Event(true, (const UTF8Char*)"Media.WaveInSource.dataEvt"));
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

Int32 Media::WaveInSource::GetStreamTime()
{
	return -1;
}

UInt32 Media::WaveInSource::SeekToTime(UInt32 time)
{
	return 0;
}

Bool Media::WaveInSource::TrimStream(UInt32 trimTimeStart, UInt32 trimTimeEnd, Int32 *syncTime)
{
	return false;
}

void Media::WaveInSource::GetFormat(AudioFormat *format)
{
	format->formatId = 1;
	format->bitpersample = this->nbits;
	format->frequency = this->freq;
	format->nChannels = this->nChannels;
	format->bitRate = this->freq * this->nChannels * this->nbits;
	format->align = this->nChannels * this->nbits >> 3;
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

UOSInt Media::WaveInSource::ReadBlock(UInt8 *buff, UOSInt blkSize)
{
	return 0;
}

UOSInt Media::WaveInSource::GetMinBlockSize()
{
	return this->nChannels * this->nbits >> 3;
}

UInt32 Media::WaveInSource::GetCurrTime()
{
	return 0;
}

Bool Media::WaveInSource::IsEnd()
{
	return true;
}
