#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "Data/RandomOS.h"
#include "Net/DNSClient.h"
#include "Sync/MutexUsage.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"

//RFC 1034, RFC 1035, RFC 3596
extern Char MyString_STRhexarr[];

void __stdcall Net::DNSClient::PacketHdlr(const Net::SocketUtil::AddressInfo *addr, UInt16 port, const UInt8 *buff, UOSInt dataSize, void *userData)
{
	Net::DNSClient *me = (Net::DNSClient*)userData;
	RequestStatus *req;
	Sync::MutexUsage mutUsage(me->reqMut);
	req = me->reqMap->Get(ReadMUInt16(buff));
	if (req)
	{
		MemCopyNO(req->respBuff, buff, dataSize);
		req->respSize = dataSize;
		req->finEvt->Set();
	}
	mutUsage.EndUse();
}

Net::DNSClient::RequestStatus *Net::DNSClient::NewReq(Int32 id)
{
	RequestStatus *req = MemAlloc(RequestStatus, 1);
	req->respSize = 0;
	NEW_CLASS(req->finEvt, Sync::Event(true, (const UTF8Char*)"Net.DNSClient.RequestStatus.finEvt"));
	Sync::MutexUsage mutUsage(this->reqMut);
	this->reqMap->Put(id, req);
	mutUsage.EndUse();
	return req;
}

void Net::DNSClient::DelReq(Int32 id)
{
	RequestStatus *req;
	Sync::MutexUsage mutUsage(this->reqMut);
	req = this->reqMap->Remove(id);
	mutUsage.EndUse();
	if (req)
	{
		DEL_CLASS(req->finEvt);
		MemFree(req);
	}
}

Int32 Net::DNSClient::NextId()
{
	this->lastID++;
	if (this->lastID >= 65536)
		this->lastID = 1;
	return this->lastID;
}

Net::DNSClient::DNSClient(Net::SocketFactory *sockf, const Net::SocketUtil::AddressInfo *serverAddr)
{
	Data::RandomOS random;
	this->sockf = sockf;
	this->serverAddr = *serverAddr;
	this->lastID = random.NextInt15();
	NEW_CLASS(this->reqMut, Sync::Mutex());
	NEW_CLASS(this->reqMap, Data::Int32Map<RequestStatus*>());
	NEW_CLASS(this->svr, Net::UDPServer(sockf, 0, 0, 0, PacketHdlr, this, 0, 0, 1, false));
}

Net::DNSClient::~DNSClient()
{
	DEL_CLASS(this->svr);
	DEL_CLASS(this->reqMap);
	DEL_CLASS(this->reqMut);
}

UOSInt Net::DNSClient::GetByEmailDomainName(Data::ArrayList<RequestAnswer*> *answers, const UTF8Char *domain)
{
	return GetByType(answers, domain, 15);
}

UOSInt Net::DNSClient::GetByDomainName(Data::ArrayList<RequestAnswer*> *answers, const UTF8Char *domain)
{
	return GetByType(answers, domain, 1);
}

