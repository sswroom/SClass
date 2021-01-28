#include "Stdafx.h"
#include "MyMemory.h"
#include "Media/Encoder/G711muLawEncoder.h"

Media::Encoder::G711muLawEncoder::G711muLawEncoder(Media::IAudioSource *sourceAudio)
{
	Media::AudioFormat fmt;
	this->sourceAudio = 0;
	this->readBuff = 0;
	this->readBuffSize = 0;
	this->align = 0;
	sourceAudio->GetFormat(&fmt);
	if (fmt.formatId != 0x1)
		return;
	if (fmt.bitpersample != 16)
		return;
	this->sourceAudio = sourceAudio;
	this->align = fmt.nChannels;
}

Media::Encoder::G711muLawEncoder::~G711muLawEncoder()
{
	if (this->readBuff)
	{
		MemFree(this->readBuff);
		this->readBuff = 0;
	}
}

WChar *Media::Encoder::G711muLawEncoder::GetName(WChar *buff)
{
	if (this->sourceAudio)
        return this->sourceAudio->GetName(buff);
	return 0;
}

Bool Media::Encoder::G711muLawEncoder::CanSeek()
{
	if (this->sourceAudio)
		return this->sourceAudio->CanSeek();
	return false;
}

Int32 Media::Encoder::G711muLawEncoder::GetStreamTime()
{
	if (this->sourceAudio)
		return this->sourceAudio->GetStreamTime();
	return 0;
}

void Media::Encoder::G711muLawEncoder::GetFormat(AudioFormat *format)
{
	if (this->sourceAudio)
	{
		Media::AudioFormat fmt;
		this->sourceAudio->GetFormat(&fmt);
		format->formatId = 7;
		format->bitpersample = 8;
		format->frequency = fmt.frequency;
		format->nChannels = fmt.nChannels;
		format->bitRate = fmt.frequency * fmt.nChannels << 3;
		format->align = fmt.nChannels;
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

Int32 Media::Encoder::G711muLawEncoder::SeekToTime(Int32 time)
{
	if (this->sourceAudio)
	{
		return this->sourceAudio->SeekToTime(time);
	}
	return 0;
}

Bool Media::Encoder::G711muLawEncoder::Start(Sync::Event *evt, Int32 blkSize)
{
	if (this->sourceAudio)
	{
		this->sourceAudio->Start(evt, blkSize << 1);
		return true;
	}
	return false;
}

void Media::Encoder::G711muLawEncoder::Stop()
{
	if (this->sourceAudio)
	{
		this->sourceAudio->Stop();
	}
}

OSInt Media::Encoder::G711muLawEncoder::ReadBlock(UInt8 *buff, OSInt blkSize)
{
	static UInt8 muLawCompressTable[] = {
	0, 0, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3,
	4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
	5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
	5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
	6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
	6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
	6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
	6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
	7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
	7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
	7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
	7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
	7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
	7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
	7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
	7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7};

	if (this->align == 0 || this->sourceAudio == 0)
	{
		return 0;
	}
	blkSize = blkSize / this->align * this->align;
	OSInt readSize = blkSize << 1;
	if (this->readBuffSize < readSize)
	{
		if (this->readBuff)
		{
			MemFree(this->readBuff);
		}
		this->readBuff = MemAlloc(UInt8, readSize);
		this->readBuffSize = readSize;
	}
	readSize = this->sourceAudio->ReadBlock(this->readBuff, readSize);
	Int16 *srcBuff;
	blkSize = readSize >> 1;
	srcBuff = (Int16*)this->readBuff;
	while (blkSize-- > 0)
	{
		Int32 sample = *srcBuff++;
		Int32 sign;
		Int32 exponent;
		Int32 mantissa;
		UInt8 s;

		if (sample < 0)
		{
			sign = 0x7f;
			sample = 0x84 - sample;
		}
		else
		{
			sample += 0x84;
			sign = 0xff;
		}
		if (sample > 32768)
		{
			sample = 32768;
		}
		if (sample >= 256) {
			exponent = muLawCompressTable[(sample >> 8) & 0x7F] + 1;
			mantissa = (sample >> (exponent + 3)) & 0x0F;
			s = (UInt8)((exponent << 4) | mantissa);
		} else {
			s = (UInt8)(sample >> 4);
		}
		s ^= (UInt8)sign;
		*buff++ = s;
	}
	return readSize >> 1;
}

OSInt Media::Encoder::G711muLawEncoder::GetMinBlockSize()
{
	return this->align;
}
