#include "Stdafx.h"
#include "Net/DNSHandler.h"
#include "Sync/MutexUsage.h"

Net::DNSHandler::DNSHandler(Net::SocketFactory *sockf, const Net::SocketUtil::AddressInfo *serverAddr)
{
	NEW_CLASS(this->dnsCli, Net::DNSClient(sockf, serverAddr));
	NEW_CLASS(this->clk, Manage::HiResClock());
	NEW_CLASS(this->reqv4Mut, Sync::Mutex());
	NEW_CLASS(this->reqv4Map, Data::ICaseBTreeUTF8Map<DomainStatus*>());
	NEW_CLASS(this->reqv6Mut, Sync::Mutex());
	NEW_CLASS(this->reqv6Map, Data::ICaseBTreeUTF8Map<DomainStatus*>());
}

Net::DNSHandler::~DNSHandler()
{
	UOSInt i;
	UOSInt j;
	DomainStatus **arr = this->reqv4Map->ToArray(&j);
	i = 0;
	while (i < j)
	{
		arr[i]->domain->Release();
		if (arr[i]->timeout < 0)
		{
//			Net::DNSClient::RequestAnswer *ans;
//			ans = arr[i]->answers->GetItem(0);
//			MemFree(ans);
		}
		else
		{
			this->dnsCli->FreeAnswers(arr[i]->answers);
		}
		DEL_CLASS(arr[i]->answers);
		MemFree(arr[i]);
		i++;
	}
	MemFree(arr);
	DEL_CLASS(this->reqv4Map);
	DEL_CLASS(this->reqv4Mut);

	arr = this->reqv6Map->ToArray(&j);
	i = 0;
	while (i < j)
	{
		arr[i]->domain->Release();
		if (arr[i]->timeout < 0)
		{
		}
		else
		{
			this->dnsCli->FreeAnswers(arr[i]->answers);
		}
		DEL_CLASS(arr[i]->answers);
		MemFree(arr[i]);
		i++;
	}
	MemFree(arr);
	DEL_CLASS(this->reqv6Map);
	DEL_CLASS(this->reqv6Mut);

	DEL_CLASS(this->clk);
	DEL_CLASS(this->dnsCli);
}

Bool Net::DNSHandler::GetByDomainNamev4(Net::SocketUtil::AddressInfo *addr, const UTF8Char *domain, UOSInt domainLen)
{
	DomainStatus *dnsStat;
	Double t;
	UOSInt i;
	UOSInt j;
	Net::DNSClient::RequestAnswer *ans;
	Sync::MutexUsage mutUsage(this->reqv4Mut);
	dnsStat = this->reqv4Map->Get({domain, domainLen});
	if (dnsStat)
	{
		if (dnsStat->timeout < 0)
		{
			*addr = dnsStat->addr;
			mutUsage.EndUse();
			return true;
		}
		else
		{
			t = this->clk->GetTimeDiff();
			if (t > dnsStat->timeout)
			{
				Data::ArrayList<Net::DNSClient::RequestAnswer*> ansArr;
				mutUsage.EndUse();

				j = this->dnsCli->GetByType(&ansArr, domain, domainLen, 1);
				mutUsage.BeginUse();
				this->dnsCli->FreeAnswers(dnsStat->answers);
				dnsStat->answers->Clear();
				dnsStat->answers->AddAll(&ansArr);
			}
			i = 0;
			j = dnsStat->answers->GetCount();
			while (i < j)
			{
				ans = dnsStat->answers->GetItem(i);
				if (ans->addr.addrType != Net::AddrType::Unknown)
				{
					*addr = ans->addr;
					dnsStat->timeout = t + ans->ttl;
					mutUsage.EndUse();
					return true;
				}
				i++;
			}
			mutUsage.EndUse();
			return false;
		}
	}
	mutUsage.EndUse();
	t = this->clk->GetTimeDiff();
	dnsStat = MemAlloc(DomainStatus, 1);
	dnsStat->domain = Text::String::New(domain, domainLen);
	dnsStat->timeout = t;
	NEW_CLASS(dnsStat->answers, Data::ArrayList<Net::DNSClient::RequestAnswer*>());
	j = this->dnsCli->GetByType(dnsStat->answers, domain, domainLen, 1);

	Bool succ = false;
	i = 0;
	j = dnsStat->answers->GetCount();
	while (i < j)
	{
		ans = dnsStat->answers->GetItem(i);
		if (ans->addr.addrType != Net::AddrType::Unknown)
		{
			*addr = ans->addr;
			dnsStat->timeout = t + ans->ttl;
			succ = true;
			break;
		}
		i++;
	}
	mutUsage.BeginUse();
	dnsStat = this->reqv4Map->Put({domain, domainLen}, dnsStat);
	mutUsage.EndUse();
	if (dnsStat)
	{
		dnsStat->domain->Release();
		this->dnsCli->FreeAnswers(dnsStat->answers);
		DEL_CLASS(dnsStat->answers);
		MemFree(dnsStat);
	}
	return succ;
}