UOSInt Net::DNSClient::GetByType(Data::ArrayList<RequestAnswer*> *answers, const UTF8Char *domain, UInt16 reqType)
{
	UOSInt ret = 0;
	UInt8 buff[512];
	UTF8Char sbuff[256];
	Char *ptr1;
	Char *ptr2;
	const UTF8Char *cptr1;
	const UTF8Char *cptr2;
	UTF8Char c;
	Int32 currId = NextId();

	WriteMInt16(&buff[0], currId);
	WriteMInt16(&buff[2], 0x100); //flags
	WriteMInt16(&buff[4], 1); //reqCount
	WriteMInt16(&buff[6], 0); //respCount;
	WriteMInt16(&buff[8], 0); //authorCount
	WriteMInt16(&buff[10], 0); //extraCount
	ptr1 = (Char*)&buff[12];
	if (reqType == 12)
	{
		Net::SocketUtil::AddressInfo addr;
		if (Net::SocketUtil::GetIPAddr(domain, &addr))
		{
			if (addr.addrType == Net::SocketUtil::AT_IPV4)
			{
				UTF8Char *sptr = sbuff;
				sptr = Text::StrUInt16(sptr, addr.addr[3]);
				*sptr++ = '.';
				sptr = Text::StrUInt16(sptr, addr.addr[2]);
				*sptr++ = '.';
				sptr = Text::StrUInt16(sptr, addr.addr[1]);
				*sptr++ = '.';
				sptr = Text::StrUInt16(sptr, addr.addr[0]);
				sptr = Text::StrConcat(sptr, (const UTF8Char*)".in-addr.arpa");
				cptr1 = sbuff;
			}
			else if (addr.addrType == Net::SocketUtil::AT_IPV6)
			{
				UTF8Char *sptr = sbuff;
				OSInt i = 16;
				while (i-- > 0)
				{
					*sptr++ = (UTF8Char)MyString_STRhexarr[addr.addr[i] & 15];
					*sptr++ = '.';
					*sptr++ = (UTF8Char)MyString_STRhexarr[addr.addr[i] >> 4];
					*sptr++ = '.';
				}
				sptr = Text::StrConcat(sptr, (const UTF8Char*)"ip6.arpa");
				cptr1 = sbuff;
			}
			else
			{
				cptr1 = domain;
			}
		}
		else
		{
			cptr1 = domain;
		}
	}
	else
	{
		cptr1 = domain;
	}
	while (true)
	{
		cptr2 = cptr1;
		while ((c = *cptr2) != 0)
		{
			if (c == '.')
				break;
			cptr2++;
		}
		*ptr1++ = (Char)(cptr2 - cptr1);
		while (cptr1 < cptr2)
		{
			*ptr1++ = (Char)*cptr1++;
		}
		if (*cptr1 == 0)
		{
			*ptr1++ = 0;
			break;
		}
		else
		{
			cptr1++;
		}
	}
	WriteMInt16((UInt8*)&ptr1[0], reqType);
	WriteMInt16((UInt8*)&ptr1[2], 1);
	ptr1 += 4;
	ptr2 = (Char*)buff;
	
	RequestStatus *req = this->NewReq(currId);
	this->svr->SendTo(&this->serverAddr, 53, buff, (UOSInt)(ptr1 - ptr2));
	req->finEvt->Wait(2000);
	if (req->respSize > 12)
	{
		ret = ParseAnswers(req->respBuff, req->respSize, answers);
	}
	this->DelReq(currId);
	return ret;
}

UOSInt Net::DNSClient::GetByIPv4Name(Data::ArrayList<RequestAnswer*> *answers, UInt32 ip)
{
	UOSInt ret = 0;
	UInt8 buff[512];
	UInt8 localIP[4];
	Char *ptr1;
	Char *ptr2;
	Int32 currId = NextId();
	*(UInt32*)localIP = ip;

	WriteMInt16(&buff[0], currId);
	WriteMInt16(&buff[2], 0x100); //flags
	WriteMInt16(&buff[4], 1); //reqCount
	WriteMInt16(&buff[6], 0);
	WriteMInt32(&buff[8], 0);
	ptr1 = (Char*)&buff[12];
	ptr2 = Text::StrInt32(ptr1 + 1, localIP[3]);
	*ptr1 = (Char)(ptr2 - ptr1 - 1);
	ptr1 = ptr2;
	ptr2 = Text::StrInt32(ptr1 + 1, localIP[2]);
	*ptr1 = (Char)(ptr2 - ptr1 - 1);
	ptr1 = ptr2;
	ptr2 = Text::StrInt32(ptr1 + 1, localIP[1]);
	*ptr1 = (Char)(ptr2 - ptr1 - 1);
	ptr1 = ptr2;
	ptr2 = Text::StrInt32(ptr1 + 1, localIP[0]);
	*ptr1 = (Char)(ptr2 - ptr1 - 1);
	ptr1 = ptr2;
	*ptr1++ = 7;
	ptr1 = Text::StrConcat(ptr1, "in-addr");
	*ptr1++ = 4;
	ptr1 = Text::StrConcat(ptr1, "arpa");
	*ptr1++ = 0;
	WriteMInt16((UInt8*)&ptr1[0], 12);
	WriteMInt16((UInt8*)&ptr1[2], 1);
	ptr1 += 4;
	ptr2 = (Char*)buff;
	
	RequestStatus *req = this->NewReq(currId);
	this->svr->SendTo(&this->serverAddr, 53, buff, (UOSInt)(ptr1 - ptr2));
	req->finEvt->Wait(2000);
	if (req->respSize > 12)
	{
		ret = ParseAnswers(req->respBuff, req->respSize, answers);
	}
	this->DelReq(currId);
	return ret;
}

