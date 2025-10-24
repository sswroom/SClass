#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "Manage/HiResClock.h"
#include "Math/Math_C.h"
#include "Net/NTPServer.h"
#include "Net/NTPClient.h"
#include "Sync/MutexUsage.h"
#include "Text/MyString.h"

void __stdcall Net::NTPClient::PacketHdlr(NN<const Net::SocketUtil::AddressInfo> addr, UInt16 port, Data::ByteArrayR data, AnyType userData)
{
	NN<Net::NTPClient> me = userData.GetNN<Net::NTPClient>();
//	UInt8 li = buff[0] >> 6;
//	UInt8 vn = (buff[0] >> 3) & 7;
	UInt8 mode = data[0] & 7;
//	UInt8 stratum = buff[1];
//	UInt8 poll = buff[2];
//	UInt8 precision = buff[3];
//	UInt32 rootDelay = *(UInt32*)&buff[4];
//	UInt32 rootDispersion = *(UInt32*)&buff[8];
//	UInt32 ri = *(UInt32*)&buff[12];

	if (mode == 4 && data.GetSize() >= 40)
	{
		me->resultTime = Net::NTPServer::ReadTime(&data[32]);
		me->hasResult = true;
		me->evt.Set();
	}
}

Net::NTPClient::NTPClient(NN<Net::SocketFactory> sockf, UInt16 port, NN<IO::LogTool> log)
{
	this->sockf = sockf;
	this->resultTime = 0;
	NEW_CLASS(this->svr, Net::UDPServer(sockf, 0, port, nullptr, PacketHdlr, this, log, nullptr, 1, false));
}

Net::NTPClient::~NTPClient()
{
	DEL_CLASS(this->svr);
}

Bool Net::NTPClient::GetServerTime(Text::CStringNN host, UInt16 port, NN<Data::DateTime> svrTime)
{
	Net::SocketUtil::AddressInfo addr;
	if (!sockf->DNSResolveIP(host, addr))
		return false;
	return GetServerTime(addr, port, svrTime);
}

Bool Net::NTPClient::GetServerTime(Text::CStringNN host, UInt16 port, OutParam<Data::Timestamp> svrTime)
{
	Net::SocketUtil::AddressInfo addr;
	if (!sockf->DNSResolveIP(host, addr))
		return false;
	return GetServerTime(addr, port, svrTime);
}

Bool Net::NTPClient::GetServerTime(NN<const Net::SocketUtil::AddressInfo> addr, UInt16 port, NN<Data::DateTime> svrTime)
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
	Net::NTPServer::WriteTime(&buff[40], Data::Timestamp::UtcNow());

	this->hasResult = false;
	this->svr->SendTo(addr, port, buff, 48);
	Manage::HiResClock clk;
	clk.Start();
	while (!this->hasResult)
	{
		this->evt.Wait(1000);
		if (clk.GetTimeDiff() >= 10)
			break;
	}
	hasResult = this->hasResult;
	if (hasResult)
	{
		Data::Timestamp ts = this->resultTime.AddNS(Double2Int64(clk.GetTimeDiff() * 500000000));
		svrTime->SetInstant(ts.inst);
	}
	return hasResult;
}

Bool Net::NTPClient::GetServerTime(NN<const Net::SocketUtil::AddressInfo> addr, UInt16 port, OutParam<Data::Timestamp> svrTime)
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

	Net::NTPServer::WriteTime(&buff[40], Data::Timestamp::UtcNow());
	this->hasResult = false;
	this->svr->SendTo(addr, port, buff, 48);
	Manage::HiResClock clk;
	clk.Start();
	while (!this->hasResult)
	{
		this->evt.Wait(1000);
		if (clk.GetTimeDiff() >= 10)
			break;
	}
	hasResult = this->hasResult;
	if (hasResult)
	{
		svrTime.Set(this->resultTime.AddNS(Double2Int64(clk.GetTimeDiff() * 500000000)));
	}
	return hasResult;
}

UInt16 Net::NTPClient::GetDefaultPort()
{
	return 123;
}
