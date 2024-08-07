#include "Stdafx.h"
#include "Net/DNSHandler.h"
#include "Sync/MutexUsage.h"

Net::DNSHandler::DNSHandler(NN<Net::SocketFactory> sockf, NN<const Net::SocketUtil::AddressInfo> serverAddr, NN<IO::LogTool> log) : dnsCli(sockf, serverAddr, log)
{
}

Net::DNSHandler::~DNSHandler()
{
	UOSInt i;
	UOSInt j;
	DomainStatus **arr = this->reqv4Map.ToArray(j);
	i = 0;
	while (i < j)
	{
		arr[i]->domain->Release();
		if (arr[i]->timeout.IsNull())
		{
//			Net::DNSClient::RequestAnswer *ans;
//			ans = arr[i]->answers->GetItem(0);
//			MemFree(ans);
		}
		else
		{
			this->dnsCli.FreeAnswers(arr[i]->answers);
		}
		DEL_CLASS(arr[i]);
		i++;
	}
	MemFree(arr);

	arr = this->reqv6Map.ToArray(j);
	i = 0;
	while (i < j)
	{
		arr[i]->domain->Release();
		if (arr[i]->timeout.IsNull())
		{
		}
		else
		{
			this->dnsCli.FreeAnswers(arr[i]->answers);
		}
		DEL_CLASS(arr[i]);
		i++;
	}
	MemFree(arr);
}

Bool Net::DNSHandler::GetByDomainNamev4(NN<Net::SocketUtil::AddressInfo> addr, Text::CStringNN domain)
{
	DomainStatus *dnsStat;
	DomainStatus *newDnsStat;
	UOSInt i;
	UOSInt j;
	Data::Timestamp currTime;
	NN<Net::DNSClient::RequestAnswer> ans;
	Sync::MutexUsage mutUsage(this->reqv4Mut);
	dnsStat = this->reqv4Map.Get(domain);
	if (dnsStat)
	{
		if (dnsStat->timeout.IsNull())
		{
			addr.SetVal(dnsStat->addr);
			return true;
		}
		else
		{
			Bool newReq = false;
			currTime = Data::Timestamp::UtcNow();
			if (currTime > dnsStat->timeout)
			{
				Data::ArrayListNN<Net::DNSClient::RequestAnswer> ansArr;
				mutUsage.EndUse();

				j = this->dnsCli.GetByType(ansArr, domain, 1);
				mutUsage.BeginUse();
				this->dnsCli.FreeAnswers(dnsStat->answers);
				dnsStat->answers.Clear();
				dnsStat->answers.AddAll(ansArr);
				newReq = true;
			}
			i = 0;
			j = dnsStat->answers.GetCount();
			while (i < j)
			{
				ans = dnsStat->answers.GetItemNoCheck(i);
				if (ans->addr.addrType != Net::AddrType::Unknown && ans->recType == 1)
				{
					addr.SetVal(ans->addr);
					if (newReq)
					{
						dnsStat->timeout = currTime.AddSecond((OSInt)ans->ttl);
					}
					return true;
				}
				i++;
			}
			return false;
		}
	}
	mutUsage.EndUse();
	NEW_CLASS(dnsStat, DomainStatus());
	dnsStat->domain = Text::String::New(domain);
	dnsStat->timeout = Data::Timestamp::UtcNow();
	j = this->dnsCli.GetByType(dnsStat->answers, domain, 1);

	Bool succ = false;
	i = 0;
	j = dnsStat->answers.GetCount();
	while (i < j)
	{
		ans = dnsStat->answers.GetItemNoCheck(i);
		if (ans->addr.addrType != Net::AddrType::Unknown && ans->recType == 1)
		{
			addr.SetVal(ans->addr);
			dnsStat->timeout = dnsStat->timeout.AddSecond((OSInt)ans->ttl);
			succ = true;
			break;
		}
		i++;
	}
	mutUsage.BeginUse();
	newDnsStat = this->reqv4Map.Get(domain);
	if (newDnsStat)
	{
		dnsStat->domain->Release();
		this->dnsCli.FreeAnswers(dnsStat->answers);
		DEL_CLASS(dnsStat);
	}
	else
	{
		this->reqv4Map.Put(domain, dnsStat);
	}
	return succ;
}