UOSInt Net::DNSClient::GetByAddrName(Data::ArrayList<RequestAnswer*> *answers, const Net::SocketUtil::AddressInfo *addr)
{
	UOSInt ret = 0;
	UInt8 buff[512];
	Char *ptr1;
	Char *ptr2;
	Int32 currId = NextId();

	if (addr->addrType == Net::SocketUtil::AT_IPV4)
	{
		WriteMInt16(&buff[0], currId);
		WriteMInt16(&buff[2], 0x100); //flags
		WriteMInt16(&buff[4], 1); //reqCount
		WriteMInt16(&buff[6], 0);
		WriteMInt32(&buff[8], 0);
		ptr1 = (Char*)&buff[12];
		ptr2 = Text::StrInt32(ptr1 + 1, addr->addr[3]);
		*ptr1 = (Char)(ptr2 - ptr1 - 1);
		ptr1 = ptr2;
		ptr2 = Text::StrInt32(ptr1 + 1, addr->addr[2]);
		*ptr1 = (Char)(ptr2 - ptr1 - 1);
		ptr1 = ptr2;
		ptr2 = Text::StrInt32(ptr1 + 1, addr->addr[1]);
		*ptr1 = (Char)(ptr2 - ptr1 - 1);
		ptr1 = ptr2;
		ptr2 = Text::StrInt32(ptr1 + 1, addr->addr[0]);
		*ptr1 = (Char)(ptr2 - ptr1 - 1);
		ptr1 = ptr2;
		*ptr1++ = 7;
		ptr1 = Text::StrConcat(ptr1, "in-addr");
		*ptr1++ = 4;
		ptr1 = Text::StrConcat(ptr1, "arpa");
		*ptr1++ = 0;
		WriteMInt16((UInt8*)&ptr1[0], 12);
		WriteMInt16((UInt8*)&ptr1[2], 1);
		ptr1 += 4;
		ptr2 = (Char*)buff;
	}
	else if (addr->addrType == Net::SocketUtil::AT_IPV6)
	{
		WriteMInt16(&buff[0], currId);
		WriteMInt16(&buff[2], 0x100); //flags
		WriteMInt16(&buff[4], 1); //reqCount
		WriteMInt16(&buff[6], 0);
		WriteMInt32(&buff[8], 0);
		ptr1 = (Char*)&buff[12];
		*ptr1++ = 1;
		*ptr1++ = MyString_STRhexarr[addr->addr[15] & 15];
		*ptr1++ = 1;
		*ptr1++ = MyString_STRhexarr[addr->addr[15] >> 4];
		*ptr1++ = 1;
		*ptr1++ = MyString_STRhexarr[addr->addr[14] & 15];
		*ptr1++ = 1;
		*ptr1++ = MyString_STRhexarr[addr->addr[14] >> 4];
		*ptr1++ = 1;
		*ptr1++ = MyString_STRhexarr[addr->addr[13] & 15];
		*ptr1++ = 1;
		*ptr1++ = MyString_STRhexarr[addr->addr[13] >> 4];
		*ptr1++ = 1;
		*ptr1++ = MyString_STRhexarr[addr->addr[12] & 15];
		*ptr1++ = 1;
		*ptr1++ = MyString_STRhexarr[addr->addr[12] >> 4];
		*ptr1++ = 1;
		*ptr1++ = MyString_STRhexarr[addr->addr[11] & 15];
		*ptr1++ = 1;
		*ptr1++ = MyString_STRhexarr[addr->addr[11] >> 4];
		*ptr1++ = 1;
		*ptr1++ = MyString_STRhexarr[addr->addr[10] & 15];
		*ptr1++ = 1;
		*ptr1++ = MyString_STRhexarr[addr->addr[10] >> 4];
		*ptr1++ = 1;
		*ptr1++ = MyString_STRhexarr[addr->addr[9] & 15];
		*ptr1++ = 1;
		*ptr1++ = MyString_STRhexarr[addr->addr[9] >> 4];
		*ptr1++ = 1;
		*ptr1++ = MyString_STRhexarr[addr->addr[8] & 15];
		*ptr1++ = 1;
		*ptr1++ = MyString_STRhexarr[addr->addr[8] >> 4];
		*ptr1++ = 1;
		*ptr1++ = MyString_STRhexarr[addr->addr[7] & 15];
		*ptr1++ = 1;
		*ptr1++ = MyString_STRhexarr[addr->addr[7] >> 4];
		*ptr1++ = 1;
		*ptr1++ = MyString_STRhexarr[addr->addr[6] & 15];
		*ptr1++ = 1;
		*ptr1++ = MyString_STRhexarr[addr->addr[6] >> 4];
		*ptr1++ = 1;
		*ptr1++ = MyString_STRhexarr[addr->addr[5] & 15];
		*ptr1++ = 1;
		*ptr1++ = MyString_STRhexarr[addr->addr[5] >> 4];
		*ptr1++ = 1;
		*ptr1++ = MyString_STRhexarr[addr->addr[4] & 15];
		*ptr1++ = 1;
		*ptr1++ = MyString_STRhexarr[addr->addr[4] >> 4];
		*ptr1++ = 1;
		*ptr1++ = MyString_STRhexarr[addr->addr[3] & 15];
		*ptr1++ = 1;
		*ptr1++ = MyString_STRhexarr[addr->addr[3] >> 4];
		*ptr1++ = 1;
		*ptr1++ = MyString_STRhexarr[addr->addr[2] & 15];
		*ptr1++ = 1;
		*ptr1++ = MyString_STRhexarr[addr->addr[2] >> 4];
		*ptr1++ = 1;
		*ptr1++ = MyString_STRhexarr[addr->addr[1] & 15];
		*ptr1++ = 1;
		*ptr1++ = MyString_STRhexarr[addr->addr[1] >> 4];
		*ptr1++ = 1;
		*ptr1++ = MyString_STRhexarr[addr->addr[0] & 15];
		*ptr1++ = 1;
		*ptr1++ = MyString_STRhexarr[addr->addr[0] >> 4];
		*ptr1++ = 3;
		ptr1 = Text::StrConcat(ptr1, "ip6");
		*ptr1++ = 4;
		ptr1 = Text::StrConcat(ptr1, "arpa");
		*ptr1++ = 0;
		WriteMInt16((UInt8*)&ptr1[0], 12);
		WriteMInt16((UInt8*)&ptr1[2], 1);
		ptr1 += 4;
		ptr2 = (Char*)buff;
	}
	else
	{
		return 0;
	}
	
	RequestStatus *req = this->NewReq(currId);
	this->svr->SendTo(&this->serverAddr, 53, buff, (UOSInt)(ptr1 - ptr2));
	req->finEvt->Wait(2000);
	if (req->respSize > 12)
	{
		ret = ParseAnswers(req->respBuff, req->respSize, answers);
	}
	this->DelReq(currId);
	return ret;
}

