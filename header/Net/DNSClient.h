//require dnsapi.lib
#ifndef _SM_NET_DNSCLIENT
#define _SM_NET_DNSCLIENT
#include "Data/UInt32Map.h"
#include "Net/UDPServer.h"

namespace Net
{
	class DNSClient
	{
	public:
		typedef struct
		{
			Text::String *name;
			UInt16 recType;
			UInt16 recClass;
			UInt32 ttl;
			Text::String *rd;
			Net::SocketUtil::AddressInfo addr;
			UInt16 priority;
		} RequestAnswer;

	private:
		typedef struct
		{
			UInt8 respBuff[512];
			UOSInt respSize;
			Sync::Event *finEvt;
		} RequestStatus;

		Net::UDPServer *svr;
		Net::SocketFactory *sockf;
		UInt32 lastID;
		Net::SocketUtil::AddressInfo serverAddr;

		Sync::Mutex *reqMut;
		Data::UInt32Map<RequestStatus*> *reqMap;

		static void __stdcall PacketHdlr(const Net::SocketUtil::AddressInfo *addr, UInt16 port, const UInt8 *buff, UOSInt dataSize, void *userData);
		RequestStatus *NewReq(UInt32 id);
		void DelReq(UInt32 id);
		UInt32 NextId();
	public:
		DNSClient(Net::SocketFactory *sockf, const Net::SocketUtil::AddressInfo *serverAddr);
		~DNSClient();

		UOSInt GetByEmailDomainName(Data::ArrayList<RequestAnswer*> *answers, const UTF8Char *domain, UOSInt domainLen);
		UOSInt GetByDomainName(Data::ArrayList<RequestAnswer*> *answers, const UTF8Char *domain, UOSInt domainLen);
		UOSInt GetByType(Data::ArrayList<RequestAnswer*> *answers, const UTF8Char *domain, UOSInt domainLen, UInt16 type);
		UOSInt GetByIPv4Name(Data::ArrayList<RequestAnswer*> *answers, UInt32 ip);
		UOSInt GetByAddrName(Data::ArrayList<RequestAnswer*> *answers, const Net::SocketUtil::AddressInfo *addr);
		UOSInt GetServerName(Data::ArrayList<RequestAnswer*> *answers);

		void UpdateDNSAddr(const Net::SocketUtil::AddressInfo *serverAddr);

		static UOSInt ParseString(UTF8Char *sbuff, const UInt8 *buff, UOSInt stringOfst, UOSInt endOfst); //return actEndOfst
		static UOSInt ParseAnswers(const UInt8 *buff, UOSInt dataSize, Data::ArrayList<RequestAnswer*> *answers);
		static RequestAnswer *ParseAnswer(const UInt8 *buff, UOSInt dataSize, UOSInt *index);
		static void FreeAnswers(Data::ArrayList<RequestAnswer*> *answers);
		static void FreeAnswer(RequestAnswer *answer);
		static UInt32 GetResponseTTL(const UInt8 *buff, UOSInt buffSize);
		static UOSInt SkipString(const UInt8 *buff, UOSInt stringOfst, UOSInt endOfst);
		static const UTF8Char *TypeGetID(UInt16 type);
	};
}
#endif
