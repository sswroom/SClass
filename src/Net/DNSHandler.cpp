#include "Stdafx.h"
#include "Net/DNSHandler.h"

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
		Text::StrDelNew(arr[i]->domain);
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
		Text::StrDelNew(arr[i]->domain);
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

Bool Net::DNSHandler::GetByDomainNamev4(Net::SocketUtil::AddressInfo *addr, const UTF8Char *domain)
{
	DomainStatus *dnsStat;
	Double t;
	OSInt i;
	OSInt j;
	Net::DNSClient::RequestAnswer *ans;
	this->reqv4Mut->Lock();
	dnsStat = this->reqv4Map->Get(domain);
	if (dnsStat)
	{
		if (dnsStat->timeout < 0)
		{
			*addr = dnsStat->addr;
			this->reqv4Mut->Unlock();
			return true;
		}
		else
		{
			t = this->clk->GetTimeDiff();
			if (t > dnsStat->timeout)
			{
				Data::ArrayList<Net::DNSClient::RequestAnswer*> ansArr;
				this->reqv4Mut->Unlock();

				j = this->dnsCli->GetByType(&ansArr, domain, 1);
				this->reqv4Mut->Lock();
				this->dnsCli->FreeAnswers(dnsStat->answers);
				dnsStat->answers->Clear();
				dnsStat->answers->AddRange(&ansArr);
			}
			i = 0;
			j = dnsStat->answers->GetCount();
			while (i < j)
			{
				ans = dnsStat->answers->GetItem(i);
				if (ans->addr.addrType != Net::SocketUtil::AT_UNKNOWN)
				{
					*addr = ans->addr;
					dnsStat->timeout = t + ans->ttl;
					this->reqv4Mut->Unlock();
					return true;
				}
				i++;
			}
			this->reqv4Mut->Unlock();
			return false;
		}
	}
	this->reqv4Mut->Unlock();
	t = this->clk->GetTimeDiff();
	dnsStat = MemAlloc(DomainStatus, 1);
	dnsStat->domain = Text::StrCopyNew(domain);
	dnsStat->timeout = t;
	NEW_CLASS(dnsStat->answers, Data::ArrayList<Net::DNSClient::RequestAnswer*>());
	j = this->dnsCli->GetByType(dnsStat->answers, domain, 1);

	Bool succ = false;
	i = 0;
	j = dnsStat->answers->GetCount();
	while (i < j)
	{
		ans = dnsStat->answers->GetItem(i);
		if (ans->addr.addrType != Net::SocketUtil::AT_UNKNOWN)
		{
			*addr = ans->addr;
			dnsStat->timeout = t + ans->ttl;
			succ = true;
			break;
		}
		i++;
	}
	this->reqv4Mut->Lock();
	dnsStat = this->reqv4Map->Put(domain, dnsStat);
	this->reqv4Mut->Unlock();
	if (dnsStat)
	{
		Text::StrDelNew(dnsStat->domain);
		this->dnsCli->FreeAnswers(dnsStat->answers);
		DEL_CLASS(dnsStat->answers);
		MemFree(dnsStat);
	}
	return succ;
}

