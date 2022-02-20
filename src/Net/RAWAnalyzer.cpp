#include "Stdafx.h"
#include "MyMemory.h"
#include "Net/EthernetWebHandler.h"
#include "Net/RAWAnalyzer.h"
#include "Sync/Interlocked.h"
#include "Sync/Thread.h"

UInt32 __stdcall Net::RAWAnalyzer::RecvThread(void *userObj)
{
	Net::RAWAnalyzer *me = (Net::RAWAnalyzer*)userObj;
	UInt8 packetBuff[10240];
	UOSInt packetSize;
	Net::SocketUtil::AddressInfo addr;
	UInt16 port;
	Sync::Interlocked::Increment(&me->threadCnt);
	while (!me->threadToStop)
	{
		packetSize = me->sockf->UDPReceive(me->rawSock, packetBuff, 10240, &addr, &port, 0);
		if (packetSize >= 14)
		{
			me->analyzer->PacketEthernet(packetBuff, packetSize);
		}
	}
	Sync::Interlocked::Decrement(&me->threadCnt);
	return 0;
}

Net::RAWAnalyzer::RAWAnalyzer(Net::SocketFactory *sockf, UInt16 infoPort, IO::Writer *errWriter, Net::EthernetAnalyzer::AnalyzeType atype)
{
	this->sockf = sockf;
	NEW_CLASS(this->analyzer, Net::EthernetAnalyzer(errWriter, atype, CSTR("RAWAnalyzer"))); 
	this->listener = 0;
	this->webHdlr = 0;
	this->rawSock = 0;
	this->threadCnt = 0;
	this->threadToStop = false;

	NEW_CLASS(this->webHdlr, Net::EthernetWebHandler(this->analyzer));
	NEW_CLASS(this->listener, Net::WebServer::WebListener(this->sockf, 0, this->webHdlr, infoPort, 120, 8, CSTR("NetRAWCapture/1.0"), false, true));
	if (this->listener->IsError())
	{
		DEL_CLASS(this->listener);
		this->webHdlr->Release();
		this->listener = 0;
		this->webHdlr = 0;
	}
	else
	{
		this->rawSock = this->sockf->CreateRAWSocket();
		if (this->rawSock)
		{
			OSInt i = 3;
			while (i-- > 0)
			{
				Sync::Thread::Create(RecvThread, this);
			}
		}
	}
}

Net::RAWAnalyzer::~RAWAnalyzer()
{
	if (this->rawSock)
	{
		this->threadToStop = true;
		this->sockf->DestroySocket(this->rawSock);
		while (this->threadCnt > 0)
		{
			Sync::Thread::Sleep(1);
		}
		this->rawSock = 0;
	}

	SDEL_CLASS(this->listener);
	if (this->webHdlr)
	{
		this->webHdlr->Release();
		this->webHdlr = 0;
	}
	DEL_CLASS(this->analyzer);
}

Bool Net::RAWAnalyzer::IsError()
{
	if (this->listener && this->rawSock)
	{
		return false;
	}
	return true;
}

Net::EthernetAnalyzer *Net::RAWAnalyzer::GetAnalyzer()
{
	return this->analyzer;
}
