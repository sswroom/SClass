#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/Device/SIM7000SocketFactory.h"
#include "Net/ConnectionInfo.h"
#include "Sync/MutexUsage.h"
#include "Sync/Thread.h"
#include "Text/MyString.h"
#include <stdio.h>

void __stdcall IO::Device::SIM7000SocketFactory::OnReceiveData(void *userObj, UOSInt index, UInt32 remoteIP, UInt16 remotePort, const UInt8 *buff, UOSInt buffSize)
{
	IO::Device::SIM7000SocketFactory *me = (IO::Device::SIM7000SocketFactory*)userObj;
	DataPacket *packet;
	UTF8Char sbuff[32];
	Net::SocketUtil::GetIPv4Name(sbuff, remoteIP, remotePort);
	if (me->status[index].state == SocketState::UDP_Opened || me->status[index].state == SocketState::TCP_Opened)
	{
		packet = (DataPacket*)MAlloc(sizeof(DataPacket) + buffSize);
		packet->remoteIP = remoteIP;
		packet->remotePort = remotePort;
		packet->dataSize = buffSize;
		MemCopyNO(packet->data, buff, buffSize);
		Sync::MutexUsage mutUsage(me->status[index].dataMut);
		me->status[index].dataList->Put(packet);
		me->status[index].dataEvt->Set();
		mutUsage.EndUse();
	}
}

void IO::Device::SIM7000SocketFactory::CloseAllSockets()
{
	UOSInt i = 8;
	DataPacket *packet;
	while (i-- > 0)
	{
		switch (this->status[i].state)
		{
		case SocketState::Empty:
			break;
		default:
		case SocketState::TCP_Unopened:
		case SocketState::UDP_Unopened:
			this->status[i].state = SocketState::Empty;
			break;
		case SocketState::TCP_Opened:
		case SocketState::UDP_Opened:
			{
				this->modem->NetCloseSocket(i);
				this->status[i].state = SocketState::Empty;
				Sync::MutexUsage mutUsage(this->status[i].dataMut);
				while ((packet = (DataPacket*)this->status[i].dataList->Get()) != 0)
				{
					MemFree(packet);
				}
				mutUsage.EndUse();
			}
			break;
		}
	}
}

IO::Device::SIM7000SocketFactory::SIM7000SocketFactory(IO::Device::SIM7000 *modem, Bool needRelease) : Net::SocketFactory(true)
{
	this->modem = modem;
	this->needRelease = needRelease;
	this->apn = 0;
	this->modem->SetReceiveHandler(OnReceiveData, this);
	OSInt i = 8;
	while (i-- > 0)
	{
		this->status[i].state = SocketState::Empty;
		NEW_CLASS(this->status[i].dataMut, Sync::Mutex());
		NEW_CLASS(this->status[i].dataList, Data::SyncLinkedList());
		NEW_CLASS(this->status[i].dataEvt, Sync::Event(true, (const UTF8Char*)"IO.Device.SIM7000SocketFactory.status.dataEvt"));
	}
	this->Init();
}

IO::Device::SIM7000SocketFactory::~SIM7000SocketFactory()
{
	OSInt i;
	this->CloseAllSockets();
	this->modem->SetReceiveHandler(0, 0);
	i = 8;
	while (i-- > 0)
	{
		DEL_CLASS(this->status[i].dataMut);
		DEL_CLASS(this->status[i].dataList);
		DEL_CLASS(this->status[i].dataEvt);
	}
	SDEL_TEXT(this->apn);
	if (this->needRelease)
	{
		DEL_CLASS(this->modem);
	}
}

void IO::Device::SIM7000SocketFactory::SetAPN(const UTF8Char *apn)
{
	SDEL_TEXT(this->apn);
	if (apn)
	{
		this->apn = Text::StrCopyNew(apn);
	}
}

const UTF8Char *IO::Device::SIM7000SocketFactory::GetAPN()
{
	return this->apn;
}

void IO::Device::SIM7000SocketFactory::Init()
{
	this->modem->NetSetMultiIP(true);
	this->modem->NetShowRemoteIP(true);
	this->modem->NetSetSendHex(true);
	this->modem->NetSetOutputHex(true);
}

