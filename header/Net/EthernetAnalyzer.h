#ifndef _SM_NET_ETHERNETANALYZER
#define _SM_NET_ETHERNETANALYZER
#include "Data/ArrayListNN.h"
#include "Data/FastMap.h"
#include "Data/FixedCircularBuff.h"
#include "Data/ICaseStringMap.h"
#include "IO/Writer.h"
#include "IO/ParsedObject.h"
#include "Net/DNSClient.h"
#include "Sync/MutexUsage.h"
#include "Text/String.h"

namespace Net
{
	class EthernetAnalyzer : public IO::ParsedObject
	{
	public:
		typedef void (__stdcall *Pingv4Handler)(void *userData, UInt32 fromIP, UInt32 toIP, UInt8 ttl, UOSInt packetSize);

		typedef enum
		{
			AT_DEVICE = 1,
			AT_IPTRANSFER = 2,
			AT_DNSCLI = 4,
			AT_DNSREQ = 8,
			AT_DNSTARGET = 16,
			AT_IPLOG = 32,
			AT_DHCP = 64,
			AT_UDP = AT_IPTRANSFER | AT_DNSCLI | AT_DNSREQ | AT_DNSTARGET | AT_IPLOG | AT_DHCP,
			AT_ICMP = AT_IPTRANSFER | AT_IPLOG,
			AT_ALL = AT_DEVICE | AT_IPTRANSFER | AT_DNSCLI | AT_DNSREQ | AT_DNSTARGET | AT_IPLOG | AT_DHCP
		} AnalyzeType;

		typedef struct
		{
			UInt32 srcIP;
			UInt32 destIP;
			UInt64 tcpCnt;
			UInt64 udpCnt;
			UInt64 icmpCnt;
			UInt64 otherCnt;
			UInt64 tcpSize;
			UInt64 udpSize;
			UInt64 icmpSize;
			UInt64 otherSize;
		} IPTranStatus;

		typedef struct
		{
			UInt64 macAddr;
			UInt32 ipv4Addr[4];
			Net::SocketUtil::AddressInfo ipv6Addr;
			UInt64 ipv4SrcCnt;
			UInt64 ipv4DestCnt;
			UInt64 ipv6SrcCnt;
			UInt64 ipv6DestCnt;
			UInt64 othSrcCnt;
			UInt64 othDestCnt;
			Text::String *name;
			Int64 packetTime[16];
			UInt64 packetDestMAC[16];
			UInt16 packetEtherType[16];
			UOSInt packetSize[16];
			UInt8 *packetData[16];
		} MACStatus;

		typedef struct
		{
			Int32 year;
			Int32 month;
			Int32 day;
			Int32 hour;
			UInt64 reqCount;
		} DNSCliHourInfo;

		struct DNSClientInfo
		{
			UInt32 cliId;
			Net::SocketUtil::AddressInfo addr;
			Sync::Mutex mut;
			Data::ArrayList<DNSCliHourInfo*> hourInfos;
		};

		struct DNSRequestResult
		{
			UInt8 recBuff[512];
			UOSInt recSize;
			Data::Timestamp reqTime;
			UInt32 ttl;
			Int32 status; //0 = normal, 1 = blocked
			Sync::Mutex mut;
		};

		struct DNSTargetInfo
		{
			UInt32 ip;
			Sync::Mutex mut;
			Data::ArrayListICaseString addrList;
		};

		struct IPLogInfo
		{
			UInt32 ip;
			Sync::Mutex mut;
			Data::ArrayListNN<Text::String> logList;
		};

		struct DHCPInfo
		{
			UInt64 iMAC;
			Bool updated;
			Sync::Mutex mut;
			Data::Timestamp ipAddrTime;
			UInt32 ipAddr;
			UInt32 ipAddrLease;
			UInt32 subnetMask;
			UInt32 gwAddr;
			UInt32 dhcpServer;
			UInt32 renewTime;
			UInt32 rebindTime;
			UInt32 router;
			UInt32 dns[4];
			Text::String *hostName;
			Text::String *vendorClass;
		};

		struct TCP4SYNInfo
		{
			UInt32 srcAddr;
			UInt32 destAddr;
			UInt16 srcPort;
			UInt16 destPort;
			Data::Timestamp reqTime;

			TCP4SYNInfo() = default;
			TCP4SYNInfo(TCP4SYNInfo*)
			{
				this->srcAddr = 0;
				this->destAddr = 0;
				this->srcPort = 0;
				this->destPort = 0;
				this->reqTime = Data::Timestamp(0);
			}

			Bool operator==(TCP4SYNInfo info)
			{
				if (this->srcAddr != info.srcAddr) return false;
				if (this->destAddr != info.destAddr) return false;
				if (this->srcPort != info.srcPort) return false;
				if (this->destPort != info.destPort) return false;
				return this->reqTime == info.reqTime;
			}
		};
		
