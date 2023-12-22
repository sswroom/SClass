#ifndef _SM_NET_SOCKETFACTORY
#define _SM_NET_SOCKETFACTORY
#include "Data/ArrayList.h"
#include "Data/DataArray.h"
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
		Net::DNSHandler *dnsHdlr;
		Bool GetEffectiveDNS(NotNullPtr<Net::SocketUtil::AddressInfo> addr);
	public:
		SocketFactory(Bool noV6DNS);
		virtual ~SocketFactory();

		virtual Socket *CreateTCPSocketv4() = 0;
		virtual Socket *CreateTCPSocketv6() = 0;
		virtual Socket *CreateUDPSocketv4() = 0;
		virtual Socket *CreateUDPSocketv6() = 0;

		virtual Socket *CreateICMPIPv4Socket(UInt32 ip) = 0;
		virtual Socket *CreateUDPRAWv4Socket(UInt32 ip) = 0;
		virtual Socket *CreateRAWIPv4Socket(UInt32 ip) = 0;
		virtual Socket *CreateARPSocket() = 0;
		virtual Socket *CreateRAWSocket() = 0;

		virtual void DestroySocket(Socket *socket) = 0;
		virtual Bool SocketIsInvalid(Socket *socket) = 0;
		virtual Bool SocketBindv4(Socket *socket, UInt32 ip, UInt16 port) = 0;
		virtual Bool SocketBind(Socket *socket, const Net::SocketUtil::AddressInfo *addr, UInt16 port) = 0;
		virtual Bool SocketBindRAWIf(Socket *socket, UOSInt ifIndex) = 0;
		virtual Bool SocketListen(Socket *socket) = 0;
		virtual Socket *SocketAccept(Socket *socket) = 0;
		virtual Int32 SocketGetLastError() = 0;
		virtual Bool GetRemoteAddr(Socket *socket, NotNullPtr<Net::SocketUtil::AddressInfo> addr, UInt16 *port) = 0;
		virtual Bool GetLocalAddr(Socket *socket, NotNullPtr<Net::SocketUtil::AddressInfo> addr, UInt16 *port) = 0;
		virtual Int32 SocketGetFD(Socket *socket) = 0;

		virtual void SetDontLinger(Socket *socket, Bool val) = 0;
		virtual void SetLinger(Socket *socket, UInt32 ms) = 0;
		virtual void SetRecvBuffSize(Socket *socket, Int32 buffSize) = 0;
		virtual void SetNoDelay(Socket *socket, Bool val) = 0;
		virtual void SetRecvTimeout(Socket *socket, Data::Duration timeout) = 0;
		virtual void SetReuseAddr(Socket *socket, Bool val) = 0;
		virtual void SetIPv4TTL(Socket *socket, Int32 ttl) = 0;
		virtual void SetBroadcast(Socket *socket, Bool val) = 0;
		virtual void AddIPMembership(Socket *socket, UInt32 ip) = 0;

		virtual UOSInt SendData(Socket *socket, const UInt8 *buff, UOSInt buffSize, OptOut<ErrorType> et) = 0;
		virtual UOSInt ReceiveData(Socket *socket, UInt8 *buff, UOSInt buffSize, OptOut<ErrorType> et) = 0;
		virtual void *BeginReceiveData(Socket *socket, UInt8 *buff, UOSInt buffSize, Sync::Event *evt, OptOut<ErrorType> et) = 0;
		virtual UOSInt EndReceiveData(void *reqData, Bool toWait, OutParam<Bool> incomplete) = 0;
		virtual void CancelReceiveData(void *reqData) = 0;

		virtual UOSInt UDPReceive(Socket *socket, UInt8 *buff, UOSInt buffSize, NotNullPtr<Net::SocketUtil::AddressInfo> addr, OutParam<UInt16> port, OptOut<ErrorType> et) = 0;
		virtual UOSInt SendTo(Socket *socket, const UInt8 *buff, UOSInt buffSize, NotNullPtr<const Net::SocketUtil::AddressInfo> addr, UInt16 port) = 0;
		virtual UOSInt SendToIF(Socket *socket, const UInt8 *buff, UOSInt buffSize, const UTF8Char *ifName) = 0;

		virtual Bool IcmpSendEcho2(NotNullPtr<const Net::SocketUtil::AddressInfo> addr, UInt32 *respTime_us, UInt32 *ttl) = 0;

		virtual Bool Connect(Socket *socket, UInt32 ip, UInt16 port, Data::Duration timeout) = 0;
		virtual Bool Connect(Socket *socket, NotNullPtr<const Net::SocketUtil::AddressInfo> addr, UInt16 port, Data::Duration timeout) = 0;
		virtual void ShutdownSend(Socket *socket) = 0;
		virtual void ShutdownSocket(Socket *socket) = 0;

		virtual Bool SocketGetReadBuff(Socket *socket, UInt32 *size) = 0;

		virtual Bool DNSResolveIPDef(const Char *host, NotNullPtr<Net::SocketUtil::AddressInfo> addr) = 0;
		virtual Bool GetDefDNS(NotNullPtr<Net::SocketUtil::AddressInfo> addr) = 0;
		virtual UOSInt GetDNSList(Data::ArrayList<UInt32> *dnsList) = 0;
		virtual Bool LoadHosts(Net::DNSHandler *dnsHdlr) = 0;

		virtual Bool ARPAddRecord(UOSInt ifIndex, const UInt8 *hwAddr, UInt32 ipv4) = 0;

		virtual UOSInt GetConnInfoList(Data::ArrayList<Net::ConnectionInfo*> *connInfoList) = 0;
		virtual Bool GetIPInfo(NotNullPtr<IPInfo> info) = 0; //////////////////////////////////
		virtual Bool GetTCPInfo(NotNullPtr<TCPInfo> info) = 0; //////////////////////////////////
		virtual Bool GetUDPInfo(NotNullPtr<UDPInfo> info) = 0; //////////////////////////////////
		virtual UOSInt QueryPortInfos(Data::ArrayList<PortInfo*> *portInfoList, ProtocolType protoType, UInt16 procId) = 0;
		virtual void FreePortInfos(Data::ArrayList<PortInfo*> *portInfoList) = 0;
		virtual UOSInt QueryPortInfos2(Data::ArrayList<PortInfo3*> *portInfoList, ProtocolType protoType, UInt16 procId) = 0;
		virtual void FreePortInfos2(Data::ArrayList<PortInfo3*> *portInfoList) = 0;

		virtual Bool AdapterSetHWAddr(Text::CString adapterName, const UInt8 *hwAddr);
		virtual Bool AdapterEnable(Text::CString adapterName, Bool enable);

		Bool ReloadDNS();
		Bool ForceDNSServer(Text::CStringNN ip);
		Bool DNSResolveIP(Text::CStringNN host, NotNullPtr<Net::SocketUtil::AddressInfo> addr);
		UOSInt DNSResolveIPs(Text::CStringNN host, Data::DataArray<Net::SocketUtil::AddressInfo> addrs);
		UInt32 DNSResolveIPv4(Text::CStringNN host);
//		Bool GetIPByHost(const WChar *host, Net::SocketUtil::AddressInfo *addr);
//		UInt32 GetIPv4ByHost(const WChar *host);		UInt32 DNSResolveIPv4(const UTF8Char *host);
//		UInt32 GetIPv4ByHostOS(const WChar *host
//		UInt32 GetLocalIPByDest(const WChar *host);////////////////////
//		UInt32 GetLocalIPByDest(UInt32 ip);//////////////////////////

		UTF8Char *GetRemoteName(UTF8Char *buff, Socket *socket);
		UTF8Char *GetLocalName(UTF8Char *buff, Socket *socket);
#ifdef HAS_INT64
		UInt64 GenSocketId(Socket *socket);
		static void FromSocketId(UInt64 socketId, UInt32 *ip, UInt16 *port);
#endif

		static ErrorType FromSystemError(Int32 sysErr);
	};
}
#endif
