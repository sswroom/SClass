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
	UIntOS inBuffSamples;
	UIntOS outBuffSamples;
	UnsafeArrayOpt<UInt8> inReadBuff;
	UnsafeArrayOpt<UInt8> inFloatBuff;
	UnsafeArrayOpt<UInt8> outFloatBuff;
};

Media::SOXRFilter::SOXRFilter(NN<Media::AudioSource> sourceAudio, UInt32 targetFreq) : Media::AudioFilter(sourceAudio)
{
	this->clsData = MemAllocNN(ClassData);
	this->clsData->targetFreq = targetFreq;
	this->clsData->inBuffSamples = 0;
	this->clsData->outBuffSamples = 0;
	this->clsData->inReadBuff = nullptr;
	this->clsData->inFloatBuff = nullptr;
	this->clsData ->outFloatBuff = nullptr;
	this->clsData->afmt.extra = 0;
	sourceAudio->GetFormat(this->clsData->afmt);
	this->clsData->soxr = soxr_create(this->clsData->afmt.frequency, targetFreq, this->clsData->afmt.nChannels, &this->clsData->error, 0, 0, 0);
}

Media::SOXRFilter::~SOXRFilter()
{
	UnsafeArray<UInt8> buff;
	soxr_delete(this->clsData->soxr);
	if (this->clsData->inReadBuff.SetTo(buff))
	{
		MemFreeArr(buff);
	}
	if (this->clsData->inFloatBuff.SetTo(buff))
	{
		MemFreeArr(buff);
	}
	if (this->clsData->outFloatBuff.SetTo(buff))
	{
		MemFreeArr(buff);
	}
	MemFreeNN(this->clsData);
}

UIntOS Media::SOXRFilter::ReadBlock(Data::ByteArray blk)
{
	UnsafeArray<UInt8> inReadBuff;
	UnsafeArray<UInt8> inFloatBuff;
	UnsafeArray<UInt8> outFloatBuff;
	UIntOS nOutSample = blk.GetSize() / this->clsData->afmt.align;
	UIntOS nInSample = nOutSample * this->clsData->afmt.frequency / this->clsData->targetFreq;
	if (this->clsData->afmt.formatId == 3)
	{
		UIntOS readBlkSize = nInSample * 4 * this->clsData->afmt.nChannels;
		if (this->clsData->inBuffSamples < nInSample || !this->clsData->inReadBuff.SetTo(inReadBuff))
		{
			this->clsData->inBuffSamples = nInSample;
			if (this->clsData->inReadBuff.SetTo(inReadBuff))
			{
				MemFreeArr(inReadBuff);
			}
			this->clsData->inReadBuff = inReadBuff = MemAllocArr(UInt8, readBlkSize);
		}
		UIntOS actSize = this->sourceAudio->ReadBlock(Data::ByteArray(inReadBuff, readBlkSize));
		size_t outSize;
		this->clsData->error = soxr_process(this->clsData->soxr, inReadBuff.Ptr(), (actSize >> 2) / this->clsData->afmt.nChannels, 0, blk.Arr().Ptr(), (blk.GetSize() >> 2) / this->clsData->afmt.nChannels, &outSize);
		return (UIntOS)outSize * 4 * this->clsData->afmt.nChannels;
	}
	else
	{
		UIntOS readBlkSize = nInSample * this->clsData->afmt.align;
		UIntOS readFloatSize = nInSample * 4 * this->clsData->afmt.nChannels;
		if (this->clsData->inBuffSamples < nInSample || !this->clsData->inReadBuff.SetTo(inReadBuff) || !this->clsData->inFloatBuff.SetTo(inFloatBuff))
		{
			this->clsData->inBuffSamples = nInSample;
			if (this->clsData->inReadBuff.SetTo(inReadBuff))
			{
				MemFreeArr(inReadBuff);
			}
			if (this->clsData->inFloatBuff.SetTo(inFloatBuff))
			{
				MemFreeArr(inFloatBuff);
			}
			this->clsData->inReadBuff = inReadBuff = MemAllocArr(UInt8, readBlkSize);
			this->clsData->inFloatBuff = inFloatBuff = MemAllocArr(UInt8, readFloatSize);
		}
		UIntOS outFloatSize = nOutSample * 4 * this->clsData->afmt.nChannels;
		if (this->clsData->outBuffSamples < nOutSample || !this->clsData->outFloatBuff.SetTo(outFloatBuff))
		{
			this->clsData->outBuffSamples = nOutSample;
			if (this->clsData->outFloatBuff.SetTo(outFloatBuff))
			{
				MemFreeArr(outFloatBuff);
			}
			this->clsData->outFloatBuff = outFloatBuff = MemAllocArr(UInt8, outFloatSize);
		}
		UIntOS actReadSize = this->sourceAudio->ReadBlock(Data::ByteArray(inReadBuff, readBlkSize));
		size_t outSize;
		switch (this->clsData->afmt.bitpersample)
		{
		case 16:
			AudioUtil_ConvI16_F32(inReadBuff.Ptr(), inFloatBuff.Ptr(), actReadSize >> 1);
			this->clsData->error = soxr_process(this->clsData->soxr, inFloatBuff.Ptr(), (actReadSize >> 1) / this->clsData->afmt.nChannels, 0, outFloatBuff.Ptr(), (outFloatSize >> 2) / this->clsData->afmt.nChannels, &outSize);
			AudioUtil_ConvF32_I16(outFloatBuff.Ptr(), blk.Arr().Ptr(), (UIntOS)outSize);
			return (UIntOS)outSize * 2 * this->clsData->afmt.nChannels;
		}
		return 0;
	}
}
