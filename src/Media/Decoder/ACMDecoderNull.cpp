#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "Media/Decoder/ACMDecoder.h"

void Media::Decoder::ACMDecoder::FreeACM()
{
}

void Media::Decoder::ACMDecoder::InitACM()
{
}

Media::Decoder::ACMDecoder::ACMDecoder(NotNullPtr<Media::IAudioSource> sourceAudio)
{
	this->sourceAudio = 0;
	this->hAcmStream = 0;
	this->acmFmt = 0;
	this->decFmt = 0;
	this->acmsh = 0;
	this->acmInpBuff = 0;
	this->acmOupBuff = 0;
	this->seeked = true;
	this->sourceAudio = sourceAudio.Ptr();
	this->srcFormatTag = 0;
	InitACM();
}

Media::Decoder::ACMDecoder::~ACMDecoder()
{
	FreeACM();
}

void Media::Decoder::ACMDecoder::GetFormat(NotNullPtr<AudioFormat> format)
{
	format->formatId = 0;
}

Data::Duration Media::Decoder::ACMDecoder::SeekToTime(Data::Duration time)
{
	if (this->sourceAudio)
	{
		this->seeked = true;
		this->acmOupBuffLeft = 0;
		return this->sourceAudio->SeekToTime(time);
	}
	return 0;
}

Bool Media::Decoder::ACMDecoder::Start(Sync::Event *evt, UOSInt blkSize)
{
	if (this->sourceAudio)
	{
		this->seeked = true;
		this->acmOupBuffLeft = 0;
		this->sourceAudio->Start(0, blkSize);
		this->readEvt = evt;

		if (this->readEvt)
			this->readEvt->Set();
		return true;
	}
	return false;
}

void Media::Decoder::ACMDecoder::Stop()
{
	if (this->sourceAudio)
	{
		this->sourceAudio->Stop();
	}
	this->readEvt = 0;
}

UOSInt Media::Decoder::ACMDecoder::ReadBlock(Data::ByteArray blk)
{
	return 0;
}

UOSInt Media::Decoder::ACMDecoder::GetMinBlockSize()
{
	return 1;
}
