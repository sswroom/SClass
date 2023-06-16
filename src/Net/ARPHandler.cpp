#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "Net/ARPHandler.h"
#include "Sync/ThreadUtil.h"
#include "Text/MyString.h"

UInt32 __stdcall Net::ARPHandler::DataThread(void *obj)
{
	Net::ARPHandler::ThreadStat *stat = (Net::ARPHandler::ThreadStat*)obj;
	stat->threadRunning = true;
	stat->me->ctrlEvt->Set();

	UInt8 *buff = MemAlloc(UInt8, 2048);
	while (!stat->toStop)
	{
		UOSInt recvSize;
		Net::SocketUtil::AddressInfo recvAddr;
		UInt16 recvPort;

		recvSize = stat->me->sockf->UDPReceive(stat->me->soc, buff, 2048, &recvAddr, &recvPort, 0);
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
	MemFree(buff);
	stat->threadRunning = false;
	stat->me->ctrlEvt->Set();
	return 0;
}

Net::ARPHandler::ARPHandler(Net::SocketFactory *sockf, const UTF8Char *ifName, const UInt8 *hwAddr, UInt32 adapterIP, ARPResponseHdlr hdlr, void *userData, UOSInt threadCnt)
{
	UOSInt i;
	this->threadCnt = threadCnt;
	this->sockf = sockf;
	this->hdlr = hdlr;
	this->ifName = Text::StrCopyNew(ifName);
	MemCopyNO(this->hwAddr, hwAddr, 6);
	this->ipAddr = adapterIP;
	this->userData = userData;
	this->ctrlEvt = 0;
	this->soc = this->sockf->CreateARPSocket();
	this->threadStats = 0;

	if (this->soc)
	{
		NEW_CLASS(this->ctrlEvt, Sync::Event(true));

		this->threadStats = MemAlloc(Net::ARPHandler::ThreadStat, this->threadCnt);

		i = this->threadCnt;
		while (i-- > 0)
		{
			this->threadStats[i].toStop = false;
			this->threadStats[i].threadRunning = false;
			NEW_CLASS(this->threadStats[i].evt, Sync::Event(true));
			this->threadStats[i].me = this;
			Sync::ThreadUtil::Create(DataThread, &this->threadStats[i]);
		}
		Bool running;
		while (true)
		{
			running = true;
			i = this->threadCnt;
			while (i-- > 0)
			{
				if (!this->threadStats[i].threadRunning)
				{
					running = false;
					break;
				}
			}
			if (running)
				break;
			this->ctrlEvt->Wait(10);
		}
	}
}

Net::ARPHandler::~ARPHandler()
{
	UOSInt i;
	if (this->threadStats)
	{
		i = this->threadCnt;
		while (i-- > 0)
		{
			this->threadStats[i].toStop = true;
		}
	}
	if (this->soc)
	{
		this->sockf->DestroySocket(this->soc);
	}
	if (this->threadStats)
	{
		i = this->threadCnt;
		while (i-- > 0)
		{
			this->threadStats[i].evt->Set();
		}

		Bool threadRunning = true;
		while (threadRunning)
		{
			threadRunning = false;
			i = this->threadCnt;
			while (i-- > 0)
			{
				if (this->threadStats[i].threadRunning)
				{
					threadRunning = true;
					break;
				}
			}
			if (!threadRunning)
				break;
			this->ctrlEvt->Wait(10);
		}

		i = this->threadCnt;
		while (i-- > 0)
		{
			DEL_CLASS(this->threadStats[i].evt);
		}
		MemFree(this->threadStats);
	}
	if (this->soc)
	{
		this->sockf->DestroySocket(this->soc);
		this->soc = 0;
	}

	SDEL_CLASS(this->ctrlEvt);
	Text::StrDelNew(this->ifName);
}

Bool Net::ARPHandler::IsError()
{
	return this->soc == 0;
}

Bool Net::ARPHandler::MakeRequest(UInt32 targetIP)
{
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

	succ = (this->sockf->SendToIF(this->soc, buff, 60, this->ifName) == 60);
	return succ;
}