	private:
		AnalyzeType atype;
		Sync::Mutex ipTranMut;
		Data::FastMap<Int64, IPTranStatus*> ipTranMap;
		Sync::Mutex macMut;
		Data::FastMap<UInt64, MACStatus*> macMap;
		Sync::Mutex dnsCliInfoMut;
		Data::FastMap<UInt32, DNSClientInfo*> dnsCliInfos;
		Sync::Mutex dnsReqv4Mut;
		Data::ICaseStringMap<DNSRequestResult*> dnsReqv4Map;
		Sync::Mutex dnsReqv6Mut;
		Data::ICaseStringMap<DNSRequestResult*> dnsReqv6Map;
		Sync::Mutex dnsReqOthMut;
		Data::ICaseStringMap<DNSRequestResult*> dnsReqOthMap;
		Sync::Mutex dnsTargetMut;
		Data::FastMap<UInt32, DNSTargetInfo*> dnsTargetMap;
		Sync::Mutex ipLogMut;
		Data::FastMap<UInt32, IPLogInfo*> ipLogMap;
		Sync::Mutex dhcpMut;
		Data::FastMap<UInt64, DHCPInfo*> dhcpMap;
		Sync::Mutex mdnsMut;
		Data::ArrayList<Net::DNSClient::RequestAnswer*> mdnsList;
		Sync::Mutex tcp4synMut;
		Data::FixedCircularBuff<TCP4SYNInfo> tcp4synList;

		Pingv4Handler pingv4ReqHdlr;
		void *pingv4ReqObj;

		UInt64 packetCnt;
		UInt64 packetTotalSize;
		Bool isFirst;
		IO::Writer *errWriter;

		static void NetBIOSDecName(UTF8Char *nameBuff, UOSInt nameSize);

		MACStatus *MACGet(UInt64 macAddr);
		void MDNSAdd(Net::DNSClient::RequestAnswer *ans);
	public:
		EthernetAnalyzer(IO::Writer *errWriter, AnalyzeType ctype, NotNullPtr<Text::String> name);
		EthernetAnalyzer(IO::Writer *errWriter, AnalyzeType ctype, Text::CString name);
		virtual ~EthernetAnalyzer();

		virtual IO::ParserType GetParserType() const;

		UInt64 GetPacketCnt() const;
		UInt64 GetPacketTotalSize() const;
		void UseIPTran(Sync::MutexUsage *mutUsage);
		NotNullPtr<const Data::ReadingList<IPTranStatus*>> IPTranGetList() const;
		UOSInt IPTranGetCount() const;
		void UseMAC(Sync::MutexUsage *mutUsage);
		const Data::ReadingList<MACStatus*> *MACGetList() const;
		void UseDNSCli(Sync::MutexUsage *mutUsage);
		NotNullPtr<const Data::ReadingList<DNSClientInfo*>> DNSCliGetList() const;
		UOSInt DNSCliGetCount();
		UOSInt DNSReqv4GetList(Data::ArrayList<Text::String *> *reqList); //no need release
		UOSInt DNSReqv4GetCount();
		Bool DNSReqv4GetInfo(Text::CString req, Data::ArrayList<Net::DNSClient::RequestAnswer*> *ansList, Data::DateTime *reqTime, UInt32 *ttl);
		UOSInt DNSReqv6GetList(Data::ArrayList<Text::String *> *reqList); //no need release
		UOSInt DNSReqv6GetCount();
		Bool DNSReqv6GetInfo(Text::CString req, Data::ArrayList<Net::DNSClient::RequestAnswer*> *ansList, Data::DateTime *reqTime, UInt32 *ttl);
		UOSInt DNSReqOthGetList(Data::ArrayList<Text::String *> *reqList); //no need release
		UOSInt DNSReqOthGetCount();
		Bool DNSReqOthGetInfo(Text::CString req, Data::ArrayList<Net::DNSClient::RequestAnswer*> *ansList, Data::DateTime *reqTime, UInt32 *ttl);
		UOSInt DNSTargetGetList(Data::ArrayList<DNSTargetInfo *> *targetList); //no need release
		UOSInt DNSTargetGetCount();
		UOSInt MDNSGetList(Data::ArrayList<Net::DNSClient::RequestAnswer *> *mdnsList); //no need release
		UOSInt MDNSGetCount();
		void UseDHCP(Sync::MutexUsage *mutUsage);
		const Data::ReadingList<DHCPInfo*> *DHCPGetList() const;
		void UseIPLog(Sync::MutexUsage *mutUsage);
		NotNullPtr<const Data::ReadingList<IPLogInfo*>> IPLogGetList() const;
		UOSInt IPLogGetCount() const;
		Bool TCP4SYNIsDiff(UOSInt lastIndex) const;
		UOSInt TCP4SYNGetList(Data::ArrayList<TCP4SYNInfo> *synList, UOSInt *thisIndex) const;

		Bool PacketData(UInt32 linkType, const UInt8 *packet, UOSInt packetSize); //Return valid
		Bool PacketNull(const UInt8 *packet, UOSInt packetSize); //Return valid
		Bool PacketEthernet(const UInt8 *packet, UOSInt packetSize); //Return valid
		Bool PacketLinux(const UInt8 *packet, UOSInt packetSize); //Return valid
		Bool PacketEthernetData(const UInt8 *packet, UOSInt packetSize, UInt16 etherType, UInt64 srcMAC, UInt64 destMAC); //Return valid
		Bool PacketIPv4(const UInt8 *packet, UOSInt packetSize, UInt64 srcMAC, UInt64 destMAC); //Return valid
		Bool PacketIPv6(const UInt8 *packet, UOSInt packetSize, UInt64 srcMAC, UInt64 destMAC); //Return valid
		Bool PacketARP(const UInt8 *packet, UOSInt packetSize, UInt64 srcMAC, UInt64 destMAC); //Return valid

		AnalyzeType GetAnalyzeType();
		void HandlePingv4Request(Pingv4Handler pingv4Hdlr, void *userObj);
	};
}
#endif
