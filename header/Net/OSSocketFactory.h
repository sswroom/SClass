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
		NN<ClassData> clsData;

		static OSInt SSocketGetFD(NN<Socket> socket);
		static Bool MyConnect(NN<Socket> socket, UnsafeArray<const UInt8> addrBuff, UOSInt addrLen, Data::Duration timeout);
	public:
		OSSocketFactory(Bool noV6DNS);
		virtual ~OSSocketFactory();

		virtual Optional<Socket> CreateTCPSocketv4();
		virtual Optional<Socket> CreateTCPSocketv6();
		virtual Optional<Socket> CreateUDPSocketv4();
		virtual Optional<Socket> CreateUDPSocketv6();

		virtual Optional<Socket> CreateICMPIPv4Socket(UInt32 ip);
		virtual Optional<Socket> CreateUDPRAWv4Socket(UInt32 ip);
		virtual Optional<Socket> CreateRAWIPv4Socket(UInt32 ip);
		virtual Optional<Socket> CreateARPSocket();
		virtual Optional<Socket> CreateRAWSocket();

		virtual void DestroySocket(NN<Socket> socket);
		virtual Bool SocketBindv4(NN<Socket> socket, UInt32 ip, UInt16 port);
		virtual Bool SocketBind(NN<Socket> socket, Optional<const Net::SocketUtil::AddressInfo> addr, UInt16 port);
		virtual Bool SocketBindRAWIf(NN<Socket> socket, UOSInt ifIndex);
		virtual Bool SocketListen(NN<Socket> socket);
		virtual Optional<Socket> SocketAccept(NN<Socket> socket);
		virtual Int32 SocketGetLastError();
		virtual Bool GetRemoteAddr(NN<Socket> socket, NN<Net::SocketUtil::AddressInfo> addr, OptOut<UInt16> port);
		virtual Bool GetLocalAddr(NN<Socket> socket, NN<Net::SocketUtil::AddressInfo> addr, OptOut<UInt16> port);
		virtual OSInt SocketGetFD(NN<Socket> socket);
		virtual Bool SocketWait(NN<Socket> socket, Data::Duration dur);

		virtual void SetDontLinger(NN<Socket> socket, Bool val);
		virtual void SetLinger(NN<Socket> socket, UInt32 ms);
		virtual void SetRecvBuffSize(NN<Socket> socket, Int32 buffSize);
		virtual void SetNoDelay(NN<Socket> socket, Bool val);
		virtual void SetSendTimeout(NN<Socket> socket, Data::Duration timeout);
		virtual void SetRecvTimeout(NN<Socket> socket, Data::Duration timeout);
		virtual void SetReuseAddr(NN<Socket> socket, Bool val);
		virtual void SetIPv4TTL(NN<Socket> socket, Int32 ttl);
		virtual void SetBroadcast(NN<Socket> socket, Bool val);
		virtual void AddIPMembership(NN<Socket> socket, UInt32 ip);

		virtual UOSInt SendData(NN<Socket> socket, UnsafeArray<const UInt8> buff, UOSInt buffSize, OptOut<ErrorType> et);
		virtual UOSInt ReceiveData(NN<Socket> socket, UnsafeArray<UInt8> buff, UOSInt buffSize, OptOut<ErrorType> et);
		virtual Optional<SocketRecvSess> BeginReceiveData(NN<Socket> socket, UnsafeArray<UInt8> buff, UOSInt buffSize, NN<Sync::Event> evt, OptOut<ErrorType> et);
		virtual UOSInt EndReceiveData(NN<SocketRecvSess> reqData, Bool toWait, OutParam<Bool> incomplete);
		virtual void CancelReceiveData(NN<SocketRecvSess> reqData);

		virtual UOSInt UDPReceive(NN<Socket> socket, UnsafeArray<UInt8> buff, UOSInt buffSize, NN<Net::SocketUtil::AddressInfo> addr, OutParam<UInt16> port, OptOut<ErrorType> et);
		virtual UOSInt SendTo(NN<Socket> socket, UnsafeArray<const UInt8> buff, UOSInt buffSize, NN<const Net::SocketUtil::AddressInfo> addr, UInt16 port);
		virtual UOSInt SendToIF(NN<Socket> socket, UnsafeArray<const UInt8> buff, UOSInt buffSize, UnsafeArray<const UTF8Char> ifName);

		virtual Bool IcmpSendEcho2(NN<const Net::SocketUtil::AddressInfo> addr, OutParam<UInt32> respTime_us, OutParam<UInt32> ttl);

		virtual Bool Connect(NN<Socket> socket, UInt32 ip, UInt16 port, Data::Duration timeout);
		virtual Bool Connect(NN<Socket> socket, NN<const Net::SocketUtil::AddressInfo> addr, UInt16 port, Data::Duration timeout);
		virtual void ShutdownSend(NN<Socket> socket);
		virtual void ShutdownSocket(NN<Socket> socket);

		virtual Bool SocketGetReadBuff(NN<Socket> socket, OutParam<UInt32> size);

		virtual Bool DNSResolveIPDef(UnsafeArray<const Char> host, NN<Net::SocketUtil::AddressInfo> addr);
		virtual Bool GetDefDNS(NN<Net::SocketUtil::AddressInfo> addr);
		virtual UOSInt GetDNSList(NN<Data::ArrayList<UInt32>> dnsList);
		virtual Bool LoadHosts(NN<Net::DNSHandler> dnsHdlr);

		virtual Bool ARPAddRecord(UOSInt ifIndex, UnsafeArray<const UInt8> hwAddr, UInt32 ipv4);

		virtual UOSInt GetConnInfoList(NN<Data::ArrayListNN<Net::ConnectionInfo>> connInfoList);
		virtual Bool GetIPInfo(NN<IPInfo> info); //////////////////////////////////
		virtual Bool GetTCPInfo(NN<TCPInfo> info); //////////////////////////////////
		virtual Bool GetUDPInfo(NN<UDPInfo> info); //////////////////////////////////
		virtual UOSInt QueryPortInfos(NN<Data::ArrayListNN<PortInfo>> portInfoList, ProtocolType protoType, UInt16 procId);
		virtual void FreePortInfos(NN<Data::ArrayListNN<PortInfo>> portInfoList);
		virtual UOSInt QueryPortInfos2(NN<Data::ArrayListNN<PortInfo3>> portInfoList, ProtocolType protoType, UInt16 procId);
		virtual void FreePortInfos2(NN<Data::ArrayListNN<PortInfo3>> portInfoList);

		virtual Bool AdapterSetHWAddr(Text::CStringNN adapterName, UnsafeArray<const UInt8> hwAddr);
		virtual Bool AdapterEnable(Text::CStringNN adapterName, Bool enable);

		UOSInt GetBroadcastAddrs(NN<Data::ArrayList<UInt32>> addrs);

		static ErrorType FromSystemError(Int32 sysErr);
	};
}
#endif
