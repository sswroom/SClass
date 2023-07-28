#include "Stdafx.h"
#include "MyMemory.h"
#include "Net/LoRaGateway.h"
#include "Net/LoRaGWUtil.h"
#include "Sync/MutexUsage.h"
#include "Sync/ThreadUtil.h"
#include "Text/StringBuilderUTF8.h"

#include <stdio.h>

void __stdcall Net::LoRaGateway::OnUDPPacket(const Net::SocketUtil::AddressInfo *addr, UInt16 port, const UInt8 *buff, UOSInt dataSize, void *userData)
{
	Text::StringBuilderUTF8 sb;
	Net::LoRaGWUtil::ParseUDPMessage(&sb, false, buff, dataSize);
	printf("%s\r\n", sb.ToString());
}

UInt32 __stdcall Net::LoRaGateway::PullThread(void *userObj)
{
	Net::LoRaGateway *me = (Net::LoRaGateway*)userObj;
	Data::Timestamp currTime;
	{
		Sync::Event evt;
		me->threadEvt = &evt;
		me->threadRunning = true;
		me->mainEvt.Set();
		while (!me->threadToStop)
		{
			currTime = Data::Timestamp::UtcNow();
			if (currTime.DiffMS(me->lastPullTime) >=  me->pullInterval * 1000)
			{
				me->lastPullTime = currTime;
				me->SendPullData();
			}
			else if (currTime.DiffMS(me->lastStatTime) >= me->statInterval * 1000)
			{
				me->lastStatTime = currTime;
				me->SendStatData();
			}
			evt.Wait(1000);
		}
	}
	me->threadRunning = false;
	me->mainEvt.Set();
	return 0;
}

Bool Net::LoRaGateway::SendPullData()
{
	UInt8 buff[12];
	UInt16 token = this->NextToken();
	buff[0] = 1; //Version
	WriteMUInt16(&buff[1], token);
	buff[3] = 2; //PULL_DATA
	MemCopyNO(&buff[4], this->gatewayEUI, 8);
	return this->udp.SendTo(&this->svrAddr, this->svrPort, buff, 12);
}

Bool Net::LoRaGateway::SendStatData()
{
	Text::StringBuilderUTF8 sb;
	if (this->hasPos)
	{
		Net::LoRaGWUtil::GenStatJSON(&sb, Data::Timestamp::UtcNow(), 0, 0, 0, 100.0, 0, 0, this->lat, this->lon, this->altitude);
	}
	else
	{
		Net::LoRaGWUtil::GenStatJSON(&sb, Data::Timestamp::UtcNow(), 0, 0, 0, 100.0, 0, 0);
	}
	return this->SendPushData(sb.ToString(), sb.GetLength());
}

Net::LoRaGateway::LoRaGateway(NotNullPtr<Net::SocketFactory> sockf, const Net::SocketUtil::AddressInfo *svrAddr, UInt16 svrPort, const UInt8 *gatewayEUI, IO::LogTool *log) : udp(sockf, 0, 0, CSTR_NULL, OnUDPPacket, this, log, CSTR("LoRa: "), 4, false)
{
	this->svrAddr = *svrAddr;
	this->svrPort = svrPort;
	MemCopyNO(this->gatewayEUI, gatewayEUI, 8);
	this->tokenNext = (UInt16)(Data::DateTimeUtil::GetCurrTimeMillis() & 0xffff);
	this->statInterval = 30;
	this->pullInterval = 5;
	this->lastPullTime = Data::Timestamp(0);
	this->lastStatTime = Data::Timestamp(0);
	this->threadRunning = false;
	this->threadToStop = false;
	this->threadEvt = 0;
	this->hasPos = false;
	this->lat = 0;
	this->lon = 0;
	this->altitude = 0;
	Sync::ThreadUtil::Create(PullThread, this);
	while (!this->threadRunning)
	{
		this->mainEvt.Wait(1000);
	}
}

Net::LoRaGateway::~LoRaGateway()
{
	if (this->threadRunning)
	{
		this->threadToStop = true;
		this->threadEvt->Set();
		while (this->threadRunning)
		{
			this->mainEvt.Wait(1000);
		}
	}
}

Bool Net::LoRaGateway::IsError()
{
	return this->udp.IsError();
}

UInt16 Net::LoRaGateway::NextToken()
{
	Sync::MutexUsage mutUsage(this->tokenMut);
	return this->tokenNext++;
}

Bool Net::LoRaGateway::SendPushData(const UInt8 *data, UOSInt dataLeng)
{
	UInt8 *buff = MemAlloc(UInt8, 12 + dataLeng);
	UInt16 token = this->NextToken();
	buff[0] = 1; //Version
	WriteMUInt16(&buff[1], token);
	buff[3] = 0; //PUSH_DATA
	MemCopyNO(&buff[4], this->gatewayEUI, 8);
	MemCopyNO(&buff[12], data, dataLeng);
	Bool ret = this->udp.SendTo(&this->svrAddr, this->svrPort, buff, 12 + dataLeng);
	MemFree(buff);
	return ret;
}
