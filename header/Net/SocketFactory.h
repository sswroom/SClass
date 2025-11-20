#ifndef _SM_NET_SOCKETFACTORY
#define _SM_NET_SOCKETFACTORY
#include "Data/ArrayList.hpp"
#include "Data/DataArray.hpp"
#include "Data/Duration.h"
#include "IO/LogTool.h"
#include "Net/SocketUtil.h"
#include "Sync/Event.h"
#include "Sync/Mutex.h"
#include "Text/CString.h"

struct Socket;

namespace Net
{
	class ConnectionInfo;
	class DNSHandler;
	class SocketRecvSess;

	class SocketFactory
	{
	public:
		typedef enum
		{
			PT_TCP = 1,
			PT_UDP = 2,
			PT_RAW = 4,
			PT_TCP6 = 8,
			PT_UDP6 = 16,
			PT_RAW6 = 32,
			PT_ALL = 63
		} ProtocolType;

		typedef enum
		{
			PS_UNKNOWN,
			PS_CLOSED,
			PS_LISTEN,
			PS_SYN_SENT,
			PS_SYN_RCVD,
			PS_ESTAB,
			PS_FIN_WAIT1,
			PS_FIN_WAIT2,
			PS_CLOSE_WAIT,
			PS_CLOSING,
			PS_LAST_ACK,
			PS_TIME_WAIT,
			PS_DELETE_TCB
		} PortState;

		typedef struct
		{
			UInt32 dwForwarding;
			UInt32 dwDefaultTTL;
			UInt32 dwInReceives;
			UInt32 dwInHdrErrors;
			UInt32 dwInAddrErrors;
			UInt32 dwForwDatagrams;
			UInt32 dwInUnknownProtos;
			UInt32 dwInDiscards;
			UInt32 dwInDelivers;
			UInt32 dwOutRequests;
			UInt32 dwRoutingDiscards;
			UInt32 dwOutDiscards;
			UInt32 dwOutNoRoutes;
			UInt32 dwReasmTimeout;
			UInt32 dwReasmReqds;
			UInt32 dwReasmOks;
			UInt32 dwReasmFails;
			UInt32 dwFragOks;
			UInt32 dwFragFails;
			UInt32 dwFragCreates;
			UInt32 dwNumIf;
			UInt32 dwNumAddr;
			UInt32 dwNumRoutes;
		} IPInfo;

		typedef enum
		{
			RTO_OTHER = 1,
			RTO_CONSTANT = 2,
			RTO_RSRE = 3,
			RTO_VANJ = 4
		} RTOAlgorithm;

		typedef struct
		{
			UInt32 dwRtoAlgorithm;
			UInt32 dwRtoMin;
			UInt32 dwRtoMax;
			Int32 dwMaxConn;
			UInt32 dwActiveOpens;
			UInt32 dwPassiveOpens;
			UInt32 dwAttemptFails;
			UInt32 dwEstabResets;
			UInt32 dwCurrEstab;
			UInt32 dwInSegs;
			UInt32 dwOutSegs;
			UInt32 dwRetransSegs;
			UInt32 dwInErrs;
			UInt32 dwOutRsts;
			UInt32 dwNumConns;
		} TCPInfo;

		typedef struct
		{
			UInt32 dwInDatagrams;
			UInt32 dwNoPorts;
			UInt32 dwInErrors;
			UInt32 dwOutDatagrams;
			UInt32 dwNumAddrs;
		} UDPInfo;

		typedef struct
		{
			ProtocolType protoType;
			UInt32 localIP;
			UInt32 localPort;
			UInt32 foreignIP;
			UInt32 foreignPort;
			PortState portState;
			UInt16 processId;
		} PortInfo;

		typedef struct
		{
			ProtocolType protoType;
			Net::SocketUtil::AddressInfo localAddr;
			UInt32 localPort;
			Net::SocketUtil::AddressInfo foreignAddr;
			UInt32 foreignPort;
			PortState portState;
			UInt16 processId;
			UInt32 socketId;
		} PortInfo3;

