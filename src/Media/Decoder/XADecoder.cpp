#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "Media/Decoder/XADecoder.h"

Media::Decoder::XADecoder::XADecoder(NN<Media::AudioSource> sourceAudio)
{
	Media::AudioFormat fmt;
	this->sourceAudio = 0;
	this->nChannels = 0;
	sourceAudio->GetFormat(fmt);
	if (fmt.formatId != 0x2082)
		return;
	if (fmt.nChannels == 1)
	{
		if (fmt.align != 128)
			return;
		this->nChannels = 1;
	}
	else if (fmt.nChannels == 2)
	{
		if (fmt.align != 128)
			return;
		this->nChannels = 2;
	}
	else
	{
		return;
	}
	this->sourceAudio = sourceAudio.Ptr();
}

Media::Decoder::XADecoder::~XADecoder()
{
}

void Media::Decoder::XADecoder::GetFormat(NN<AudioFormat> format)
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
		format->align = (UInt32)(fmt.nChannels * 2);
		format->other = 0;
		format->intType = Media::AudioFormat::IT_NORMAL;
		format->extraSize = 0;
		format->extra = 0;
	}
	else
	{
		format->Clear();
	}
}

Data::Duration Media::Decoder::XADecoder::SeekToTime(Data::Duration time)
{
	NN<Media::AudioSource> sourceAudio;
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

Bool Media::Decoder::XADecoder::Start(Optional<Sync::Event> evt, UOSInt blkSize)
{
	NN<Sync::Event> readEvt;
	NN<Media::AudioSource> sourceAudio;
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

void Media::Decoder::XADecoder::Stop()
{
	NN<Media::AudioSource> sourceAudio;
	if (this->sourceAudio.SetTo(sourceAudio))
	{
		sourceAudio->Stop();
	}
	this->readEvt = 0;
}

UOSInt Media::Decoder::XADecoder::ReadBlock(Data::ByteArray buff)
{
	UOSInt outSize;
	UOSInt readSize;
	Data::ByteArray src;
	Data::ByteArray dest;
	NN<Sync::Event> readEvt;
	NN<Media::AudioSource> sourceAudio;
	if (!this->sourceAudio.SetTo(sourceAudio))
		return 0;

	UOSInt blkSize = buff.GetSize() / 448;
	readSize = blkSize << 7;
	if (readBuff.GetSize() < readSize)
	{
		readBuff.ChangeSizeAndClear(readSize);
	}
	readSize = sourceAudio->ReadBlock(readBuff.WithSize(readSize));
	blkSize = readSize >> 7;
	outSize = blkSize * 448;

	src = readBuff;
	dest = buff;
	while (blkSize-- > 0)
	{
		Convert(src, dest, 2, 0);
		src += 128;
		dest += 448;
	}
	if (this->readEvt.SetTo(readEvt))
		readEvt->Set();
	return outSize;
}

UOSInt Media::Decoder::XADecoder::GetMinBlockSize()
{
	return 448;
}

#define ROUND64_32(x) \
if (x > 0x7fffffff) \
	x = 0x7fffffff; \
else if (((Int32*)&x)[1] < -1) \
	x = (Int64)0xffffffff80000000LL; \
else if (((Int32*)&x)[1] == -1) \
	if (((*(UInt32*)&x) & 0x80000000) == 0) \
		x = (Int64)0xffffffff80000000LL;

void Media::Decoder::XADecoder::Convert(Data::ByteArray src, Data::ByteArray dest, Int32 sampleByte, Int32 channel)
{
	Int32 j, r0, r1, f;
	Int32 i;
	Int64 y,x;

	Int32 iF00, iF01, iF10, iF11;
	Int32 iD;

	Int32 K0[5] = { 0x0000, 0x00f0, 0x01cc, 0x0188, 0x01E8};
	Int32 K1[5] = { 0x0000, 0x0000,-0x00d0,-0x00dc, -0x0F0};

	if (this->nChannels == 2)
	{
		i = 0;
		while (i < 4)
		{
			r0 = 28 - (src[i * 2 + 4] & 15);
			if ((f = src[i * 2 + 4] >> 4) > 4)
				f = 4;
			iF00 = K0[f];
			iF10 = K1[f];

			r1	= 28 - (src[i * 2 + 5] & 15);
			if ((f = src[i * 2 + 5] >> 4) > 4)
				f = 4;
			iF01 = K0[f];
			iF11 = K1[f];

			j = 0;
			while (j < 28)
			{
				iD = src[i + j * 4 + 16];
				x = iD & 0xf;
				if(x >= 8)
					x -= 16;

				x <<= r0;
				y = (adxSample1[0] * iF00 + adxSample2[0] * iF10) >> 7;
				x += (y >> 1) + (y & 1);
				adxSample2[0] = adxSample1[0];
				if (x > 0x7fffffff)
					x = 0x7fffffff;
				else if (((Int32*)&x)[1] < -1)
					x = (Int64)0xffffffff80000000LL;
				else if (((Int32*)&x)[1] == -1)
					if (((*(UInt32*)&x) & 0x80000000) == 0)
						x = (Int64)0xffffffff80000000LL;
				adxSample1[0] = x;
				dest.WriteNI16(0, (Int16)(x >> 16));
				dest += 2;

				x = iD >> 4;
				if(x >= 8)
					x -= 16;

				x <<= r1;
				y = (adxSample1[1] * iF01 + adxSample2[1] * iF11) >> 7;
				x += (y >> 1) + (y & 1);
				adxSample2[1] = adxSample1[1];
				if (x > 0x7fffffff)
					x = 0x7fffffff;
				else if (((Int32*)&x)[1] < -1)
					x = (Int64)0xffffffff80000000LL;
				else if (((Int32*)&x)[1] == -1)
					if (((*(UInt32*)&x) & 0x80000000) == 0)
						x = (Int64)0xffffffff80000000LL;
				adxSample1[1] = x;
				dest.WriteNI16(0, (Int16)(x >> 16));
				dest += 2;

				j++;
			}
			i++;
		}
	}
	else
	{
		i = 0;
		while (i < 4)
		{
			r0 = 28 - (src[i * 2 + 4] & 15);
			if ((f = src[i * 2 + 4] >> 4) > 4)
				f = 4;
			iF00 = K0[f];
			iF10 = K1[f];

			j = 0;
			while (j < 28)
			{
				iD = src[i + j * 4 + 16];
				x = iD & 0xf;
				if(x >= 8)
					x -= 16;

				x <<= r0;
				y = (adxSample1[0] * iF00 + adxSample2[0] * iF10) >> 7;
				x += (y >> 1) + (y & 1);
				adxSample2[0] = adxSample1[0];
				ROUND64_32(x);
				adxSample1[0] = x;
				dest.WriteNI16(0, (Int16)(x >> 16));
				dest += 2;
				j++;
			}

			r0 = 28 - (src[i * 2 + 5] & 15);
			if ((f = src[i * 2 + 5] >> 4) > 4)
				f = 4;
			iF00 = K0[f];
			iF10 = K1[f];

			j = 0;
			while (j < 28)
			{
				iD = src[i + j * 4 + 16];
				x = iD >> 4;
				if(x >= 8)
					x -= 16;

				x <<= r0;
				y = (adxSample1[0] * iF00 + adxSample2[0] * iF10) >> 7;
				x += (y >> 1) + (y & 1);
				adxSample2[0] = adxSample1[0];
				ROUND64_32(x);
				adxSample1[0] = x;
				dest.WriteNI16(0, (Int16)(x >> 16));
				dest += 2;
				j++;
			}
			i++;
		}
	}
}
