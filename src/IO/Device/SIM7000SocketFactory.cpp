#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/Device/SIM7000SocketFactory.h"
#include "Net/ConnectionInfo.h"
#include "Sync/MutexUsage.h"
#include "Sync/ThreadUtil.h"
#include "Text/MyString.h"
#include <stdio.h>

void __stdcall IO::Device::SIM7000SocketFactory::OnReceiveData(AnyType userObj, UOSInt index, UInt32 remoteIP, UInt16 remotePort, const UInt8 *buff, UOSInt buffSize)
{
	NN<IO::Device::SIM7000SocketFactory> me = userObj.GetNN<IO::Device::SIM7000SocketFactory>();
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
		me->status[index].dataList.Put(packet);
		me->status[index].dataEvt.Set();
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
				while ((packet = (DataPacket*)this->status[i].dataList.Get()) != 0)
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
	}
	this->Init();
}

IO::Device::SIM7000SocketFactory::~SIM7000SocketFactory()
{
	this->CloseAllSockets();
	this->modem->SetReceiveHandler(0, 0);
	SDEL_STRING(this->apn);
	if (this->needRelease)
	{
		DEL_CLASS(this->modem);
	}
}

void IO::Device::SIM7000SocketFactory::SetAPN(Text::CString apn)
{
	SDEL_STRING(this->apn);
	if (apn.v)
	{
		this->apn = Text::String::New(apn).Ptr();
	}
}

Text::String *IO::Device::SIM7000SocketFactory::GetAPN()
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
	UTF8Char *sptr;
	Bool bVal;
	modem->GPRSEPSReg();
	modem->GPRSNetworkReg();
	if (modem->GPRSServiceIsAttached(bVal))
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
		if (!modem->NetSetAPN(this->apn->ToCString()))
		{
//			return false;			
		}
	}
	else
	{
		if ((sptr = modem->SIMCOMGetNetworkAPN(sbuff)) == 0)
		{
			return false;
		}

		if (!modem->NetSetAPN(CSTRP(sbuff, sptr)))
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

Optional<Socket> IO::Device::SIM7000SocketFactory::CreateTCPSocketv4()
{
	return 0;
}

Optional<Socket> IO::Device::SIM7000SocketFactory::CreateTCPSocketv6()
{
	return 0;
}

Optional<Socket> IO::Device::SIM7000SocketFactory::CreateUDPSocketv4()
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

Optional<Socket> IO::Device::SIM7000SocketFactory::CreateUDPSocketv6()
{
	return 0;
}

Optional<Socket> IO::Device::SIM7000SocketFactory::CreateICMPIPv4Socket(UInt32 ip)
{
	return 0;
}

Optional<Socket> IO::Device::SIM7000SocketFactory::CreateUDPRAWv4Socket(UInt32 ip)
{
	return 0;
}

Optional<Socket> IO::Device::SIM7000SocketFactory::CreateRAWIPv4Socket(UInt32 ip)
{
	return 0;
}

Optional<Socket> IO::Device::SIM7000SocketFactory::CreateARPSocket()
{
	return 0;
}

Optional<Socket> IO::Device::SIM7000SocketFactory::CreateRAWSocket()
{
	return 0;
}

void IO::Device::SIM7000SocketFactory::DestroySocket(NN<Socket> socket)
{
	UOSInt i = (UOSInt)socket.Ptr() - 1;
	DataPacket *packet;
	if (i >= 8)
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
			while ((packet = (DataPacket*)this->status[i].dataList.Get()) != 0)
			{
				MemFree(packet);
			}
			mutUsage.EndUse();
		}
		break;
	}
}

