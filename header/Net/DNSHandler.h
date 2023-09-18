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
		DNSHandler(NotNullPtr<Net::SocketFactory> sockf, NotNullPtr<const Net::SocketUtil::AddressInfo> serverAddr, NotNullPtr<IO::LogTool> log);
		~DNSHandler();
	
		Bool GetByDomainNamev4(NotNullPtr<Net::SocketUtil::AddressInfo> addr, Text::CStringNN domain);
		Bool GetByDomainNamev6(NotNullPtr<Net::SocketUtil::AddressInfo> addr, Text::CStringNN domain);
		UOSInt GetByDomainNamesv4(Data::DataArray<Net::SocketUtil::AddressInfo> addrs, Text::CStringNN domain);
		UOSInt GetByDomainNamesv6(Data::DataArray<Net::SocketUtil::AddressInfo> addrs, Text::CStringNN domain);

		Bool AddHost(NotNullPtr<const Net::SocketUtil::AddressInfo> addr, const UTF8Char *domain, UOSInt domainLen);
		void UpdateDNSAddr(NotNullPtr<const Net::SocketUtil::AddressInfo> serverAddr);
	};
}
#endif
