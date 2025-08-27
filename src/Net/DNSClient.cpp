#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "Data/RandomOS.h"
#include "Net/DNSClient.h"
#include "Sync/MutexUsage.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"

//RFC 1034, RFC 1035, RFC 3596
extern Char MyString_STRhexarr[];

void __stdcall Net::DNSClient::PacketHdlr(NN<const Net::SocketUtil::AddressInfo> addr, UInt16 port, Data::ByteArrayR data, AnyType userData)
{
	NN<Net::DNSClient> me = userData.GetNN<Net::DNSClient>();
	NN<RequestStatus> req;
	Sync::MutexUsage mutUsage(me->reqMut);
	if (me->reqMap.Get(ReadMUInt16(&data[0])).SetTo(req))
	{
		MemCopyNO(req->respBuff, &data[0], data.GetSize());
		req->respSize = data.GetSize();
		req->finEvt.Set();
	}
	mutUsage.EndUse();
}

NN<Net::DNSClient::RequestStatus> Net::DNSClient::NewReq(UInt32 id)
{
	NN<RequestStatus> req;
	NEW_CLASSNN(req, RequestStatus());
	req->respSize = 0;
	Sync::MutexUsage mutUsage(this->reqMut);
	this->reqMap.Put(id, req);
	mutUsage.EndUse();
	return req;
}

void Net::DNSClient::DelReq(UInt32 id)
{
	NN<RequestStatus> req;
	Sync::MutexUsage mutUsage(this->reqMut);
	if (this->reqMap.Remove(id).SetTo(req))
	{
		mutUsage.EndUse();
		req.Delete();
	}
}

UInt32 Net::DNSClient::NextId()
{
	this->lastID++;
	if (this->lastID >= 65536)
		this->lastID = 1;
	return this->lastID;
}

Net::DNSClient::DNSClient(NN<Net::SocketFactory> sockf, NN<const Net::SocketUtil::AddressInfo> serverAddr, NN<IO::LogTool> log)
{
	Data::RandomOS random;
	this->sockf = sockf;
	this->serverAddr = serverAddr.Ptr()[0];
	this->lastID = random.NextInt15();
	NEW_CLASS(this->svr, Net::UDPServer(sockf, 0, 0, nullptr, PacketHdlr, this, log, nullptr, 1, false));
}

Net::DNSClient::~DNSClient()
{
	DEL_CLASS(this->svr);
}

UOSInt Net::DNSClient::GetByEmailDomainName(NN<Data::ArrayListNN<RequestAnswer>> answers, Text::CStringNN domain)
{
	return GetByType(answers, domain, 15);
}

UOSInt Net::DNSClient::GetByDomainName(NN<Data::ArrayListNN<RequestAnswer>> answers, Text::CStringNN domain)
{
	return GetByType(answers, domain, 1);
}

UOSInt Net::DNSClient::GetByType(NN<Data::ArrayListNN<RequestAnswer>> answers, Text::CStringNN domain, UInt16 reqType)
{
	UOSInt ret = 0;
	UInt8 buff[512];
	UTF8Char sbuff[256];
	Char *ptr1;
	Char *ptr2;
	UnsafeArray<const UTF8Char> cptr1;
	UnsafeArray<const UTF8Char> cptr2;
	UTF8Char c;
	UInt32 currId = NextId();

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
		if (Net::SocketUtil::SetAddrInfo(addr, domain))
		{
			if (addr.addrType == Net::AddrType::IPv4)
			{
				UnsafeArray<UTF8Char> sptr = sbuff;
				sptr = Text::StrUInt16(sptr, addr.addr[3]);
				*sptr++ = '.';
				sptr = Text::StrUInt16(sptr, addr.addr[2]);
				*sptr++ = '.';
				sptr = Text::StrUInt16(sptr, addr.addr[1]);
				*sptr++ = '.';
				sptr = Text::StrUInt16(sptr, addr.addr[0]);
				sptr = Text::StrConcatC(sptr, UTF8STRC(".in-addr.arpa"));
				cptr1 = sbuff;
			}
			else if (addr.addrType == Net::AddrType::IPv6)
			{
				UnsafeArray<UTF8Char> sptr = sbuff;
				OSInt i = 16;
				while (i-- > 0)
				{
					*sptr++ = (UTF8Char)MyString_STRhexarr[addr.addr[i] & 15];
					*sptr++ = '.';
					*sptr++ = (UTF8Char)MyString_STRhexarr[addr.addr[i] >> 4];
					*sptr++ = '.';
				}
				sptr = Text::StrConcatC(sptr, UTF8STRC("ip6.arpa"));
				cptr1 = sbuff;
			}
			else
			{
				cptr1 = domain.v;
			}
		}
		else
		{
			cptr1 = domain.v;
		}
	}
	else
	{
		cptr1 = domain.v;
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
	
	NN<RequestStatus> req = this->NewReq(currId);
	this->svr->SendTo(this->serverAddr, 53, buff, (UOSInt)(ptr1 - ptr2));
	req->finEvt.Wait(2000);
	if (req->respSize > 12)
	{
		ret = ParseAnswers(req->respBuff, req->respSize, answers);
	}
	this->DelReq(currId);
	return ret;
}

