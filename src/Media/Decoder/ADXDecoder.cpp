#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "Media/Decoder/ADXDecoder.h"

#define BASEVOLUME 0x4000 //0x4000

Media::Decoder::ADXDecoder::ADXDecoder(NN<Media::IAudioSource> sourceAudio)
{
	Media::AudioFormat fmt;
	this->sourceAudio = 0;
	this->nChannels = 0;
	sourceAudio->GetFormat(fmt);
	if (fmt.formatId != 0x2080)
		return;
	if (fmt.nChannels == 1)
	{
		if (fmt.align != 18)
			return;
		this->nChannels = 1;
	}
	else if (fmt.nChannels == 2)
	{
		if (fmt.align != 36)
			return;
		this->nChannels = 2;
	}
	else
	{
		return;
	}
	this->sourceAudio = sourceAudio.Ptr();
}

Media::Decoder::ADXDecoder::~ADXDecoder()
{
}

void Media::Decoder::ADXDecoder::GetFormat(NN<AudioFormat> format)
{
	NN<Media::IAudioSource> sourceAudio;
	if (this->sourceAudio.SetTo(sourceAudio))
	{
		Media::AudioFormat fmt;
		sourceAudio->GetFormat(fmt);
		format->formatId = 1;
		format->bitpersample = 16;
		format->frequency = fmt.frequency;
		format->nChannels = fmt.nChannels;
		format->bitRate = fmt.frequency * fmt.nChannels << 4;
	}
	else
	{
		format->Clear();
	}
}

Data::Duration Media::Decoder::ADXDecoder::SeekToTime(Data::Duration time)
{
	NN<Media::IAudioSource> sourceAudio;
	if (this->sourceAudio.SetTo(sourceAudio))
	{
		adxSample1[0] = 0;
		adxSample1[1] = 0;
		adxSample2[0] = 0;
		adxSample2[1] = 0;
		return sourceAudio->SeekToTime(time);
	}
	return 0;
}

Bool Media::Decoder::ADXDecoder::Start(Optional<Sync::Event> evt, UOSInt blkSize)
{
	NN<Sync::Event> readEvt;
	NN<Media::IAudioSource> sourceAudio;
	if (this->sourceAudio.SetTo(sourceAudio))
	{
		sourceAudio->Start(0, blkSize);
		this->readEvt = evt;
		adxSample1[0] = 0;
		adxSample1[1] = 0;
		adxSample2[0] = 0;
		adxSample2[1] = 0;
		if (this->readEvt.SetTo(readEvt))
			readEvt->Set();
		return true;
	}
	return false;
}

void Media::Decoder::ADXDecoder::Stop()
{
	NN<Media::IAudioSource> sourceAudio;
	if (this->sourceAudio.SetTo(sourceAudio))
	{
		sourceAudio->Stop();
	}
	this->readEvt = 0;
}

UOSInt Media::Decoder::ADXDecoder::ReadBlock(Data::ByteArray blk)
{
	NN<Sync::Event> readEvt;
	UOSInt blkSize = blk.GetSize();
	UOSInt outSize;
	UOSInt readSize;
	Data::ByteArray src;
	Data::ByteArray dest;
	NN<Media::IAudioSource> sourceAudio;
	if (!this->sourceAudio.SetTo(sourceAudio))
	{
		return 0;
	}
	else if (this->nChannels == 1)
	{
		blkSize = blkSize >> 6;
		readSize = blkSize * 18;
		if (readBuff.GetSize() < readSize)
		{
			readBuff.ChangeSize(readSize);
		}
		readSize = sourceAudio->ReadBlock(readBuff.WithSize(readSize));
		blkSize = readSize / 18;
		outSize = blkSize << 6;

		src = readBuff;
		dest = blk;
		while (blkSize-- > 0)
		{
			Convert(src, dest, 2, 0);
			src += 18;
			dest += 64;
		}
		if (this->readEvt.SetTo(readEvt))
			readEvt->Set();
		return outSize;
	}
	else if (this->nChannels == 2)
	{
		blkSize = blkSize >> 7;
		outSize = blkSize << 7;
		readSize = blkSize * 36;
		if (readBuff.GetSize() < readSize)
		{
			readBuff.ChangeSize(readSize);
		}
		readSize = sourceAudio->ReadBlock(readBuff.WithSize(readSize));
		blkSize = readSize / 36;
		outSize = blkSize << 7;

		src = readBuff;
		dest = blk;
		while (blkSize-- > 0)
		{
			Convert(src, dest, 4, 0);
			Convert(src.SubArray(18), dest.SubArray(2), 4, 1);
			src += 36;
			dest += 128;
		}
		if (this->readEvt.SetTo(readEvt))
			readEvt->Set();
		return outSize;
	}
	else
	{
		return 0;
	}
}

UOSInt Media::Decoder::ADXDecoder::GetMinBlockSize()
{
	if (this->nChannels == 1)
	{
		return 64;
	}
	else if (this->nChannels == 2)
	{
		return 128;
	}
	return 0;
}


void Media::Decoder::ADXDecoder::Convert(Data::ByteArray src, Data::ByteArray dest, Int32 sampleByte, Int32 channel)
{
	Int32 scale = ( (src[0] << 8) | (src[1])) << 2;
	//if (scale & 0x8000)
	//	scale -= 65536;
	Int32 i;
	Int64 s0, d;

	src += 2;
	i = 16;
	while (i--)
	{
		d = (*src) >> 4;
		if (d > 8)
			d -= 16;
		s0 = (BASEVOLUME * d * scale + ((0x7298 * adxSample1[channel]) >> 14) - ((0x3350 * adxSample2[channel]) >> 14));
		adxSample2[channel] = adxSample1[channel];
		if (s0 > 0x7fffffff)
			s0 = 0x7fffffff;
		else if (((Int32*)&s0)[1] < -1)
			s0 = (Int64)0xffffffff80000000;
		else if (((Int32*)&s0)[1] == -1)
			if (((*(Int32*)&s0) & (Int32)0x80000000) == 0)
				s0 = (Int64)0xffffffff80000000;
		adxSample1[channel] = s0;
		dest.WriteNU16(0, (UInt16)(s0 >> 16));
		dest += sampleByte;


		d = (*src++) & 15;
		if (d >= 8)
			d -= 16;
		s0 = (BASEVOLUME * d * scale + ((0x7298 * adxSample1[channel]) >> 14) - ((0x3350 * adxSample2[channel]) >> 14));
		adxSample2[channel] = adxSample1[channel];
		if (s0 > 0x7fffffff)
			s0 = 0x7fffffff;
		else if (((Int32*)&s0)[1] < -1)
			s0 = (Int64)0xffffffff80000000;
		else if (((Int32*)&s0)[1] == -1)
			if (((*(Int32*)&s0) & (Int32)0x80000000) == 0)
				s0 = (Int64)0xffffffff80000000;
		adxSample1[channel] = s0;
		dest.WriteNU16(0, (UInt16)(s0 >> 16));
		dest += sampleByte;
	}
}
