#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "Math/Math_C.h"
#include "Media/AFilter/AudioLevelMeter.h"
#include "Sync/MutexUsage.h"

void Media::AFilter::AudioLevelMeter::ResetStatus()
{
	Sync::MutexUsage mutUsage(this->mut);
	UOSInt i = this->soundBuffLeng;
	while (i-- > 0)
	{
		this->soundBuff[i] = 0;
	}
	i = this->nChannel;
	while (i-- > 0)
	{
		this->status[i].maxLevel = 0;
		this->status[i].minLevel = 0;
		this->status[i].levelChanged = true;
	}
	this->soundBuffOfst = 0;
	mutUsage.EndUse();
}

Media::AFilter::AudioLevelMeter::AudioLevelMeter(NN<Media::AudioSource> sourceAudio) : Media::AudioFilter(sourceAudio)
{
	Media::AudioFormat fmt;
	this->soundBuff = 0;
	this->status = 0;
	this->nChannel = 0;
	this->bitCount = 0;
	sourceAudio->GetFormat(fmt);
	if (fmt.formatId != 0x1)
		return;
	this->nChannel = fmt.nChannels;
	this->bitCount = fmt.bitpersample;
	this->soundBuffLeng = (fmt.frequency / 10) * fmt.nChannels;
	this->soundBuff = MemAlloc(Int32, this->soundBuffLeng);
	this->status = MemAlloc(ChannelStatus, this->nChannel);
	this->ResetStatus();
}

Media::AFilter::AudioLevelMeter::~AudioLevelMeter()
{
	if (this->soundBuff)
	{
		MemFree(this->soundBuff);
		this->soundBuff = 0;
	}
	if (this->status)
	{
		MemFree(this->status);
		this->status = 0;
	}
}

Data::Duration Media::AFilter::AudioLevelMeter::SeekToTime(Data::Duration time)
{
	this->ResetStatus();
	return this->sourceAudio->SeekToTime(time);
}

UOSInt Media::AFilter::AudioLevelMeter::ReadBlock(Data::ByteArray blk)
{
	UOSInt readSize = this->sourceAudio->ReadBlock(blk);
	if (this->bitCount == 16)
	{
		UOSInt i = 0;
		UOSInt j;
		Sync::MutexUsage mutUsage(this->mut);
		UInt32 k = this->soundBuffOfst;
		Int32 v;
		while (i < blk.GetSize())
		{
			j = 0;
			while (j < this->nChannel)
			{
				v = this->soundBuff[k + j];
				if (v == this->status[j].maxLevel || v == this->status[j].minLevel)
				{
					this->status[j].levelChanged = true;
				}
				v = ReadInt16(&blk[i]);
				this->soundBuff[k + j] = v;
				if (v > this->status[j].maxLevel)
				{
					this->status[j].maxLevel = v;
				}
				if (v < this->status[j].minLevel)
				{
					this->status[j].minLevel = v;
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
		UOSInt i = 0;
		UOSInt j;
		Sync::MutexUsage mutUsage(this->mut);
		UInt32 k = this->soundBuffOfst;
		Int32 v;
		while (i < blk.GetSize())
		{
			j = 0;
			while (j < this->nChannel)
			{
				v = this->soundBuff[k + j];
				if (v == this->status[j].maxLevel || v == this->status[j].minLevel)
				{
					this->status[j].levelChanged = true;
				}
				v = blk[i] - 128;
				this->soundBuff[k + j] = v;
				if (v > this->status[j].maxLevel)
				{
					this->status[j].maxLevel = v;
				}
				if (v < this->status[j].minLevel)
				{
					this->status[j].minLevel = v;
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

Double Media::AFilter::AudioLevelMeter::GetLevel(UOSInt channel)
{
	Double ret = 0;
	Int32 v;
	if (channel >= this->nChannel)
		return 0;
	Sync::MutexUsage mutUsage(this->mut);
	if (this->status[channel].levelChanged)
	{
		this->status[channel].levelChanged = false;
		UOSInt i = channel;
		Int32 v;
		this->status[channel].minLevel = this->soundBuff[i];
		this->status[channel].maxLevel = this->soundBuff[i];
		while (i < this->soundBuffLeng)
		{
			v = this->soundBuff[i];
			if (v > this->status[channel].maxLevel)
				this->status[channel].maxLevel = v;
			if (v < this->status[channel].minLevel)
				this->status[channel].minLevel = v;
			i += this->nChannel;
		}
	}
	v = this->status[channel].minLevel;
	if (v < 0)
	{
		v = -v;
	}
	if (this->status[channel].maxLevel > v)
	{
		v = this->status[channel].maxLevel;
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