UOSInt Net::DNSClient::GetByIPv4Name(NN<Data::ArrayListNN<RequestAnswer>> answers, UInt32 ip)
{
	UOSInt ret = 0;
	UInt8 buff[512];
	UInt8 localIP[4];
	UnsafeArray<Char> ptr1;
	UnsafeArray<Char> ptr2;
	UInt32 currId = NextId();
	*(UInt32*)localIP = ip;

	WriteMUInt16(&buff[0], currId);
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
	
	NN<RequestStatus> req = this->NewReq(currId);
	this->svr->SendTo(this->serverAddr, 53, buff, (UOSInt)(ptr1 - ptr2));
	req->finEvt.Wait(2000);
	if (req->respSize > 12)
	{
		ret = ParseAnswers(req->respBuff, req->respSize, answers);
	}
	this->DelReq(currId);
	return ret;
}

UOSInt Net::DNSClient::GetByAddrName(NN<Data::ArrayListNN<RequestAnswer>> answers, NN<const Net::SocketUtil::AddressInfo> addr)
{
	UOSInt ret = 0;
	UInt8 buff[512];
	UnsafeArray<Char> ptr1;
	UnsafeArray<Char> ptr2;
	UInt32 currId = NextId();

	if (addr->addrType == Net::AddrType::IPv4)
	{
		WriteMUInt16(&buff[0], currId);
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
	else if (addr->addrType == Net::AddrType::IPv6)
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
	
	NN<RequestStatus> req = this->NewReq(currId);
	this->svr->SendTo(this->serverAddr, 53, buff, (UOSInt)(ptr1 - ptr2));
	req->finEvt.Wait(2000);
	if (req->respSize > 12)
	{
		ret = ParseAnswers(req->respBuff, req->respSize, answers);
	}
	this->DelReq(currId);
	return ret;
}

UOSInt Net::DNSClient::GetServerName(NN<Data::ArrayListNN<RequestAnswer>> answers)
{
	return GetByAddrName(answers, this->serverAddr);
}

UOSInt Net::DNSClient::GetCAARecord(NN<Data::ArrayListNN<RequestAnswer>> answers, Text::CStringNN domain)
{
	return GetByType(answers, domain, 257);
}

void Net::DNSClient::UpdateDNSAddr(NN<const Net::SocketUtil::AddressInfo> serverAddr)
{
	this->serverAddr = serverAddr.Ptr()[0];
}

UOSInt Net::DNSClient::ParseString(UnsafeArray<UTF8Char> sbuff, UnsafeArray<const UInt8> buff, UOSInt stringOfst, UOSInt endOfst, OptOut<UnsafeArray<UTF8Char>> sbuffEndOut)
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
	sbuffEndOut.Set(sbuff);
	return i;
}

UOSInt Net::DNSClient::ParseAnswers(UnsafeArray<const UInt8> buff, UOSInt dataSize, NN<Data::ArrayListNN<RequestAnswer>> answers)
{
	UTF8Char sbuff[512];
	NN<RequestAnswer> ans;
	UOSInt ansCount = ReadMUInt16(&buff[6]);
	UOSInt cnt2 = ReadMUInt16(&buff[8]);
	UOSInt cnt3 = ReadMUInt16(&buff[10]);
	ansCount += cnt2 + cnt3;
	UOSInt i;
	UOSInt j;
	i = ParseString(sbuff, buff, 12, dataSize, 0);
	i += 4;

	j = 0;
	while (j < ansCount && i < dataSize)
	{
		ans = ParseAnswer(buff, dataSize, i);
		answers->Add(ans);

		j++;
	}
	return ansCount;
}

