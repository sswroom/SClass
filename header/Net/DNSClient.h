//require dnsapi.lib
#ifndef _SM_NET_DNSCLIENT
#define _SM_NET_DNSCLIENT
#include "Data/FastMapNN.hpp"
#include "Net/UDPServer.h"
#include "Text/CString.h"

namespace Net
{
	class DNSClient
	{
	public:
		typedef struct
		{
			NN<Text::String> name;
			UInt16 recType;
			UInt16 recClass;
			UInt32 ttl;
			Text::String *rd;
			Net::SocketUtil::AddressInfo addr;
			UInt16 priority;
			UInt16 weight;
			UInt16 port;
		} RequestAnswer;

	private:
		class RequestStatus
		{
		public:
			UInt8 respBuff[512];
			UIntOS respSize;
			Sync::Event finEvt;
		};

		Net::UDPServer *svr;
		NN<Net::SocketFactory> sockf;
		UInt32 lastID;
		Net::SocketUtil::AddressInfo serverAddr;

		Sync::Mutex reqMut;
		Data::FastMapNN<UInt32, RequestStatus> reqMap;

		static void __stdcall PacketHdlr(NN<const Net::SocketUtil::AddressInfo> addr, UInt16 port, Data::ByteArrayR buff, AnyType userData);
		NN<RequestStatus> NewReq(UInt32 id);
		void DelReq(UInt32 id);
		UInt32 NextId();
	public:
		DNSClient(NN<Net::SocketFactory> sockf, NN<const Net::SocketUtil::AddressInfo> serverAddr, NN<IO::LogTool> log);
		~DNSClient();

		UIntOS GetByEmailDomainName(NN<Data::ArrayListNN<RequestAnswer>> answers, Text::CStringNN domain);
		UIntOS GetByDomainName(NN<Data::ArrayListNN<RequestAnswer>> answers, Text::CStringNN domain);
		UIntOS GetByType(NN<Data::ArrayListNN<RequestAnswer>> answers, Text::CStringNN domain, UInt16 type);
		UIntOS GetByIPv4Name(NN<Data::ArrayListNN<RequestAnswer>> answers, UInt32 ip);
		UIntOS GetByAddrName(NN<Data::ArrayListNN<RequestAnswer>> answers, NN<const Net::SocketUtil::AddressInfo> addr);
		UIntOS GetServerName(NN<Data::ArrayListNN<RequestAnswer>> answers);
		UIntOS GetCAARecord(NN<Data::ArrayListNN<RequestAnswer>> answers, Text::CStringNN domain);

		void UpdateDNSAddr(NN<const Net::SocketUtil::AddressInfo> serverAddr);

		static UIntOS ParseString(UnsafeArray<UTF8Char> sbuff, UnsafeArray<const UInt8> buff, UIntOS stringOfst, UIntOS endOfst, OptOut<UnsafeArray<UTF8Char>> sbuffEndOut); //return actEndOfst
		static UIntOS ParseAnswers(UnsafeArray<const UInt8> buff, UIntOS dataSize, NN<Data::ArrayListNN<RequestAnswer>> answers);
		static NN<RequestAnswer> ParseAnswer(UnsafeArray<const UInt8> buff, UIntOS dataSize, InOutParam<UIntOS> index);
		static void FreeAnswers(NN<Data::ArrayListNN<RequestAnswer>> answers);
		static void __stdcall FreeAnswer(NN<RequestAnswer> answer);
		static UInt32 GetResponseTTL(UnsafeArray<const UInt8> buff, UIntOS buffSize);
		static UIntOS SkipString(UnsafeArray<const UInt8> buff, UIntOS stringOfst, UIntOS endOfst);
		static Text::CString TypeGetID(UInt16 type);
	};
}
#endif
