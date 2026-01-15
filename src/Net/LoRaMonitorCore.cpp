#include "Stdafx.h"
#include "IO/Path.h"
#include "Net/LoRaMonitorCore.h"
#include "Net/LoRaMonitorHandler.h"
#include "Net/PacketExtractorEthernet.h"
#include "Sync/MutexUsage.h"

void __stdcall Net::LoRaMonitorCore::OnRAWPacket(AnyType userData, UnsafeArray<const UInt8> packetData, UOSInt packetSize)
{
	NN<Net::LoRaMonitorCore> me = userData.GetNN<Net::LoRaMonitorCore>();
	Net::PacketExtractorEthernet::IPv4Header ipv4Hdr;
	Net::PacketExtractorEthernet::UDPHeader udpHdr;
	Net::PacketExtractorEthernet::EthernetHeader etherHdr;
	UOSInt udpSize;
	UnsafeArray<const UInt8> udpData;
	if (Net::PacketExtractorEthernet::EthernetExtractUDP(packetData, packetSize, udpSize, etherHdr, ipv4Hdr, udpHdr).SetTo(udpData) &&
		udpSize >= ((UOSInt)udpHdr.leng - 8) && udpHdr.leng >= 12)
	{
		if (udpHdr.srcPort == me->loraPort)
		{
			me->OnLoRaPacket(false, udpData[0], ReadMUInt16(&udpData[1]), udpData[3], udpData + 4, (UOSInt)udpHdr.leng - 4 - 8);
		}
		else if (udpHdr.destPort == me->loraPort)
		{
			me->OnLoRaPacket(true, udpData[0], ReadMUInt16(&udpData[1]), udpData[3], udpData + 4, (UOSInt)udpHdr.leng - 4 - 8);
		}
	}
}

void Net::LoRaMonitorCore::OnLoRaPacket(Bool toServer, UInt8 ver, UInt16 token, UInt8 msgType, UnsafeArray<const UInt8> msg, UOSInt msgSize)
{
	UInt64 gweui;
	NN<GWInfo> gw;
	switch (msgType)
	{
	case 0: //PUSH_DATA
		if (msgSize >= 8)
		{
			gweui = ReadMUInt64(&msg[0]);
			gw = GetGWOrCreate(gweui);
			GWAddData(gw, msgType, msg + 8, msgSize - 8);
		}
		break;
	case 1: //PUSH_ACK
		break;
	case 2: //PULL_DATA
		if (msgSize >= 8)
		{
			gweui = ReadMUInt64(&msg[0]);
			gw = GetGWOrCreate(gweui);
			GWAddData(gw, msgType, msg + 8, msgSize - 8);
		}
		break;
	case 3: //PULL_RESP
		break;
	case 4: //PULL_ACK
		if (msgSize >= 8)
		{
			gweui = ReadMUInt64(&msg[0]);
			gw = GetGWOrCreate(gweui);
			GWAddData(gw, msgType, msg + 8, msgSize - 8);
		}
		break;
	case 5: //TX_ACK
		break;
	}
}

NN<Net::LoRaMonitorCore::GWInfo> Net::LoRaMonitorCore::GetGWOrCreate(UInt64 gweui)
{
	Sync::MutexUsage mutUsage(this->gwMut);
	NN<Net::LoRaMonitorCore::GWInfo> gw;
	if (this->gwMap.Get(gweui).SetTo(gw))
	{
		gw->lastSeenTime = Data::Timestamp::UtcNow();
		gw->updated = true;
		return gw;
	}
	gw = MemAllocNN(GWInfo);
	gw->gweui = gweui;
	gw->name = nullptr;
	gw->model = nullptr;
	gw->sn = nullptr;
	gw->imei = nullptr;
	gw->location = nullptr;
	gw->lastSeenTime = Data::Timestamp::UtcNow();
	gw->updated = false;
	gw->lastDataBegin = 0;
	gw->lastDataEnd = 0;
	DB::SQLBuilder sql(this->db->GetSQLType(), false, this->db->GetTzQhr());
	sql.AppendCmdC(CSTR("insert into gateway (gweui, last_seen_time) values ("));
	sql.AppendInt64((Int64)gw->gweui);
	sql.AppendCmdC(CSTR(", "));
	sql.AppendTS(gw->lastSeenTime);
	sql.AppendCmdC(CSTR(")"));
	this->db->ExecuteNonQuery(sql.ToCString());
	this->gwMap.Put(gw->gweui, gw);
	return gw;
}

void Net::LoRaMonitorCore::GWAddData(NN<GWInfo> gw, UInt8 msgType, UnsafeArray<const UInt8> msg, UOSInt msgSize)
{
	NN<DataPacket> data;
	if (gw->lastDataBegin == gw->lastDataEnd)
	{
		data = MemAllocNN(DataPacket);
		data->recvTime = Data::Timestamp::UtcNow();
		data->msgType = msgType;
		data->msgSize = msgSize;
		data->msg = MemAllocArr(UInt8, msgSize);
		data->msg.CopyFromNO(msg, msgSize);
		gw->lastData[gw->lastDataEnd] = data;
		gw->lastDataEnd = (gw->lastDataEnd + 1) & 15;
	}
	else
	{
		UOSInt nextIndex = (gw->lastDataEnd + 1) & 15;
		if (nextIndex == gw->lastDataBegin)
		{
			if (gw->lastData[gw->lastDataBegin].SetTo(data))
			{
				MemFreeArr(data->msg);
				MemFreeNN(data);
			}
			gw->lastDataBegin = (gw->lastDataBegin + 1) & 15;
		}
		data = MemAllocNN(DataPacket);
		data->recvTime = Data::Timestamp::UtcNow();
		data->msgType = msgType;
		data->msgSize = msgSize;
		data->msg = MemAllocArr(UInt8, msgSize);
		data->msg.CopyFromNO(msg, msgSize);
		gw->lastData[gw->lastDataEnd] = data;
		gw->lastDataEnd = nextIndex;
	}
}