Bool Net::DNSHandler::GetByDomainNamev6(Net::SocketUtil::AddressInfo *addr, const UTF8Char *domain)
{
	DomainStatus *dnsStat;
	Double t;
	OSInt i;
	OSInt j;
	Net::DNSClient::RequestAnswer *ans;
	this->reqv6Mut->Lock();
	dnsStat = this->reqv6Map->Get(domain);
	if (dnsStat)
	{
		if (dnsStat->timeout < 0)
		{
			*addr = dnsStat->addr;
			this->reqv6Mut->Unlock();
			return true;
		}
		else
		{
			t = this->clk->GetTimeDiff();
			if (t > dnsStat->timeout)
			{
				Data::ArrayList<Net::DNSClient::RequestAnswer*> ansArr;
				this->reqv6Mut->Unlock();

				j = this->dnsCli->GetByType(&ansArr, domain, 28);
				this->reqv6Mut->Lock();
				this->dnsCli->FreeAnswers(dnsStat->answers);
				dnsStat->answers->Clear();
				dnsStat->answers->AddRange(&ansArr);
			}
			i = 0;
			j = dnsStat->answers->GetCount();
			while (i < j)
			{
				ans = dnsStat->answers->GetItem(i);
				if (ans->addr.addrType != Net::SocketUtil::AT_UNKNOWN)
				{
					*addr = ans->addr;
					dnsStat->timeout = t + ans->ttl;
					this->reqv6Mut->Unlock();
					return true;
				}
				i++;
			}
			this->reqv6Mut->Unlock();
			return false;
		}
	}
	this->reqv6Mut->Unlock();
	t = this->clk->GetTimeDiff();
	dnsStat = MemAlloc(DomainStatus, 28);
	dnsStat->domain = Text::StrCopyNew(domain);
	dnsStat->timeout = t;
	NEW_CLASS(dnsStat->answers, Data::ArrayList<Net::DNSClient::RequestAnswer*>());
	j = this->dnsCli->GetByType(dnsStat->answers, domain, 1);

	Bool succ = false;
	i = 0;
	j = dnsStat->answers->GetCount();
	while (i < j)
	{
		ans = dnsStat->answers->GetItem(i);
		if (ans->addr.addrType != Net::SocketUtil::AT_UNKNOWN)
		{
			*addr = ans->addr;
			dnsStat->timeout = t + ans->ttl;
			succ = true;
			break;
		}
		i++;
	}
	this->reqv6Mut->Lock();
	dnsStat = this->reqv6Map->Put(domain, dnsStat);
	this->reqv6Mut->Unlock();
	if (dnsStat)
	{
		Text::StrDelNew(dnsStat->domain);
		this->dnsCli->FreeAnswers(dnsStat->answers);
		DEL_CLASS(dnsStat->answers);
		MemFree(dnsStat);
	}
	return succ;
}

Bool Net::DNSHandler::AddHost(const Net::SocketUtil::AddressInfo *addr, const UTF8Char *domain)
{
	DomainStatus *dnsStat;
	if (addr->addrType == Net::SocketUtil::AT_IPV4)
	{
		this->reqv4Mut->Lock();
		dnsStat = this->reqv4Map->Get(domain);
		if (dnsStat)
		{
			dnsStat->timeout = -1;
			dnsStat->addr = *addr;
		}
		else
		{
			dnsStat = MemAlloc(DomainStatus, 1);
			dnsStat->domain = Text::StrCopyNew(domain);
			dnsStat->timeout = -1;
			dnsStat->addr = *addr;
			NEW_CLASS(dnsStat->answers, Data::ArrayList<Net::DNSClient::RequestAnswer*>());
			this->reqv4Map->Put(domain, dnsStat);
		}
		this->reqv4Mut->Unlock();
		return true;
	}
	else if (addr->addrType == Net::SocketUtil::AT_IPV6)
	{
		this->reqv6Mut->Lock();
		dnsStat = this->reqv6Map->Get(domain);
		if (dnsStat)
		{
			dnsStat->timeout = -1;
			dnsStat->addr = *addr;
		}
		else
		{
			dnsStat = MemAlloc(DomainStatus, 1);
			dnsStat->domain = Text::StrCopyNew(domain);
			dnsStat->timeout = -1;
			dnsStat->addr = *addr;
			NEW_CLASS(dnsStat->answers, Data::ArrayList<Net::DNSClient::RequestAnswer*>());
			this->reqv6Map->Put(domain, dnsStat);
		}
		this->reqv6Mut->Unlock();
		return true;
	}
	return false;
}

void Net::DNSHandler::UpdateDNSAddr(const Net::SocketUtil::AddressInfo *serverAddr)
{
	this->dnsCli->UpdateDNSAddr(serverAddr);
}
