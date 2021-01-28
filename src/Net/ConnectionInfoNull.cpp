#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/FileStream.h"
#include "Net/ConnectionInfo.h"
#include "Net/SocketFactory.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/UTF8Reader.h"

typedef struct
{
	int sock;
	const Char *name;
} ConnectionData;

Net::ConnectionInfo::ConnectionInfo(void *info)
{
}

Net::ConnectionInfo::~ConnectionInfo()
{
}

Bool Net::ConnectionInfo::SetInfo(void *info)
{
	return false;
}

UTF8Char *Net::ConnectionInfo::GetName(UTF8Char *buff)
{
	return 0;
}

UTF8Char *Net::ConnectionInfo::GetDescription(UTF8Char *buff)
{
	return 0;
}

UTF8Char *Net::ConnectionInfo::GetDNSSuffix(UTF8Char *buff)
{
	return 0;
}

UInt32 Net::ConnectionInfo::GetIPAddress(OSInt index)
{
	return 0;
}

UInt32 Net::ConnectionInfo::GetDNSAddress(OSInt index)
{
	return 0;
}

UInt32 Net::ConnectionInfo::GetDefaultGW()
{
	return this->defGW;
}

OSInt Net::ConnectionInfo::GetPhysicalAddress(UInt8 *buff, OSInt buffSize)
{
	return 0;
}

UInt32 Net::ConnectionInfo::GetMTU()
{
	return this->mtu;
}

Bool Net::ConnectionInfo::IsDhcpEnabled()
{
	return this->dhcpEnabled;
}

UInt32 Net::ConnectionInfo::GetDhcpServer()
{
	return this->dhcpSvr;
}

Data::DateTime *Net::ConnectionInfo::GetDhcpLeaseTime()
{
	return this->dhcpLeaseTime;
}

Data::DateTime *Net::ConnectionInfo::GetDhcpLeaseExpire()
{
	return this->dhcpLeaseExpire;
}

Net::ConnectionInfo::ConnectionType Net::ConnectionInfo::GetConnectionType()
{
	return this->connType;
}

Net::ConnectionInfo::ConnectionStatus Net::ConnectionInfo::GetConnectionStatus()
{
	return this->connStatus;
}