void Net::LoRaMonitorCore::LoadDB()
{
	NN<GWInfo> gw;
	NN<DB::DBReader> r;
	if (this->db->ExecuteReader(CSTR("select gweui, name, model, sn, imei, location, last_seen_time from gateway")).SetTo(r))
	{
		while (r->ReadNext())
		{
			gw = MemAllocNN(GWInfo);
			gw->gweui = (UInt64)r->GetInt64(0);
			gw->name = r->GetNewStr(1);
			gw->model = r->GetNewStr(2);
			gw->sn = r->GetNewStr(3);
			gw->imei = r->GetNewStr(4);
			gw->location = r->GetNewStr(5);
			gw->lastSeenTime = r->GetTimestamp(6);
			gw->updated = false;
			gw->lastDataBegin = 0;
			gw->lastDataEnd = 0;
			this->gwMap.Put(gw->gweui, gw);
		}
		this->db->CloseReader(r);
	}
}

void Net::LoRaMonitorCore::SaveGWList()
{
	DB::SQLBuilder sql(this->db->GetSQLType(), false, this->db->GetTzQhr());
	Optional<DB::DBTransaction> tran = nullptr;
	NN<DB::DBTransaction> nntran;
	Sync::MutexUsage mutUsage(this->gwMut);
	NN<Net::LoRaMonitorCore::GWInfo> gw;
	UOSInt i = 0;
	UOSInt j = this->gwMap.GetCount();
	while (i < j)
	{
		gw = this->gwMap.GetItemNoCheck(i);
		if (gw->updated)
		{
			gw->updated = false;
			sql.Clear();
			sql.AppendCmdC(CSTR("update gateway set last_seen_time = "));
			sql.AppendTS(gw->lastSeenTime);
			sql.AppendCmdC(CSTR(" where gweui = "));
			sql.AppendInt64((Int64)gw->gweui);
			if (tran.IsNull())
				tran = this->db->BeginTransaction();
			this->db->ExecuteNonQuery(sql.ToCString());
		}
		i++;
	}
	if (tran.SetTo(nntran))
	{
		this->db->Commit(nntran);
	}
}

void __stdcall Net::LoRaMonitorCore::FreeGW(NN<GWInfo> gw)
{
	OPTSTR_DEL(gw->name);
	OPTSTR_DEL(gw->model);
	OPTSTR_DEL(gw->sn);
	OPTSTR_DEL(gw->imei);
	OPTSTR_DEL(gw->location);
	MemFreeNN(gw);
}

Net::LoRaMonitorCore::LoRaMonitorCore(NN<Net::SocketFactory> sockf, UInt16 loraPort, UInt16 uiPort)
{
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	this->sockf = sockf;
	NEW_CLASSNN(this->clif, Net::TCPClientFactory(sockf));
	this->loraPort = loraPort;
	sbuff[0] = 0;
	sptr = IO::Path::GetProcessFileName(sbuff).Or(sbuff);
	sptr = IO::Path::ReplaceExt(sbuff, UTF8STRC(".db"));
	NEW_CLASSNN(this->db, DB::SQLiteFile(CSTRP(sbuff, sptr)));
	this->LoadDB();
	sptr = IO::Path::GetProcessFileName(sbuff).Or(sbuff);
	sptr = IO::Path::AppendPath(sbuff, sptr, CSTR("web"));
	NEW_CLASSNN(this->handler, LoRaMonitorHandler(*this, CSTRP(sbuff, sptr)));
	this->listener = nullptr;
	this->s = this->sockf->CreateRAWSocket();
	NN<Net::WebServer::WebListener> listener;
	NEW_CLASSNN(listener, Net::WebServer::WebListener(this->clif, Optional<Net::SSLEngine>(nullptr), this->handler, uiPort, 120, 1, 4, CSTR("LoRaMonitor/1.0"), false, Net::WebServer::KeepAlive::Default, true));
	if (listener->IsError())
	{
		listener.Delete();
		this->listener = nullptr;
		this->socMon = nullptr;
	}
	else
	{
		NN<Socket> s;
		this->listener = listener;
		if (this->s.SetTo(s))
		{
			NEW_CLASSOPT(this->socMon, Net::SocketMonitor(this->sockf, s, OnRAWPacket, this, 4));
		}
		else
		{
			this->socMon = nullptr;
		}
	}
}

Net::LoRaMonitorCore::~LoRaMonitorCore()
{
	this->listener.Delete();
	this->socMon.Delete();
	this->db.Delete();
	this->handler.Delete();
	this->clif.Delete();
}

Bool Net::LoRaMonitorCore::IsError()
{
	return this->socMon.IsNull() || this->db->IsError();
}

NN<Data::ReadingListNN<Net::LoRaMonitorCore::GWInfo>> Net::LoRaMonitorCore::GetGWList(NN<Sync::MutexUsage> mutUsage)
{
	mutUsage->ReplaceMutex(this->gwMut);
	return this->gwMap;
}

Optional<Net::LoRaMonitorCore::GWInfo> Net::LoRaMonitorCore::GetGW(UInt64 gweui, NN<Sync::MutexUsage> mutUsage)
{
	mutUsage->ReplaceMutex(this->gwMut);
	return this->gwMap.Get(gweui);
}