UOSInt Net::DNSClient::GetServerName(Data::ArrayList<RequestAnswer*> *answers)
{
	return GetByAddrName(answers, &this->serverAddr);
}

void Net::DNSClient::UpdateDNSAddr(const Net::SocketUtil::AddressInfo *serverAddr)
{
	this->serverAddr = *serverAddr;
}

UOSInt Net::DNSClient::ParseString(UTF8Char *sbuff, const UInt8 *buff, UOSInt stringOfst, UOSInt endOfst)
{
	Bool found = false;
	UOSInt i = stringOfst;
	UOSInt j;
	UOSInt l;
	while (i < endOfst)
	{
		j = buff[i];
		if (j == 0)
		{
			i++;
			break;
		}
		if (j >= 0xc0)
		{
			l = ((j - 0xc0) << 8) + buff[i + 1];
			while (l < endOfst)
			{
				j = buff[l];
				if (j == 0)
				{
					break;
				}
				if (j >= 0xc0)
				{
					l = ((j - 0xc0) << 8) + buff[l + 1];
				}
				else
				{
					if (found)
						*sbuff++ = '.';
					l++;
					while (j-- > 0)
					{
						*sbuff++ = buff[l++];
					}
					found = true;
				}
			}
			i += 2;
			break;
		}
		else
		{
			if (found)
				*sbuff++ = '.';
			i++;
			while (j-- > 0)
			{
				*sbuff++ = buff[i++];
			}
			found = true;
		}
	}
	*sbuff = 0;
	return i;
}