Bool Net::DNSHandler::GetByDomainNamev6(NN<Net::SocketUtil::AddressInfo> addr, Text::CStringNN domain)
{
	DomainStatus *newDnsStat;
	DomainStatus *dnsStat;
	UOSInt i;
	UOSInt j;
	NN<Net::DNSClient::RequestAnswer> ans;
	Data::Timestamp currTime;
	Sync::MutexUsage mutUsage(this->reqv6Mut);
	dnsStat = this->reqv6Map.Get(domain);
	if (dnsStat)
	{
		if (dnsStat->timeout.IsNull())
		{
			addr.SetVal(dnsStat->addr);
			return true;
		}
		else
		{
			Bool newReq = false;
			currTime = Data::Timestamp::UtcNow();
			if (currTime > dnsStat->timeout)
			{
				Data::ArrayListNN<Net::DNSClient::RequestAnswer> ansArr;
				mutUsage.EndUse();

				j = this->dnsCli.GetByType(ansArr, domain, 28);
				mutUsage.BeginUse();
				this->dnsCli.FreeAnswers(dnsStat->answers);
				dnsStat->answers.Clear();
				dnsStat->answers.AddAll(ansArr);
				newReq = true;
			}
			i = 0;
			j = dnsStat->answers.GetCount();
			while (i < j)
			{
				ans = dnsStat->answers.GetItemNoCheck(i);
				if (ans->addr.addrType != Net::AddrType::Unknown && ans->recType == 28)
				{
					addr.SetVal(ans->addr);
					if (newReq)
					{
						dnsStat->timeout = currTime.AddSecond((OSInt)ans->ttl);
					}
					return true;
				}
				i++;
			}
			return false;
		}
	}
	mutUsage.EndUse();
	currTime = Data::Timestamp::UtcNow();
	NEW_CLASS(dnsStat, DomainStatus());
	dnsStat->domain = Text::String::New(domain);
	dnsStat->timeout = currTime;
	j = this->dnsCli.GetByType(dnsStat->answers, domain, 1);

	Bool succ = false;
	i = 0;
	j = dnsStat->answers.GetCount();
	while (i < j)
	{
		ans = dnsStat->answers.GetItemNoCheck(i);
		if (ans->addr.addrType != Net::AddrType::Unknown && ans->recType == 28)
		{
			addr.SetVal(ans->addr);
			dnsStat->timeout = currTime.AddSecond((OSInt)ans->ttl);
			break;
		}
		i++;
	}
	mutUsage.BeginUse();
	newDnsStat = this->reqv6Map.Get(domain);
	if (newDnsStat)
	{
		dnsStat->domain->Release();
		this->dnsCli.FreeAnswers(dnsStat->answers);
		DEL_CLASS(dnsStat);
	}
	else
	{
		this->reqv6Map.Put(domain, dnsStat);
	}
	mutUsage.EndUse();
	return succ;
}

UOSInt Net::DNSHandler::GetByDomainNamesv4(Data::DataArray<Net::SocketUtil::AddressInfo> addrs, Text::CStringNN domain)
{
	if (addrs.GetCount() == 0)
		return 0;

	DomainStatus *newDnsStat;
	DomainStatus *dnsStat;
	UOSInt i;
	UOSInt j;
	Data::Timestamp currTime;
	NN<Net::DNSClient::RequestAnswer> ans;
	UOSInt ret = 0;
	Sync::MutexUsage mutUsage(this->reqv4Mut);
	dnsStat = this->reqv4Map.Get(domain);
	if (dnsStat)
	{
		if (dnsStat->timeout.IsNull())
		{
			addrs[0] = dnsStat->addr;
			return 1;
		}
		else
		{
			Bool newReq = false;
			currTime = Data::Timestamp::UtcNow();
			if (currTime > dnsStat->timeout)
			{
				Data::ArrayListNN<Net::DNSClient::RequestAnswer> ansArr;
				mutUsage.EndUse();

				j = this->dnsCli.GetByType(ansArr, domain, 1);
				mutUsage.BeginUse();
				this->dnsCli.FreeAnswers(dnsStat->answers);
				dnsStat->answers.Clear();
				dnsStat->answers.AddAll(ansArr);
				newReq = true;
			}
			i = 0;
			j = dnsStat->answers.GetCount();
			while (i < j)
			{
				ans = dnsStat->answers.GetItemNoCheck(i);
				if (ans->addr.addrType != Net::AddrType::Unknown && ans->recType == 1)
				{
					addrs[ret] = ans->addr;
					if (newReq)
					{
						dnsStat->timeout = currTime.AddSecond((OSInt)ans->ttl);
					}
					ret++;
					if (ret >= addrs.GetCount())
					{
						return ret;
					}
				}
				i++;
			}
			return ret;
		}
	}
	mutUsage.EndUse();
	NEW_CLASS(dnsStat, DomainStatus());
	dnsStat->domain = Text::String::New(domain);
	dnsStat->timeout = Data::Timestamp::UtcNow();
	j = this->dnsCli.GetByType(dnsStat->answers, domain, 1);

	i = 0;
	j = dnsStat->answers.GetCount();
	while (i < j)
	{
		ans = dnsStat->answers.GetItemNoCheck(i);
		if (ans->addr.addrType != Net::AddrType::Unknown && ans->recType == 1)
		{
			addrs[ret] = ans->addr;
			dnsStat->timeout = dnsStat->timeout.AddSecond((OSInt)ans->ttl);
			ret++;
			if (ret >= addrs.GetCount())
			{
				break;
			}
		}
		i++;
	}
	mutUsage.BeginUse();
	newDnsStat = this->reqv4Map.Get(domain);
	if (newDnsStat)
	{
		dnsStat->domain->Release();
		this->dnsCli.FreeAnswers(dnsStat->answers);
		DEL_CLASS(dnsStat);
	}
	else
	{
		this->reqv4Map.Put(domain, dnsStat);
	}
	mutUsage.EndUse();
	return ret;
}

