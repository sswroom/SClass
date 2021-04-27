#include "Stdafx.h"
#include "MyMemory.h"
#include "Media/IAudioSource.h"
#include "Media/IMediaSource.h"
#include "Media/WaveInSource.h"
#include "Sync/Event.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"

#include <windows.h>
#include <mmreg.h>
#include <ks.h>
#include <ksmedia.h>

void Media::WaveInSource::OpenAudio()
{
	WAVEFORMATEXTENSIBLE format;
	if (this->nChannels <= 2 && this->nbits <= 16)
	{
		format.Format.wFormatTag = 1;
		format.Format.nSamplesPerSec = this->freq;
		format.Format.wBitsPerSample = this->nbits;
		format.Format.nChannels = this->nChannels;
		format.Format.nBlockAlign = this->nChannels * this->nbits >> 3;
		format.Format.nAvgBytesPerSec = this->freq * format.Format.nBlockAlign;
		format.Format.cbSize = 0;
	}
	else
	{
		format.Format.wFormatTag = WAVE_FORMAT_EXTENSIBLE;
		format.Format.nSamplesPerSec = this->freq;
		format.Format.wBitsPerSample = this->nbits;
		format.Format.nChannels = this->nChannels;
		format.Format.nBlockAlign = this->nChannels * this->nbits >> 3;
		format.Format.nAvgBytesPerSec = this->freq * format.Format.nBlockAlign;
		format.Format.cbSize = 22;
		format.Samples.wValidBitsPerSample = this->nbits;
		format.SubFormat = KSDATAFORMAT_SUBTYPE_PCM;
		if (this->nChannels == 1)
		{
			format.dwChannelMask = SPEAKER_FRONT_CENTER;
		}
		else if (this->nChannels == 2)
		{
			format.dwChannelMask = SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT;
		}
		else if (this->nChannels == 3)
		{
			format.dwChannelMask = SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT | SPEAKER_FRONT_CENTER;
		}
		else if (this->nChannels == 4)
		{
			format.dwChannelMask = SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT | SPEAKER_BACK_LEFT | SPEAKER_BACK_RIGHT;
		}
		else if (this->nChannels == 5)
		{
			format.dwChannelMask = SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT | SPEAKER_FRONT_CENTER | SPEAKER_BACK_LEFT | SPEAKER_BACK_RIGHT;
		}
		else if (this->nChannels == 6)
		{
			format.dwChannelMask = SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT | SPEAKER_FRONT_CENTER | SPEAKER_LOW_FREQUENCY | SPEAKER_BACK_LEFT | SPEAKER_BACK_RIGHT;
		}
		else if (this->nChannels == 7)
		{
			format.dwChannelMask = SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT | SPEAKER_FRONT_CENTER | SPEAKER_LOW_FREQUENCY | SPEAKER_BACK_LEFT | SPEAKER_BACK_RIGHT | SPEAKER_BACK_CENTER;
		}
		else if (this->nChannels == 8)
		{
			format.dwChannelMask = SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT | SPEAKER_FRONT_CENTER | SPEAKER_LOW_FREQUENCY | SPEAKER_BACK_LEFT | SPEAKER_BACK_RIGHT | SPEAKER_SIDE_LEFT | SPEAKER_SIDE_RIGHT;
		}
	}
	waveInOpen((HWAVEIN*)&this->hWaveIn, this->devId, (LPCWAVEFORMATEX)&format, (DWORD_PTR)AudioBlock, (DWORD_PTR)this, CALLBACK_FUNCTION);
}

void __stdcall Media::WaveInSource::AudioBlock(void *hwi, UInt32 uMsg, UInt32 *dwInstance, UInt32 *dwParam1, UInt32 *dwParam2)
{
	Media::WaveInSource *me = (Media::WaveInSource*)dwInstance;
	if (me->started)
	{
		if (uMsg == WIM_DATA)
		{
			WAVEHDR *hdr = (WAVEHDR*)dwParam1;
			hdr->dwUser = me->nextId++;
			if (me->evt)
				me->evt->Set();
			me->dataEvt->Set();
		}
	}
	else
	{
		me->dataEvt->Set();
	}
}

Int32 Media::WaveInSource::GetDeviceCount()
{
	return waveInGetNumDevs();
}

UTF8Char *Media::WaveInSource::GetDeviceName(UTF8Char *u8buff, Int32 devNo)
{
	WAVEINCAPSW caps;
	waveInGetDevCapsW(devNo, &caps, sizeof(caps));
	return Text::StrWChar_UTF8(u8buff, caps.szPname, -1);
}

