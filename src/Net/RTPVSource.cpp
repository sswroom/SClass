#include "Stdafx.h"
#include "MyMemory.h"
#include "Net/RTPVSource.h"

Net::RTPVSource::RTPVSource(NN<Net::RTPCliChannel> ch, NN<Net::RTPVPLHandler> hdlr)
{
	this->ch = ch;
	this->hdlr = hdlr;
}

Net::RTPVSource::~RTPVSource()
{
	this->ch.Delete();
}

UnsafeArrayOpt<UTF8Char> Net::RTPVSource::GetSourceName(UnsafeArray<UTF8Char> buff)
{
	return this->hdlr->GetSourceName(buff);
}

Text::CStringNN Net::RTPVSource::GetFilterName()
{
	return CSTR("RTPVSource");
}

Bool Net::RTPVSource::GetVideoInfo(NN<Media::FrameInfo> info, OutParam<UInt32> frameRateNorm, OutParam<UInt32> frameRateDenorm, OutParam<UOSInt> maxFrameSize)
{
	return this->hdlr->GetVideoInfo(info, frameRateNorm, frameRateDenorm, maxFrameSize);
}

Bool Net::RTPVSource::Init(FrameCallback cb, FrameChangeCallback fcCb, AnyType userData)
{
	return this->hdlr->Init(cb, fcCb, userData);
}

Bool Net::RTPVSource::Start()
{
	this->hdlr->Start();
	return this->ch->StartPlay();
}

void Net::RTPVSource::Stop()
{
	this->hdlr->Stop();
	this->ch->StopPlay();
}

Bool Net::RTPVSource::IsRunning()
{
	return this->ch->IsRunning();
}

Data::Duration Net::RTPVSource::GetStreamTime()
{
	return this->hdlr->GetStreamTime();
}

Bool Net::RTPVSource::CanSeek()
{
	return false;
	//return ch->CanSeek();
}

Data::Duration Net::RTPVSource::SeekToTime(Data::Duration time)
{
	return 0;
//	return ch->SeekToTime();
}

Bool Net::RTPVSource::IsRealTimeSrc()
{
	return true;
}

Bool Net::RTPVSource::TrimStream(UInt32 trimTimeStart, UInt32 trimTimeEnd, Int32 *syncTime)
{
	return false;
}

UOSInt Net::RTPVSource::GetDataSeekCount()
{
	return 0;
}

Bool Net::RTPVSource::HasFrameCount()
{
	return false;
}

UOSInt Net::RTPVSource::GetFrameCount()
{
	return 0;
}

Data::Duration Net::RTPVSource::GetFrameTime(UOSInt frameIndex)
{
	return 0;
}

void Net::RTPVSource::EnumFrameInfos(FrameInfoCallback cb, AnyType userData)
{
}

UOSInt Net::RTPVSource::ReadNextFrame(UInt8 *frameBuff, UInt32 *frameTime, Media::FrameType *ftype)
{
	return this->hdlr->ReadNextFrame(frameBuff, frameTime, ftype);
}
