#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/ByteTool_C.h"
#include "Net/ARPHandler.h"
#include "Sync/ThreadUtil.h"
#include "Text/MyString.h"

UInt32 __stdcall Net::ARPHandler::DataThread(AnyType obj)
{
	NN<Net::ARPHandler::ThreadStat> stat = obj.GetNN<Net::ARPHandler::ThreadStat>();
	NN<Socket> soc;
	NN<Sync::Event> ctrlEvt;
	stat->threadRunning = true;
	if (stat->me->ctrlEvt.SetTo(ctrlEvt))
	{
		ctrlEvt->Set();
	}

	if (stat->me->soc.SetTo(soc))
	{
		UnsafeArray<UInt8> buff = MemAllocArr(UInt8, 2048);
		while (!stat->toStop)
		{
			UIntOS recvSize;
			Net::SocketUtil::AddressInfo recvAddr;
			UInt16 recvPort;

			recvSize = stat->me->sockf->UDPReceive(soc, buff, 2048, recvAddr, recvPort, 0);
			if (recvSize >= 42)
			{
				UInt16 opcode = ReadMUInt16(&buff[20]);
				if (ReadNInt32(&buff[0]) == ReadNInt32(&stat->me->hwAddr[0]) && ReadNInt16(&buff[4]) == ReadNInt16(&stat->me->hwAddr[4]))
				{
					if (opcode == 1) //Request
					{

					}
					else if (opcode == 2) //Reply
					{
						stat->me->hdlr(&buff[22], ReadNUInt32(&buff[28]), stat->me->userData);
					}
				}
			}
		}
		MemFreeArr(buff);
	}
	stat->threadRunning = false;
	if (stat->me->ctrlEvt.SetTo(ctrlEvt))
	{
		ctrlEvt->Set();
	}
	return 0;
}

Net::ARPHandler::ARPHandler(NN<Net::SocketFactory> sockf, UnsafeArray<const UTF8Char> ifName, const UInt8 *hwAddr, UInt32 adapterIP, ARPResponseHdlr hdlr, AnyType userData, UIntOS threadCnt)
{
	UIntOS i;
	this->threadCnt = threadCnt;
	this->sockf = sockf;
	this->hdlr = hdlr;
	this->ifName = Text::StrCopyNew(ifName);
	MemCopyNO(this->hwAddr, hwAddr, 6);
	this->ipAddr = adapterIP;
	this->userData = userData;
	this->ctrlEvt = nullptr;
	this->soc = this->sockf->CreateARPSocket();
	this->threadStats = nullptr;

	UnsafeArray<ThreadStat> threadStats;
	NN<Socket> soc;
	if (this->soc.SetTo(soc))
	{
		NN<Sync::Event> ctrlEvt;
		NEW_CLASSNN(ctrlEvt, Sync::Event(true));
		this->ctrlEvt = ctrlEvt;

		this->threadStats = threadStats = MemAllocArr(Net::ARPHandler::ThreadStat, this->threadCnt);

		i = this->threadCnt;
		while (i-- > 0)
		{
			threadStats[i].toStop = false;
			threadStats[i].threadRunning = false;
			NEW_CLASSNN(threadStats[i].evt, Sync::Event(true));
			threadStats[i].me = *this;
			Sync::ThreadUtil::Create(DataThread, &threadStats[i]);
		}
		Bool running;
		while (true)
		{
			running = true;
			i = this->threadCnt;
			while (i-- > 0)
			{
				if (!threadStats[i].threadRunning)
				{
					running = false;
					break;
				}
			}
			if (running)
				break;
			ctrlEvt->Wait(10);
		}
	}
}

Net::ARPHandler::~ARPHandler()
{
	UIntOS i;
	NN<Socket> soc;
	UnsafeArray<ThreadStat> threadStats;
	NN<Sync::Event> ctrlEvt;
	if (this->threadStats.SetTo(threadStats))
	{
		i = this->threadCnt;
		while (i-- > 0)
		{
		threadStats[i].toStop = true;
		}
	}
	if (this->soc.SetTo(soc))
	{
		this->sockf->DestroySocket(soc);
	}
	if (this->threadStats.SetTo(threadStats) && this->ctrlEvt.SetTo(ctrlEvt))
	{
		i = this->threadCnt;
		while (i-- > 0)
		{
			threadStats[i].evt->Set();
		}

		Bool threadRunning = true;
		while (threadRunning)
		{
			threadRunning = false;
			i = this->threadCnt;
			while (i-- > 0)
			{
				if (threadStats[i].threadRunning)
				{
					threadRunning = true;
					break;
				}
			}
			if (!threadRunning)
				break;
			ctrlEvt->Wait(10);
		}

		i = this->threadCnt;
		while (i-- > 0)
		{
			threadStats[i].evt.Delete();
		}
		MemFreeArr(threadStats);
	}
	this->soc = nullptr;

	this->ctrlEvt.Delete();
	Text::StrDelNew(this->ifName);
}

Bool Net::ARPHandler::IsError()
{
	return this->soc.IsNull();
}

Bool Net::ARPHandler::MakeRequest(UInt32 targetIP)
{
	NN<Socket> soc;
	if (!this->soc.SetTo(soc))
		return false;
	UInt8 buff[256];
	Bool succ = false;
	WriteNUInt32(&buff[0], 0xffffffff);
	WriteNInt16(&buff[4], (Int16)0xffff);
	MemCopyNO(&buff[6], this->hwAddr, 6);
	WriteMInt16(&buff[12], 0x806); //ARP
	WriteMInt16(&buff[14], 1); //HWType
	WriteMInt16(&buff[16], 0x800); //IPv4
	buff[18] = 6; //HW Size
	buff[19] = 4; //Protocol Size;
	WriteMInt16(&buff[20], 1); //Opcode = request
	MemCopyNO(&buff[22], this->hwAddr, 6); // Sender Address
	WriteNUInt32(&buff[28], this->ipAddr); // Sender IP
	MemClear(&buff[32], 6); // Target Address
	WriteNUInt32(&buff[38], targetIP); //Target IP
	MemClear(&buff[42], 18);

	succ = (this->sockf->SendToIF(soc, buff, 60, this->ifName) == 60);
	return succ;
}
