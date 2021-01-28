//require ws2_32.lib iphlpapi.lib dnsapi.lib
#ifndef _SM_NET_OSSOCKETFACTORY
#define _SM_NET_OSSOCKETFACTORY
#include "Data/ArrayList.h"
#include "Net/SocketFactory.h"

namespace Net
{
	class OSSocketFactory : public Net::SocketFactory
	{
	private:
		void *icmpHand;
		Bool toRelease;

	public:
		OSSocketFactory(Bool noV6DNS);
		virtual ~OSSocketFactory();

		virtual UInt32 *CreateTCPSocketv4();
		virtual UInt32 *CreateTCPSocketv6();
		virtual UInt32 *CreateUDPSocketv4();
		virtual UInt32 *CreateUDPSocketv6();

		virtual UInt32 *CreateICMPIPv4Socket(UInt32 ip);
		virtual UInt32 *CreateUDPRAWv4Socket(UInt32 ip);
		virtual UInt32 *CreateRAWIPv4Socket(UInt32 ip);
		virtual UInt32 *CreateARPSocket();
		virtual UInt32 *CreateRAWSocket();

		virtual void DestroySocket(UInt32 *socket);
		virtual Bool SocketIsInvalid(UInt32 *socket);
		virtual Bool SocketBindv4(UInt32 *socket, UInt32 ip, UInt16 port);
		virtual Bool SocketBind(UInt32 *socket, const Net::SocketUtil::AddressInfo *addr, UInt16 port);
		virtual Bool SocketListen(UInt32 *socket);
		virtual UInt32 *SocketAccept(UInt32 *socket);
		virtual Int32 SocketGetLastError();
		virtual Bool GetRemoteAddr(UInt32 *socket, Net::SocketUtil::AddressInfo *addr, UInt16 *port);
		virtual Bool GetLocalAddr(UInt32 *socket, Net::SocketUtil::AddressInfo *addr, UInt16 *port);

		virtual void SetDontLinger(UInt32 *socket, Bool val);
		virtual void SetLinger(UInt32 *socket, Int32 ms);
		virtual void SetRecvBuffSize(UInt32 *socket, Int32 buffSize);
		virtual void SetNoDelay(UInt32 *socket, Bool val);
		virtual void SetRecvTimeout(UInt32 *socket, Int32 ms);
		virtual void SetReuseAddr(UInt32 *socket, Bool val);
		virtual void SetIPv4TTL(UInt32 *socket, Int32 ttl);
		virtual void SetBroadcast(UInt32 *socket, Bool val);
		virtual void AddIPMembership(UInt32 *socket, UInt32 ip);

		virtual OSInt SendData(UInt32 *socket, const UInt8 *buff, OSInt buffSize, ErrorType *et);
		virtual OSInt ReceiveData(UInt32 *socket, UInt8 *buff, OSInt buffSize, ErrorType *et);
		virtual void *BeginReceiveData(UInt32 *socket, UInt8 *buff, OSInt buffSize, Sync::Event *evt, ErrorType *et);
		virtual OSInt EndReceiveData(void *reqData, Bool toWait);
		virtual void CancelReceiveData(void *reqData);

		virtual UOSInt UDPReceive(UInt32 *socket, UInt8 *buff, UOSInt buffSize, Net::SocketUtil::AddressInfo *addr, UInt16 *port, ErrorType *et);
		virtual UOSInt SendTo(UInt32 *socket, const UInt8 *buff, UOSInt buffSize, const Net::SocketUtil::AddressInfo *addr, UInt16 port);
		virtual UOSInt SendToIF(UInt32 *socket, const UInt8 *buff, UOSInt buffSize, const UTF8Char *ifName);

		virtual Bool IcmpSendEcho2(const Net::SocketUtil::AddressInfo *addr, Int32 *respTime_us, Int32 *ttl);

		virtual Bool Connect(UInt32 *socket, UInt32 ip, UInt16 port);
		virtual Bool Connect(UInt32 *socket, const Net::SocketUtil::AddressInfo *addr, UInt16 port);
		virtual void ShutdownSend(UInt32 *socket);

		virtual Bool SocketGetReadBuff(UInt32 *socket, UInt32 *size);

		virtual Bool DNSResolveIPDef(const Char *host, Net::SocketUtil::AddressInfo *addr);
		virtual Bool GetDefDNS(Net::SocketUtil::AddressInfo *addr);
		virtual OSInt GetDNSList(Data::ArrayList<UInt32> *dnsList);
		virtual Bool LoadHosts(Net::DNSHandler *dnsHdlr);

		virtual Bool ARPAddRecord(OSInt ifIndex, const UInt8 *hwAddr, UInt32 ipv4);

		virtual OSInt GetConnInfoList(Data::ArrayList<Net::ConnectionInfo*> *connInfoList);
		virtual Bool GetIPInfo(IPInfo *info); //////////////////////////////////
		virtual Bool GetTCPInfo(TCPInfo *info); //////////////////////////////////
		virtual Bool GetUDPInfo(UDPInfo *info); //////////////////////////////////
		virtual OSInt QueryPortInfos(Data::ArrayList<PortInfo*> *portInfoList, ProtocolType protoType, UInt16 procId);
		virtual void FreePortInfos(Data::ArrayList<PortInfo*> *portInfoList);
		virtual OSInt QueryPortInfos2(Data::ArrayList<PortInfo2*> *portInfoList, ProtocolType protoType, UInt16 procId);
		virtual void FreePortInfos2(Data::ArrayList<PortInfo2*> *portInfoList);

		virtual Bool AdapterSetHWAddr(const UTF8Char *adapterName, const UInt8 *hwAddr);

		OSInt GetBroadcastAddrs(Data::ArrayList<UInt32> *addrs);

		static ErrorType FromSystemError(Int32 sysErr);
	};
}
#endif