Bool IO::Device::SIM7000SocketFactory::NetworkStart()
{
	UTF8Char sbuff[256];
	Bool bVal;
	modem->GPRSEPSReg();
	modem->GPRSNetworkReg();
	if (modem->GPRSServiceIsAttached(&bVal))
	{
		if (!bVal)
		{
			if (!modem->GPRSServiceSetAttached(true))
			{
				return false;
			}
		}
	}
	else
	{
		return false;
	}
	modem->GPRSSetPDPActive(true);

	if (this->apn)
	{
		if (!modem->NetSetAPN(this->apn))
		{
//			return false;			
		}
	}
	else
	{
		if (modem->SIMCOMGetNetworkAPN(sbuff) == 0)
		{
			return false;
		}

		if (!modem->NetSetAPN(sbuff))
		{
	//		return false;
		}
	}
	
	if (!modem->NetDataStart())
	{
//		return false;
	}
	return true;
}

Bool IO::Device::SIM7000SocketFactory::NetworkEnd()
{
	this->CloseAllSockets();
	return true; //this->modem->GPRSServiceSetAttached(false);
}

Socket *IO::Device::SIM7000SocketFactory::CreateTCPSocketv4()
{
	return 0;
}

Socket *IO::Device::SIM7000SocketFactory::CreateTCPSocketv6()
{
	return 0;
}

Socket *IO::Device::SIM7000SocketFactory::CreateUDPSocketv4()
{
	OSInt i = 0;
	OSInt j = 8;
	while (i < j)
	{
		if (this->status[i].state == SocketState::Empty)
		{
			this->status[i].state = SocketState::UDP_Unopened;
			this->status[i].udpRIP = 0;
			this->status[i].udpRPort = 0;
			return (Socket*)(i + 1);
		}
		i++;
	}
	return 0;
}

Socket *IO::Device::SIM7000SocketFactory::CreateUDPSocketv6()
{
	return 0;
}

Socket *IO::Device::SIM7000SocketFactory::CreateICMPIPv4Socket(UInt32 ip)
{
	return 0;
}

Socket *IO::Device::SIM7000SocketFactory::CreateUDPRAWv4Socket(UInt32 ip)
{
	return 0;
}

Socket *IO::Device::SIM7000SocketFactory::CreateRAWIPv4Socket(UInt32 ip)
{
	return 0;
}

Socket *IO::Device::SIM7000SocketFactory::CreateARPSocket()
{
	return 0;
}

Socket *IO::Device::SIM7000SocketFactory::CreateRAWSocket()
{
	return 0;
}

void IO::Device::SIM7000SocketFactory::DestroySocket(Socket *socket)
{
	UOSInt i = (UOSInt)socket - 1;
	DataPacket *packet;
	if (socket == 0 || i >= 8)
	{
		return;
	}
	switch (this->status[i].state)
	{
	case SocketState::Empty:
		break;
	default:
	case SocketState::TCP_Unopened:
	case SocketState::UDP_Unopened:
		this->status[i].state = SocketState::Empty;
		break;
	case SocketState::TCP_Opened:
	case SocketState::UDP_Opened:
		{
			this->modem->NetCloseSocket(i);
			this->status[i].state = SocketState::Empty;
			Sync::MutexUsage mutUsage(this->status[i].dataMut);
			while ((packet = (DataPacket*)this->status[i].dataList->Get()) != 0)
			{
				MemFree(packet);
			}
			mutUsage.EndUse();
		}
		break;
	}
}

Bool IO::Device::SIM7000SocketFactory::SocketIsInvalid(Socket *socket)
{
	OSInt i = -1 + (OSInt)socket;
	if (i < 0 || i >= 8)
	{
		return true;
	}
	switch (this->status[i].state)
	{
	case SocketState::Empty:
		return true;
	default:
	case SocketState::TCP_Unopened:
	case SocketState::UDP_Unopened:
	case SocketState::TCP_Opened:
	case SocketState::UDP_Opened:
		return false;
	}
}