UOSInt Net::DNSHandler::GetByDomainNamesv6(Data::DataArray<Net::SocketUtil::AddressInfo> addrs, Text::CStringNN domain)
{
	if (addrs.GetCount() == 0)
		return 0;
	DomainStatus *newDnsStat;
	DomainStatus *dnsStat;
	UOSInt i;
	UOSInt j;
	NN<Net::DNSClient::RequestAnswer> ans;
	Data::Timestamp currTime;
	Sync::MutexUsage mutUsage(this->reqv6Mut);
	UOSInt ret = 0;
	dnsStat = this->reqv6Map.Get(domain);
	if (dnsStat)
	{
		if (dnsStat->timeout.IsNull())
		{
			addrs[0] = dnsStat->addr;
			return 1;
		}
		else
		{
			Bool newReq = false;
			currTime = Data::Timestamp::UtcNow();
			if (currTime > dnsStat->timeout)
			{
				Data::ArrayListNN<Net::DNSClient::RequestAnswer> ansArr;
				mutUsage.EndUse();

				j = this->dnsCli.GetByType(ansArr, domain, 28);
				mutUsage.BeginUse();
				this->dnsCli.FreeAnswers(dnsStat->answers);
				dnsStat->answers.Clear();
				dnsStat->answers.AddAll(ansArr);
				newReq = true;
			}
			i = 0;
			j = dnsStat->answers.GetCount();
			while (i < j)
			{
				ans = dnsStat->answers.GetItemNoCheck(i);
				if (ans->addr.addrType != Net::AddrType::Unknown && ans->recType == 28)
				{
					addrs[ret] = ans->addr;
					if (newReq)
					{
						dnsStat->timeout = currTime.AddSecond((OSInt)ans->ttl);
					}
					ret++;
					if (ret >= addrs.GetCount())
						return ret;
				}
				i++;
			}
			return ret;
		}
	}
	mutUsage.EndUse();
	currTime = Data::Timestamp::UtcNow();
	NEW_CLASS(dnsStat, DomainStatus());
	dnsStat->domain = Text::String::New(domain);
	dnsStat->timeout = currTime;
	j = this->dnsCli.GetByType(dnsStat->answers, domain, 28);

	i = 0;
	j = dnsStat->answers.GetCount();
	while (i < j)
	{
		ans = dnsStat->answers.GetItemNoCheck(i);
		if (ans->addr.addrType != Net::AddrType::Unknown && ans->recType == 28)
		{
			addrs[ret] = ans->addr;
			dnsStat->timeout = currTime.AddSecond((OSInt)ans->ttl);
			ret++;
			if (ret >= addrs.GetCount())
				break;
		}
		i++;
	}
	mutUsage.BeginUse();
	newDnsStat = this->reqv6Map.Get(domain);
	if (newDnsStat)
	{
		dnsStat->domain->Release();
		this->dnsCli.FreeAnswers(dnsStat->answers);
		DEL_CLASS(dnsStat);
	}
	else
	{
		dnsStat = this->reqv6Map.Put(domain, dnsStat);
	}
	mutUsage.EndUse();
	return ret;
}

Bool Net::DNSHandler::AddHost(NN<const Net::SocketUtil::AddressInfo> addr, Text::CStringNN domain)
{
	DomainStatus *dnsStat;
	if (addr->addrType == Net::AddrType::IPv4)
	{
		Sync::MutexUsage mutUsage(this->reqv4Mut);
		dnsStat = this->reqv4Map.Get(domain);
		if (dnsStat)
		{
			dnsStat->timeout = Data::Timestamp(0);
			dnsStat->addr = addr.Ptr()[0];
		}
		else
		{
			NEW_CLASS(dnsStat, DomainStatus());
			dnsStat->domain = Text::String::New(domain);
			dnsStat->timeout = Data::Timestamp(0);
			dnsStat->addr = addr.Ptr()[0];
			this->reqv4Map.Put(domain, dnsStat);
		}
		return true;
	}
	else if (addr->addrType == Net::AddrType::IPv6)
	{
		Sync::MutexUsage mutUsage(this->reqv6Mut);
		dnsStat = this->reqv6Map.Get(domain);
		if (dnsStat)
		{
			dnsStat->timeout = Data::Timestamp(0);
			dnsStat->addr = addr.Ptr()[0];
		}
		else
		{
			NEW_CLASS(dnsStat, DomainStatus());
			dnsStat->domain = Text::String::New(domain);
			dnsStat->timeout = Data::Timestamp(0);
			dnsStat->addr = addr.Ptr()[0];
			this->reqv6Map.Put(domain, dnsStat);
		}
		return true;
	}
	return false;
}

void Net::DNSHandler::UpdateDNSAddr(NN<const Net::SocketUtil::AddressInfo> serverAddr)
{
	this->dnsCli.UpdateDNSAddr(serverAddr);
}