UOSInt Net::DNSClient::ParseAnswers(const UInt8 *buff, UOSInt dataSize, Data::ArrayList<RequestAnswer*> *answers)
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	RequestAnswer *ans;
	UOSInt ansCount = ReadMUInt16(&buff[6]);
	UOSInt cnt2 = ReadMUInt16(&buff[8]);
	UOSInt cnt3 = ReadMUInt16(&buff[10]);
	ansCount += cnt2 + cnt3;
	UOSInt i;
	UOSInt j;
	UOSInt k;
	i = ParseString(sbuff, buff, 12, dataSize);
	i += 4;

	j = 0;
	while (j < ansCount && i < dataSize)
	{
		i = ParseString(sbuff, buff, i, dataSize);
		ans = MemAlloc(RequestAnswer, 1);
		ans->name = Text::StrCopyNew(sbuff);
		ans->recType = ReadMUInt16(&buff[i]);
		ans->recClass = ReadMUInt16(&buff[i + 2]);
		ans->ttl = ReadMUInt32(&buff[i + 4]);
		ans->addr.addrType = Net::SocketUtil::AT_UNKNOWN;
		k = ReadMUInt16(&buff[i + 8]);
		switch (ans->recType)
		{
		case 1: // A - a host address
			Net::SocketUtil::SetAddrInfoV4(&ans->addr, ReadUInt32(&buff[i + 10]));
			Net::SocketUtil::GetAddrName(sbuff, &ans->addr);
			ans->rd = Text::StrCopyNew(sbuff);
			break;
		case 2: // NS - an authoritative name server
		case 5: // CNAME - the canonical name for an alias
		case 12: // PTR - a domain name pointer
			ParseString(sbuff, buff, i + 10, i + 10 + k);
			ans->rd = Text::StrCopyNew(sbuff);
			break;
		case 6: // SOA - Start of [a zone of] authority
			{
				UOSInt l;
				Text::StringBuilderUTF8 sb;
				l = ParseString(sbuff, buff, i + 10, i + 10 + k);
				sb.Append(sbuff);
				sb.Append((const UTF8Char*)", MailAddr=");
				l = ParseString(sbuff, buff, l, i + 10 + k);
				sb.Append(sbuff);
				if (l + 20 <= i + 10 + k)
				{
					sb.Append((const UTF8Char*)", SN=");
					sb.AppendU32(ReadMUInt32(&buff[l]));
					sb.Append((const UTF8Char*)", Refresh=");
					sb.AppendU32(ReadMUInt32(&buff[l + 4]));
					sb.Append((const UTF8Char*)", Retry=");
					sb.AppendU32(ReadMUInt32(&buff[l + 8]));
					sb.Append((const UTF8Char*)", Expire=");
					sb.AppendU32(ReadMUInt32(&buff[l + 12]));
					sb.Append((const UTF8Char*)", DefTTL=");
					sb.AppendU32(ReadMUInt32(&buff[l + 16]));
				}
				ans->rd = Text::StrCopyNew(sb.ToString());
			}
			break;
		case 15: // MX - mail exchange
			ans->priority = ReadMUInt16(&buff[i + 10]);
			ParseString(sbuff, buff, i + 12, i + 10 + k);
			ans->rd = Text::StrCopyNew(sbuff);
			break;
		case 28: // AAAA
			{
				Net::SocketUtil::SetAddrInfoV6(&ans->addr, &buff[i + 10], 0);
				Net::SocketUtil::GetAddrName(sbuff, &ans->addr);
				ans->rd = Text::StrCopyNew(sbuff);
			}
			break;
		case 48: // DNSKEY - DNS Key record
			{
				sptr = Text::StrConcat(sbuff, (const UTF8Char*)"Flags = ");
				sptr = Text::StrUInt16(sptr, ReadMUInt16(&buff[i + 10]));
				sptr = Text::StrConcat(sptr, (const UTF8Char*)", Protocol = ");
				sptr = Text::StrUInt16(sptr, buff[i + 12]);
				sptr = Text::StrConcat(sptr, (const UTF8Char*)", Algorithm = ");
				sptr = Text::StrUInt16(sptr, buff[i + 13]);
				sptr = Text::StrConcat(sptr, (const UTF8Char*)", Public Key = ");
				sptr = Text::StrHexBytes(sptr, &buff[i + 14], k - 4, ' ');
				ans->rd = Text::StrCopyNew(sbuff);
			}
			break;
		case 46: // RRSIG - DNSSEC signature
			{
				sptr = Text::StrConcat(sbuff, (const UTF8Char*)"Type Covered = ");
				sptr = Text::StrUInt16(sptr, ReadMUInt16(&buff[i + 10]));
				sptr = Text::StrConcat(sptr, (const UTF8Char*)", Algorithm = ");
				sptr = Text::StrUInt16(sptr, buff[i + 12]);
				sptr = Text::StrConcat(sptr, (const UTF8Char*)", Labels = ");
				sptr = Text::StrUInt16(sptr, buff[i + 13]);
				sptr = Text::StrConcat(sptr, (const UTF8Char*)", Original TTL = ");
				sptr = Text::StrUInt32(sptr, ReadMUInt32(&buff[i + 14]));
				sptr = Text::StrConcat(sptr, (const UTF8Char*)", Signature Expiration = ");
				sptr = Text::StrUInt32(sptr, ReadMUInt32(&buff[i + 18]));
				sptr = Text::StrConcat(sptr, (const UTF8Char*)", Signature Inception = ");
				sptr = Text::StrUInt32(sptr, ReadMUInt32(&buff[i + 22]));
				sptr = Text::StrConcat(sptr, (const UTF8Char*)", Key Tag = ");
				sptr = Text::StrUInt16(sptr, ReadMUInt16(&buff[i + 26]));
				sptr = Text::StrConcat(sptr, (const UTF8Char*)", Signer's Name = ");
				const UInt8 *tmpPtr = &buff[i + 28];
				while ((*sptr++ = *tmpPtr++) != 0);
				sptr--;
				sptr = Text::StrConcat(sptr, (const UTF8Char*)", Signature = ");
				sptr = Text::StrHexBytes(sptr, tmpPtr, k - (UOSInt)(tmpPtr - &buff[i + 10]), ' ');
				ans->rd = Text::StrCopyNew(sbuff);
			}
			break;
		case 43: // DS - Delegation signer
			{
				sptr = Text::StrConcat(sbuff, (const UTF8Char*)"Key Tag = ");
				sptr = Text::StrUInt16(sptr, ReadMUInt16(&buff[i + 10]));
				sptr = Text::StrConcat(sptr, (const UTF8Char*)", Algorithm = ");
				sptr = Text::StrUInt16(sptr, buff[i + 12]);
				sptr = Text::StrConcat(sptr, (const UTF8Char*)", Digest Type = ");
				sptr = Text::StrUInt16(sptr, buff[i + 13]);
				sptr = Text::StrConcat(sptr, (const UTF8Char*)", Digest = ");
				sptr = Text::StrHexBytes(sptr, &buff[i + 14], k - 4, ' ');
				ans->rd = Text::StrCopyNew(sbuff);
			}
			break;
		case 47: // NSEC - Next Secure record
		default:
			ans->rd = 0;
			break;
		}
		i += k + 10;
		answers->Add(ans);

		j++;
	}
	return ansCount;
}