NN<Net::DNSClient::RequestAnswer> Net::DNSClient::ParseAnswer(UnsafeArray<const UInt8> buff, UOSInt dataSize, InOutParam<UOSInt> index)
{
	UTF8Char sbuff[2048];
	UnsafeArray<UTF8Char> sptr;
	NN<RequestAnswer> ans;
	UOSInt i = index.Get();
	UOSInt k;
	i = ParseString(sbuff, buff, i, dataSize, sptr);
	ans = MemAllocNN(RequestAnswer);
	ans->name = Text::String::New(sbuff, (UOSInt)(sptr - sbuff));
	ans->recType = ReadMUInt16(&buff[i]);
	ans->recClass = ReadMUInt16(&buff[i + 2]);
	ans->ttl = ReadMUInt32(&buff[i + 4]);
	ans->addr.addrType = Net::AddrType::Unknown;
	k = ReadMUInt16(&buff[i + 8]);
	switch (ans->recType)
	{
	case 1: // A - a host address
		Net::SocketUtil::SetAddrInfoV4(ans->addr, ReadNUInt32(&buff[i + 10]));
		sptr = Net::SocketUtil::GetAddrName(sbuff, ans->addr).Or(sbuff);
		ans->rd = Text::String::New(sbuff, (UOSInt)(sptr - sbuff)).Ptr();
		break;
	case 2: // NS - an authoritative name server
	case 5: // CNAME - the canonical name for an alias
	case 12: // PTR - a domain name pointer
	case 47: // NSEC - Next Secure
		ParseString(sbuff, buff, i + 10, i + 10 + k, sptr);
		ans->rd = Text::String::New(sbuff, (UOSInt)(sptr - sbuff)).Ptr();
		break;
	case 6: // SOA - Start of [a zone of] authority
		{
			UOSInt l;
			Text::StringBuilderUTF8 sb;
			l = ParseString(sbuff, buff, i + 10, i + 10 + k, sptr);
			sb.AppendC(sbuff, (UOSInt)(sptr - sbuff));
			sb.AppendC(UTF8STRC(", MailAddr="));
			l = ParseString(sbuff, buff, l, i + 10 + k, sptr);
			sb.AppendC(sbuff, (UOSInt)(sptr - sbuff));
			if (l + 20 <= i + 10 + k)
			{
				sb.AppendC(UTF8STRC(", SN="));
				sb.AppendU32(ReadMUInt32(&buff[l]));
				sb.AppendC(UTF8STRC(", Refresh="));
				sb.AppendU32(ReadMUInt32(&buff[l + 4]));
				sb.AppendC(UTF8STRC(", Retry="));
				sb.AppendU32(ReadMUInt32(&buff[l + 8]));
				sb.AppendC(UTF8STRC(", Expire="));
				sb.AppendU32(ReadMUInt32(&buff[l + 12]));
				sb.AppendC(UTF8STRC(", DefTTL="));
				sb.AppendU32(ReadMUInt32(&buff[l + 16]));
			}
			ans->rd = Text::String::New(sb.ToString(), sb.GetLength()).Ptr();
		}
		break;
	case 15: // MX - mail exchange
		ans->priority = ReadMUInt16(&buff[i + 10]);
		ParseString(sbuff, buff, i + 12, i + 10 + k, sptr);
		ans->rd = Text::String::New(sbuff, (UOSInt)(sptr - sbuff)).Ptr();
		break;
	case 16: // TXT - Text strings
		{
			sptr = sbuff;
			*sptr = 0;
			UOSInt currInd = i + 10;
			UOSInt endInd = i + 10 + k;
			while (currInd < endInd && currInd + buff[currInd] < currInd)
			{
				if (sptr != sbuff)
				{
					sptr = Text::StrConcatC(sptr, UTF8STRC(", "));
				}
				sptr = Text::StrConcatC(sptr, &buff[currInd + 1], buff[currInd]);
				currInd += 1 + (UOSInt)buff[currInd];
			}
			ans->rd = Text::String::New(sbuff, (UOSInt)(sptr - sbuff)).Ptr();
		}
		break;
	case 28: // AAAA
		{
			Net::SocketUtil::SetAddrInfoV6(ans->addr, &buff[i + 10], 0);
			sptr = Net::SocketUtil::GetAddrName(sbuff, ans->addr).Or(sbuff);
			ans->rd = Text::String::New(sbuff, (UOSInt)(sptr - sbuff)).Ptr();
		}
		break;
	case 33: // SRV - Server Selection
		{
			ans->priority = ReadMUInt16(&buff[i + 10]);
			ans->weight = ReadMUInt16(&buff[i + 12]);
			ans->port = ReadMUInt16(&buff[i + 14]);
			ParseString(sbuff, buff, i + 16, i + 10 + k, sptr);
			ans->rd = Text::String::New(sbuff, (UOSInt)(sptr - sbuff)).Ptr();
		}
		break;
	case 48: // DNSKEY - DNS Key record
		{
			sptr = Text::StrConcatC(sbuff, UTF8STRC("Flags = "));
			sptr = Text::StrUInt16(sptr, ReadMUInt16(&buff[i + 10]));
			sptr = Text::StrConcatC(sptr, UTF8STRC(", Protocol = "));
			sptr = Text::StrUInt16(sptr, buff[i + 12]);
			sptr = Text::StrConcatC(sptr, UTF8STRC(", Algorithm = "));
			sptr = Text::StrUInt16(sptr, buff[i + 13]);
			sptr = Text::StrConcatC(sptr, UTF8STRC(", Public Key = "));
			sptr = Text::StrHexBytes(sptr, &buff[i + 14], k - 4, ' ');
			ans->rd = Text::String::New(sbuff, (UOSInt)(sptr - sbuff)).Ptr();
		}
		break;
	case 46: // RRSIG - DNSSEC signature
		{
			sptr = Text::StrConcatC(sbuff, UTF8STRC("Type Covered = "));
			sptr = Text::StrUInt16(sptr, ReadMUInt16(&buff[i + 10]));
			sptr = Text::StrConcatC(sptr, UTF8STRC(", Algorithm = "));
			sptr = Text::StrUInt16(sptr, buff[i + 12]);
			sptr = Text::StrConcatC(sptr, UTF8STRC(", Labels = "));
			sptr = Text::StrUInt16(sptr, buff[i + 13]);
			sptr = Text::StrConcatC(sptr, UTF8STRC(", Original TTL = "));
			sptr = Text::StrUInt32(sptr, ReadMUInt32(&buff[i + 14]));
			sptr = Text::StrConcatC(sptr, UTF8STRC(", Signature Expiration = "));
			sptr = Text::StrUInt32(sptr, ReadMUInt32(&buff[i + 18]));
			sptr = Text::StrConcatC(sptr, UTF8STRC(", Signature Inception = "));
			sptr = Text::StrUInt32(sptr, ReadMUInt32(&buff[i + 22]));
			sptr = Text::StrConcatC(sptr, UTF8STRC(", Key Tag = "));
			sptr = Text::StrUInt16(sptr, ReadMUInt16(&buff[i + 26]));
			sptr = Text::StrConcatC(sptr, UTF8STRC(", Signer's Name = "));
			const UInt8 *tmpPtr = &buff[i + 28];
			while ((*sptr++ = *tmpPtr++) != 0);
			sptr--;
			sptr = Text::StrConcatC(sptr, UTF8STRC(", Signature = "));
			sptr = Text::StrHexBytes(sptr, tmpPtr, k - (UOSInt)(tmpPtr - &buff[i + 10]), ' ');
			ans->rd = Text::String::New(sbuff, (UOSInt)(sptr - sbuff)).Ptr();
		}
		break;
	case 43: // DS - Delegation signer
		{
			sptr = Text::StrConcatC(sbuff, UTF8STRC("Key Tag = "));
			sptr = Text::StrUInt16(sptr, ReadMUInt16(&buff[i + 10]));
			sptr = Text::StrConcatC(sptr, UTF8STRC(", Algorithm = "));
			sptr = Text::StrUInt16(sptr, buff[i + 12]);
			sptr = Text::StrConcatC(sptr, UTF8STRC(", Digest Type = "));
			sptr = Text::StrUInt16(sptr, buff[i + 13]);
			sptr = Text::StrConcatC(sptr, UTF8STRC(", Digest = "));
			sptr = Text::StrHexBytes(sptr, &buff[i + 14], k - 4, ' ');
			ans->rd = Text::String::New(sbuff, (UOSInt)(sptr - sbuff)).Ptr();
		}
		break;
	case 257: // CAA - Certification Authority Authorization
		{
			Text::StringBuilderUTF8 sb;
			sb.AppendC(UTF8STRC("CAA "));
			sb.AppendU16(buff[i + 10]);
			sb.AppendUTF8Char(' ');
			sb.AppendC(&buff[i + 12], buff[i + 11]);
			sb.AppendUTF8Char(' ');
			UOSInt l = i + 12 + buff[i + 11];
			sb.AppendC(&buff[l], i + 10 + k - l);
			ans->rd = Text::String::New(sb.ToString(), sb.GetLength()).Ptr();
		}
		break;
	default:
		ans->rd = 0;
		break;
	}
	index.Set(i + k + 10);
	return ans;
}

