#ifndef _SM_NET_DNSHANDLER
#define _SM_NET_DNSHANDLER
#include "Data/ICaseBTreeStringMapObj.hpp"
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
			NN<Text::String> domain;
			Data::Timestamp timeout;
			Data::ArrayListNN<Net::DNSClient::RequestAnswer> answers;
			Net::SocketUtil::AddressInfo addr;
		};
	private:
		Net::DNSClient dnsCli;
		Sync::Mutex reqv4Mut;
		Data::ICaseBTreeStringMapObj<DomainStatus*> reqv4Map;
		Sync::Mutex reqv6Mut;
		Data::ICaseBTreeStringMapObj<DomainStatus*> reqv6Map;

	public:
		DNSHandler(NN<Net::SocketFactory> sockf, NN<const Net::SocketUtil::AddressInfo> serverAddr, NN<IO::LogTool> log);
		~DNSHandler();
	
		Bool GetByDomainNamev4(NN<Net::SocketUtil::AddressInfo> addr, Text::CStringNN domain);
		Bool GetByDomainNamev6(NN<Net::SocketUtil::AddressInfo> addr, Text::CStringNN domain);
		UOSInt GetByDomainNamesv4(Data::DataArray<Net::SocketUtil::AddressInfo> addrs, Text::CStringNN domain);
		UOSInt GetByDomainNamesv6(Data::DataArray<Net::SocketUtil::AddressInfo> addrs, Text::CStringNN domain);

		Bool AddHost(NN<const Net::SocketUtil::AddressInfo> addr, Text::CStringNN domain);
		void UpdateDNSAddr(NN<const Net::SocketUtil::AddressInfo> serverAddr);
	};
}
#endif
