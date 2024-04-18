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
			NotNullPtr<Text::String> name;
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
		NotNullPtr<Net::SocketFactory> sockf;
		UInt32 lastID;
		Net::SocketUtil::AddressInfo serverAddr;

		Sync::Mutex reqMut;
		Data::FastMapNN<UInt32, RequestStatus> reqMap;

		static void __stdcall PacketHdlr(NotNullPtr<const Net::SocketUtil::AddressInfo> addr, UInt16 port, const UInt8 *buff, UOSInt dataSize, AnyType userData);
		NotNullPtr<RequestStatus> NewReq(UInt32 id);
		void DelReq(UInt32 id);
		UInt32 NextId();
	public:
		DNSClient(NotNullPtr<Net::SocketFactory> sockf, NotNullPtr<const Net::SocketUtil::AddressInfo> serverAddr, NotNullPtr<IO::LogTool> log);
		~DNSClient();

		UOSInt GetByEmailDomainName(NotNullPtr<Data::ArrayListNN<RequestAnswer>> answers, Text::CStringNN domain);
		UOSInt GetByDomainName(NotNullPtr<Data::ArrayListNN<RequestAnswer>> answers, Text::CStringNN domain);
		UOSInt GetByType(NotNullPtr<Data::ArrayListNN<RequestAnswer>> answers, Text::CStringNN domain, UInt16 type);
		UOSInt GetByIPv4Name(NotNullPtr<Data::ArrayListNN<RequestAnswer>> answers, UInt32 ip);
		UOSInt GetByAddrName(NotNullPtr<Data::ArrayListNN<RequestAnswer>> answers, NotNullPtr<const Net::SocketUtil::AddressInfo> addr);
		UOSInt GetServerName(NotNullPtr<Data::ArrayListNN<RequestAnswer>> answers);
		UOSInt GetCAARecord(NotNullPtr<Data::ArrayListNN<RequestAnswer>> answers, Text::CStringNN domain);

		void UpdateDNSAddr(NotNullPtr<const Net::SocketUtil::AddressInfo> serverAddr);

		static UOSInt ParseString(UTF8Char *sbuff, const UInt8 *buff, UOSInt stringOfst, UOSInt endOfst, UTF8Char **sbuffEndOut); //return actEndOfst
		static UOSInt ParseAnswers(const UInt8 *buff, UOSInt dataSize, NotNullPtr<Data::ArrayListNN<RequestAnswer>> answers);
		static NotNullPtr<RequestAnswer> ParseAnswer(const UInt8 *buff, UOSInt dataSize, InOutParam<UOSInt> index);
		static void FreeAnswers(NotNullPtr<Data::ArrayListNN<RequestAnswer>> answers);
		static void FreeAnswer(NotNullPtr<RequestAnswer> answer);
		static UInt32 GetResponseTTL(const UInt8 *buff, UOSInt buffSize);
		static UOSInt SkipString(const UInt8 *buff, UOSInt stringOfst, UOSInt endOfst);
		static Text::CString TypeGetID(UInt16 type);
	};
}
#endif
