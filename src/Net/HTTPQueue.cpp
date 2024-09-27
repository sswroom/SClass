#include "Stdafx.h"
#include "Net/HTTPQueue.h"
#include "Sync/MutexUsage.h"
#include "Sync/SimpleThread.h"
#include "Text/URLString.h"

Net::HTTPQueue::HTTPQueue(NN<Net::TCPClientFactory> clif, Optional<Net::SSLEngine> ssl)
{
	this->clif = clif;
	this->ssl = ssl;
}

Net::HTTPQueue::~HTTPQueue()
{
	this->Clear();
}

NN<Net::HTTPClient> Net::HTTPQueue::MakeRequest(Text::CStringNN url, Net::WebUtil::RequestMethod method, Bool noShutdown)
{
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	sptr = Text::URLString::GetURLDomain(sbuff, url, 0);
	Bool found = false;;
	NN<DomainStatus> status;
	NN<Net::HTTPClient> cli;
	while (true)
	{
		Sync::MutexUsage mutUsage(this->statusMut);
		if (this->statusMap.Get(CSTRP(sbuff, sptr)).SetTo(status))
		{
			if (status->req1 == 0)
			{
				cli = Net::HTTPClient::CreateConnect(this->clif, this->ssl, url, method, noShutdown);
				status->req1 = cli.Ptr();
				found = true;
			}
			else if (status->req2 == 0)
			{
				cli = Net::HTTPClient::CreateConnect(this->clif, this->ssl, url, method, noShutdown);
				status->req2 = cli.Ptr();
				found = true;
			}
		}
		else
		{
			status = MemAllocNN(DomainStatus);
			status->req1 = 0;
			status->req2 = 0;
			cli = Net::HTTPClient::CreateConnect(this->clif, this->ssl, url, method, noShutdown);
			status->req1 = cli.Ptr();
			this->statusMap.Put(CSTRP(sbuff, sptr), status);
			found = true;
		}
		mutUsage.EndUse();
		if (found)
			break;
		this->statusEvt.Wait(1000);
	}
	return cli;
}

void Net::HTTPQueue::EndRequest(NN<Net::HTTPClient> cli)
{
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	NN<DomainStatus> status;
	NN<Text::String> url = Text::String::OrEmpty(cli->GetURL());
	sptr = Text::URLString::GetURLDomain(sbuff, url->ToCString(), 0);

	Sync::MutexUsage mutUsage(this->statusMut);
	if (this->statusMap.Get(CSTRP(sbuff, sptr)).SetTo(status))
	{
		if (status->req1 == cli.Ptr())
		{
			status->req1 = 0;
		}
		else if (status->req2 == cli.Ptr())
		{
			status->req2 = 0;
		}
		cli.Delete();
		if (status->req1 == 0 && status->req2 == 0)
		{
			MemFreeNN(status);
			this->statusMap.Remove(CSTRP(sbuff, sptr));
		}
		this->statusEvt.Set();
	}
	else
	{
		cli.Delete();
	}
	mutUsage.EndUse();
}

void Net::HTTPQueue::Clear()
{
	NN<const Data::ArrayListNN<DomainStatus>> statusList;
	NN<DomainStatus> status;
	UOSInt i;

	Sync::MutexUsage mutUsage(this->statusMut);
	statusList = this->statusMap.GetValues();
	i = statusList->GetCount();
	while (i-- > 0)
	{
		status = statusList->GetItemNoCheck(i);
		if (status->req1)
		{
			status->req1->Close();
		}
		if (status->req2)
		{
			status->req2->Close();
		}
	}
	mutUsage.EndUse();
	while (this->statusMap.GetCount() > 0)
	{
		Sync::SimpleThread::Sleep(10);
	}
}
