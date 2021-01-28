//require ws2_32.lib iphlpapi.lib dnsapi.lib
#ifndef _SM_NET_SOCKETFACTORY
#define _SM_NET_SOCKETFACTORY
#include "Data/ArrayList.h"
#include "Net/SocketUtil.h"
#include "Sync/Event.h"
#include "Sync/Mutex.h"

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
		} PortInfo2;

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
		Sync::Mutex *dnsMut;
		Bool noV6DNS;
	protected:
		Net::DNSHandler *dnsHdlr;
	public:
		SocketFactory(Bool noV6DNS);
		virtual ~SocketFactory();

		virtual UInt32 *CreateTCPSocketv4() = 0;
		virtual UInt32 *CreateTCPSocketv6() = 0;
		virtual UInt32 *CreateUDPSocketv4() = 0;
		virtual UInt32 *CreateUDPSocketv6() = 0;

		virtual UInt32 *CreateICMPIPv4Socket(UInt32 ip) = 0;
		virtual UInt32 *CreateUDPRAWv4Socket(UInt32 ip) = 0;
		virtual UInt32 *CreateRAWIPv4Socket(UInt32 ip) = 0;
		virtual UInt32 *CreateARPSocket() = 0;
		virtual UInt32 *CreateRAWSocket() = 0;

		virtual void DestroySocket(UInt32 *socket) = 0;
		virtual Bool SocketIsInvalid(UInt32 *socket) = 0;
		virtual Bool SocketBindv4(UInt32 *socket, UInt32 ip, UInt16 port) = 0;
		virtual Bool SocketBind(UInt32 *socket, const Net::SocketUtil::AddressInfo *addr, UInt16 port) = 0;
		virtual Bool SocketListen(UInt32 *socket) = 0;
		virtual UInt32 *SocketAccept(UInt32 *socket) = 0;
		virtual Int32 SocketGetLastError() = 0;
		virtual Bool GetRemoteAddr(UInt32 *socket, Net::SocketUtil::AddressInfo *addr, UInt16 *port) = 0;
		virtual Bool GetLocalAddr(UInt32 *socket, Net::SocketUtil::AddressInfo *addr, UInt16 *port) = 0;

		virtual void SetDontLinger(UInt32 *socket, Bool val) = 0;
		virtual void SetLinger(UInt32 *socket, Int32 ms) = 0;
		virtual void SetRecvBuffSize(UInt32 *socket, Int32 buffSize) = 0;
		virtual void SetNoDelay(UInt32 *socket, Bool val) = 0;
		virtual void SetRecvTimeout(UInt32 *socket, Int32 ms) = 0;
		virtual void SetReuseAddr(UInt32 *socket, Bool val) = 0;
		virtual void SetIPv4TTL(UInt32 *socket, Int32 ttl) = 0;
		virtual void SetBroadcast(UInt32 *socket, Bool val) = 0;
		virtual void AddIPMembership(UInt32 *socket, UInt32 ip) = 0;

		virtual OSInt SendData(UInt32 *socket, const UInt8 *buff, OSInt buffSize, ErrorType *et) = 0;
		virtual OSInt ReceiveData(UInt32 *socket, UInt8 *buff, OSInt buffSize, ErrorType *et) = 0;
		virtual void *BeginReceiveData(UInt32 *socket, UInt8 *buff, OSInt buffSize, Sync::Event *evt, ErrorType *et) = 0;
		virtual OSInt EndReceiveData(void *reqData, Bool toWait) = 0;
		virtual void CancelReceiveData(void *reqData) = 0;

		virtual UOSInt UDPReceive(UInt32 *socket, UInt8 *buff, UOSInt buffSize, Net::SocketUtil::AddressInfo *addr, UInt16 *port, ErrorType *et) = 0;
		virtual UOSInt SendTo(UInt32 *socket, const UInt8 *buff, UOSInt buffSize, const Net::SocketUtil::AddressInfo *addr, UInt16 port) = 0;
		virtual UOSInt SendToIF(UInt32 *socket, const UInt8 *buff, UOSInt buffSize, const UTF8Char *ifName) = 0;

		virtual Bool IcmpSendEcho2(const Net::SocketUtil::AddressInfo *addr, Int32 *respTime_us, Int32 *ttl) = 0;

		virtual Bool Connect(UInt32 *socket, UInt32 ip, UInt16 port) = 0;
		virtual Bool Connect(UInt32 *socket, const Net::SocketUtil::AddressInfo *addr, UInt16 port) = 0;
		virtual void ShutdownSend(UInt32 *socket) = 0;

		virtual Bool SocketGetReadBuff(UInt32 *socket, UInt32 *size) = 0;

		virtual Bool DNSResolveIPDef(const Char *host, Net::SocketUtil::AddressInfo *addr) = 0;
		virtual Bool GetDefDNS(Net::SocketUtil::AddressInfo *addr) = 0;
		virtual OSInt GetDNSList(Data::ArrayList<UInt32> *dnsList) = 0;
		virtual Bool LoadHosts(Net::DNSHandler *dnsHdlr) = 0;

		virtual Bool ARPAddRecord(OSInt ifIndex, const UInt8 *hwAddr, UInt32 ipv4) = 0;

		virtual OSInt GetConnInfoList(Data::ArrayList<Net::ConnectionInfo*> *connInfoList) = 0;
		virtual Bool GetIPInfo(IPInfo *info) = 0; //////////////////////////////////
		virtual Bool GetTCPInfo(TCPInfo *info) = 0; //////////////////////////////////
		virtual Bool GetUDPInfo(UDPInfo *info) = 0; //////////////////////////////////
		virtual OSInt QueryPortInfos(Data::ArrayList<PortInfo*> *portInfoList, ProtocolType protoType, UInt16 procId) = 0;
		virtual void FreePortInfos(Data::ArrayList<PortInfo*> *portInfoList) = 0;
		virtual OSInt QueryPortInfos2(Data::ArrayList<PortInfo2*> *portInfoList, ProtocolType protoType, UInt16 procId) = 0;
		virtual void FreePortInfos2(Data::ArrayList<PortInfo2*> *portInfoList) = 0;

		virtual Bool AdapterSetHWAddr(const UTF8Char *adapterName, const UInt8 *hwAddr);

		Bool DNSResolveIP(const Char *host, Net::SocketUtil::AddressInfo *addr);
		FORCEINLINE Bool DNSResolveIP(const UTF8Char *host, Net::SocketUtil::AddressInfo *addr) { return DNSResolveIP((const Char *)host, addr); }
		UInt32 DNSResolveIPv4(const UTF8Char *host);
//		Bool GetIPByHost(const WChar *host, Net::SocketUtil::AddressInfo *addr);
//		UInt32 GetIPv4ByHost(const WChar *host);		UInt32 DNSResolveIPv4(const UTF8Char *host);
//		UInt32 GetIPv4ByHostOS(const WChar *host
//		UInt32 GetLocalIPByDest(const WChar *host);////////////////////
//		UInt32 GetLocalIPByDest(UInt32 ip);//////////////////////////

		UTF8Char *GetRemoteName(UTF8Char *buff, UInt32 *socket);
		UTF8Char *GetLocalName(UTF8Char *buff, UInt32 *socket);
#ifdef HAS_INT64
		Int64 GenSocketId(UInt32 *socket);
		static void FromSocketId(Int64 socketId, UInt32 *ip, UInt16 *port);
#endif

		static ErrorType FromSystemError(Int32 sysErr);
	};
}
#endif
