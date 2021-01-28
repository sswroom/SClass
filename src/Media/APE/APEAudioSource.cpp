#include "stdafx.h"
#include "MyMemory.h"
#include "Media/APE/APEAudioSource.h"
#include "Text/MyString.h"

Media::APE::APEAudioSource::APEAudioSource(IAPEDecompress *ape, Media::APE::APEIO *io)
{
	WAVEFORMATEX waveFmt;
	this->ape = ape;
	this->io = io;
	NEW_CLASS(this->fmt, Media::AudioFormat());
	this->ape->GetInfo(APE_INFO_WAVEFORMATEX, (Int32)(OSInt)&waveFmt, 0);
	this->fmt->FromWAVEFORMATEX((UInt8*)&waveFmt);
	this->evt = 0;
}

Media::APE::APEAudioSource::~APEAudioSource()
{
	delete this->ape;
	DEL_CLASS(this->io);
	DEL_CLASS(this->fmt);
}

WChar *Media::APE::APEAudioSource::GetName(WChar *buff)
{
	return Text::StrConcat(buff, this->io->GetData()->GetFullName());
}

Bool Media::APE::APEAudioSource::CanSeek()
{
	return true;
}

Int32 Media::APE::APEAudioSource::GetStreamTime()
{
	return this->ape->GetInfo(APE_INFO_LENGTH_MS, 0, 0);
}

void Media::APE::APEAudioSource::GetFormat(AudioFormat *format)
{
	format->FromAudioFormat(this->fmt);
}

Int32 Media::APE::APEAudioSource::SeekToTime(Int32 time)
{
	Int32 newBlock = MulDiv(time, this->fmt->frequency, 1000);
	if (this->ape->Seek(newBlock) == 0)
	{
		this->currBlock = newBlock;
	}
	return MulDiv(this->currBlock, 1000, this->fmt->frequency);
}

Bool Media::APE::APEAudioSource::Start(Sync::Event *evt, Int32 blkSize)
{
	this->evt = evt;
	if (this->evt)
	{
		this->evt->Set();
	}
	return true;
}

void Media::APE::APEAudioSource::Stop()
{
	this->evt = 0;
}

OSInt Media::APE::APEAudioSource::ReadBlock(UInt8 *buff, OSInt blkSize)
{
	Int32 readSize = 0;
	if (this->ape->GetData((Char*)buff, (Int32)(blkSize / (this->fmt->nChannels * this->fmt->bitpersample >> 3)), &readSize) != 0)
		return 0;
	if (readSize > 0)
	{
		this->currBlock += readSize;
	}
	if (this->evt)
	{
		this->evt->Set();
	}
	return readSize * this->fmt->nChannels * this->fmt->bitpersample >> 3;
}

OSInt Media::APE::APEAudioSource::GetMinBlockSize()
{
	return this->fmt->nChannels * this->fmt->bitpersample >> 3;
}

Int32 Media::APE::APEAudioSource::GetCurrTime()
{
	return MulDiv(this->currBlock, 1000, this->fmt->frequency);
}