void Net::DNSClient::FreeAnswers(NN<Data::ArrayListNN<RequestAnswer>> answers)
{
	answers->FreeAll(FreeAnswer);
}

void __stdcall Net::DNSClient::FreeAnswer(NN<RequestAnswer> ans)
{
	ans->name->Release();;
	SDEL_STRING(ans->rd);
	MemFreeNN(ans);
}

UInt32 Net::DNSClient::GetResponseTTL(UnsafeArray<const UInt8> buff, UOSInt buffSize)
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

UOSInt Net::DNSClient::SkipString(UnsafeArray<const UInt8> buff, UOSInt stringOfst, UOSInt endOfst)
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

Text::CString Net::DNSClient::TypeGetID(UInt16 type)
{
	switch (type)
	{
	case 1:
		return CSTR("A");
	case 2:
		return CSTR("NS");
	case 5:
		return CSTR("CNAME");
	case 6:
		return CSTR("SOA");
	case 18:
		return CSTR("AFSDB");
	case 12:
		return CSTR("PTR");
	case 15:
		return CSTR("MX");
	case 16:
		return CSTR("TXT");
	case 17:
		return CSTR("RP");
	case 24:
		return CSTR("SIG");
	case 25:
		return CSTR("KEY");
	case 28:
		return CSTR("AAAA");
	case 29:
		return CSTR("LOC");
	case 33:
		return CSTR("SRV");
	case 35:
		return CSTR("NAPTR");
	case 36:
		return CSTR("KX");
	case 37:
		return CSTR("CERT");
	case 39:
		return CSTR("DNAME");
	case 41:
		return CSTR("OPT");
	case 42:
		return CSTR("APL");
	case 43:
		return CSTR("DS");
	case 44:
		return CSTR("SSHFP");
	case 45:
		return CSTR("IPSECKEY");
	case 46:
		return CSTR("RRSIG");
	case 47:
		return CSTR("NSEC");
	case 48:
		return CSTR("DNSKEY");
	case 49:
		return CSTR("DHCID");
	case 50:
		return CSTR("NSEC3");
	case 51:
		return CSTR("NSEC3PARAM");
	case 52:
		return CSTR("TLSA");
	case 53:
		return CSTR("SMIMEA");
	case 55:
		return CSTR("HIP");
	case 59:
		return CSTR("CDS");
	case 60:
		return CSTR("CDNSKEY");
	case 61:
		return CSTR("OPENPGPKEY");
	case 62:
		return CSTR("CSYNC");
	case 249:
		return CSTR("TKEY");
	case 250:
		return CSTR("TSIG");
	case 251:
		return CSTR("IXFR");
	case 252:
		return CSTR("AXFR");
	case 255:
		return CSTR("*");
	case 256:
		return CSTR("URI");
	case 257:
		return CSTR("CAA");
	case 32768:
		return CSTR("TA");
	case 32769:
		return CSTR("DLV");
	}
	return nullptr;
}
