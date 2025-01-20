#include "Stdafx.h"
#include "Media/Decoder/ADecoderBase.h"

Media::Decoder::ADecoderBase::ADecoderBase()
{
	this->sourceAudio = 0;
}

Media::Decoder::ADecoderBase::~ADecoderBase()
{
}

UnsafeArrayOpt<UTF8Char> Media::Decoder::ADecoderBase::GetSourceName(UnsafeArray<UTF8Char> buff)
{
	NN<Media::AudioSource> sourceAudio;
	if (this->sourceAudio.SetTo(sourceAudio))
        return sourceAudio->GetSourceName(buff);
	return 0;
}

Bool Media::Decoder::ADecoderBase::CanSeek()
{
	NN<Media::AudioSource> sourceAudio;
	if (this->sourceAudio.SetTo(sourceAudio))
		return sourceAudio->CanSeek();
	return false;
}

Data::Duration Media::Decoder::ADecoderBase::GetStreamTime()
{
	NN<Media::AudioSource> sourceAudio;
	if (this->sourceAudio.SetTo(sourceAudio))
		return sourceAudio->GetStreamTime();
	return 0;
}

Data::Duration Media::Decoder::ADecoderBase::GetCurrTime()
{
	NN<Media::AudioSource> sourceAudio;
	if (this->sourceAudio.SetTo(sourceAudio))
		return sourceAudio->GetCurrTime();
	return 0;
}

Bool Media::Decoder::ADecoderBase::IsEnd()
{
	NN<Media::AudioSource> sourceAudio;
	if (this->sourceAudio.SetTo(sourceAudio))
		return sourceAudio->IsEnd();
	return true;
}

Bool Media::Decoder::ADecoderBase::TrimStream(UInt32 trimTimeStart, UInt32 trimTimeEnd, OptOut<Int32> syncTime)
{
	NN<Media::AudioSource> sourceAudio;
	if (this->sourceAudio.SetTo(sourceAudio))
		return sourceAudio->TrimStream(trimTimeStart, trimTimeEnd, syncTime);
	return false;
}
