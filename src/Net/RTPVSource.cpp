#include "Stdafx.h"
#include "MyMemory.h"
#include "Net/RTPVSource.h"

Net::RTPVSource::RTPVSource(Net::RTPCliChannel *ch, Net::RTPVPLHandler *hdlr)
{
	this->ch = ch;
	this->hdlr = hdlr;
}

Net::RTPVSource::~RTPVSource()
{
	DEL_CLASS(this->ch);
}

UTF8Char *Net::RTPVSource::GetSourceName(UTF8Char *buff)
{
	return this->hdlr->GetSourceName(buff);
}

const UTF8Char *Net::RTPVSource::GetFilterName()
{
	return (const UTF8Char*)"RTPVSource";
}

Bool Net::RTPVSource::GetVideoInfo(Media::FrameInfo *info, UInt32 *frameRateNorm, UInt32 *frameRateDenorm, UOSInt *maxFrameSize)
{
	return this->hdlr->GetVideoInfo(info, frameRateNorm, frameRateDenorm, maxFrameSize);
}

Bool Net::RTPVSource::Init(FrameCallback cb, FrameChangeCallback fcCb, void *userData)
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

Int32 Net::RTPVSource::GetStreamTime()
{
	return this->hdlr->GetStreamTime();
}

Bool Net::RTPVSource::CanSeek()
{
	return false;
	//return ch->CanSeek();
}

UInt32 Net::RTPVSource::SeekToTime(UInt32 time)
{
	return false;
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

OSInt Net::RTPVSource::GetFrameCount()
{
	return -1;
}

UInt32 Net::RTPVSource::GetFrameTime(UOSInt frameIndex)
{
	return 0;
}

void Net::RTPVSource::EnumFrameInfos(FrameInfoCallback cb, void *userData)
{
}

OSInt Net::RTPVSource::ReadNextFrame(UInt8 *frameBuff, Int32 *frameTime, Media::FrameType *ftype)
{
	return this->hdlr->ReadNextFrame(frameBuff, frameTime, ftype);
}
