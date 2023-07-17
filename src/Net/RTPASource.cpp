#include "Stdafx.h"
#include "MyMemory.h"
#include "Net/RTPASource.h"

Net::RTPASource::RTPASource(Net::RTPCliChannel *ch, Net::RTPAPLHandler *hdlr)
{
	this->ch = ch;
	this->hdlr = hdlr;
}

Net::RTPASource::~RTPASource()
{
	DEL_CLASS(this->ch);
}

UTF8Char *Net::RTPASource::GetSourceName(UTF8Char *buff)
{
	return this->hdlr->GetSourceName(buff);
}

Bool Net::RTPASource::CanSeek()
{
	return false;
}

Int32 Net::RTPASource::GetStreamTime()
{
	return this->hdlr->GetStreamTime();
}

UInt32 Net::RTPASource::SeekToTime(UInt32 time)
{
	return 0;
}

Bool Net::RTPASource::TrimStream(UInt32 trimTimeStart, UInt32 trimTimeEnd, Int32 *syncTime)
{
	return false;
}

void Net::RTPASource::GetFormat(Media::AudioFormat *format)
{
	return this->hdlr->GetFormat(format);
}

Bool Net::RTPASource::Start(Sync::Event *evt, UOSInt blkSize)
{
	this->hdlr->Start(evt, blkSize);
	this->ch->StartPlay();
	return true;
}

void Net::RTPASource::Stop()
{
	this->ch->StopPlay();
	this->hdlr->Stop();
}

UOSInt Net::RTPASource::ReadBlock(Data::ByteArray blk)
{
	return this->hdlr->ReadBlock(blk);
}

UOSInt Net::RTPASource::GetMinBlockSize()
{
	return this->hdlr->GetMinBlockSize();
}

UInt32 Net::RTPASource::GetCurrTime()
{
	return this->hdlr->GetCurrTime();
}

Bool Net::RTPASource::IsEnd()
{
	return !this->ch->IsRunning();
}
