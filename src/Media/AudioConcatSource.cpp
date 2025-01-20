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
	UOSInt i = this->stmList.GetCount();
	while (i-- > 0)
	{
		this->stmList.RemoveAt(i).Delete();
	}
}

Bool Media::AudioConcatSource::AppendAudio(NN<Media::AudioSource> audio)
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
		this->stmTime = audio->GetStreamTime();
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
		this->stmTime += audio->GetStreamTime();
		return true;
	}
}

Bool Media::AudioConcatSource::AppendSilent(UInt32 time)
{
	if (time <= 0 || this->format.frequency == 0)
		return false;
	UInt64 sampleCnt = time * (UInt64)this->format.frequency / 1000;
	NN<Media::SilentSource> src;
	NEW_CLASSNN(src, Media::SilentSource(this->format.frequency, this->format.nChannels, this->format.bitpersample, CSTR("Silent"), sampleCnt));
	this->stmList.Add(src);
	this->stmTime += time;
	return true;
}

UnsafeArrayOpt<UTF8Char> Media::AudioConcatSource::GetSourceName(UnsafeArray<UTF8Char> buff)
{
	NN<Media::AudioSource> audSrc;
	if (this->stmList.GetItem(0).SetTo(audSrc))
	{
		return audSrc->GetSourceName(buff);
	}
	return 0;
}

Bool Media::AudioConcatSource::CanSeek()
{
	return this->canSeek;
}

Data::Duration Media::AudioConcatSource::GetStreamTime()
{
	return this->stmTime;
}

void Media::AudioConcatSource::GetFormat(NN<AudioFormat> format)
{
	format->FromAudioFormat(this->format);
}

Bool Media::AudioConcatSource::TrimStream(UInt32 trimTimeStart, UInt32 trimTimeEnd, OptOut<Int32> syncTime)
{
	return false;
}

Data::Duration Media::AudioConcatSource::SeekToTime(Data::Duration time)
{
	Data::Duration stmTotal;
	Data::Duration stmTime = 0;
	NN<Media::AudioSource> audSrc1;
	NN<Media::AudioSource> audSrc2;
	UOSInt stmIndex;
	UOSInt stmCnt;
	stmTotal = 0;
	stmIndex = 0;
	stmCnt = this->stmList.GetCount();
	if (stmCnt == 0)
		return 0;

	while (stmIndex < stmCnt)
	{
		if (this->stmList.GetItem(stmIndex).SetTo(audSrc1))
		{
			stmTime = audSrc1->GetStreamTime();
			if (stmTime > time)
			{
				if (this->stmList.GetItem(this->currStm).SetTo(audSrc2))
				{
					if (this->currStm != stmIndex && this->readEvt.NotNull())
					{
						audSrc2->Stop();
						this->currStm = stmIndex;
						audSrc2 = audSrc1;
						audSrc2->Start(this->readEvt, this->readBlkSize);
					}

					return audSrc2->SeekToTime(time) + stmTotal;
				}
				else
				{
					return 0;
				}
			}
			else
			{
				time -= stmTime;
				stmTotal += stmTime;
			}
		}
		stmIndex++;
	}
	stmIndex = stmCnt - 1;
	time += stmTime;
	stmTotal -= stmTime;

	if (this->stmList.GetItem(this->currStm).SetTo(audSrc2))
	{
		if (this->currStm != stmIndex && this->readEvt.NotNull() && this->stmList.GetItem(stmIndex).SetTo(audSrc1))
		{
			audSrc2->Stop();
			this->currStm = stmIndex;
			audSrc2 = audSrc1;
			audSrc2->Start(this->readEvt, this->readBlkSize);
		}

		return audSrc2->SeekToTime(time) + stmTotal;
	}
	return stmTotal;
}

Bool Media::AudioConcatSource::Start(Optional<Sync::Event> evt, UOSInt blkSize)
{
	NN<Media::AudioSource> audSrc;
	if (this->readEvt.NotNull() && this->currStm > 0 && this->stmList.GetItem(this->currStm).SetTo(audSrc))
	{
		audSrc->Stop();
	}
	this->readEvt = evt;
	this->readBlkSize = blkSize;

	this->readOfst = 0;
	this->currStm = 0;
	if (this->stmList.GetItem(this->currStm).SetTo(audSrc))
	{
		audSrc->Start(this->readEvt, this->readBlkSize);
		return true;
	}
	return false;
}

void Media::AudioConcatSource::Stop()
{
	NN<Media::AudioSource> audSrc;
	if (this->readEvt.NotNull() && this->stmList.GetItem(this->currStm).SetTo(audSrc))
	{
		audSrc->Stop();
	}
	this->readEvt = 0;
}

UOSInt Media::AudioConcatSource::ReadBlock(Data::ByteArray blk)
{
	NN<Media::AudioSource> audSrc;
	if (!this->stmList.GetItem(this->currStm).SetTo(audSrc))
		return 0;
	UOSInt readSize = audSrc->ReadBlock(blk);
	if (readSize > 0)
		return readSize;
	if (this->currStm + 1 >= this->stmList.GetCount())
	{
		return 0;
	}
	if (this->readEvt.NotNull())
	{
		audSrc->Stop();
		this->currStm++;
		if (!this->stmList.GetItem(this->currStm).SetTo(audSrc))
			return 0;
		audSrc->Start(this->readEvt, this->readBlkSize);
	}
	else
	{
		this->currStm++;
		if (!this->stmList.GetItem(this->currStm).SetTo(audSrc))
			return 0;
	}
	return audSrc->ReadBlock(blk);
}

Data::Duration Media::AudioConcatSource::GetCurrTime()
{
	Data::Duration totalTime = 0;
	UOSInt i = this->currStm;
	NN<Media::AudioSource> astm;
	if (this->stmList.GetItem(i).SetTo(astm))
	{
		totalTime = astm->GetCurrTime();
	}
	while (i-- > 0)
	{
		if (this->stmList.GetItem(i).SetTo(astm))
		{
			totalTime = totalTime + astm->GetStreamTime();
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
		NN<Media::AudioSource> audioSrc;
		if (!this->stmList.GetItem(this->currStm).SetTo(audioSrc) || audioSrc->IsEnd())
		{
			return true;
		}
	}
	return false;
}