Media::WaveInSource::WaveInSource(const UTF8Char *devName, Int32 freq, Int16 nbits, Int16 nChannels)
{
	UTF8Char u8buff[256];
	Int32 i;
	this->devId = -1;
	this->freq = freq;
	this->nbits = nbits;
	this->nChannels = nChannels;
	this->hWaveIn = 0;
	this->hdrs = 0;
	this->hdrsCnt = 0;
	NEW_CLASS(this->dataEvt, Sync::Event(true, (const UTF8Char*)"Media.WaveInSource.dataEvt"));

	i = GetDeviceCount();
	while (i-- > 0)
	{
		GetDeviceName(u8buff, i);
		if (Text::StrCompare(u8buff, devName) == 0)
		{
			this->devId = i;
			break;
		}
	}
	if (this->devId == -1)
		return;
	
	OpenAudio();
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
	OpenAudio();
}

Media::WaveInSource::~WaveInSource()
{
	if (this->hWaveIn)
	{
		Stop();
		waveInClose((HWAVEIN)hWaveIn);
		hWaveIn = 0;
	}
	DEL_CLASS(this->dataEvt);
}

Bool Media::WaveInSource::IsError()
{
	return hWaveIn == 0;
}

UTF8Char *Media::WaveInSource::GetSourceName(UTF8Char *buff)
{
	return GetDeviceName(buff, this->devId);
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
	Stop();
	UOSInt bSize = blkSize / (this->nChannels * this->nbits >> 3);
	if (bSize < 10)
		bSize = 10;
	blkSize = bSize * (this->nChannels * this->nbits >> 3);
	this->evt = evt;

	this->hdrsCnt = 4;
	this->hdrs = MemAlloc(WAVEHDR, this->hdrsCnt);
	MemClear(this->hdrs, sizeof(WAVEHDR) * this->hdrsCnt);
	Int32 i = this->hdrsCnt;
	while (i-- > 0)
	{
		((WAVEHDR*)hdrs)[i].dwBufferLength = (DWORD)blkSize;
		((WAVEHDR*)hdrs)[i].lpData = (LPSTR)MAlloc(blkSize);
		waveInPrepareHeader((HWAVEIN)this->hWaveIn, &((WAVEHDR*)hdrs)[i], sizeof(WAVEHDR));
		waveInAddBuffer((HWAVEIN)this->hWaveIn, &((WAVEHDR*)hdrs)[i], sizeof(WAVEHDR));
	}
	this->nextId = 0;
	this->dataEvt->Clear();
	waveInStart((HWAVEIN)this->hWaveIn);
	this->started = true;
	return true;
}

void Media::WaveInSource::Stop()
{
	this->started = false;
	waveInStop((HWAVEIN)this->hWaveIn);
	waveInReset((HWAVEIN)this->hWaveIn);
	if (this->hdrs)
	{
		Int32 i = this->hdrsCnt;
		while (i-- > 0)
		{
			MemFree(((WAVEHDR*)this->hdrs)[i].lpData);
		}
		MemFree(this->hdrs);
		this->hdrs = 0;
		this->hdrsCnt = 0;
	}
}

UOSInt Media::WaveInSource::ReadBlock(UInt8 *buff, UOSInt blkSize)
{
	Int32 j = -1;
	Int32 k = this->nextId;
	Int32 i = this->hdrsCnt;
	WAVEHDR *hdr;
	while (i-- > 0)
	{
		hdr = &((WAVEHDR*)hdrs)[i];
		if (hdr->dwFlags & WHDR_DONE)
		{
			if (hdr->dwUser <= (UInt32)k)
			{
				j = i;
				k = (Int32)hdr->dwUser;
			}
		}
	}
	if (j == -1 && this->started)
	{
		this->dataEvt->Wait(1000);
		i = this->hdrsCnt;
		while (i-- > 0 && this->started)
		{
			hdr = &((WAVEHDR*)hdrs)[i];
			if (hdr->dwFlags & WHDR_DONE)
			{
				if (hdr->dwUser <= (UInt32)k)
				{
					j = i;
					k = (Int32)hdr->dwUser;
				}
			}
		}
	}
	if (j != -1 && this->started)
	{
		UInt32 retSize;
		hdr = &((WAVEHDR*)hdrs)[j];
		retSize = hdr->dwBytesRecorded;
		MemCopyNO(buff, hdr->lpData, retSize);
		waveInAddBuffer((HWAVEIN)this->hWaveIn, hdr, sizeof(WAVEHDR));
		return retSize;
	}
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
	return !this->started;
}
