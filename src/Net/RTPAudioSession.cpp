#include "Stdafx.h"
#include "Text/MyString.h"
#include "Data/ByteTool.h"
#include "Data/Random.h"
#include "Sync/Thread.h"
#include "Net/RTPAudioSession.h"

void __stdcall Net::RTPAudioSession::UDPData(UInt32 ip, UInt16 port, UInt8 *buff, OSInt dataSize, void *userData)
{
	Net::RTPAudioSession *me = (Net::RTPAudioSession*)userData;
	if (dataSize < 12)
		return;
	Int32 v = buff[0] >> 6;
	if (v != 2)
		return;
	if (buff[0] & 0x20) //padding
	{
		dataSize -= buff[dataSize - 1];
		if (dataSize < 12)
			return;
	}
	if (me->readEvt)
		me->readEvt->Set();
	Bool extension = (buff[0] & 0x10) != 0;
	Int32 csrcCnt = buff[0] & 15;
	Bool marker = (buff[1] & 0x80) != 0;
	Int32 payloadType = buff[1] & 0x7f;
	Int32 seqNum = ReadMUInt16(&buff[2]);
	UInt32 timestamp = ReadMUInt32(&buff[4]);
	me->lastSSRC = ReadMUInt32(&buff[8]);

	if (me->started)
	{
		if (me->readBuff)
		{
			MemCopy(me->readBuff, &buff[12], dataSize - 12);
			me->readBuff = 0;
			me->sizeRead = dataSize - 12;
			me->readBuffEvt->Set();
		}
	}
}

UInt32 __stdcall Net::RTPAudioSession::SendThread(void *userObj)
{
	UInt8 buff[512];
	Sync::Event *evt;
	OSInt readSize;
	Int32 seqId;
	Int32 ts;
	Data::Random *random;

	Net::RTPAudioSession *me = (Net::RTPAudioSession*)userObj;
	me->outRunning = true;
	NEW_CLASS(random, Data::Random());
	NEW_CLASS(evt, Sync::Event(true, L"Net.RTPAudioSession.SendThread.evt"));
	seqId = random->NextInt30() & 0xffff;
	ts = 0;
	me->outAudio->Start(evt, 160);
	while (!me->outToStop)
	{
		evt->Wait(1000);
		if (me->outToStop)
			break;
		readSize = me->outAudio->ReadBlock(&buff[12], 160);
		if (readSize == 160)
		{
			buff[0] = 0x80;
			buff[1] = 0;
			WriteMInt16(&buff[2], seqId);
			WriteMInt32(&buff[4], ts);
			WriteMInt32(&buff[8], me->outSSRC);
			me->svr->SendTo(me->outIP, me->outPort, buff, 172);
			
			seqId++;
			ts += 160;
		}
	}
	me->outAudio->Stop();
	DEL_CLASS(evt);
	DEL_CLASS(random);
	me->outRunning = false;
	return 0;
}

Net::RTPAudioSession::RTPAudioSession(Net::SocketFactory *sockf, const Char *ip, UInt16 port, IO::LogTool *log)
{
	this->sockf = sockf;
	this->log = log;
	this->svr = 0;
	this->format = 0;
	this->readEvt = 0;
	this->lastSSRC = 0;
	this->readBuff = 0;
	this->outRunning = false;
	NEW_CLASS(this->readMut, Sync::Mutex());
	NEW_CLASS(this->readBuffEvt, Sync::Event(true, L"Net.RTPAudioSession.readBuffEvt"));
	NEW_CLASS(this->svr, Net::UDPServer(sockf, port, 0, UDPData, this, log, 0, Sync::Thread::GetThreadCnt(), false));
	if (this->svr->IsError())
	{
		DEL_CLASS(this->svr);
		this->svr = 0;
	}
}

Net::RTPAudioSession::~RTPAudioSession()
{
	this->StopSend();
	this->Stop();
	if (svr)
	{
		DEL_CLASS(svr);
	}
	DEL_CLASS(this->readMut);
	DEL_CLASS(this->readBuffEvt);
	if (this->format)
	{
		DEL_CLASS(this->format);
	}
}

Bool Net::RTPAudioSession::IsError()
{
	return svr == 0;
}

void Net::RTPAudioSession::SetAudioFormat(Net::RTPAudioSession::RTPAudioFormat afmt, Int32 frequency)
{
	if (this->format == 0)
	{
		NEW_CLASS(this->format, Media::AudioFormat());
	}

	switch (afmt)
	{
	case RTPAFMT_PCMU:
		this->afmt = afmt;
		this->fmtFreq = frequency;
		this->format->formatId = 7;
		this->format->frequency = frequency;
		this->format->bitRate = frequency << 3;
		this->format->bitpersample = 8;
		this->format->align = 1;
		this->format->nChannels = 1;
		break;
	}
}

Bool Net::RTPAudioSession::StartSend(Media::IAudioSource *audSrc, UInt32 destIP, UInt16 destPort, Int32 outSSRC)
{
	if (this->outRunning)
	{
		return false;
	}
	this->outToStop = false;
	this->outAudio = audSrc;
	this->outIP = destIP;
	this->outPort = destPort;
	this->outSSRC = outSSRC;
	Sync::Thread::Create(SendThread, this);
	return true;
}

void Net::RTPAudioSession::StopSend()
{
	if (this->outRunning)
	{
		this->outToStop = true;
		while (this->outRunning)
		{
			Sync::Thread::Sleep(10);
		}
	}
}

WChar *Net::RTPAudioSession::GetName(WChar *buff)
{
	return Text::StrConcat(buff, L"RTP Audio");
}

Bool Net::RTPAudioSession::CanSeek()
{
	return false;
}

Int32 Net::RTPAudioSession::GetStreamTime()
{
	return 0;
}

void Net::RTPAudioSession::GetFormat(Media::AudioFormat *format)
{
	if (this->format == 0)
	{
		format->Clear();
	}
	else
	{
		MemCopy(format, this->format, sizeof(Media::AudioFormat));
	}
}

Int32 Net::RTPAudioSession::SeekToTime(Int32 time)
{
	return 0;
}

Bool Net::RTPAudioSession::Start(Sync::Event *evt, Int32 blkSize)
{
	if (this->svr == 0)
		return false;
	this->readEvt = evt;
	this->started = true;
	return true;
}

void Net::RTPAudioSession::Stop()
{
	if (started)
	{
		started = false;
		this->sizeRead = -1;
		this->readBuffEvt->Set();
	}
}

OSInt Net::RTPAudioSession::ReadBlock(UInt8 *buff, OSInt blkSize)
{
	if (!started)
		return 0;
	OSInt retSize;
	this->readMut->Lock();
	this->sizeRead = 0;
	this->readBuffSize = blkSize;
	this->readBuff = buff;
	while (this->sizeRead == 0)
	{
		this->readBuffEvt->Wait(100);
	}
	retSize = this->sizeRead;
	this->readBuff = 0;
	this->readMut->Unlock();
	return retSize;
}

OSInt Net::RTPAudioSession::GetMinBlockSize()
{
	return 160;
}