OSInt Net::ConnectionInfo::GetConnInfoList(Data::ArrayList<Net::ConnectionInfo*> *connInfoList)
{
	return 0;
}
/*
Bool Net::ConnectionInfo::GetIPInfo(IPInfo *info)
{
	return GetIpStatisticsEx((MIB_IPSTATS*)info, AF_INET) == NO_ERROR;
}

Bool Net::ConnectionInfo::GetTCPInfo(TCPInfo *info)
{
	return GetTcpStatisticsEx((MIB_TCPSTATS*)info, AF_INET) == NO_ERROR;
}

Bool Net::ConnectionInfo::GetUDPInfo(UDPInfo *info)
{
	return GetUdpStatisticsEx((MIB_UDPSTATS*)info, AF_INET) == NO_ERROR;
}

OSInt Net::ConnectionInfo::QueryPortInfos(Data::ArrayList<Net::ConnectionInfo::PortInfo*> *portInfoList, ProtocolType protoType, UInt16 procId)
{
	OSInt retCnt = 0;
	UInt32 ret;
	if (protoType & Net::ConnectionInfo::PT_TCP)
	{
		Net::ConnectionInfo::PortInfo *port;
		MIB_TCPTABLE *tcpTable;
		UInt32 dwSize = 0;
		ret = GetTcpTable(0, (ULONG*)&dwSize, TRUE);
		if (ret == ERROR_INSUFFICIENT_BUFFER)
		{
			tcpTable = (MIB_TCPTABLE*)MAlloc(dwSize);
			ret = GetTcpTable(tcpTable, (ULONG*)&dwSize, TRUE);

			if (ret == NO_ERROR)
			{
				dwSize = 0;
				while (dwSize < tcpTable->dwNumEntries)
				{
					port = MemAlloc(Net::ConnectionInfo::PortInfo, 1);
					port->protoType = Net::ConnectionInfo::PT_TCP;
					port->localIP = tcpTable->table[dwSize].dwLocalAddr;
					port->localPort = tcpTable->table[dwSize].dwLocalPort;
					port->foreignIP = tcpTable->table[dwSize].dwRemoteAddr;
					port->foreignPort = tcpTable->table[dwSize].dwRemotePort;
					switch (tcpTable->table[dwSize].dwState)
					{
					case MIB_TCP_STATE_CLOSED:
						port->portState = Net::ConnectionInfo::PS_CLOSED;
						break;
					case MIB_TCP_STATE_LISTEN:
						port->portState = Net::ConnectionInfo::PS_LISTEN;
						break;
					case MIB_TCP_STATE_SYN_SENT:
						port->portState = Net::ConnectionInfo::PS_SYN_SENT;
						break;
					case MIB_TCP_STATE_SYN_RCVD:
						port->portState = Net::ConnectionInfo::PS_SYN_RCVD;
						break;
					case MIB_TCP_STATE_ESTAB:
						port->portState = Net::ConnectionInfo::PS_ESTAB;
						break;
					case MIB_TCP_STATE_FIN_WAIT1:
						port->portState = Net::ConnectionInfo::PS_FIN_WAIT1;
						break;
					case MIB_TCP_STATE_FIN_WAIT2:
						port->portState = Net::ConnectionInfo::PS_FIN_WAIT2;
						break;
					case MIB_TCP_STATE_CLOSE_WAIT:
						port->portState = Net::ConnectionInfo::PS_CLOSE_WAIT;
						break;
					case MIB_TCP_STATE_CLOSING:
						port->portState = Net::ConnectionInfo::PS_CLOSING;
						break;
					case MIB_TCP_STATE_LAST_ACK:
						port->portState = Net::ConnectionInfo::PS_LAST_ACK;
						break;
					case MIB_TCP_STATE_TIME_WAIT:
						port->portState = Net::ConnectionInfo::PS_TIME_WAIT;
						break;
					case MIB_TCP_STATE_DELETE_TCB:
						port->portState = Net::ConnectionInfo::PS_DELETE_TCB;
						break;
					default:
						port->portState = Net::ConnectionInfo::PS_UNKNOWN;
						break;
					}
					port->processId = 0;
					portInfoList->Add(port);
					dwSize++;
				}
				retCnt += tcpTable->dwNumEntries;
			}
			MemFree(tcpTable);
		}
	}
	if (protoType & Net::ConnectionInfo::PT_UDP)
	{
		Net::ConnectionInfo::PortInfo *port;
		MIB_UDPTABLE *udpTable;
		UInt32 dwSize = 0;
		ret = GetUdpTable(0, (ULONG*)&dwSize, TRUE);
		if (ret == ERROR_INSUFFICIENT_BUFFER)
		{
			udpTable = (MIB_UDPTABLE*)MAlloc(dwSize);
			ret = GetUdpTable(udpTable, (ULONG*)&dwSize, TRUE);

			if (ret == NO_ERROR)
			{
				dwSize = 0;
				while (dwSize < udpTable->dwNumEntries)
				{
					port = MemAlloc(Net::ConnectionInfo::PortInfo, 1);
					port->protoType = Net::ConnectionInfo::PT_UDP;
					port->localIP = udpTable->table[dwSize].dwLocalAddr;
					port->localPort = udpTable->table[dwSize].dwLocalPort;
					port->foreignIP = 0;
					port->foreignPort = 0;
					port->portState = Net::ConnectionInfo::PS_UNKNOWN;
					port->processId = 0;
					portInfoList->Add(port);
					dwSize++;
				}
				retCnt += udpTable->dwNumEntries;
			}
			MemFree(udpTable);
		}
	}
	return retCnt;
}

void Net::ConnectionInfo::FreePortInfos(Data::ArrayList<Net::ConnectionInfo::PortInfo*> *portInfoList)
{
	OSInt i = portInfoList->GetCount();
	while (i-- > 0)
	{
		MemFree(portInfoList->RemoveAt(i));
	}
}
*/
