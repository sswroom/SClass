//require dnsapi.lib
#ifndef _SM_NET_DNSCLIENT
#define _SM_NET_DNSCLIENT
#include "Data/Int32Map.h"
#include "Net/UDPServer.h"

namespace Net
{
	class DNSClient
	{
	public:
		typedef struct
		{
			const UTF8Char *name;
			UInt16 recType;
			UInt16 recClass;
			UInt32 ttl;
			const UTF8Char *rd;
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
		Int32 lastID;
		Net::SocketUtil::AddressInfo serverAddr;

		Sync::Mutex *reqMut;
		Data::Int32Map<RequestStatus*> *reqMap;

		static void __stdcall PacketHdlr(const Net::SocketUtil::AddressInfo *addr, UInt16 port, const UInt8 *buff, UOSInt dataSize, void *userData);
		RequestStatus *NewReq(Int32 id);
		void DelReq(Int32 id);
		Int32 NextId();
	public:
		DNSClient(Net::SocketFactory *sockf, const Net::SocketUtil::AddressInfo *serverAddr);
		~DNSClient();

		UOSInt GetByEmailDomainName(Data::ArrayList<RequestAnswer*> *answers, const UTF8Char *domain);
		UOSInt GetByDomainName(Data::ArrayList<RequestAnswer*> *answers, const UTF8Char *domain);
		UOSInt GetByType(Data::ArrayList<RequestAnswer*> *answers, const UTF8Char *domain, UInt16 type);
		UOSInt GetByIPv4Name(Data::ArrayList<RequestAnswer*> *answers, UInt32 ip);
		UOSInt GetByAddrName(Data::ArrayList<RequestAnswer*> *answers, const Net::SocketUtil::AddressInfo *addr);
		UOSInt GetServerName(Data::ArrayList<RequestAnswer*> *answers);

		void UpdateDNSAddr(const Net::SocketUtil::AddressInfo *serverAddr);

		static OSInt ParseString(UTF8Char *sbuff, const UInt8 *buff, OSInt stringOfst, OSInt endOfst); //return actEndOfst
		static UOSInt ParseAnswers(const UInt8 *buff, UOSInt dataSize, Data::ArrayList<RequestAnswer*> *answers);
		static void FreeAnswers(Data::ArrayList<RequestAnswer*> *answers);
		static Int32 GetResponseTTL(const UInt8 *buff, UOSInt buffSize);
		static OSInt SkipString(const UInt8 *buff, OSInt stringOfst, OSInt endOfst);
		static const UTF8Char *TypeGetID(UInt16 type);
	};
}
#endif
