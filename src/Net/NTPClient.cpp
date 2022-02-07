#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "Manage/HiResClock.h"
#include "Math/Math.h"
#include "Net/NTPServer.h"
#include "Net/NTPClient.h"
#include "Sync/MutexUsage.h"
#include "Text/MyString.h"

void __stdcall Net::NTPClient::PacketHdlr(const Net::SocketUtil::AddressInfo *addr, UInt16 port, const UInt8 *buff, UOSInt dataSize, void *userData)
{
	Net::NTPClient *me = (Net::NTPClient*)userData;
//	UInt8 li = buff[0] >> 6;
//	UInt8 vn = (buff[0] >> 3) & 7;
	UInt8 mode = buff[0] & 7;
//	UInt8 stratum = buff[1];
//	UInt8 poll = buff[2];
//	UInt8 precision = buff[3];
//	UInt32 rootDelay = *(UInt32*)&buff[4];
//	UInt32 rootDispersion = *(UInt32*)&buff[8];
//	UInt32 ri = *(UInt32*)&buff[12];

	if (mode == 4 && me->resultTime)
	{
		Net::NTPServer::ReadTime(&buff[32], me->resultTime);
		me->hasResult = true;
		me->evt->Set();
	}
}

Net::NTPClient::NTPClient(Net::SocketFactory *sockf, UInt16 port)
{
	this->sockf = sockf;
	this->resultTime = 0;
	NEW_CLASS(this->mut, Sync::Mutex());
	NEW_CLASS(this->evt, Sync::Event(true));
	NEW_CLASS(this->svr, Net::UDPServer(sockf, 0, port, 0, PacketHdlr, this, 0, 0, 1, false));
}

Net::NTPClient::~NTPClient()
{
	DEL_CLASS(this->svr);
	DEL_CLASS(this->evt);
	DEL_CLASS(this->mut);
}

Bool Net::NTPClient::GetServerTime(const UTF8Char *host, UInt16 port, Data::DateTime *svrTime)
{
	Net::SocketUtil::AddressInfo addr;
	if (!sockf->DNSResolveIP(host, Text::StrCharCnt(host), &addr))
		return false;
	return GetServerTime(&addr, port, svrTime);
}

Bool Net::NTPClient::GetServerTime(const Net::SocketUtil::AddressInfo *addr, UInt16 port, Data::DateTime *svrTime)
{
	UInt8 buff[48];
	Bool hasResult;

	if (port == 0)
	{
		port = 123;
	}

	Sync::MutexUsage mutUsage(this->mut);
	MemClear(buff, 48);
	buff[0] = 0xe3;
	buff[12] = 'S';
	buff[13] = 'S';
	buff[14] = 'W';
	buff[15] = 0;
	svrTime->SetCurrTimeUTC();
	Net::NTPServer::WriteTime(&buff[40], svrTime);

	this->resultTime = svrTime;
	this->hasResult = false;
	this->svr->SendTo(addr, port, buff, 48);
	Manage::HiResClock clk;
	clk.Start();
	while (!this->hasResult)
	{
		this->evt->Wait(1000);
		if (clk.GetTimeDiff() >= 10)
			break;
	}
	hasResult = this->hasResult;
	this->resultTime = 0;
	if (hasResult)
	{
		svrTime->AddMS(Double2Int32(clk.GetTimeDiff() * 500));
	}
	mutUsage.EndUse();
	return hasResult;
}

UInt16 Net::NTPClient::GetDefaultPort()
{
	return 123;
}
