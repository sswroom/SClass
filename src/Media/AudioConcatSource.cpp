#include "Stdafx.h"
#include "MyMemory.h"
#include "Media/AudioConcatSource.h"
#include "Media/SilentSource.h"

Media::AudioConcatSource::AudioConcatSource()
{
	this->format.bitpersample = 0;
	this->format.bitRate = 0;
	this->format.formatId = 0;
	this->format.frequency = 0;
	this->format.nChannels = 0;

	this->canSeek = true;
	this->stmTime = 0;

	this->currStm = 0;
	this->readEvt = 0;
	this->readOfst = 0;
	this->readBlkSize = 0;
}

Media::AudioConcatSource::~AudioConcatSource()
{
	Media::IAudioSource *audio;
	UOSInt i = this->stmList.GetCount();
	while (i-- > 0)
	{
		audio = (Media::IAudioSource*)this->stmList.RemoveAt(i);
		DEL_CLASS(audio);
	}
}

Bool Media::AudioConcatSource::AppendAudio(NotNullPtr<Media::IAudioSource> audio)
{
	if (!audio->CanSeek())
		return false;

	if (this->stmList.GetCount() == 0)
	{
		this->stmList.Add(audio);
		audio->GetFormat(this->format);
		if (!audio->CanSeek())
		{
			this->canSeek = false;
		}
		this->stmTime = (UInt32)audio->GetStreamTime();
		return true;
	}
	else
	{
		Media::AudioFormat fmt;
		audio->GetFormat(fmt);
		if (fmt.bitpersample != this->format.bitpersample)
			return false;
		if (fmt.bitRate != this->format.bitRate)
			return false;
		if (fmt.formatId != this->format.formatId)
			return false;
		if (fmt.frequency != this->format.frequency)
			return false;
		if (fmt.bitpersample != this->format.bitpersample)
			return false;
		this->stmList.Add(audio);
		this->stmTime += (UInt32)audio->GetStreamTime();
		return true;
	}
}

Bool Media::AudioConcatSource::AppendSilent(UInt32 time)
{
	if (time <= 0 || this->format.frequency == 0)
		return false;
	UInt64 sampleCnt = time * (UInt64)this->format.frequency / 1000;
	NotNullPtr<Media::SilentSource> src;
	NEW_CLASSNN(src, Media::SilentSource(this->format.frequency, this->format.nChannels, this->format.bitpersample, CSTR("Silent"), sampleCnt));
	this->stmList.Add(src);
	this->stmTime += time;
	return true;
}

UTF8Char *Media::AudioConcatSource::GetSourceName(UTF8Char *buff)
{
	if (this->stmList.GetCount() > 0)
	{
		return ((Media::IAudioSource*)this->stmList.GetItem(0))->GetSourceName(buff);
	}
	return 0;
}

Bool Media::AudioConcatSource::CanSeek()
{
	return this->canSeek;
}

Int32 Media::AudioConcatSource::GetStreamTime()
{
	return (Int32)this->stmTime;
}

void Media::AudioConcatSource::GetFormat(NotNullPtr<AudioFormat> format)
{
	format->FromAudioFormat(this->format);
}

Bool Media::AudioConcatSource::TrimStream(UInt32 trimTimeStart, UInt32 trimTimeEnd, Int32 *syncTime)
{
	return false;
}

UInt32 Media::AudioConcatSource::SeekToTime(UInt32 time)
{
	UInt32 stmTotal;
	UInt32 stmTime = 0;
	UOSInt stmIndex;
	UOSInt stmCnt;
	stmTotal = 0;
	stmIndex = 0;
	stmCnt = this->stmList.GetCount();
	if (stmCnt == 0)
		return 0;

	while (stmIndex < stmCnt)
	{
		stmTime = (UInt32)((Media::IAudioSource *)this->stmList.GetItem(stmIndex))->GetStreamTime();
		if (stmTime > time)
		{
			if (this->currStm != stmIndex && this->readEvt)
			{
				((Media::IAudioSource*)this->stmList.GetItem(this->currStm))->Stop();
				this->currStm = stmIndex;
				((Media::IAudioSource*)this->stmList.GetItem(this->currStm))->Start(this->readEvt, this->readBlkSize);
			}

			return ((Media::IAudioSource*)this->stmList.GetItem(this->currStm))->SeekToTime(time) + stmTotal;
		}
		else
		{
			time -= stmTime;
			stmTotal += stmTime;
		}
		stmIndex++;
	}
	stmIndex = stmCnt - 1;
	time += stmTime;
	stmTotal -= stmTime;

	if (this->currStm != stmIndex && this->readEvt)
	{
		((Media::IAudioSource*)this->stmList.GetItem(this->currStm))->Stop();
		this->currStm = stmIndex;
		((Media::IAudioSource*)this->stmList.GetItem(this->currStm))->Start(this->readEvt, this->readBlkSize);
	}

	return ((Media::IAudioSource*)this->stmList.GetItem(this->currStm))->SeekToTime(time) + stmTotal;
}

Bool Media::AudioConcatSource::Start(Sync::Event *evt, UOSInt blkSize)
{
	if (this->readEvt && this->currStm > 0)
	{
		((Media::IAudioSource*)this->stmList.GetItem(this->currStm))->Stop();
	}
	this->readEvt = evt;
	this->readBlkSize = blkSize;

	this->readOfst = 0;
	this->currStm = 0;
	((Media::IAudioSource*)this->stmList.GetItem(this->currStm))->Start(this->readEvt, this->readBlkSize);
	return true;
}

void Media::AudioConcatSource::Stop()
{
	if (this->readEvt && this->currStm < this->stmList.GetCount())
	{
		((Media::IAudioSource*)this->stmList.GetItem(this->currStm))->Stop();
	}
	this->readEvt = 0;
}

UOSInt Media::AudioConcatSource::ReadBlock(Data::ByteArray blk)
{
	UOSInt readSize = ((Media::IAudioSource*)this->stmList.GetItem(this->currStm))->ReadBlock(blk);
	if (readSize > 0)
		return readSize;
	if (this->currStm + 1 >= this->stmList.GetCount())
	{
		return 0;
	}
	if (this->readEvt)
	{
		((Media::IAudioSource*)this->stmList.GetItem(this->currStm))->Stop();
		this->currStm++;
		((Media::IAudioSource*)this->stmList.GetItem(this->currStm))->Start(this->readEvt, this->readBlkSize);
	}
	else
	{
		this->currStm++;
	}
	return ((Media::IAudioSource*)this->stmList.GetItem(this->currStm))->ReadBlock(blk);
}

UInt32 Media::AudioConcatSource::GetCurrTime()
{
	UInt32 totalTime = 0;
	UOSInt i = this->currStm;
	Media::IAudioSource *astm = this->stmList.GetItem(i);
	if (astm)
	{
		totalTime = astm->GetCurrTime();
	}
	while (i-- > 0)
	{
		astm = this->stmList.GetItem(i);
		if (astm)
		{
			totalTime += (UInt32)astm->GetStreamTime();
		}
	}
	return totalTime;
}

UOSInt Media::AudioConcatSource::GetMinBlockSize()
{
	return this->format.align;
}

Bool Media::AudioConcatSource::IsEnd()
{
	if (this->currStm + 1 >= this->stmList.GetCount())
	{
		Media::IAudioSource *audioSrc = this->stmList.GetItem(this->currStm);
		if (audioSrc == 0 || audioSrc->IsEnd())
		{
			return true;
		}
	}
	return false;
}
