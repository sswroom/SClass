#include "Stdafx.h"
#include "MyMemory.h"
#include "Net/RTPASource.h"

Net::RTPASource::RTPASource(NN<Net::RTPCliChannel> ch, NN<Net::RTPAPLHandler> hdlr)
{
	this->ch = ch;
	this->hdlr = hdlr;
}

Net::RTPASource::~RTPASource()
{
	this->ch.Delete();
}

UnsafeArrayOpt<UTF8Char> Net::RTPASource::GetSourceName(UnsafeArray<UTF8Char> buff)
{
	return this->hdlr->GetSourceName(buff);
}

Bool Net::RTPASource::CanSeek()
{
	return false;
}

Data::Duration Net::RTPASource::GetStreamTime()
{
	return this->hdlr->GetStreamTime();
}

Data::Duration Net::RTPASource::SeekToTime(Data::Duration time)
{
	return 0;
}

Bool Net::RTPASource::TrimStream(UInt32 trimTimeStart, UInt32 trimTimeEnd, OptOut<Int32> syncTime)
{
	return false;
}

void Net::RTPASource::GetFormat(NN<Media::AudioFormat> format)
{
	return this->hdlr->GetFormat(format);
}

Bool Net::RTPASource::Start(Optional<Sync::Event> evt, UOSInt blkSize)
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

Data::Duration Net::RTPASource::GetCurrTime()
{
	return this->hdlr->GetCurrTime();
}

Bool Net::RTPASource::IsEnd()
{
	return !this->ch->IsRunning();
}
