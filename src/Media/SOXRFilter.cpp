#include "Stdafx.h"
#include "Media/AudioUtil.h"
#include "Media/SOXRFilter.h"
#include <soxr.h>

struct Media::SOXRFilter::ClassData
{
	soxr_t soxr;
	Media::AudioFormat afmt;
	soxr_error_t error;
	UInt32 targetFreq;
	UOSInt inBuffSamples;
	UOSInt outBuffSamples;
	UInt8 *inReadBuff;
	UInt8 *inFloatBuff;
	UInt8 *outFloatBuff;
};

Media::SOXRFilter::SOXRFilter(Media::IAudioSource *sourceAudio, UInt32 targetFreq) : Media::IAudioFilter(sourceAudio)
{
	this->clsData = MemAlloc(ClassData, 1);
	this->clsData->targetFreq = targetFreq;
	this->clsData->inBuffSamples = 0;
	this->clsData->outBuffSamples = 0;
	this->clsData->inReadBuff = 0;
	this->clsData->inFloatBuff = 0;
	this->clsData ->outFloatBuff = 0;
	this->clsData->afmt.extra = 0;
	sourceAudio->GetFormat(&this->clsData->afmt);
	this->clsData->soxr = soxr_create(this->clsData->afmt.frequency, targetFreq, this->clsData->afmt.nChannels, &this->clsData->error, 0, 0, 0);
}

Media::SOXRFilter::~SOXRFilter()
{
	soxr_delete(this->clsData->soxr);
	if (this->clsData->inReadBuff)
	{
		MemFree(this->clsData->inReadBuff);
	}
	if (this->clsData->inFloatBuff)
	{
		MemFree(this->clsData->inFloatBuff);
	}
	if (this->clsData->outFloatBuff)
	{
		MemFree(this->clsData->outFloatBuff);
	}
	MemFree(this->clsData);
}

UOSInt Media::SOXRFilter::ReadBlock(Data::ByteArray blk)
{
	UOSInt nOutSample = blk.GetSize() / this->clsData->afmt.align;
	UOSInt nInSample = nOutSample * this->clsData->afmt.frequency / this->clsData->targetFreq;
	if (this->clsData->afmt.formatId == 3)
	{
		UOSInt readBlkSize = nInSample * 4 * this->clsData->afmt.nChannels;
		if (this->clsData->inBuffSamples < nInSample)
		{
			this->clsData->inBuffSamples = nInSample;
			if (this->clsData->inReadBuff)
			{
				MemFree(this->clsData->inReadBuff);
			}
			this->clsData->inReadBuff = MemAlloc(UInt8, readBlkSize);
		}
		UOSInt actSize = this->sourceAudio->ReadBlock(Data::ByteArray(this->clsData->inReadBuff, readBlkSize));
		size_t outSize;
		this->clsData->error = soxr_process(this->clsData->soxr, this->clsData->inReadBuff, (actSize >> 2) / this->clsData->afmt.nChannels, 0, blk.Ptr(), (blk.GetSize() >> 2) / this->clsData->afmt.nChannels, &outSize);
		return (UOSInt)outSize * 4 * this->clsData->afmt.nChannels;
	}
	else
	{
		UOSInt readBlkSize = nInSample * this->clsData->afmt.align;
		UOSInt readFloatSize = nInSample * 4 * this->clsData->afmt.nChannels;
		if (this->clsData->inBuffSamples < nInSample)
		{
			this->clsData->inBuffSamples = nInSample;
			if (this->clsData->inReadBuff)
			{
				MemFree(this->clsData->inReadBuff);
			}
			if (this->clsData->inFloatBuff)
			{
				MemFree(this->clsData->inFloatBuff);
			}
			this->clsData->inReadBuff = MemAlloc(UInt8, readBlkSize);
			this->clsData->inFloatBuff = MemAlloc(UInt8, readFloatSize);
		}
		UOSInt outFloatSize = nOutSample * 4 * this->clsData->afmt.nChannels;
		if (this->clsData->outBuffSamples < nOutSample)
		{
			this->clsData->outBuffSamples = nOutSample;
			if (this->clsData->outFloatBuff)
			{
				MemFree(this->clsData->outFloatBuff);
			}
			this->clsData->outFloatBuff = MemAlloc(UInt8, outFloatSize);
		}
		UOSInt actReadSize = this->sourceAudio->ReadBlock(Data::ByteArray(this->clsData->inReadBuff, readBlkSize));
		size_t outSize;
		switch (this->clsData->afmt.bitpersample)
		{
		case 16:
			AudioUtil_ConvI16_F32(this->clsData->inReadBuff, this->clsData->inFloatBuff, actReadSize >> 1);
			this->clsData->error = soxr_process(this->clsData->soxr, this->clsData->inFloatBuff, (actReadSize >> 1) / this->clsData->afmt.nChannels, 0, this->clsData->outFloatBuff, (outFloatSize >> 2) / this->clsData->afmt.nChannels, &outSize);
			AudioUtil_ConvF32_I16(this->clsData->outFloatBuff, blk.Ptr(), (UOSInt)outSize);
			return (UOSInt)outSize * 2 * this->clsData->afmt.nChannels;
		}
		return 0;
	}
}
