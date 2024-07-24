//require dnsapi.lib
#ifndef _SM_NET_DNSCLIENT
#define _SM_NET_DNSCLIENT
#include "Data/FastMapNN.h"
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
		} RequestAnswer;

	private:
		class RequestStatus
		{
		public:
			UInt8 respBuff[512];
			UOSInt respSize;
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

		UOSInt GetByEmailDomainName(NN<Data::ArrayListNN<RequestAnswer>> answers, Text::CStringNN domain);
		UOSInt GetByDomainName(NN<Data::ArrayListNN<RequestAnswer>> answers, Text::CStringNN domain);
		UOSInt GetByType(NN<Data::ArrayListNN<RequestAnswer>> answers, Text::CStringNN domain, UInt16 type);
		UOSInt GetByIPv4Name(NN<Data::ArrayListNN<RequestAnswer>> answers, UInt32 ip);
		UOSInt GetByAddrName(NN<Data::ArrayListNN<RequestAnswer>> answers, NN<const Net::SocketUtil::AddressInfo> addr);
		UOSInt GetServerName(NN<Data::ArrayListNN<RequestAnswer>> answers);
		UOSInt GetCAARecord(NN<Data::ArrayListNN<RequestAnswer>> answers, Text::CStringNN domain);

		void UpdateDNSAddr(NN<const Net::SocketUtil::AddressInfo> serverAddr);

		static UOSInt ParseString(UnsafeArray<UTF8Char> sbuff, UnsafeArray<const UInt8> buff, UOSInt stringOfst, UOSInt endOfst, OptOut<UnsafeArray<UTF8Char>> sbuffEndOut); //return actEndOfst
		static UOSInt ParseAnswers(UnsafeArray<const UInt8> buff, UOSInt dataSize, NN<Data::ArrayListNN<RequestAnswer>> answers);
		static NN<RequestAnswer> ParseAnswer(UnsafeArray<const UInt8> buff, UOSInt dataSize, InOutParam<UOSInt> index);
		static void FreeAnswers(NN<Data::ArrayListNN<RequestAnswer>> answers);
		static void FreeAnswer(NN<RequestAnswer> answer);
		static UInt32 GetResponseTTL(UnsafeArray<const UInt8> buff, UOSInt buffSize);
		static UOSInt SkipString(UnsafeArray<const UInt8> buff, UOSInt stringOfst, UOSInt endOfst);
		static Text::CString TypeGetID(UInt16 type);
	};
}
#endif
