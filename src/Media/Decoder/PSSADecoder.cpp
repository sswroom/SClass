#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "Media/Decoder/PSSADecoder.h"

#define BASEVOLUME 0x4000 //0x4000

Media::Decoder::PSSADecoder::PSSADecoder(NN<Media::AudioSource> sourceAudio)
{
	Media::AudioFormat fmt;
	this->sourceAudio = nullptr;
	this->nChannels = 0;
	this->readBuffSize = 0;
	this->buffSize = 0;
	sourceAudio->GetFormat(fmt);
	if (fmt.formatId != 0x2081)
		return;
	if (fmt.nChannels == 1)
	{
		if (fmt.bitpersample != 16)
			return;
		this->nChannels = 1;
		this->nBlockAlign = fmt.align;
	}
	else if (fmt.nChannels == 2)
	{
		if (fmt.bitpersample & 255)
			return;
		this->nChannels = 2;
		this->nBlockAlign = fmt.align;
	}
	else
	{
		return;
	}
	this->sourceAudio = sourceAudio.Ptr();
	this->readBuffSize = sourceAudio->GetMinBlockSize();
	this->readBuff.ChangeSizeAndClear(this->readBuffSize);
}

Media::Decoder::PSSADecoder::~PSSADecoder()
{
}

void Media::Decoder::PSSADecoder::GetFormat(NN<AudioFormat> format)
{
	NN<Media::AudioSource> sourceAudio;
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
		format->formatId = 0;
		format->bitpersample = 0;
		format->frequency = 0;
		format->nChannels = 0;
		format->bitRate = 0;
	}
}

Data::Duration Media::Decoder::PSSADecoder::SeekToTime(Data::Duration time)
{
	NN<Media::AudioSource> sourceAudio;
	if (this->sourceAudio.SetTo(sourceAudio))
	{
		adxSample1[0] = 0;
		adxSample1[1] = 0;
		adxSample2[0] = 0;
		adxSample2[1] = 0;
		this->buffSize = 0;
		return sourceAudio->SeekToTime(time);
	}
	return 0;
}