Bool Net::DNSHandler::GetByDomainNamev6(Net::SocketUtil::AddressInfo *addr, const UTF8Char *domain, UOSInt domainLen)
{
	DomainStatus *dnsStat;
	Double t;
	UOSInt i;
	UOSInt j;
	Net::DNSClient::RequestAnswer *ans;
	Sync::MutexUsage mutUsage(this->reqv6Mut);
	dnsStat = this->reqv6Map->Get({domain, domainLen});
	if (dnsStat)
	{
		if (dnsStat->timeout < 0)
		{
			*addr = dnsStat->addr;
			mutUsage.EndUse();
			return true;
		}
		else
		{
			t = this->clk->GetTimeDiff();
			if (t > dnsStat->timeout)
			{
				Data::ArrayList<Net::DNSClient::RequestAnswer*> ansArr;
				mutUsage.EndUse();

				j = this->dnsCli->GetByType(&ansArr, domain, domainLen, 28);
				mutUsage.BeginUse();
				this->dnsCli->FreeAnswers(dnsStat->answers);
				dnsStat->answers->Clear();
				dnsStat->answers->AddAll(&ansArr);
			}
			i = 0;
			j = dnsStat->answers->GetCount();
			while (i < j)
			{
				ans = dnsStat->answers->GetItem(i);
				if (ans->addr.addrType != Net::AddrType::Unknown)
				{
					*addr = ans->addr;
					dnsStat->timeout = t + ans->ttl;
					mutUsage.EndUse();
					return true;
				}
				i++;
			}
			mutUsage.EndUse();
			return false;
		}
	}
	mutUsage.EndUse();
	t = this->clk->GetTimeDiff();
	dnsStat = MemAlloc(DomainStatus, 28);
	dnsStat->domain = Text::String::New(domain, domainLen);
	dnsStat->timeout = t;
	NEW_CLASS(dnsStat->answers, Data::ArrayList<Net::DNSClient::RequestAnswer*>());
	j = this->dnsCli->GetByType(dnsStat->answers, domain, domainLen, 1);

	Bool succ = false;
	i = 0;
	j = dnsStat->answers->GetCount();
	while (i < j)
	{
		ans = dnsStat->answers->GetItem(i);
		if (ans->addr.addrType != Net::AddrType::Unknown)
		{
			*addr = ans->addr;
			dnsStat->timeout = t + ans->ttl;
			succ = true;
			break;
		}
		i++;
	}
	mutUsage.BeginUse();
	dnsStat = this->reqv6Map->Put({domain, domainLen}, dnsStat);
	mutUsage.EndUse();
	if (dnsStat)
	{
		dnsStat->domain->Release();
		this->dnsCli->FreeAnswers(dnsStat->answers);
		DEL_CLASS(dnsStat->answers);
		MemFree(dnsStat);
	}
	return succ;
}

Bool Net::DNSHandler::AddHost(const Net::SocketUtil::AddressInfo *addr, const UTF8Char *domain, UOSInt domainLen)
{
	DomainStatus *dnsStat;
	if (addr->addrType == Net::AddrType::IPv4)
	{
		Sync::MutexUsage mutUsage(this->reqv4Mut);
		dnsStat = this->reqv4Map->Get({domain, domainLen});
		if (dnsStat)
		{
			dnsStat->timeout = -1;
			dnsStat->addr = *addr;
		}
		else
		{
			dnsStat = MemAlloc(DomainStatus, 1);
			dnsStat->domain = Text::String::New(domain, domainLen);
			dnsStat->timeout = -1;
			dnsStat->addr = *addr;
			NEW_CLASS(dnsStat->answers, Data::ArrayList<Net::DNSClient::RequestAnswer*>());
			this->reqv4Map->Put({domain, domainLen}, dnsStat);
		}
		mutUsage.EndUse();
		return true;
	}
	else if (addr->addrType == Net::AddrType::IPv6)
	{
		Sync::MutexUsage mutUsage(this->reqv6Mut);
		dnsStat = this->reqv6Map->Get({domain, domainLen});
		if (dnsStat)
		{
			dnsStat->timeout = -1;
			dnsStat->addr = *addr;
		}
		else
		{
			dnsStat = MemAlloc(DomainStatus, 1);
			dnsStat->domain = Text::String::New(domain, domainLen);
			dnsStat->timeout = -1;
			dnsStat->addr = *addr;
			NEW_CLASS(dnsStat->answers, Data::ArrayList<Net::DNSClient::RequestAnswer*>());
			this->reqv6Map->Put({domain, domainLen}, dnsStat);
		}
		mutUsage.EndUse();
		return true;
	}
	return false;
}

void Net::DNSHandler::UpdateDNSAddr(const Net::SocketUtil::AddressInfo *serverAddr)
{
	this->dnsCli->UpdateDNSAddr(serverAddr);
}
