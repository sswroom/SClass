//require dnsapi.lib
#ifndef _SM_NET_DNSCLIENT
#define _SM_NET_DNSCLIENT
#include "Data/FastMap.h"
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
		Data::FastMap<UInt32, RequestStatus*> reqMap;

		static void __stdcall PacketHdlr(NotNullPtr<const Net::SocketUtil::AddressInfo> addr, UInt16 port, const UInt8 *buff, UOSInt dataSize, void *userData);
		RequestStatus *NewReq(UInt32 id);
		void DelReq(UInt32 id);
		UInt32 NextId();
	public:
		DNSClient(NotNullPtr<Net::SocketFactory> sockf, NotNullPtr<const Net::SocketUtil::AddressInfo> serverAddr);
		~DNSClient();

		UOSInt GetByEmailDomainName(Data::ArrayList<RequestAnswer*> *answers, Text::CString domain);
		UOSInt GetByDomainName(Data::ArrayList<RequestAnswer*> *answers, Text::CString domain);
		UOSInt GetByType(Data::ArrayList<RequestAnswer*> *answers, Text::CString domain, UInt16 type);
		UOSInt GetByIPv4Name(Data::ArrayList<RequestAnswer*> *answers, UInt32 ip);
		UOSInt GetByAddrName(Data::ArrayList<RequestAnswer*> *answers, const Net::SocketUtil::AddressInfo *addr);
		UOSInt GetServerName(Data::ArrayList<RequestAnswer*> *answers);
		UOSInt GetCAARecord(Data::ArrayList<RequestAnswer*> *answers, Text::CString domain);

		void UpdateDNSAddr(NotNullPtr<const Net::SocketUtil::AddressInfo> serverAddr);

		static UOSInt ParseString(UTF8Char *sbuff, const UInt8 *buff, UOSInt stringOfst, UOSInt endOfst, UTF8Char **sbuffEndOut); //return actEndOfst
		static UOSInt ParseAnswers(const UInt8 *buff, UOSInt dataSize, Data::ArrayList<RequestAnswer*> *answers);
		static RequestAnswer *ParseAnswer(const UInt8 *buff, UOSInt dataSize, UOSInt *index);
		static void FreeAnswers(Data::ArrayList<RequestAnswer*> *answers);
		static void FreeAnswer(RequestAnswer *answer);
		static UInt32 GetResponseTTL(const UInt8 *buff, UOSInt buffSize);
		static UOSInt SkipString(const UInt8 *buff, UOSInt stringOfst, UOSInt endOfst);
		static Text::CString TypeGetID(UInt16 type);
	};
}
#endif