		typedef enum
		{
			ET_UNKNOWN,
			ET_CONN_RESET,
			ET_CONN_ABORT,
			ET_DISCONNECT,
			ET_SHUTDOWN,
			ET_NO_ERROR = 1024
		} ErrorType;
	private:
		Sync::Mutex dnsMut;
		IO::LogTool log;
		Bool noV6DNS;
		Optional<Text::String> forceDNS;
	protected:
		Optional<Net::DNSHandler> dnsHdlr;
		Bool GetEffectiveDNS(NN<Net::SocketUtil::AddressInfo> addr);
	public:
		SocketFactory(Bool noV6DNS);
		virtual ~SocketFactory();

		virtual Optional<Socket> CreateTCPSocketv4() = 0;
		virtual Optional<Socket> CreateTCPSocketv6() = 0;
		virtual Optional<Socket> CreateUDPSocketv4() = 0;
		virtual Optional<Socket> CreateUDPSocketv6() = 0;

		virtual Optional<Socket> CreateICMPIPv4Socket(UInt32 ip) = 0;
		virtual Optional<Socket> CreateUDPRAWv4Socket(UInt32 ip) = 0;
		virtual Optional<Socket> CreateRAWIPv4Socket(UInt32 ip) = 0;
		virtual Optional<Socket> CreateARPSocket() = 0;
		virtual Optional<Socket> CreateRAWSocket() = 0;

		virtual void DestroySocket(NN<Socket> socket) = 0;
		virtual Bool SocketBindv4(NN<Socket> socket, UInt32 ip, UInt16 port) = 0;
		virtual Bool SocketBind(NN<Socket> socket, Optional<const Net::SocketUtil::AddressInfo> addr, UInt16 port) = 0;
		virtual Bool SocketBindRAWIf(NN<Socket> socket, UOSInt ifIndex) = 0;
		virtual Bool SocketListen(NN<Socket> socket) = 0;
		virtual Optional<Socket> SocketAccept(NN<Socket> socket) = 0;
		virtual Int32 SocketGetLastError() = 0;
		virtual Bool GetRemoteAddr(NN<Socket> socket, NN<Net::SocketUtil::AddressInfo> addr, OptOut<UInt16> port) = 0;
		virtual Bool GetLocalAddr(NN<Socket> socket, NN<Net::SocketUtil::AddressInfo> addr, OptOut<UInt16> port) = 0;
		virtual OSInt SocketGetFD(NN<Socket> socket) = 0;
		virtual Bool SocketWait(NN<Socket> socket, Data::Duration dur) = 0;

		virtual void SetDontLinger(NN<Socket> socket, Bool val) = 0;
		virtual void SetLinger(NN<Socket> socket, UInt32 ms) = 0;
		virtual void SetRecvBuffSize(NN<Socket> socket, Int32 buffSize) = 0;
		virtual void SetNoDelay(NN<Socket> socket, Bool val) = 0;
		virtual void SetRecvTimeout(NN<Socket> socket, Data::Duration timeout) = 0;
		virtual void SetReuseAddr(NN<Socket> socket, Bool val) = 0;
		virtual void SetIPv4TTL(NN<Socket> socket, Int32 ttl) = 0;
		virtual void SetBroadcast(NN<Socket> socket, Bool val) = 0;
		virtual void AddIPMembership(NN<Socket> socket, UInt32 ip) = 0;

		virtual UOSInt SendData(NN<Socket> socket, UnsafeArray<const UInt8> buff, UOSInt buffSize, OptOut<ErrorType> et) = 0;
		virtual UOSInt ReceiveData(NN<Socket> socket, UnsafeArray<UInt8> buff, UOSInt buffSize, OptOut<ErrorType> et) = 0;
		virtual Optional<SocketRecvSess> BeginReceiveData(NN<Socket> socket, UnsafeArray<UInt8> buff, UOSInt buffSize, NN<Sync::Event> evt, OptOut<ErrorType> et) = 0;
		virtual UOSInt EndReceiveData(NN<SocketRecvSess> reqData, Bool toWait, OutParam<Bool> incomplete) = 0;
		virtual void CancelReceiveData(NN<SocketRecvSess> reqData) = 0;