void Net::DNSClient::FreeAnswers(Data::ArrayList<RequestAnswer*> *answers)
{
	RequestAnswer *ans;
	UOSInt i = answers->GetCount();
	while (i-- > 0)
	{
		ans = answers->RemoveAt(i);
		Text::StrDelNew(ans->name);
		SDEL_TEXT(ans->rd);
		MemFree(ans);
	}
}

UInt32 Net::DNSClient::GetResponseTTL(const UInt8 *buff, UOSInt buffSize)
{
	UOSInt ansCount = ReadMUInt16(&buff[6]);
	UOSInt i;
	UOSInt j;
	UOSInt k;
	UInt32 minTTL = 0x7fffffff;
	UInt32 ttl;
	i = SkipString(buff, 12, buffSize);
	i += 4;
	j = 0;
	while (j < ansCount && i < buffSize)
	{
		i = SkipString(buff, i, buffSize);
		ttl = ReadMUInt32(&buff[i + 4]);
		k = ReadMUInt16(&buff[i + 8]);
		if (ttl < minTTL)
			minTTL = ttl;
		i += k + 10;
		j++;
	}
	if (minTTL == 0x7fffffff)
	{
		minTTL = 0;
	}
	return minTTL;
}

UOSInt Net::DNSClient::SkipString(const UInt8 *buff, UOSInt stringOfst, UOSInt endOfst)
{
	UOSInt i = stringOfst;
	UOSInt j;
	while (i < endOfst)
	{
		j = buff[i];
		if (j == 0)
		{
			i++;
			return i;
		}
		else if (j == 0xc0)
		{
			return i + 2;
		}
		else
		{
			i += j + 1;
		}
	}
	return i;
}