Bool IO::Device::SIM7000SocketFactory::SocketBindv4(NN<Socket> socket, UInt32 ip, UInt16 port)
{
	UOSInt i = (UOSInt)socket.Ptr() - 1;
	if (i >= 8)
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

Bool IO::Device::SIM7000SocketFactory::SocketBind(NN<Socket> socket, const Net::SocketUtil::AddressInfo *addr, UInt16 port)
{
	return false;
}

Bool IO::Device::SIM7000SocketFactory::SocketBindRAWIf(NN<Socket> socket, UOSInt ifIndex)
{
	return false;
}

Bool IO::Device::SIM7000SocketFactory::SocketListen(NN<Socket> socket)
{
	return false;
}

Optional<Socket> IO::Device::SIM7000SocketFactory::SocketAccept(NN<Socket> socket)
{
	return 0;
}

Int32 IO::Device::SIM7000SocketFactory::SocketGetLastError()
{
	return 0;
}

Bool IO::Device::SIM7000SocketFactory::GetRemoteAddr(NN<Socket> socket, NN<Net::SocketUtil::AddressInfo> addr, OptOut<UInt16> port)
{
	return false;
}

Bool IO::Device::SIM7000SocketFactory::GetLocalAddr(NN<Socket> socket, NN<Net::SocketUtil::AddressInfo> addr, OptOut<UInt16> port)
{
	return false;
}

OSInt IO::Device::SIM7000SocketFactory::SocketGetFD(NN<Socket> socket)
{
	return false;
}

Bool IO::Device::SIM7000SocketFactory::SocketWait(NN<Socket> socket, Data::Duration dur)
{
	return false;
}

void IO::Device::SIM7000SocketFactory::SetDontLinger(NN<Socket> socket, Bool val)
{

}

void IO::Device::SIM7000SocketFactory::SetLinger(NN<Socket> socket, UInt32 ms)
{

}

void IO::Device::SIM7000SocketFactory::SetRecvBuffSize(NN<Socket> socket, Int32 buffSize)
{

}

void IO::Device::SIM7000SocketFactory::SetNoDelay(NN<Socket> socket, Bool val)
{
	this->modem->NetSetDisableNagle(val);
}

void IO::Device::SIM7000SocketFactory::SetRecvTimeout(NN<Socket> socket, Data::Duration timeout)
{

}

void IO::Device::SIM7000SocketFactory::SetReuseAddr(NN<Socket> socket, Bool val)
{

}

void IO::Device::SIM7000SocketFactory::SetIPv4TTL(NN<Socket> socket, Int32 ttl)
{
}

void IO::Device::SIM7000SocketFactory::SetBroadcast(NN<Socket> socket, Bool val)
{
}

void IO::Device::SIM7000SocketFactory::AddIPMembership(NN<Socket> socket, UInt32 ip)
{

}

UOSInt IO::Device::SIM7000SocketFactory::SendData(NN<Socket> socket, const UInt8 *buff, UOSInt buffSize, OptOut<ErrorType> et)
{
	return 0;
}

UOSInt IO::Device::SIM7000SocketFactory::ReceiveData(NN<Socket> socket, UInt8 *buff, UOSInt buffSize, OptOut<ErrorType> et)
{
	return 0;
}

void *IO::Device::SIM7000SocketFactory::BeginReceiveData(NN<Socket> socket, UInt8 *buff, UOSInt buffSize, Sync::Event *evt, OptOut<ErrorType> et)
{
	return 0;
}

UOSInt IO::Device::SIM7000SocketFactory::EndReceiveData(void *reqData, Bool toWait, OutParam<Bool> incomplete)
{
	incomplete.Set(false);
	return 0;
}

void IO::Device::SIM7000SocketFactory::CancelReceiveData(void *reqData)
{

}

UOSInt IO::Device::SIM7000SocketFactory::UDPReceive(NN<Socket> socket, UInt8 *buff, UOSInt buffSize, NN<Net::SocketUtil::AddressInfo> addr, OutParam<UInt16> port, OptOut<ErrorType> et)
{
	OSInt i = -1 + (OSInt)socket.Ptr();
	if (i < 0 || i >= 8)
	{
		return false;
	}
	DataPacket *packet;
	while (this->status[i].state == SocketState::UDP_Opened)
	{
		if (this->status[i].dataList.GetCount() > 0)
		{
			Sync::MutexUsage mutUsage(this->status[i].dataMut);
			packet = (DataPacket*)this->status[i].dataList.Get();
			mutUsage.EndUse();
			if (packet)
			{
				Net::SocketUtil::SetAddrInfoV4(addr, packet->remoteIP);
				port.Set(packet->remotePort);
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
		this->status[i].dataEvt.Wait(10000);
	}
	et.Set(Net::SocketFactory::ET_DISCONNECT);
	return 0;
}

UOSInt IO::Device::SIM7000SocketFactory::SendTo(NN<Socket> socket, const UInt8 *buff, UOSInt buffSize, NN<const Net::SocketUtil::AddressInfo> addr, UInt16 port)
{
	UOSInt i = (UOSInt)socket.Ptr() - 1;
	if (i >= 8)
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

UOSInt IO::Device::SIM7000SocketFactory::SendToIF(NN<Socket> socket, const UInt8 *buff, UOSInt buffSize, const UTF8Char *ifName)
{
	return 0;
}

Bool IO::Device::SIM7000SocketFactory::IcmpSendEcho2(NN<const Net::SocketUtil::AddressInfo> addr, UInt32 *respTime_us, UInt32 *ttl)
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

Bool IO::Device::SIM7000SocketFactory::Connect(NN<Socket> socket, UInt32 ip, UInt16 port, Data::Duration timeout)
{
	return false;
}

Bool IO::Device::SIM7000SocketFactory::Connect(NN<Socket> socket, NN<const Net::SocketUtil::AddressInfo> addr, UInt16 port, Data::Duration timeout)
{
	return false;
}

void IO::Device::SIM7000SocketFactory::ShutdownSend(NN<Socket> socket)
{
}

void IO::Device::SIM7000SocketFactory::ShutdownSocket(NN<Socket> socket)
{
}

Bool IO::Device::SIM7000SocketFactory::SocketGetReadBuff(NN<Socket> socket, UInt32 *size)
{
	return false;
}

Bool IO::Device::SIM7000SocketFactory::DNSResolveIPDef(const Char *host, NN<Net::SocketUtil::AddressInfo> addr)
{
	return this->modem->NetDNSResolveIP(Text::CString::FromPtr((const UTF8Char*)host), addr);
}

Bool IO::Device::SIM7000SocketFactory::GetDefDNS(NN<Net::SocketUtil::AddressInfo> addr)
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

Bool IO::Device::SIM7000SocketFactory::LoadHosts(NN<Net::DNSHandler> dnsHdlr)
{
	return true;
}

Bool IO::Device::SIM7000SocketFactory::ARPAddRecord(UOSInt ifIndex, const UInt8 *hwAddr, UInt32 ipv4)
{
	return false;
}

UOSInt IO::Device::SIM7000SocketFactory::GetConnInfoList(NN<Data::ArrayListNN<Net::ConnectionInfo>> connInfoList)
{
	NN<Net::ConnectionInfo> connInfo;
	UTF8Char sbuff[64];
	UTF8Char *sptr;
	if ((sptr = this->modem->NetGetIFAddr(sbuff)) != 0)
	{
		Net::ConnectionInfo::ConnectionEntry ent;
		ent.index = 0;
		ent.internalName = 0;
		ent.name = 0;
		ent.description = 0;
		ent.dnsSuffix = 0;
		ent.ipaddr.Add(Net::SocketUtil::GetIPAddr(CSTRP(sbuff, sptr)));
		ent.defGW = 0;
		ent.dhcpSvr = 0;
		ent.physicalAddr = 0;
		ent.physicalAddrLeng = 0;
		ent.mtu = 256;
		ent.dhcpEnabled = false;
		ent.connType = Net::ConnectionInfo::ConnectionType::DialUp;
		ent.connStatus = Net::ConnectionInfo::CS_UP;
		///////////////////////////////////////
		NEW_CLASSNN(connInfo, Net::ConnectionInfo(&ent));
		connInfoList->Add(connInfo);
		return 1;
	}
	return 0;
}

Bool IO::Device::SIM7000SocketFactory::GetIPInfo(NN<IPInfo> info)
{
	return false;
}

Bool IO::Device::SIM7000SocketFactory::GetTCPInfo(NN<TCPInfo> info)
{
	return false;
}

Bool IO::Device::SIM7000SocketFactory::GetUDPInfo(NN<UDPInfo> info)
{
	return false;
}

UOSInt IO::Device::SIM7000SocketFactory::QueryPortInfos(NN<Data::ArrayListNN<PortInfo>> portInfoList, ProtocolType protoType, UInt16 procId)
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

void IO::Device::SIM7000SocketFactory::FreePortInfos(NN<Data::ArrayListNN<PortInfo>> portInfoList)
{

}

UOSInt IO::Device::SIM7000SocketFactory::QueryPortInfos2(NN<Data::ArrayListNN<PortInfo3>> portInfoList, ProtocolType protoType, UInt16 procId)
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

void IO::Device::SIM7000SocketFactory::FreePortInfos2(NN<Data::ArrayListNN<PortInfo3>> portInfoList)
{

}