Bool IO::Device::SIM7000SocketFactory::SocketBindv4(Socket *socket, UInt32 ip, UInt16 port)
{
	UOSInt i = (UOSInt)socket - 1;
	if (socket == 0 || i >= 8)
	{
		return false;
	}
	if (this->status[i].state == SocketState::TCP_Unopened)
	{
		return this->modem->NetSetLocalPortTCP(i, port);
	}
	else if (this->status[i].state == SocketState::UDP_Unopened)
	{
		if (this->modem->NetSetLocalPortUDP(i, port))
		{
			return true;
		}
		this->modem->NetCloseSocket(i);
		return this->modem->NetSetLocalPortUDP(i, port);
	}
	return false;
}

Bool IO::Device::SIM7000SocketFactory::SocketBind(Socket *socket, const Net::SocketUtil::AddressInfo *addr, UInt16 port)
{
	return false;
}

Bool IO::Device::SIM7000SocketFactory::SocketListen(Socket *socket)
{
	return false;
}

Socket *IO::Device::SIM7000SocketFactory::SocketAccept(Socket *socket)
{
	return 0;
}

Int32 IO::Device::SIM7000SocketFactory::SocketGetLastError()
{
	return 0;
}

Bool IO::Device::SIM7000SocketFactory::GetRemoteAddr(Socket *socket, Net::SocketUtil::AddressInfo *addr, UInt16 *port)
{
	return false;
}

Bool IO::Device::SIM7000SocketFactory::GetLocalAddr(Socket *socket, Net::SocketUtil::AddressInfo *addr, UInt16 *port)
{
	return false;
}

Int32 IO::Device::SIM7000SocketFactory::SocketGetFD(Socket *socket)
{
	return false;
}

void IO::Device::SIM7000SocketFactory::SetDontLinger(Socket *socket, Bool val)
{

}

void IO::Device::SIM7000SocketFactory::SetLinger(Socket *socket, UInt32 ms)
{

}

void IO::Device::SIM7000SocketFactory::SetRecvBuffSize(Socket *socket, Int32 buffSize)
{

}

void IO::Device::SIM7000SocketFactory::SetNoDelay(Socket *socket, Bool val)
{
	this->modem->NetSetDisableNagle(val);
}

void IO::Device::SIM7000SocketFactory::SetRecvTimeout(Socket *socket, Int32 ms)
{

}

void IO::Device::SIM7000SocketFactory::SetReuseAddr(Socket *socket, Bool val)
{

}

void IO::Device::SIM7000SocketFactory::SetIPv4TTL(Socket *socket, Int32 ttl)
{
}

void IO::Device::SIM7000SocketFactory::SetBroadcast(Socket *socket, Bool val)
{
}

void IO::Device::SIM7000SocketFactory::AddIPMembership(Socket *socket, UInt32 ip)
{

}

UOSInt IO::Device::SIM7000SocketFactory::SendData(Socket *socket, const UInt8 *buff, UOSInt buffSize, ErrorType *et)
{
	return 0;
}

UOSInt IO::Device::SIM7000SocketFactory::ReceiveData(Socket *socket, UInt8 *buff, UOSInt buffSize, ErrorType *et)
{
	return 0;
}

void *IO::Device::SIM7000SocketFactory::BeginReceiveData(Socket *socket, UInt8 *buff, UOSInt buffSize, Sync::Event *evt, ErrorType *et)
{
	return 0;
}

UOSInt IO::Device::SIM7000SocketFactory::EndReceiveData(void *reqData, Bool toWait, Bool *incomplete)
{
	*incomplete = false;
	return 0;
}

void IO::Device::SIM7000SocketFactory::CancelReceiveData(void *reqData)
{

}

