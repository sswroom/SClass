#include "Stdafx.h"
#include "Media/Decoder/ADecoderBase.h"

Media::Decoder::ADecoderBase::ADecoderBase()
{
	this->sourceAudio = 0;
}

Media::Decoder::ADecoderBase::~ADecoderBase()
{
}

UTF8Char *Media::Decoder::ADecoderBase::GetSourceName(UTF8Char *buff)
{
	if (this->sourceAudio)
        return this->sourceAudio->GetSourceName(buff);
	return 0;
}

Bool Media::Decoder::ADecoderBase::CanSeek()
{
	if (this->sourceAudio)
		return this->sourceAudio->CanSeek();
	return false;
}

Data::Duration Media::Decoder::ADecoderBase::GetStreamTime()
{
	if (this->sourceAudio)
		return this->sourceAudio->GetStreamTime();
	return 0;
}

Data::Duration Media::Decoder::ADecoderBase::GetCurrTime()
{
	if (this->sourceAudio)
		return this->sourceAudio->GetCurrTime();
	return 0;
}

Bool Media::Decoder::ADecoderBase::IsEnd()
{
	if (this->sourceAudio)
		return this->sourceAudio->IsEnd();
	return true;
}

Bool Media::Decoder::ADecoderBase::TrimStream(UInt32 trimTimeStart, UInt32 trimTimeEnd, Int32 *syncTime)
{
	if (this->sourceAudio)
		return this->sourceAudio->TrimStream(trimTimeStart, trimTimeEnd, syncTime);
	return false;
}
