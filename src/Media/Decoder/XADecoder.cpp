#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "Media/Decoder/XADecoder.h"

Media::Decoder::XADecoder::XADecoder(Media::IAudioSource *sourceAudio)
{
	Media::AudioFormat fmt;
	this->sourceAudio = 0;
	this->nChannels = 0;
	this->readBuff = 0;
	this->readBuffSize = 0;
	sourceAudio->GetFormat(&fmt);
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
	this->sourceAudio = sourceAudio;
}

Media::Decoder::XADecoder::~XADecoder()
{
	if (this->readBuff)
	{
		MemFree(this->readBuff);
		this->readBuff = 0;
	}
}

void Media::Decoder::XADecoder::GetFormat(AudioFormat *format)
{
	if (this->sourceAudio)
	{
		Media::AudioFormat fmt;
		this->sourceAudio->GetFormat(&fmt);
		format->formatId = 1;
		format->bitpersample = 16;
		format->frequency = fmt.frequency;
		format->nChannels = fmt.nChannels;
		format->bitRate = fmt.frequency * fmt.nChannels << 4;
		format->align = fmt.nChannels * 2;
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

UInt32 Media::Decoder::XADecoder::SeekToTime(UInt32 time)
{
	if (this->sourceAudio)
	{
		adxSample1[0] = 0;
		adxSample1[1] = 0;
		adxSample2[0] = 0;
		adxSample2[1] = 0;
		return this->sourceAudio->SeekToTime(time);
	}
	return 0;
}

Bool Media::Decoder::XADecoder::Start(Sync::Event *evt, UOSInt blkSize)
{
	if (this->sourceAudio)
	{
		this->sourceAudio->Start(0, blkSize);
		this->readEvt = evt;
		adxSample1[0] = 0;
		adxSample1[1] = 0;
		adxSample2[0] = 0;
		adxSample2[1] = 0;
		if (this->readEvt)
			this->readEvt->Set();
		return true;
	}
	return false;
}

void Media::Decoder::XADecoder::Stop()
{
	if (this->sourceAudio)
	{
		this->sourceAudio->Stop();
	}
	this->readEvt = 0;
}

UOSInt Media::Decoder::XADecoder::ReadBlock(UInt8 *buff, UOSInt blkSize)
{
	UOSInt outSize;
	UOSInt readSize;
	UInt8 *src;
	UInt8 *dest;

	blkSize = blkSize / 448;
	readSize = blkSize << 7;
	if (readBuffSize < readSize)
	{
		if (readBuff)
		{
			MemFree(readBuff);
		}
		readBuff = MemAlloc(UInt8, readBuffSize = readSize);
	}
	readSize = this->sourceAudio->ReadBlock(readBuff, readSize);
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
	if (this->readEvt)
		this->readEvt->Set();
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
	x = 0xffffffff80000000LL; \
else if (((Int32*)&x)[1] == -1) \
	if (((*(Int32*)&x) & 0x80000000) == 0) \
		x = 0xffffffff80000000LL;

void Media::Decoder::XADecoder::Convert(UInt8 *src, UInt8 *dest, Int32 sampleByte, Int32 channel)
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
				else if (((long*)&x)[1] < -1)
					x = 0xffffffff80000000LL;
				else if (((long*)&x)[1] == -1)
					if (((*(long*)&x) & 0x80000000) == 0)
						x = 0xffffffff80000000LL;
				adxSample1[0] = x;
				*(Int16*)dest = (Int16)(x >> 16);
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
				else if (((long*)&x)[1] < -1)
					x = 0xffffffff80000000LL;
				else if (((long*)&x)[1] == -1)
					if (((*(long*)&x) & 0x80000000) == 0)
						x = 0xffffffff80000000LL;
				adxSample1[1] = x;
				*(Int16*)dest = (Int16)(x >> 16);
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
				*(Int16*)dest = (Int16)(x >> 16);
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
				*(Int16*)dest = (Int16)(x >> 16);
				dest += 2;
				j++;
			}
			i++;
		}
	}
}