const UTF8Char *Net::DNSClient::TypeGetID(UInt16 type)
{
	switch (type)
	{
	case 1:
		return (const UTF8Char*)"A";
	case 2:
		return (const UTF8Char*)"NS";
	case 5:
		return (const UTF8Char*)"CNAME";
	case 6:
		return (const UTF8Char*)"SOA";
	case 18:
		return (const UTF8Char*)"AFSDB";
	case 12:
		return (const UTF8Char*)"PTR";
	case 15:
		return (const UTF8Char*)"MX";
	case 16:
		return (const UTF8Char*)"TXT";
	case 17:
		return (const UTF8Char*)"RP";
	case 24:
		return (const UTF8Char*)"SIG";
	case 25:
		return (const UTF8Char*)"KEY";
	case 28:
		return (const UTF8Char*)"AAAA";
	case 29:
		return (const UTF8Char*)"LOC";
	case 33:
		return (const UTF8Char*)"SRV";
	case 35:
		return (const UTF8Char*)"NAPTR";
	case 36:
		return (const UTF8Char*)"KX";
	case 37:
		return (const UTF8Char*)"CERT";
	case 39:
		return (const UTF8Char*)"DNAME";
	case 41:
		return (const UTF8Char*)"OPT";
	case 42:
		return (const UTF8Char*)"APL";
	case 43:
		return (const UTF8Char*)"DS";
	case 44:
		return (const UTF8Char*)"SSHFP";
	case 45:
		return (const UTF8Char*)"IPSECKEY";
	case 46:
		return (const UTF8Char*)"RRSIG";
	case 47:
		return (const UTF8Char*)"NSEC";
	case 48:
		return (const UTF8Char*)"DNSKEY";
	case 49:
		return (const UTF8Char*)"DHCID";
	case 50:
		return (const UTF8Char*)"NSEC3";
	case 51:
		return (const UTF8Char*)"NSEC3PARAM";
	case 52:
		return (const UTF8Char*)"TLSA";
	case 53:
		return (const UTF8Char*)"SMIMEA";
	case 55:
		return (const UTF8Char*)"HIP";
	case 59:
		return (const UTF8Char*)"CDS";
	case 60:
		return (const UTF8Char*)"CDNSKEY";
	case 61:
		return (const UTF8Char*)"OPENPGPKEY";
	case 62:
		return (const UTF8Char*)"CSYNC";
	case 249:
		return (const UTF8Char*)"TKEY";
	case 250:
		return (const UTF8Char*)"TSIG";
	case 251:
		return (const UTF8Char*)"IXFR";
	case 252:
		return (const UTF8Char*)"AXFR";
	case 255:
		return (const UTF8Char*)"*";
	case 256:
		return (const UTF8Char*)"URI";
	case 257:
		return (const UTF8Char*)"CAA";
	case 32768:
		return (const UTF8Char*)"TA";
	case 32769:
		return (const UTF8Char*)"DLV";
	}
	return 0;
}
