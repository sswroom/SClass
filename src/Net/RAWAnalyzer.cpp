#include "Stdafx.h"
#include "MyMemory.h"
#include "Net/EthernetWebHandler.h"
#include "Net/RAWAnalyzer.h"
#include "Sync/Interlocked.h"
#include "Sync/SimpleThread.h"
#include "Sync/ThreadUtil.h"

UInt32 __stdcall Net::RAWAnalyzer::RecvThread(AnyType userObj)
{
	NN<Net::RAWAnalyzer> me = userObj.GetNN<Net::RAWAnalyzer>();
	UInt8 packetBuff[10240];
	UOSInt packetSize;
	Net::SocketUtil::AddressInfo addr;
	UInt16 port;
	Sync::Interlocked::IncrementI32(me->threadCnt);
	NN<Socket> soc;
	if (me->rawSock.SetTo(soc))
	{
		while (!me->threadToStop)
		{
			packetSize = me->clif->GetSocketFactory()->UDPReceive(soc, packetBuff, 10240, addr, port, 0);
			if (packetSize >= 14)
			{
				me->analyzer->PacketEthernet(packetBuff, packetSize);
			}
		}
	}
	Sync::Interlocked::DecrementI32(me->threadCnt);
	return 0;
}

Net::RAWAnalyzer::RAWAnalyzer(NN<Net::TCPClientFactory> clif, UInt16 infoPort, Optional<IO::Writer> errWriter, Net::EthernetAnalyzer::AnalyzeType atype)
{
	this->clif = clif;
	NEW_CLASSNN(this->analyzer, Net::EthernetAnalyzer(errWriter, atype, CSTR("RAWAnalyzer"))); 
	this->listener = nullptr;
	this->rawSock = nullptr;
	this->threadCnt = 0;
	this->threadToStop = false;

	NEW_CLASSNN(this->webHdlr, Net::EthernetWebHandler(this->analyzer));
	NN<Net::WebServer::WebListener> listener;
	NEW_CLASSNN(listener, Net::WebServer::WebListener(this->clif, nullptr, this->webHdlr, infoPort, 120, 1, 8, CSTR("NetRAWCapture/1.0"), false, Net::WebServer::KeepAlive::Default, true));
	if (listener->IsError())
	{
		listener.Delete();
	}
	else
	{
		this->listener = listener;
		this->rawSock = this->clif->GetSocketFactory()->CreateRAWSocket();
		NN<Socket> soc;
		if (this->rawSock.SetTo(soc))
		{
			OSInt i = 3;
			while (i-- > 0)
			{
				Sync::ThreadUtil::Create(RecvThread, this);
			}
		}
	}
}

Net::RAWAnalyzer::~RAWAnalyzer()
{
	NN<Socket> soc;
	if (this->rawSock.SetTo(soc))
	{
		this->threadToStop = true;
		this->clif->GetSocketFactory()->DestroySocket(soc);
		while (this->threadCnt > 0)
		{
			Sync::SimpleThread::Sleep(1);
		}
		this->rawSock = nullptr;
	}

	this->listener.Delete();
	this->webHdlr.Delete();
	this->analyzer.Delete();
}

Bool Net::RAWAnalyzer::IsError()
{
	if (this->listener.NotNull() && this->rawSock.NotNull())
	{
		return false;
	}
	return true;
}

NN<Net::EthernetAnalyzer> Net::RAWAnalyzer::GetAnalyzer()
{
	return this->analyzer;
}