Bool Media::Decoder::PSSADecoder::Start(Optional<Sync::Event> evt, UOSInt blkSize)
{
	NN<Sync::Event> readEvt;
	NN<Media::AudioSource> sourceAudio;
	if (this->sourceAudio.SetTo(sourceAudio))
	{
		sourceAudio->Start(nullptr, blkSize);
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

void Media::Decoder::PSSADecoder::Stop()
{
	NN<Media::AudioSource> sourceAudio;
	if (this->sourceAudio.SetTo(sourceAudio))
	{
		sourceAudio->Stop();
	}
	this->readEvt = nullptr;
}

UOSInt Media::Decoder::PSSADecoder::ReadBlock(Data::ByteArray buff)
{
	NN<Sync::Event> readEvt;
	UOSInt blkSize = buff.GetSize();
	UOSInt outSize = 0;
	UOSInt readSize;
	Data::ByteArray src;
	Data::ByteArray dest;
	NN<Media::AudioSource> sourceAudio;
	if (!this->sourceAudio.SetTo(sourceAudio))
	{
		return 0;
	}
	else if (this->nChannels == 1)
	{
		blkSize = blkSize / 56;
		src = this->readBuff;
		dest = buff;
		while (blkSize > 0)
		{
			if (this->buffSize >= 16)
			{
				Convert(src, dest, 2, 0);
				src += 16;
				dest += 56;
				this->buffSize -= 16;
				outSize += 56;
				blkSize--;
			}
			else
			{
				if (buffSize > 0)
				{
					this->readBuff.CopyFrom(src.WithSize(this->buffSize));
				}
				readSize = sourceAudio->ReadBlock(readBuff.SubArray(buffSize, this->readBuffSize));
				if (readSize <= 0)
					break;
				this->buffSize += readSize;
				src = this->readBuff;
			}
		}
		if (buffSize > 0)
		{
			this->readBuff.CopyFrom(src.WithSize(this->buffSize));
		}
		if (this->readEvt.SetTo(readEvt))
			readEvt->Set();
		return outSize;
	}
	else if (this->nChannels == 2)
	{
		blkSize = blkSize / 56 / (this->nBlockAlign >> 4);
		src = this->readBuff;
		while (blkSize > 0)
		{
			if (this->buffSize >= this->nBlockAlign)
			{
				dest = buff.SubArray(2);
				UOSInt j;
				j = this->nBlockAlign >> 5;
				while (j-- > 0)
				{
					Convert(src, buff, 4, 0);
					src += 16;
					buff += 112;
				}
				j = this->nBlockAlign >> 5;
				while (j-- > 0)
				{
					Convert(src, dest, 4, 1);
					src += 16;
					dest += 112;
				}
				outSize += 112 * this->nBlockAlign >> 5;
				this->buffSize -= this->nBlockAlign;
				blkSize--;
			}
			else
			{
				if (buffSize > 0)
				{
					this->readBuff.CopyFrom(src.WithSize(this->buffSize));
				}
				readSize = sourceAudio->ReadBlock(readBuff.SubArray(buffSize, this->readBuffSize));
				if (readSize <= 0)
					break;
				this->buffSize += readSize;
				src = this->readBuff;
			}
		}
		if (buffSize > 0)
		{
			this->readBuff.CopyFrom(src.WithSize(this->buffSize));
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

UOSInt Media::Decoder::PSSADecoder::GetMinBlockSize()
{
	if (this->nChannels == 1)
	{
		return 56;
	}
	else if (this->nChannels == 2)
	{
		return 56 * (this->nBlockAlign >> 4);
	}
	return 0;
}

void Media::Decoder::PSSADecoder::Convert(Data::ByteArray src, Data::ByteArray dest, Int32 sampleByte, Int32 channel)
{
	Int32 j,r,f;
	Int64 y,x;
	if (src[1] & 0xf0)
	{
		j = 28;
		while (j--)
		{
			dest.WriteNI16(0, 0);
			dest += sampleByte;
		}
		return;
	}
	Int32 K0[5] = { 0x0000, 0x00f0, 0x01cc, 0x0188, 0x01E8};
	Int32 K1[5] = { 0x0000, 0x0000,-0x00d0,-0x00dc, -0x0F0};

	r = 28 - (*src & 15);
	f = *src >> 4;
	src += 2;
	j = 14;
	while (j--)
	{
		x = *src & 15;
		if(x >= 8) x -= 16;
		x <<= r;
		y = (adxSample1[channel] * K0[f] + adxSample2[channel] * K1[f]) >> 7;
		x += (y >> 1) + (y & 1);
		adxSample2[channel] = adxSample1[channel];
		if (x > 0x7fffffff)
			x = 0x7fffffff;
		else if (((Int32*)&x)[1] < -1)
			x = (Int64)0xffffffff80000000;
		else if (((Int32*)&x)[1] == -1)
			if (((*(UInt32*)&x) & 0x80000000) == 0)
				x = (Int64)0xffffffff80000000;
		adxSample1[channel] = x;
		dest.WriteNI16(0, (Int16)(x >> 16));
		dest += sampleByte;

		x = *src++ >> 4;
		if (x >= 8) x -= 16;
		x <<= r;
		y = (adxSample1[channel] * K0[f] + adxSample2[channel] * K1[f]) >> 7;
		x += (y >> 1) + (y & 1);
		adxSample2[channel] = adxSample1[channel];
		if (x > 0x7fffffff)
			x = 0x7fffffff;
		else if (((Int32*)&x)[1] < -1)
			x = (Int64)0xffffffff80000000;
		else if (((Int32*)&x)[1] == -1)
			if (((*(UInt32*)&x) & 0x80000000) == 0)
				x = (Int64)0xffffffff80000000;
		adxSample1[channel] = x;
		dest.WriteNI16(0, (Int16)(x >> 16));
		dest += sampleByte;
	}
}
