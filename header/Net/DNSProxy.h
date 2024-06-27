#ifndef _SM_NET_DNSPROXY
#define _SM_NET_DNSPROXY
#include "AnyType.h"
#include "Data/ArrayListICaseString.h"
#include "Data/ArrayListStringNN.h"
#include "Data/CallbackStorage.h"
#include "Data/ICaseStringMapNN.h"
#include "Net/DNSClient.h"
#include "Net/DNSServer.h"

namespace Net
{
	class DNSProxy
	{
	public:
		enum NameStatus
		{
			NS_NORMAL,
			NS_BLOCKED,
			NS_CUSTOM
		};

		struct TargetInfo
		{
			UInt32 ip;
			Sync::Mutex mut;
			Data::ArrayListICaseString addrList;
		};

	private:
		typedef struct
		{
			UInt8 recBuff[512];
			UOSInt recSize;
			Data::Timestamp reqTime;
			UInt32 ttl;
			NameStatus status;
			Net::SocketUtil::AddressInfo customAddr;
			Sync::Mutex mut;
		} RequestResult;

		typedef struct
		{
			UInt8 respBuff[512];
			UOSInt respSize;
			Sync::Event finEvt;
		} CliRequestStatus;
	public:
		typedef void (CALLBACKFUNC DNSProxyRequest)(AnyType userObj, Text::CStringNN reqName, Int32 reqType, Int32 reqClass, NN<const Net::SocketUtil::AddressInfo> addr, UInt16 reqPort, UInt32 reqId, Double timeUsed);
	private:
		NN<Net::SocketFactory> sockf;
		Net::UDPServer *cli;
		Net::DNSServer *svr;

		UInt32 currServerIP;
		UOSInt currServerIndex;
		Data::DateTime currIPTime;
		Sync::Mutex dnsMut;
		Data::ArrayList<UInt32> dnsList;
		Bool disableV6;

		Sync::Mutex reqv4Mut;
		Data::ICaseStringMapNN<RequestResult> reqv4Map;
		Bool reqv4Updated;

		Sync::Mutex reqv6Mut;
		Data::ICaseStringMapNN<RequestResult> reqv6Map;
		Bool reqv6Updated;

		Sync::Mutex reqothMut;
		Data::ICaseStringMapNN<RequestResult> reqothMap;
		Bool reqothUpdated;

		Sync::Mutex *targetMut;
		Data::FastMapNN<UInt32, TargetInfo> *targetMap;
		Bool targetUpdated;

		Sync::Mutex lastIdMut;
		UInt32 lastId;

		Sync::Mutex cliReqMut;
		Data::FastMapNN<UInt32, CliRequestStatus> cliReqMap;

		Sync::Mutex hdlrMut;
		Data::ArrayList<Data::CallbackStorage<DNSProxyRequest>> hdlrList;

		Sync::Mutex blackListMut;
		Data::ArrayListStringNN blackList;

//		Sync::Mutex *whiteListMut;
//		Data::ArrayListNN<const UTF8Char> *whiteList;

		static void __stdcall ClientPacket(NN<const Net::SocketUtil::AddressInfo> addr, UInt16 port, Data::ByteArrayR data, AnyType userData);
		static void __stdcall OnDNSRequest(AnyType userObj, Text::CStringNN reqName, Int32 reqType, Int32 reqClass, NN<const Net::SocketUtil::AddressInfo> reqAddr, UInt16 reqPort, UInt32 reqId);
		void RequestDNS(UnsafeArray<const UTF8Char> reqName, Int32 reqType, Int32 reqClass, NN<RequestResult> req);
		UInt32 NextId();
		NN<CliRequestStatus> NewCliReq(UInt32 id);
		void DelCliReq(UInt32 id);
		static UOSInt BuildEmptyReply(UInt8 *buff, UInt32 id, UnsafeArray<const UTF8Char> reqName, Int32 reqType, Int32 reqClass, Bool disableV6);
		static UOSInt BuildAddressReply(UInt8 *buff, UInt32 id, UnsafeArray<const UTF8Char> reqName, Int32 reqClass, const Net::SocketUtil::AddressInfo *addr);
	public:
		DNSProxy(NN<Net::SocketFactory> sockf, Bool analyzeTarget, NN<IO::LogTool> log);
		~DNSProxy();

		Bool IsError();
		Bool IsReqListv4Chg();
		Bool IsReqListv6Chg();
		Bool IsReqListOthChg();
		Bool IsTargetChg();
		UOSInt GetReqv4List(NN<Data::ArrayListNN<Text::String>> reqList); //no need release
		UOSInt GetReqv6List(NN<Data::ArrayListNN<Text::String>> reqList); //no need release
		UOSInt GetReqOthList(NN<Data::ArrayListNN<Text::String>> reqList); //no need release
		UOSInt GetTargetList(NN<Data::ArrayListNN<TargetInfo>> targetList); //no need release
		UOSInt SearchIPv4(NN<Data::ArrayListNN<Text::String>> reqList, UInt32 ip, UInt32 mask); //no need release
		Bool GetRequestInfov4(Text::CStringNN req, NN<Data::ArrayListNN<Net::DNSClient::RequestAnswer>> ansList, NN<Data::DateTime> reqTime, OutParam<UInt32> ttl);
		Bool GetRequestInfov6(Text::CStringNN req, NN<Data::ArrayListNN<Net::DNSClient::RequestAnswer>> ansList, NN<Data::DateTime> reqTime, OutParam<UInt32> ttl);
		Bool GetRequestInfoOth(Text::CStringNN req, NN<Data::ArrayListNN<Net::DNSClient::RequestAnswer>> ansList, NN<Data::DateTime> reqTime, OutParam<UInt32> ttl);
		UInt32 GetServerIP();
		void SetServerIP(UInt32 serverIP);
		void GetDNSList(NN<Data::ArrayList<UInt32>> dnsList);
		void AddDNSIP(UInt32 serverIP);
		void SwitchDNS();
		Bool IsDisableV6();
		void SetDisableV6(Bool disableV6);
		UOSInt GetBlackList(NN<Data::ArrayListNN<Text::String>> blackList);
		Bool AddBlackList(NN<Text::String> blackList);
		Bool AddBlackList(Text::CStringNN blackList);
		void HandleDNSRequest(DNSProxyRequest hdlr, AnyType userObj);
		void SetCustomAnswer(Text::CStringNN name, NN<const Net::SocketUtil::AddressInfo> addr);
		void SetWebProxyAutoDiscovery(NN<const Net::SocketUtil::AddressInfo> addr);
	};
}
#endif
