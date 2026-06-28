#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/ByteTool_C.h"
#include "Math/Math_C.h"
#include "Media/AFilter/AudioLevelMeter.h"
#include "Sync/MutexUsage.h"

void Media::AFilter::AudioLevelMeter::ResetStatus()
{
	Sync::MutexUsage mutUsage(this->mut);
	UnsafeArray<Int32> soundBuff;
	UnsafeArray<ChannelStatus> status;
	UIntOS i;
	if (this->soundBuff.SetTo(soundBuff))
	{
		i = this->soundBuffLeng;
		while (i-- > 0)
		{
			soundBuff[i] = 0;
		}
	}
	if (this->status.SetTo(status))
	{
		i = this->nChannel;
		while (i-- > 0)
		{
			status[i].maxLevel = 0;
			status[i].minLevel = 0;
			status[i].levelChanged = true;
		}
	}
	this->soundBuffOfst = 0;
}

Media::AFilter::AudioLevelMeter::AudioLevelMeter(NN<Media::AudioSource> sourceAudio) : Media::AudioFilter(sourceAudio)
{
	Media::AudioFormat fmt;
	this->soundBuff = nullptr;
	this->status = nullptr;
	this->nChannel = 0;
	this->bitCount = 0;
	sourceAudio->GetFormat(fmt);
	if (fmt.formatId != 0x1)
		return;
	this->nChannel = fmt.nChannels;
	this->bitCount = fmt.bitpersample;
	this->soundBuffLeng = (fmt.frequency / 10) * fmt.nChannels;
	this->soundBuff = MemAllocArr(Int32, this->soundBuffLeng);
	this->status = MemAllocArr(ChannelStatus, this->nChannel);
	this->ResetStatus();
}

Media::AFilter::AudioLevelMeter::~AudioLevelMeter()
{
	UnsafeArray<Int32> soundBuff;
	UnsafeArray<ChannelStatus> status;
	if (this->soundBuff.SetTo(soundBuff))
	{
		MemFreeArr(soundBuff);
		this->soundBuff = nullptr;
	}
	if (this->status.SetTo(status))
	{
		MemFreeArr(status);
		this->status = nullptr;
	}
}

Data::Duration Media::AFilter::AudioLevelMeter::SeekToTime(Data::Duration time)
{
	this->ResetStatus();
	return this->sourceAudio->SeekToTime(time);
}

UIntOS Media::AFilter::AudioLevelMeter::ReadBlock(Data::ByteArray blk)
{
	UnsafeArray<Int32> soundBuff;
	UnsafeArray<ChannelStatus> status;
	if (!this->soundBuff.SetTo(soundBuff) || !this->status.SetTo(status))
	{
		return 0;
	}
	UIntOS readSize = this->sourceAudio->ReadBlock(blk);
	if (this->bitCount == 16)
	{
		UIntOS i = 0;
		UIntOS j;
		Sync::MutexUsage mutUsage(this->mut);
		UInt32 k = this->soundBuffOfst;
		Int32 v;
		while (i < blk.GetSize())
		{
			j = 0;
			while (j < this->nChannel)
			{
				v = soundBuff[k + j];
				if (v == status[j].maxLevel || v == status[j].minLevel)
				{
					status[j].levelChanged = true;
				}
				v = ReadInt16(&blk[i]);
				soundBuff[k + j] = v;
				if (v > status[j].maxLevel)
				{
					status[j].maxLevel = v;
				}
				if (v < status[j].minLevel)
				{
					status[j].minLevel = v;
				}
				j++;
				i += 2;
			}
			k = (k + this->nChannel) % this->soundBuffLeng;
		}
		this->soundBuffOfst = k;
		mutUsage.EndUse();
	}
	else if (this->bitCount == 8)
	{
		UIntOS i = 0;
		UIntOS j;
		Sync::MutexUsage mutUsage(this->mut);
		UInt32 k = this->soundBuffOfst;
		Int32 v;
		while (i < blk.GetSize())
		{
			j = 0;
			while (j < this->nChannel)
			{
				v = soundBuff[k + j];
				if (v == status[j].maxLevel || v == status[j].minLevel)
				{
					status[j].levelChanged = true;
				}
				v = blk[i] - 128;
				soundBuff[k + j] = v;
				if (v > status[j].maxLevel)
				{
					status[j].maxLevel = v;
				}
				if (v < status[j].minLevel)
				{
					status[j].minLevel = v;
				}
				j++;
				i += 1;
			}
			k = (k + this->nChannel) % this->soundBuffLeng;
		}
		this->soundBuffOfst = k;
		mutUsage.EndUse();
	}
	return readSize;
}

Double Media::AFilter::AudioLevelMeter::GetLevel(UIntOS channel)
{
	UnsafeArray<Int32> soundBuff;
	UnsafeArray<ChannelStatus> status;
	Double ret = 0;
	Int32 v;
	if (!this->soundBuff.SetTo(soundBuff) || !this->status.SetTo(status))
	{
		return 0;
	}
	if (channel >= this->nChannel)
		return 0;
	Sync::MutexUsage mutUsage(this->mut);
	if (status[channel].levelChanged)
	{
		status[channel].levelChanged = false;
		UIntOS i = channel;
		Int32 v;
		status[channel].minLevel = soundBuff[i];
		status[channel].maxLevel = soundBuff[i];
		while (i < this->soundBuffLeng)
		{
			v = soundBuff[i];
			if (v > status[channel].maxLevel)
				status[channel].maxLevel = v;
			if (v < status[channel].minLevel)
				status[channel].minLevel = v;
			i += this->nChannel;
		}
	}
	v = status[channel].minLevel;
	if (v < 0)
	{
		v = -v;
	}
	if (status[channel].maxLevel > v)
	{
		v = status[channel].maxLevel;
	}
	mutUsage.EndUse();
	if (this->bitCount == 16)
	{
		ret = (0.5 + (UInt32)v) / 32767.5;
	}
	else if (this->bitCount == 24)
	{
		ret = (0.5 + (UInt32)v) / 8388607.5;
	}
	else if (this->bitCount == 8)
	{
		ret = (0.5 + (UInt32)v) / 127.5;
	}
	if (ret > 1)
		ret = 1;
	return ret;
}
