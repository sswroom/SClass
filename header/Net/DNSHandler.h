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
		typedef struct
		{
			Text::String *domain;
			Double timeout;
			Data::ArrayList<Net::DNSClient::RequestAnswer*> *answers;
			Net::SocketUtil::AddressInfo addr;
		} DomainStatus;
	private:
		Net::DNSClient *dnsCli;
		Sync::Mutex *reqv4Mut;
		Data::ICaseBTreeUTF8Map<DomainStatus*> *reqv4Map;
		Sync::Mutex *reqv6Mut;
		Data::ICaseBTreeUTF8Map<DomainStatus*> *reqv6Map;
		Manage::HiResClock *clk;

	public:
		DNSHandler(Net::SocketFactory *sockf, const Net::SocketUtil::AddressInfo *serverAddr);
		~DNSHandler();
	
		Bool GetByDomainNamev4(Net::SocketUtil::AddressInfo *addr, const UTF8Char *domain);
		Bool GetByDomainNamev6(Net::SocketUtil::AddressInfo *addr, const UTF8Char *domain);

		Bool AddHost(const Net::SocketUtil::AddressInfo *addr, const UTF8Char *domain);
		void UpdateDNSAddr(const Net::SocketUtil::AddressInfo *serverAddr);
	};
}
#endif
