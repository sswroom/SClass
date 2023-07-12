#ifndef _SM_NET_DNSHANDLER
#define _SM_NET_DNSHANDLER
#include "Data/ICaseBTreeUTF8Map.h"
#include "Manage/HiResClock.h"
#include "Net/DNSClient.h"
#include "Sync/Mutex.h"

namespace Net
{
	class DNSHandler
	{
	private:
		class DomainStatus
		{
		public:
			NotNullPtr<Text::String> domain;
			Data::Timestamp timeout;
			Data::ArrayList<Net::DNSClient::RequestAnswer*> answers;
			Net::SocketUtil::AddressInfo addr;
		};
	private:
		Net::DNSClient dnsCli;
		Sync::Mutex reqv4Mut;
		Data::ICaseBTreeUTF8Map<DomainStatus*> reqv4Map;
		Sync::Mutex reqv6Mut;
		Data::ICaseBTreeUTF8Map<DomainStatus*> reqv6Map;

	public:
		DNSHandler(Net::SocketFactory *sockf, const Net::SocketUtil::AddressInfo *serverAddr);
		~DNSHandler();
	
		Bool GetByDomainNamev4(Net::SocketUtil::AddressInfo *addr, Text::CString domain);
		Bool GetByDomainNamev6(Net::SocketUtil::AddressInfo *addr, Text::CString domain);
		UOSInt GetByDomainNamesv4(Net::SocketUtil::AddressInfo *addrs, Text::CString domain, UOSInt maxCnt);
		UOSInt GetByDomainNamesv6(Net::SocketUtil::AddressInfo *addrs, Text::CString domain, UOSInt maxCnt);

		Bool AddHost(const Net::SocketUtil::AddressInfo *addr, const UTF8Char *domain, UOSInt domainLen);
		void UpdateDNSAddr(const Net::SocketUtil::AddressInfo *serverAddr);
	};
}
#endif