UOSInt IO::Device::SIM7000SocketFactory::UDPReceive(Socket *socket, UInt8 *buff, UOSInt buffSize, Net::SocketUtil::AddressInfo *addr, UInt16 *port, ErrorType *et)
{
	OSInt i = -1 + (OSInt)socket;
	if (i < 0 || i >= 8)
	{
		return false;
	}
	DataPacket *packet;
	while (this->status[i].state == SocketState::UDP_Opened)
	{
		if (this->status[i].dataList->GetCount() > 0)
		{
			Sync::MutexUsage mutUsage(this->status[i].dataMut);
			packet = (DataPacket*)this->status[i].dataList->Get();
			mutUsage.EndUse();
			if (packet)
			{
				Net::SocketUtil::SetAddrInfoV4(addr, packet->remoteIP);
				*port = packet->remotePort;
				if (buffSize >= packet->dataSize)
				{
					MemCopyNO(buff, packet->data, packet->dataSize);
					buffSize = packet->dataSize;
				}
				else
				{
					MemCopyNO(buff, packet->data, buffSize);
				}
				MemFree(packet);
				return buffSize;
			}
		}
		this->status[i].dataEvt->Wait(10000);
	}
	if (et)
	{
		*et = Net::SocketFactory::ET_DISCONNECT;
	}
	return 0;
}

UOSInt IO::Device::SIM7000SocketFactory::SendTo(Socket *socket, const UInt8 *buff, UOSInt buffSize, const Net::SocketUtil::AddressInfo *addr, UInt16 port)
{
	UOSInt i = (UOSInt)socket - 1;
	if (socket == 0 || i >= 8)
	{
		return false;
	}
	if (addr->addrType != Net::AddrType::IPv4)
	{
		return false;
	}
	UInt32 remoteIP = *(UInt32*)addr->addr;
	if (this->status[i].state == SocketState::UDP_Unopened)
	{
		if (this->modem->NetIPStartUDP(i, remoteIP, port))
		{
			this->status[i].udpRIP = remoteIP;
			this->status[i].udpRPort = port;
			this->status[i].state = SocketState::UDP_Opened;
		}
		else
		{
			return false;
		}
	}
	else if (this->status[i].state == SocketState::UDP_Opened)
	{
		if (this->status[i].udpRIP != remoteIP || this->status[i].udpRPort != port)
		{
			///////////////////////////////
			return 0;
		}
	}
	else
	{
		return 0;
	}

	if (this->modem->NetIPSend(i, buff, buffSize))
	{
		return buffSize;
	}
	else
	{
		return 0;
	}
}

UOSInt IO::Device::SIM7000SocketFactory::SendToIF(Socket *socket, const UInt8 *buff, UOSInt buffSize, const UTF8Char *ifName)
{
	return 0;
}

Bool IO::Device::SIM7000SocketFactory::IcmpSendEcho2(const Net::SocketUtil::AddressInfo *addr, UInt32 *respTime_us, UInt32 *ttl)
{
	UTF8Char sbuff[32];
	if (addr->addrType != Net::AddrType::IPv4)
	{
		return false;
	}
	Net::SocketUtil::GetAddrName(sbuff, addr);
	UInt32 respTimeMS;
	Bool succ = this->modem->NetPing(sbuff, &respTimeMS, ttl);
	if (succ && respTime_us)
	{
		*respTime_us = respTimeMS * 1000;
	}
	return succ;
}

Bool IO::Device::SIM7000SocketFactory::Connect(Socket *socket, UInt32 ip, UInt16 port)
{
	return false;
}

Bool IO::Device::SIM7000SocketFactory::Connect(Socket *socket, const Net::SocketUtil::AddressInfo *addr, UInt16 port)
{
	return false;
}

void IO::Device::SIM7000SocketFactory::ShutdownSend(Socket *socket)
{
}

Bool IO::Device::SIM7000SocketFactory::SocketGetReadBuff(Socket *socket, UInt32 *size)
{
	return false;
}

Bool IO::Device::SIM7000SocketFactory::DNSResolveIPDef(const Char *host, Net::SocketUtil::AddressInfo *addr)
{
	return this->modem->NetDNSResolveIP((const UTF8Char*)host, addr);
}

Bool IO::Device::SIM7000SocketFactory::GetDefDNS(Net::SocketUtil::AddressInfo *addr)
{
	Data::ArrayList<UInt32> dnsList;
	if (this->modem->NetGetDNSList(&dnsList))
	{
		Net::SocketUtil::SetAddrInfoV4(addr, dnsList.GetItem(0));
		return true;
	}
	return false;
}

