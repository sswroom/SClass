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

Media::Decoder::ACMDecoder::ACMDecoder(NN<Media::IAudioSource> sourceAudio)
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

void Media::Decoder::ACMDecoder::GetFormat(NN<AudioFormat> format)
{
	format->formatId = 0;
}

Data::Duration Media::Decoder::ACMDecoder::SeekToTime(Data::Duration time)
{
	NN<Media::IAudioSource> sourceAudio;
	if (this->sourceAudio.SetTo(sourceAudio))
	{
		this->seeked = true;
		this->acmOupBuffLeft = 0;
		return sourceAudio->SeekToTime(time);
	}
	return 0;
}

Bool Media::Decoder::ACMDecoder::Start(Optional<Sync::Event> evt, UOSInt blkSize)
{
	NN<Sync::Event> readEvt;
	NN<Media::IAudioSource> sourceAudio;
	if (this->sourceAudio.SetTo(sourceAudio))
	{
		this->seeked = true;
		this->acmOupBuffLeft = 0;
		sourceAudio->Start(0, blkSize);
		this->readEvt = evt;

		if (this->readEvt.SetTo(readEvt))
			readEvt->Set();
		return true;
	}
	return false;
}

void Media::Decoder::ACMDecoder::Stop()
{
	NN<Media::IAudioSource> sourceAudio;
	if (this->sourceAudio.SetTo(sourceAudio))
	{
		sourceAudio->Stop();
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
