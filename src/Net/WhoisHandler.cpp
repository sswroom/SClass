#include "Stdafx.h"
#include "MyMemory.h"
#include "Net/WhoisHandler.h"
#include "Sync/MutexUsage.h"

Net::WhoisHandler::WhoisHandler(Net::SocketFactory *sockf) : client(sockf)
{
	WhoisRecord *rec;

	NEW_CLASS(rec, WhoisRecord(Net::SocketUtil::GetIPAddr(CSTR("10.0.0.1"))));
	rec->AddItem(UTF8STRC("inetnum: 10.0.0.0 - 10.255.255.225"));
	rec->AddItem(UTF8STRC("netname: LAN A"));
	rec->AddItem(UTF8STRC("country: UN"));
	this->recordList.Add(rec);

	NEW_CLASS(rec, WhoisRecord(Net::SocketUtil::GetIPAddr(CSTR("127.0.0.1"))));
	rec->AddItem(UTF8STRC("inetnum: 127.0.0.0 - 127.255.255.225"));
	rec->AddItem(UTF8STRC("netname: localhost"));
	rec->AddItem(UTF8STRC("country: UN"));
	this->recordList.Add(rec);

	NEW_CLASS(rec, WhoisRecord(Net::SocketUtil::GetIPAddr(CSTR("172.16.0.1"))));
	rec->AddItem(UTF8STRC("inetnum: 172.16.0.0 - 172.31.255.225"));
	rec->AddItem(UTF8STRC("netname: LAN B"));
	rec->AddItem(UTF8STRC("country: UN"));
	this->recordList.Add(rec);

	NEW_CLASS(rec, WhoisRecord(Net::SocketUtil::GetIPAddr(CSTR("192.168.0.1"))));
	rec->AddItem(UTF8STRC("inetnum: 192.168.0.0 - 192.168.255.225"));
	rec->AddItem(UTF8STRC("netname: LAN C"));
	rec->AddItem(UTF8STRC("country: UN"));
	this->recordList.Add(rec);

	NEW_CLASS(rec, WhoisRecord(Net::SocketUtil::GetIPAddr(CSTR("224.0.0.1"))));
	rec->AddItem(UTF8STRC("NetRange: 224.0.0.0 - 239.255.255.255"));
	rec->AddItem(UTF8STRC("CIDR: 224.0.0.0/4"));
	rec->AddItem(UTF8STRC("NetName: MCAST-NET"));
	rec->AddItem(UTF8STRC("NetHandle: NET-224-0-0-0-1"));
	rec->AddItem(UTF8STRC("NetType: IANA Special Use"));
	rec->AddItem(UTF8STRC("Country: US"));
	this->recordList.Add(rec);
}

Net::WhoisHandler::~WhoisHandler()
{
	UOSInt i = this->recordList.GetCount();
	WhoisRecord *rec;
	while (i-- > 0)
	{
		rec = this->recordList.GetItem(i);
		DEL_CLASS(rec);
	}
}

Net::WhoisRecord *Net::WhoisHandler::RequestIP(UInt32 ip)
{
	UInt32 sortableIP = Net::SocketUtil::IPv4ToSortable(ip);
	UInt32 sortableIP1;
	UInt32 sortableIP2;
	OSInt i;
	OSInt j;
	OSInt k;
	WhoisRecord *rec;
	
	Sync::MutexUsage mutUsage(&this->recordMut);
	i = 0;
	j = (OSInt)this->recordList.GetCount() - 1;
	while (i <= j)
	{
		k = (i + j) >> 1;
		rec = this->recordList.GetItem((UOSInt)k);
		sortableIP1 = Net::SocketUtil::IPv4ToSortable(rec->GetStartIP());
		sortableIP2 = Net::SocketUtil::IPv4ToSortable(rec->GetEndIP());
		if (sortableIP >= sortableIP1 && sortableIP <= sortableIP2)
		{
			return rec;
		}
		else if (sortableIP < sortableIP1)
		{
			j = k - 1;
		}
		else
		{
			i = k + 1;
		}
	}
	rec = this->client.RequestIP(ip);
	this->recordList.Insert((UOSInt)i, rec);
	return rec;
}