		virtual UOSInt UDPReceive(NN<Socket> socket, UnsafeArray<UInt8> buff, UOSInt buffSize, NN<Net::SocketUtil::AddressInfo> addr, OutParam<UInt16> port, OptOut<ErrorType> et) = 0;
		virtual UOSInt SendTo(NN<Socket> socket, UnsafeArray<const UInt8> buff, UOSInt buffSize, NN<const Net::SocketUtil::AddressInfo> addr, UInt16 port) = 0;
		virtual UOSInt SendToIF(NN<Socket> socket, UnsafeArray<const UInt8> buff, UOSInt buffSize, UnsafeArray<const UTF8Char> ifName) = 0;

		virtual Bool IcmpSendEcho2(NN<const Net::SocketUtil::AddressInfo> addr, OutParam<UInt32> respTime_us, OutParam<UInt32> ttl) = 0;

		virtual Bool Connect(NN<Socket> socket, UInt32 ip, UInt16 port, Data::Duration timeout) = 0;
		virtual Bool Connect(NN<Socket> socket, NN<const Net::SocketUtil::AddressInfo> addr, UInt16 port, Data::Duration timeout) = 0;
		virtual void ShutdownSend(NN<Socket> socket) = 0;
		virtual void ShutdownSocket(NN<Socket> socket) = 0;

		virtual Bool SocketGetReadBuff(NN<Socket> socket, OutParam<UInt32> size) = 0;

		virtual Bool DNSResolveIPDef(UnsafeArray<const Char> host, NN<Net::SocketUtil::AddressInfo> addr) = 0;
		virtual Bool GetDefDNS(NN<Net::SocketUtil::AddressInfo> addr) = 0;
		virtual UOSInt GetDNSList(NN<Data::ArrayList<UInt32>> dnsList) = 0;
		virtual Bool LoadHosts(NN<Net::DNSHandler> dnsHdlr) = 0;

		virtual Bool ARPAddRecord(UOSInt ifIndex, UnsafeArray<const UInt8> hwAddr, UInt32 ipv4) = 0;

		virtual UOSInt GetConnInfoList(NN<Data::ArrayListNN<Net::ConnectionInfo>> connInfoList) = 0;
		virtual Bool GetIPInfo(NN<IPInfo> info) = 0; //////////////////////////////////
		virtual Bool GetTCPInfo(NN<TCPInfo> info) = 0; //////////////////////////////////
		virtual Bool GetUDPInfo(NN<UDPInfo> info) = 0; //////////////////////////////////
		virtual UOSInt QueryPortInfos(NN<Data::ArrayListNN<PortInfo>> portInfoList, ProtocolType protoType, UInt16 procId) = 0;
		virtual void FreePortInfos(NN<Data::ArrayListNN<PortInfo>> portInfoList) = 0;
		virtual UOSInt QueryPortInfos2(NN<Data::ArrayListNN<PortInfo3>> portInfoList, ProtocolType protoType, UInt16 procId) = 0;
		virtual void FreePortInfos2(NN<Data::ArrayListNN<PortInfo3>> portInfoList) = 0;

		virtual Bool AdapterSetHWAddr(Text::CStringNN adapterName, UnsafeArray<const UInt8> hwAddr);
		virtual Bool AdapterEnable(Text::CStringNN adapterName, Bool enable);

		Bool ReloadDNS();
		Bool ForceDNSServer(Text::CStringNN ip);
		Bool DNSResolveIP(Text::CStringNN host, NN<Net::SocketUtil::AddressInfo> addr);
		UOSInt DNSResolveIPs(Text::CStringNN host, Data::DataArray<Net::SocketUtil::AddressInfo> addrs);
		UInt32 DNSResolveIPv4(Text::CStringNN host);

		UnsafeArrayOpt<UTF8Char> GetRemoteName(UnsafeArray<UTF8Char> buff, NN<Socket> socket);
		UnsafeArrayOpt<UTF8Char> GetLocalName(UnsafeArray<UTF8Char> buff, NN<Socket> socket);
#ifdef HAS_INT64
		UInt64 GenSocketId(NN<Socket> socket);
		static void FromSocketId(UInt64 socketId, OptOut<UInt32> ip, OptOut<UInt16> port);
#endif

		static ErrorType FromSystemError(Int32 sysErr);
	};
}
#endif