UOSInt IO::Device::SIM7000SocketFactory::GetDNSList(Data::ArrayList<UInt32> *dnsList)
{
	UOSInt i = dnsList->GetCount();
	if (this->modem->NetGetDNSList(dnsList))
	{
		return dnsList->GetCount() - i;
	}
	return 0;
}

Bool IO::Device::SIM7000SocketFactory::LoadHosts(Net::DNSHandler *dnsHdlr)
{
	return true;
}

Bool IO::Device::SIM7000SocketFactory::ARPAddRecord(UOSInt ifIndex, const UInt8 *hwAddr, UInt32 ipv4)
{
	return false;
}

UOSInt IO::Device::SIM7000SocketFactory::GetConnInfoList(Data::ArrayList<Net::ConnectionInfo*> *connInfoList)
{
	Net::ConnectionInfo *connInfo;
	UTF8Char sbuff[64];
	if (this->modem->NetGetIFAddr(sbuff))
	{
		Net::ConnectionInfo::ConnectionEntry ent;
		ent.index = 0;
		ent.internalName = 0;
		ent.name = 0;
		ent.description = 0;
		ent.dnsSuffix = 0;
		NEW_CLASS(ent.ipaddr, Data::ArrayListUInt32());
		ent.ipaddr->Add(Net::SocketUtil::GetIPAddr(sbuff));
		NEW_CLASS(ent.dnsaddr, Data::ArrayListUInt32());
		ent.defGW = 0;
		ent.dhcpSvr = 0;
		ent.dhcpLeaseTime = 0;
		ent.dhcpLeaseExpire = 0;
		ent.physicalAddr = 0;
		ent.physicalAddrLeng = 0;
		ent.mtu = 256;
		ent.dhcpEnabled = false;
		ent.connType = Net::ConnectionInfo::CT_DIALUP;
		ent.connStatus = Net::ConnectionInfo::CS_UP;
		///////////////////////////////////////
		NEW_CLASS(connInfo, Net::ConnectionInfo(&ent));
		DEL_CLASS(ent.ipaddr);
		DEL_CLASS(ent.dnsaddr);
		connInfoList->Add(connInfo);
		return 1;
	}
	return 0;
}

Bool IO::Device::SIM7000SocketFactory::GetIPInfo(IPInfo *info)
{
	return false;
}

Bool IO::Device::SIM7000SocketFactory::GetTCPInfo(TCPInfo *info)
{
	return false;
}

Bool IO::Device::SIM7000SocketFactory::GetUDPInfo(UDPInfo *info)
{
	return false;
}

UOSInt IO::Device::SIM7000SocketFactory::QueryPortInfos(Data::ArrayList<PortInfo*> *portInfoList, ProtocolType protoType, UInt16 procId)
{
	UOSInt i = 8;
	if (protoType == PT_UDP)
	{
		while (i-- > 0)
		{
			if (this->status[i].state == SocketState::UDP_Opened)
			{
				/////////////////////////
			}
		}
	}
	else if (protoType == PT_TCP)
	{
		while (i-- > 0)
		{
			if (this->status[i].state == SocketState::TCP_Opened)
			{
				/////////////////////////
			}
		}
	}
	return 0;
}

void IO::Device::SIM7000SocketFactory::FreePortInfos(Data::ArrayList<PortInfo*> *portInfoList)
{

}

UOSInt IO::Device::SIM7000SocketFactory::QueryPortInfos2(Data::ArrayList<PortInfo2*> *portInfoList, ProtocolType protoType, UInt16 procId)
{
	UOSInt i = 8;
	if (protoType == PT_UDP)
	{
		while (i-- > 0)
		{
			if (this->status[i].state == SocketState::UDP_Opened)
			{
				/////////////////////////
			}
		}
	}
	else if (protoType == PT_TCP)
	{
		while (i-- > 0)
		{
			if (this->status[i].state == SocketState::TCP_Opened)
			{
				/////////////////////////
			}
		}
	}
	return 0;
}

void IO::Device::SIM7000SocketFactory::FreePortInfos2(Data::ArrayList<PortInfo2*> *portInfoList)
{

}
