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
		struct ClassData;
	private:
		void *icmpHand;
		Bool toRelease;
		ClassData *clsData;

	public:
		OSSocketFactory(Bool noV6DNS);
		virtual ~OSSocketFactory();

		virtual Socket *CreateTCPSocketv4();
		virtual Socket *CreateTCPSocketv6();
		virtual Socket *CreateUDPSocketv4();
		virtual Socket *CreateUDPSocketv6();

		virtual Socket *CreateICMPIPv4Socket(UInt32 ip);
		virtual Socket *CreateUDPRAWv4Socket(UInt32 ip);
		virtual Socket *CreateRAWIPv4Socket(UInt32 ip);
		virtual Socket *CreateARPSocket();
		virtual Socket *CreateRAWSocket();

		virtual void DestroySocket(Socket *socket);
		virtual Bool SocketIsInvalid(Socket *socket);
		virtual Bool SocketBindv4(Socket *socket, UInt32 ip, UInt16 port);
		virtual Bool SocketBind(Socket *socket, const Net::SocketUtil::AddressInfo *addr, UInt16 port);
		virtual Bool SocketListen(Socket *socket);
		virtual Socket *SocketAccept(Socket *socket);
		virtual Int32 SocketGetLastError();
		virtual Bool GetRemoteAddr(Socket *socket, Net::SocketUtil::AddressInfo *addr, UInt16 *port);
		virtual Bool GetLocalAddr(Socket *socket, Net::SocketUtil::AddressInfo *addr, UInt16 *port);
		virtual Int32 SocketGetFD(Socket *socket);

		virtual void SetDontLinger(Socket *socket, Bool val);
		virtual void SetLinger(Socket *socket, UInt32 ms);
		virtual void SetRecvBuffSize(Socket *socket, Int32 buffSize);
		virtual void SetNoDelay(Socket *socket, Bool val);
		virtual void SetRecvTimeout(Socket *socket, Int32 ms);
		virtual void SetReuseAddr(Socket *socket, Bool val);
		virtual void SetIPv4TTL(Socket *socket, Int32 ttl);
		virtual void SetBroadcast(Socket *socket, Bool val);
		virtual void AddIPMembership(Socket *socket, UInt32 ip);

		virtual UOSInt SendData(Socket *socket, const UInt8 *buff, UOSInt buffSize, ErrorType *et);
		virtual UOSInt ReceiveData(Socket *socket, UInt8 *buff, UOSInt buffSize, ErrorType *et);
		virtual void *BeginReceiveData(Socket *socket, UInt8 *buff, UOSInt buffSize, Sync::Event *evt, ErrorType *et);
		virtual UOSInt EndReceiveData(void *reqData, Bool toWait, Bool *incomplete);
		virtual void CancelReceiveData(void *reqData);

		virtual UOSInt UDPReceive(Socket *socket, UInt8 *buff, UOSInt buffSize, Net::SocketUtil::AddressInfo *addr, UInt16 *port, ErrorType *et);
		virtual UOSInt SendTo(Socket *socket, const UInt8 *buff, UOSInt buffSize, const Net::SocketUtil::AddressInfo *addr, UInt16 port);
		virtual UOSInt SendToIF(Socket *socket, const UInt8 *buff, UOSInt buffSize, const UTF8Char *ifName);

		virtual Bool IcmpSendEcho2(const Net::SocketUtil::AddressInfo *addr, UInt32 *respTime_us, UInt32 *ttl);

		virtual Bool Connect(Socket *socket, UInt32 ip, UInt16 port);
		virtual Bool Connect(Socket *socket, const Net::SocketUtil::AddressInfo *addr, UInt16 port);
		virtual void ShutdownSend(Socket *socket);

		virtual Bool SocketGetReadBuff(Socket *socket, UInt32 *size);

		virtual Bool DNSResolveIPDef(const Char *host, Net::SocketUtil::AddressInfo *addr);
		virtual Bool GetDefDNS(Net::SocketUtil::AddressInfo *addr);
		virtual UOSInt GetDNSList(Data::ArrayList<UInt32> *dnsList);
		virtual Bool LoadHosts(Net::DNSHandler *dnsHdlr);

		virtual Bool ARPAddRecord(UOSInt ifIndex, const UInt8 *hwAddr, UInt32 ipv4);

		virtual UOSInt GetConnInfoList(Data::ArrayList<Net::ConnectionInfo*> *connInfoList);
		virtual Bool GetIPInfo(IPInfo *info); //////////////////////////////////
		virtual Bool GetTCPInfo(TCPInfo *info); //////////////////////////////////
		virtual Bool GetUDPInfo(UDPInfo *info); //////////////////////////////////
		virtual UOSInt QueryPortInfos(Data::ArrayList<PortInfo*> *portInfoList, ProtocolType protoType, UInt16 procId);
		virtual void FreePortInfos(Data::ArrayList<PortInfo*> *portInfoList);
		virtual UOSInt QueryPortInfos2(Data::ArrayList<PortInfo2*> *portInfoList, ProtocolType protoType, UInt16 procId);
		virtual void FreePortInfos2(Data::ArrayList<PortInfo2*> *portInfoList);

		virtual Bool AdapterSetHWAddr(const UTF8Char *adapterName, const UInt8 *hwAddr);

		UOSInt GetBroadcastAddrs(Data::ArrayList<UInt32> *addrs);

		static ErrorType FromSystemError(Int32 sysErr);
	};
}
#endif
