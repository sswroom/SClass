#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "Data/Int64Map.h"
#include "Net/DNSClient.h"
#include "Net/EthernetAnalyzer.h"
#include "Net/MACInfo.h"
#include "Net/NTPServer.h"
#include "Net/SNMPInfo.h"
#include "Sync/Interlocked.h"
#include "Sync/MutexUsage.h"
#include "Sync/Thread.h"
#include "Text/JSON.h"
#include "Text/JSText.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/TextBinEnc/Base64Enc.h"

#define IPLOGCNT 300

/*
NetBIOS-NS:
https://tools.ietf.org/html/rfc1002
DNS:
RFC 1034, RFC 1035, RFC 3596
DNS SRV RR:
RFC 2782
mDNS:
https://www.ietf.org/rfc/rfc6762.txt
*/

void Net::EthernetAnalyzer::NetBIOSDecName(UTF8Char *nameBuff, UOSInt nameSize)
{
	UTF8Char *destPtr = nameBuff;
	while (nameSize >= 2)
	{
		*destPtr++ = (UTF8Char)(((nameBuff[0] - 'A') << 4) | (nameBuff[1] - 'A'));
		nameBuff += 2;
		nameSize -= 2;
	}
	*destPtr = 0;
}

Net::EthernetAnalyzer::MACStatus *Net::EthernetAnalyzer::MACGet(UInt64 macAddr)
{
	MACStatus *mac = this->macMap->Get(macAddr);
	if (mac)
		return mac;
	mac = MemAlloc(MACStatus, 1);
	MemClear(mac, sizeof(MACStatus));
	mac->macAddr = macAddr;
	macMap->Put(macAddr, mac);
	return mac;
}

Net::EthernetAnalyzer::EthernetAnalyzer(IO::Writer *errWriter, AnalyzeType aType, const UTF8Char *name) : IO::ParsedObject(name)
{
	this->atype = aType;
	this->packetCnt = 0;
	this->packetTotalSize = 0;
	this->errWriter = errWriter;
	this->isFirst = true;
	this->pingv4ReqHdlr = 0;
	this->pingv4ReqObj = 0;
	NEW_CLASS(this->ipTranMut, Sync::Mutex());
	NEW_CLASS(this->ipTranMap, Data::Int64Map<IPTranStatus*>());
	NEW_CLASS(this->macMut, Sync::Mutex());
	NEW_CLASS(this->macMap, Data::UInt64Map<MACStatus*>());
	NEW_CLASS(this->dnsCliInfoMut, Sync::Mutex());
	NEW_CLASS(this->dnsCliInfos, Data::UInt32Map<DNSClientInfo*>());
	NEW_CLASS(this->dnsReqv4Mut, Sync::Mutex());
	NEW_CLASS(this->dnsReqv4Map, Data::ICaseStringUTF8Map<Net::EthernetAnalyzer::DNSRequestResult*>());
	NEW_CLASS(this->dnsReqv6Mut, Sync::Mutex());
	NEW_CLASS(this->dnsReqv6Map, Data::ICaseStringUTF8Map<Net::EthernetAnalyzer::DNSRequestResult*>());
	NEW_CLASS(this->dnsReqOthMut, Sync::Mutex());
	NEW_CLASS(this->dnsReqOthMap, Data::ICaseStringUTF8Map<Net::EthernetAnalyzer::DNSRequestResult*>());
	NEW_CLASS(this->dnsTargetMut, Sync::Mutex());
	NEW_CLASS(this->dnsTargetMap, Data::UInt32Map<Net::EthernetAnalyzer::DNSTargetInfo*>());
	NEW_CLASS(this->ipLogMut, Sync::Mutex());
	NEW_CLASS(this->ipLogMap, Data::UInt32Map<Net::EthernetAnalyzer::IPLogInfo*>());
	NEW_CLASS(this->dhcpMut, Sync::Mutex());
	NEW_CLASS(this->dhcpMap, Data::UInt64Map<DHCPInfo*>());
}

Net::EthernetAnalyzer::~EthernetAnalyzer()
{
	UOSInt i;
	UOSInt j;
	Data::ArrayList<MACStatus*> *macList = this->macMap->GetValues();
	MACStatus *mac;
	i = macList->GetCount();
	while (i-- > 0)
	{
		mac = macList->GetItem(i);
		SDEL_TEXT(mac->name);
		MemFree(mac);
	}
	DEL_CLASS(this->macMap);
	DEL_CLASS(this->macMut);

	Data::ArrayList<IPTranStatus*> *ipTranList;
	IPTranStatus *ipTran;
	ipTranList = this->ipTranMap->GetValues();
	i = ipTranList->GetCount();
	while (i-- > 0)
	{
		ipTran = ipTranList->GetItem(i);
		MemFree(ipTran);
	}
	DEL_CLASS(this->ipTranMap);
	DEL_CLASS(this->ipTranMut);

	Data::ArrayList<DNSClientInfo*> *dnsCliInfoList;
	DNSClientInfo *dnsCli;
	DEL_CLASS(this->dnsCliInfoMut);
	dnsCliInfoList = this->dnsCliInfos->GetValues();
	i = dnsCliInfoList->GetCount();
	while (i-- > 0)
	{
		dnsCli = dnsCliInfoList->GetItem(i);
		j = dnsCli->hourInfos->GetCount();
		while (j-- > 0)
		{
			MemFree(dnsCli->hourInfos->GetItem(j));
		}
		DEL_CLASS(dnsCli->hourInfos);
		DEL_CLASS(dnsCli->mut);
		MemFree(dnsCli);
	}
	DEL_CLASS(this->dnsCliInfos);

	Data::ArrayList<Net::EthernetAnalyzer::DNSRequestResult*> *dnsReqList;
	Net::EthernetAnalyzer::DNSRequestResult *req;
	dnsReqList = this->dnsReqv4Map->GetValues();
	i = dnsReqList->GetCount();
	while (i-- > 0)
	{
		req = dnsReqList->GetItem(i);
		DEL_CLASS(req->mut);
		MemFree(req);
	}
	DEL_CLASS(this->dnsReqv4Mut);
	DEL_CLASS(this->dnsReqv4Map);

	dnsReqList = this->dnsReqv6Map->GetValues();
	i = dnsReqList->GetCount();
	while (i-- > 0)
	{
		req = dnsReqList->GetItem(i);
		DEL_CLASS(req->mut);
		MemFree(req);
	}
	DEL_CLASS(this->dnsReqv6Mut);
	DEL_CLASS(this->dnsReqv6Map);

	dnsReqList = this->dnsReqOthMap->GetValues();
	i = dnsReqList->GetCount();
	while (i-- > 0)
	{
		req = dnsReqList->GetItem(i);
		DEL_CLASS(req->mut);
		MemFree(req);
	}
	DEL_CLASS(this->dnsReqOthMut);
	DEL_CLASS(this->dnsReqOthMap);
	
	Net::EthernetAnalyzer::DNSTargetInfo *target;
	Data::ArrayList<Net::EthernetAnalyzer::DNSTargetInfo*> *dnsTargetList = this->dnsTargetMap->GetValues();
	i = dnsTargetList->GetCount();
	while (i-- > 0)
	{
		target = dnsTargetList->GetItem(i);
		DEL_CLASS(target->mut);
		j = target->addrList->GetCount();
		while (j-- > 0)
		{
			Text::StrDelNew(target->addrList->GetItem(j));
		}
		DEL_CLASS(target->addrList);
		MemFree(target);
	}
	DEL_CLASS(this->dnsTargetMut);
	DEL_CLASS(this->dnsTargetMap);

	Net::EthernetAnalyzer::IPLogInfo *ipLog;
	Data::ArrayList<Net::EthernetAnalyzer::IPLogInfo*> *ipLogList = this->ipLogMap->GetValues();
	i = ipLogList->GetCount();
	while (i-- > 0)
	{
		ipLog = ipLogList->GetItem(i);
		j = ipLog->logList->GetCount();
		while (j-- > 0)
		{
			Text::StrDelNew(ipLog->logList->GetItem(j));
		}
		DEL_CLASS(ipLog->mut);
		DEL_CLASS(ipLog->logList);
		MemFree(ipLog);
	}
	DEL_CLASS(this->ipLogMut);
	DEL_CLASS(this->ipLogMap);

	DHCPInfo *dhcp;
	Data::ArrayList<DHCPInfo*> *dhcpList = this->dhcpMap->GetValues();
	i = dhcpList->GetCount();
	while (i-- > 0)
	{
		dhcp = dhcpList->GetItem(i);
		SDEL_TEXT(dhcp->vendorClass);
		SDEL_TEXT(dhcp->hostName);
		DEL_CLASS(dhcp->mut);
		MemFree(dhcp);
	}
	DEL_CLASS(this->dhcpMap);
	DEL_CLASS(this->dhcpMut);
}

IO::ParsedObject::ParserType Net::EthernetAnalyzer::GetParserType()
{
	return IO::ParsedObject::PT_ETHERNET_ANALYZER;
}

UInt64 Net::EthernetAnalyzer::GetPacketCnt()
{
	return this->packetCnt;
}

UInt64 Net::EthernetAnalyzer::GetPacketTotalSize()
{
	return this->packetTotalSize;
}

void Net::EthernetAnalyzer::UseIPTran(Sync::MutexUsage *mutUsage)
{
	mutUsage->ReplaceMutex(this->ipTranMut);
}

Data::ArrayList<Net::EthernetAnalyzer::IPTranStatus*> *Net::EthernetAnalyzer::IPTranGetList()
{
	return this->ipTranMap->GetValues();
}

UOSInt Net::EthernetAnalyzer::IPTranGetCount()
{
	return this->ipTranMap->GetCount();
}

void Net::EthernetAnalyzer::UseMAC(Sync::MutexUsage *mutUsage)
{
	mutUsage->ReplaceMutex(this->macMut);
}

Data::ArrayList<Net::EthernetAnalyzer::MACStatus*> *Net::EthernetAnalyzer::MACGetList()
{
	return this->macMap->GetValues();
}

void Net::EthernetAnalyzer::UseDNSCli(Sync::MutexUsage *mutUsage)
{
	mutUsage->ReplaceMutex(this->dnsCliInfoMut);
}

Data::ArrayList<Net::EthernetAnalyzer::DNSClientInfo*> *Net::EthernetAnalyzer::DNSCliGetList()
{
	return this->dnsCliInfos->GetValues();
}

UOSInt Net::EthernetAnalyzer::DNSCliGetCount()
{
	return this->dnsCliInfos->GetCount();
}

UOSInt Net::EthernetAnalyzer::DNSReqv4GetList(Data::ArrayList<const UTF8Char *> *reqList)
{
	Sync::MutexUsage mutUsage(this->dnsReqv4Mut);
	reqList->AddRange(this->dnsReqv4Map->GetKeys());
	mutUsage.EndUse();
	return reqList->GetCount();
}

UOSInt Net::EthernetAnalyzer::DNSReqv4GetCount()
{
	return this->dnsReqv4Map->GetCount();
}

Bool Net::EthernetAnalyzer::DNSReqv4GetInfo(const UTF8Char *req, Data::ArrayList<Net::DNSClient::RequestAnswer*> *ansList, Data::DateTime *reqTime, UInt32 *ttl)
{
	Net::EthernetAnalyzer::DNSRequestResult *result;
	Sync::MutexUsage mutUsage(this->dnsReqv4Mut);
	result = this->dnsReqv4Map->Get(req);
	mutUsage.EndUse();
	if (result)
	{
		Sync::MutexUsage mutUsage(result->mut);
		Net::DNSClient::ParseAnswers(result->recBuff, result->recSize, ansList);
		reqTime->SetTicks(result->reqTime);
		*ttl = result->ttl;
		mutUsage.EndUse();
		return true;
	}
	else
	{
		return false;
	}
}

UOSInt Net::EthernetAnalyzer::DNSReqv6GetList(Data::ArrayList<const UTF8Char *> *reqList)
{
	Sync::MutexUsage mutUsage(this->dnsReqv6Mut);
	reqList->AddRange(this->dnsReqv6Map->GetKeys());
	mutUsage.EndUse();
	return reqList->GetCount();
}

UOSInt Net::EthernetAnalyzer::DNSReqv6GetCount()
{
	return this->dnsReqv6Map->GetCount();
}

Bool Net::EthernetAnalyzer::DNSReqv6GetInfo(const UTF8Char *req, Data::ArrayList<Net::DNSClient::RequestAnswer*> *ansList, Data::DateTime *reqTime, UInt32 *ttl)
{
	Net::EthernetAnalyzer::DNSRequestResult *result;
	Sync::MutexUsage mutUsage(this->dnsReqv6Mut);
	result = this->dnsReqv6Map->Get(req);
	mutUsage.EndUse();
	if (result)
	{
		Sync::MutexUsage mutUsage(result->mut);
		Net::DNSClient::ParseAnswers(result->recBuff, result->recSize, ansList);
		reqTime->SetTicks(result->reqTime);
		*ttl = result->ttl;
		mutUsage.EndUse();
		return true;
	}
	else
	{
		return false;
	}
}

UOSInt Net::EthernetAnalyzer::DNSReqOthGetList(Data::ArrayList<const UTF8Char *> *reqList)
{
	Sync::MutexUsage mutUsage(this->dnsReqOthMut);
	reqList->AddRange(this->dnsReqOthMap->GetKeys());
	mutUsage.EndUse();
	return reqList->GetCount();
}

UOSInt Net::EthernetAnalyzer::DNSReqOthGetCount()
{
	return this->dnsReqOthMap->GetCount();
}

Bool Net::EthernetAnalyzer::DNSReqOthGetInfo(const UTF8Char *req, Data::ArrayList<Net::DNSClient::RequestAnswer*> *ansList, Data::DateTime *reqTime, UInt32 *ttl)
{
	Net::EthernetAnalyzer::DNSRequestResult *result;
	Sync::MutexUsage mutUsage(this->dnsReqOthMut);
	result = this->dnsReqOthMap->Get(req);
	mutUsage.EndUse();
	if (result)
	{
		Sync::MutexUsage mutUsage(result->mut);
		Net::DNSClient::ParseAnswers(result->recBuff, result->recSize, ansList);
		reqTime->SetTicks(result->reqTime);
		*ttl = result->ttl;
		mutUsage.EndUse();
		return true;
	}
	else
	{
		return false;
	}
}

UOSInt Net::EthernetAnalyzer::DNSTargetGetList(Data::ArrayList<Net::EthernetAnalyzer::DNSTargetInfo *> *targetList)
{
	Sync::MutexUsage mutUsage(this->dnsTargetMut);
	targetList->AddRange(this->dnsTargetMap->GetValues());
	mutUsage.EndUse();
	return targetList->GetCount();
}

UOSInt Net::EthernetAnalyzer::DNSTargetGetCount()
{
	return this->dnsTargetMap->GetCount();
}

void Net::EthernetAnalyzer::UseDHCP(Sync::MutexUsage *mutUsage)
{
	mutUsage->ReplaceMutex(this->dhcpMut);
}

Data::ArrayList<Net::EthernetAnalyzer::DHCPInfo*> *Net::EthernetAnalyzer::DHCPGetList()
{
	return this->dhcpMap->GetValues();
}

void Net::EthernetAnalyzer::UseIPLog(Sync::MutexUsage *mutUsage)
{
	mutUsage->ReplaceMutex(this->ipLogMut);
}

Data::ArrayList<Net::EthernetAnalyzer::IPLogInfo*> *Net::EthernetAnalyzer::IPLogGetList()
{
	return this->ipLogMap->GetValues();
}

UOSInt Net::EthernetAnalyzer::IPLogGetCount()
{
	return this->ipLogMap->GetCount();
}

Bool Net::EthernetAnalyzer::PacketData(UInt32 linkType, const UInt8 *packet, UOSInt packetSize)
{
	switch (linkType)
	{
	case 0:
		return PacketNull(packet, packetSize);
	case 1:
		return PacketEthernet(packet, packetSize);
	case 113:
		return PacketLinux(packet, packetSize);
	}
	return false;
}

Bool Net::EthernetAnalyzer::PacketNull(const UInt8 *packet, UOSInt packetSize)
{
	UInt32 packetType;
	Bool valid;
	if (packetSize < 4)
	{
		return false;
	}

	packetType = ReadMUInt32(packet);
	valid = false;
	switch (packetType)
	{
	case 2:
		valid = this->PacketIPv4(&packet[4], packetSize - 4, 0, 0);
		break;
	case 24:
	case 28:
	case 30:
		valid = this->PacketIPv6(&packet[14], packetSize - 14, 0, 0);
		break;
	}
	
	if (!valid)
	{
		if (this->isFirst && this->errWriter)
		{
			this->isFirst = false;
			Text::StringBuilderUTF8 sb;
			sb.AppendHexBuff(packet, packetSize, ' ', Text::LBT_CRLF);
			this->errWriter->WriteLine(sb.ToString());
		}
	}
	Sync::Interlocked::Increment(&this->packetCnt);
	Sync::Interlocked::Add(&this->packetTotalSize, packetSize);
	return valid;
}

Bool Net::EthernetAnalyzer::PacketEthernet(const UInt8 *packet, UOSInt packetSize)
{
	UInt16 etherType;
	UInt64 destAddr;
	UInt64 srcAddr;
	UInt8 tmpBuff[8];
	Bool valid;
	if (packetSize < 14)
	{
		return false;
	}

	tmpBuff[0] = 0;
	tmpBuff[1] = 0;
	MemCopyNO(&tmpBuff[2], &packet[0], 6);
	destAddr = ReadMUInt64(tmpBuff);
	MemCopyNO(&tmpBuff[2], &packet[6], 6);
	srcAddr = ReadMUInt64(tmpBuff);
	etherType = ReadMUInt16(&packet[12]);
	valid = this->PacketEthernetData(&packet[14], packetSize - 14, etherType, srcAddr, destAddr);
	
	if (!valid)
	{
		if (this->isFirst && this->errWriter)
		{
			this->isFirst = false;
			Text::StringBuilderUTF8 sb;
			sb.AppendHexBuff(packet, packetSize, ' ', Text::LBT_CRLF);
			this->errWriter->WriteLine(sb.ToString());
		}
	}
	Sync::Interlocked::Increment(&this->packetCnt);
	Sync::Interlocked::Add(&this->packetTotalSize, packetSize);
	return valid;
}

Bool Net::EthernetAnalyzer::PacketLinux(const UInt8 *packet, UOSInt packetSize)
{
	UInt16 etherType;
	UInt64 destAddr;
	UInt64 srcAddr;
	UInt16 packetType;
	UInt8 tmpBuff[8];
	Bool valid;
	if (packetSize < 14)
	{
		return false;
	}
	packetType = ReadMUInt16(&packet[0]);
	if (ReadMUInt16(&packet[4]) == 6)
	{
		tmpBuff[0] = 0;
		tmpBuff[1] = 0;
		MemCopyNO(&tmpBuff[2], &packet[6], 6);
		destAddr = ReadMUInt64(tmpBuff);
		srcAddr = destAddr;
		if (packetType == 4)
		{
			srcAddr = 0;
		}
		else
		{
			destAddr = 0;
		}
	}
	else
	{
		srcAddr = 0;
		destAddr = 0;
	}
	etherType = ReadMUInt16(&packet[14]);
	valid = this->PacketEthernetData(&packet[16], packetSize - 16, etherType, srcAddr, destAddr);
	
	if (!valid)
	{
		if (this->isFirst && this->errWriter)
		{
			this->isFirst = false;
			Text::StringBuilderUTF8 sb;
			sb.AppendHexBuff(packet, packetSize, ' ', Text::LBT_CRLF);
			this->errWriter->WriteLine(sb.ToString());
		}
	}
	Sync::Interlocked::Increment(&this->packetCnt);
	Sync::Interlocked::Add(&this->packetTotalSize, packetSize);
	return valid;
}

Bool Net::EthernetAnalyzer::PacketEthernetData(const UInt8 *packet, UOSInt packetSize, UInt16 etherType, UInt64 srcMAC, UInt64 destMAC)
{
	MACStatus *mac;
	Bool valid = true;
	switch (etherType)
	{
	case 0x0006: //ARP
/*
FF FF FF FF FF FF 40 4E 36 9C AE 4E 00 06 00 01 
AF 81 01 00 00 00 00 00 00 00 00 00 00 00 00 00 
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
00 00 00 00 00 00 00 00 00 00 00 00
*/
		if (this->atype & AT_DEVICE)
		{
			Sync::MutexUsage mutUsage(this->macMut);
			mac = this->MACGet(srcMAC);
			mac->othSrcCnt++;

			mac = this->MACGet(destMAC);
			mac->othDestCnt++;
			mutUsage.EndUse();
		}
	break;
	case 0x0026: //Unknown
/*
01 80 C2 00 00 00 18 31 BF 56 52 24 00 26 42 42 
03 00 00 00 00 00 80 00 18 31 BF 56 52 24 00 00 
00 00 80 00 18 31 BF 56 52 24 80 03 00 00 14 00 
02 00 02 00 
*/
		if (this->atype & AT_DEVICE)
		{
			Sync::MutexUsage mutUsage(this->macMut);
			mac = this->MACGet(srcMAC);
			mac->othSrcCnt++;

			mac = this->MACGet(destMAC);
			mac->othDestCnt++;
			mutUsage.EndUse();
		}
		break;
	case 0x002E: //Unknown
/*
FF FF FF FF FF FF 40 B8 37 D8 7A 9B 00 2E 00 01 
AF 81 01 00 00 00 00 00 00 00 00 00 00 00 00 00 
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
00 00 00 00 00 00 00 00 29 C4 3D 6D
*/
		if (this->atype & AT_DEVICE)
		{
			Sync::MutexUsage mutUsage(this->macMut);
			mac = this->MACGet(srcMAC);
			mac->othSrcCnt++;

			mac = this->MACGet(destMAC);
			mac->othDestCnt++;
			mutUsage.EndUse();
		}
		break;
	case 0x0046: //Unknown
/*
FF FF FF FF FF FF B4 75 0E 67 3E 0E 00 46 76 7E 
00 A0 4E 0B 00 00 26 EA AC B0 B8 FE C1 D1 72 66 
9B EC 33 CC F4 04 CB 6B 4B 1D BB 85 D8 A4 E5 BE 
E6 C1 85 57 94 C8 46 9A 15 C6 CF 92 0A 06 72 5F 
57 F8 D5 7E 4A 00 D5 ED 64 10 6F 47 65 6B A0 BB 
C6 6C 2E A2 
*/
		if (this->atype & AT_DEVICE)
		{
			Sync::MutexUsage mutUsage(this->macMut);
			mac = this->MACGet(srcMAC);
			mac->othSrcCnt++;

			mac = this->MACGet(destMAC);
			mac->othDestCnt++;
			mutUsage.EndUse();
		}
		break;
	case 0x0800: //IPv4
		valid = this->PacketIPv4(packet, packetSize, srcMAC, destMAC);
		break;
	case 0x0806: //ARP
		valid = this->PacketARP(packet, packetSize, srcMAC, destMAC);
		break;
	case 0x86DD: //IPv6
		valid = this->PacketIPv6(packet, packetSize, srcMAC, destMAC);
		break;
	case 0x888E: //EAP over LAN
/*
04 EA 56 8E 0C 42 C0 56 27 74 7F F9 88 8E 02 03 
00 8F 02 13 82 00 20 00 00 00 00 00 00 00 03 44 
BF AC E2 BF 1B 3D 2D 10 94 98 96 30 25 D4 C1 DB 
56 E8 FF 50 42 52 ED D5 3A E5 E6 3F 11 2E 06 00 
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 AB 
29 16 60 8E D5 69 3B 55 71 DC AA BF 3C 43 30 00 
30 04 54 FF F8 BC AF 90 E4 4D 43 CD F6 A9 E5 5F 
77 51 80 20 C3 20 60 59 28 50 37 1E C9 99 CD 39 
1E CB 71 FB 8A CE 98 CF 23 4A C2 50 B2 67 87 45 
95 
*/
		if (this->atype & AT_DEVICE)
		{
			Sync::MutexUsage mutUsage(this->macMut);
			mac = this->MACGet(srcMAC);
			mac->othSrcCnt++;

			mac = this->MACGet(destMAC);
			mac->othDestCnt++;
			mutUsage.EndUse();
		}
		break;
	default:
		if (this->atype & AT_DEVICE)
		{
			Sync::MutexUsage mutUsage(this->macMut);
			mac = this->MACGet(srcMAC);
			mac->othSrcCnt++;

			mac = this->MACGet(destMAC);
			mac->othDestCnt++;
			mutUsage.EndUse();
		}
		valid = false;
		break;
	}
	return valid;
}

Bool Net::EthernetAnalyzer::PacketIPv4(const UInt8 *packet, UOSInt packetSize, UInt64 srcMAC, UInt64 destMAC)
{
//	Net::SocketUtil::AddressInfo addr;
//	UInt16 port;
//	UInt16 etherType;
//	Int64 destAddr;
//	Int64 srcAddr;
	UInt32 ipAddr;
//	UInt8 tmpBuff[8];
	UTF8Char sbuff[32];
	MACStatus *mac;
	UOSInt i;
	Bool valid = true;
	if ((packet[0] & 0xf0) == 0x40)
	{
		const UInt8 *ipData;
		UOSInt ipDataSize;
		if (this->atype & AT_DEVICE)
		{
			Sync::MutexUsage mutUsage(this->macMut);
			mac = this->MACGet(srcMAC);
			ipAddr = ReadNUInt32(&packet[12]);
			i = 0;
			while (i < 4)
			{
				if (mac->ipv4Addr[i] == ipAddr)
				{
					break;
				}
				else if (mac->ipv4Addr[i] == 0)
				{
					mac->ipv4Addr[i] = ipAddr;
					break;
				}
				i++;
			}
			mac->ipv4SrcCnt++;

			mac = this->MACGet(destMAC);
			ipAddr = ReadNUInt32(&packet[16]);
			i = 0;
			while (i < 4)
			{
				if (mac->ipv4Addr[i] == ipAddr)
				{
					break;
				}
				else if (mac->ipv4Addr[i] == 0)
				{
					mac->ipv4Addr[i] = ipAddr;
					break;
				}
				i++;
			}
			mac->ipv4DestCnt++;
			mutUsage.EndUse();
		}

		IPTranStatus *ip;
		Int64 addr = ReadMInt64(&packet[12]);
		Sync::MutexUsage ipTranMutUsage(this->ipTranMut);
		ip = this->ipTranMap->Get(addr);
		if (ip == 0)
		{
			ip = MemAlloc(IPTranStatus, 1);
			ip->srcIP = ReadNUInt32(&packet[12]);
			ip->destIP = ReadNUInt32(&packet[16]);
			ip->tcpCnt = 0;
			ip->udpCnt = 0;
			ip->icmpCnt = 0;
			ip->otherCnt = 0;
			ip->tcpSize = 0;
			ip->udpSize = 0;
			ip->icmpSize = 0;
			ip->otherSize = 0;
			this->ipTranMap->Put(addr, ip);
		}

		if ((packet[0] & 0xf) <= 5)
		{
			ipData = &packet[20];
			ipDataSize = packetSize - 20;
		}
		else
		{
			ipData = &packet[(packet[0] & 0xf) << 2];
			ipDataSize = packetSize - ((packet[0] & 0xf) << 2);
		}
		
		switch (packet[9])
		{
		case 1: //ICMP
			ip->icmpCnt++;
			ip->icmpSize += ipDataSize;
			if (this->atype & AT_IPLOG)
			{
				Text::StringBuilderUTF8 sb;
				IPLogInfo *ipLog;
				Data::DateTime dt;
				dt.SetCurrTime();
				UInt32 sortableIP = ReadMUInt32(&packet[12]);
				Sync::MutexUsage mutUsage(this->ipLogMut);
				ipLog = this->ipLogMap->Get(sortableIP);
				if (ipLog == 0)
				{
					Net::SocketUtil::IPType itype = Net::SocketUtil::GetIPv4Type(ip->srcIP);
					if (itype == Net::SocketUtil::IT_LOCAL || itype == Net::SocketUtil::IT_PRIVATE)
					{
						ipLog = MemAlloc(IPLogInfo, 1);
						ipLog->ip = ip->srcIP;
						NEW_CLASS(ipLog->mut, Sync::Mutex());
						NEW_CLASS(ipLog->logList, Data::ArrayList<const UTF8Char*>());
						this->ipLogMap->Put(sortableIP, ipLog);
					}
				}
				if (ipLog)
				{
					sb.ClearStr();
					sb.AppendDate(&dt);
					sb.Append((const UTF8Char*)" ICMP ");
					switch (ipData[0])
					{
					case 0:
						sb.Append((const UTF8Char*)"Ping Reply to ");
						break;
					case 3:
						sb.Append((const UTF8Char*)"Destination unreachable (");
						switch (ipData[1])
						{
						case 0:
							sb.Append((const UTF8Char*)"Network unreachable error");
							break;
						case 1:
							sb.Append((const UTF8Char*)"Protocol unreachable error");
							break;
						case 2:
							sb.Append((const UTF8Char*)"Host unreachable error");
							break;
						case 3:
							sb.Append((const UTF8Char*)"Port unreachable error");
							break;
						case 4:
							sb.Append((const UTF8Char*)"The datagram is too big");
							break;
						case 5:
							sb.Append((const UTF8Char*)"Source route failed error");
							break;
						case 6:
							sb.Append((const UTF8Char*)"Destination network unknown error");
							break;
						case 7:
							sb.Append((const UTF8Char*)"Destination host unknown error");
							break;
						case 8:
							sb.Append((const UTF8Char*)"Source host isolated error");
							break;
						case 9:
							sb.Append((const UTF8Char*)"The destination network is administratively prohibited");
							break;
						case 10:
							sb.Append((const UTF8Char*)"The destination host is administratively prohibited");
							break;
						case 11:
							sb.Append((const UTF8Char*)"The network is unreachable for Type Of Service");
							break;
						case 12:
							sb.Append((const UTF8Char*)"The host is unreachable for Type Of Service");
							break;
						case 13:
							sb.Append((const UTF8Char*)"Communication Administratively Prohibited");
							break;
						case 14:
							sb.Append((const UTF8Char*)"Host precedence violation");
							break;
						case 15:
							sb.Append((const UTF8Char*)"Precedence cutoff in effect");
							break;
						default:
							sb.AppendU16(ipData[1]);
							break;
						}
						sb.Append((const UTF8Char*)") to ");
						break;
					case 8:
						sb.Append((const UTF8Char*)"Ping Request to ");
						break;
					case 11:
						sb.Append((const UTF8Char*)"Time Exceeded to ");
						break;
					default:
						sb.Append((const UTF8Char*)"Unknown (");
						sb.AppendU16(ipData[0]);
						sb.Append((const UTF8Char*)") to ");
						break;
					}
					Net::SocketUtil::GetIPv4Name(sbuff, ip->destIP);
					sb.Append(sbuff);
					sb.Append((const UTF8Char*)", ttl = ");
					sb.AppendU16(packet[8]);
					sb.Append((const UTF8Char*)", size = ");
					sb.AppendUOSInt(ipDataSize);
					Sync::MutexUsage mutUsage(ipLog->mut);
					while (ipLog->logList->GetCount() >= IPLOGCNT)
					{
						Text::StrDelNew(ipLog->logList->RemoveAt(0));
					}
					ipLog->logList->Add(Text::StrCopyNew(sb.ToString()));
					mutUsage.EndUse();
				}

				sortableIP = ReadMUInt32(&packet[16]);
				ipLog = this->ipLogMap->Get(sortableIP);
				if (ipLog == 0)
				{
					Net::SocketUtil::IPType itype = Net::SocketUtil::GetIPv4Type(ip->destIP);
					if (itype == Net::SocketUtil::IT_LOCAL || itype == Net::SocketUtil::IT_PRIVATE)
					{
						ipLog = MemAlloc(IPLogInfo, 1);
						ipLog->ip = ip->destIP;
						NEW_CLASS(ipLog->mut, Sync::Mutex());
						NEW_CLASS(ipLog->logList, Data::ArrayList<const UTF8Char*>());
						this->ipLogMap->Put(sortableIP, ipLog);
					}
				}
				if (ipLog)
				{
					sb.ClearStr();
					sb.AppendDate(&dt);
					sb.Append((const UTF8Char*)" ICMP ");
					switch (ipData[0])
					{
					case 0:
						sb.Append((const UTF8Char*)"Ping Reply from ");
						break;
					case 3:
						sb.Append((const UTF8Char*)"Destination unreachable (");
						switch (ipData[1])
						{
						case 0:
							sb.Append((const UTF8Char*)"Network unreachable error");
							break;
						case 1:
							sb.Append((const UTF8Char*)"Protocol unreachable error");
							break;
						case 2:
							sb.Append((const UTF8Char*)"Host unreachable error");
							break;
						case 3:
							sb.Append((const UTF8Char*)"Port unreachable error");
							break;
						case 4:
							sb.Append((const UTF8Char*)"The datagram is too big");
							break;
						case 5:
							sb.Append((const UTF8Char*)"Source route failed error");
							break;
						case 6:
							sb.Append((const UTF8Char*)"Destination network unknown error");
							break;
						case 7:
							sb.Append((const UTF8Char*)"Destination host unknown error");
							break;
						case 8:
							sb.Append((const UTF8Char*)"Source host isolated error");
							break;
						case 9:
							sb.Append((const UTF8Char*)"The destination network is administratively prohibited");
							break;
						case 10:
							sb.Append((const UTF8Char*)"The destination host is administratively prohibited");
							break;
						case 11:
							sb.Append((const UTF8Char*)"The network is unreachable for Type Of Service");
							break;
						case 12:
							sb.Append((const UTF8Char*)"The host is unreachable for Type Of Service");
							break;
						case 13:
							sb.Append((const UTF8Char*)"Communication Administratively Prohibited");
							break;
						case 14:
							sb.Append((const UTF8Char*)"Host precedence violation");
							break;
						case 15:
							sb.Append((const UTF8Char*)"Precedence cutoff in effect");
							break;
						default:
							sb.AppendU16(ipData[1]);
							break;
						}
						sb.Append((const UTF8Char*)") from ");
						break;
					case 8:
						sb.Append((const UTF8Char*)"Ping Request from ");
						break;
					case 11:
						sb.Append((const UTF8Char*)"Time Exceeded from ");
						break;
					default:
						sb.Append((const UTF8Char*)"Unknown (");
						sb.AppendU16(ipData[0]);
						sb.Append((const UTF8Char*)") from ");
						break;
					}
					Net::SocketUtil::GetIPv4Name(sbuff, ip->srcIP);
					sb.Append(sbuff);
					sb.Append((const UTF8Char*)", ttl = ");
					sb.AppendU16(packet[8]);
					sb.Append((const UTF8Char*)", size = ");
					sb.AppendUOSInt(ipDataSize);
					Sync::MutexUsage mutUsage(ipLog->mut);
					while (ipLog->logList->GetCount() >= IPLOGCNT)
					{
						Text::StrDelNew(ipLog->logList->RemoveAt(0));
					}
					ipLog->logList->Add(Text::StrCopyNew(sb.ToString()));
					mutUsage.EndUse();
				}
				mutUsage.EndUse();

				if (ipData[0] == 8)
				{
					if (this->pingv4ReqHdlr)
					{
						this->pingv4ReqHdlr(this->pingv4ReqObj, ip->srcIP, ip->destIP, packet[8], ipDataSize);
					}
				}
			}
			break;
		case 2: //IGMP
/*
01 00 5E 00 00 01 B4 75 0E 67 3E 0E 08 00 45 00 
00 1C 00 00 40 00 01 02 D9 35 C0 A8 00 01 E0 00 
00 01 11 0A EE F5 00 00 00 00 00 00 00 00 00 00 
00 00 00 00 00 00 00 00 E3 8E 8D E8 
*/
			ip->otherCnt++;
			ip->otherSize += ipDataSize;
			break;
		case 6:
			ip->tcpCnt++;
			ip->tcpSize += ipDataSize;
			break;
		case 17:
			ip->udpCnt++;
			ip->udpSize += ipDataSize;
			if (ipDataSize >= 8)
			{
				UInt16 srcPort = ReadMUInt16(&ipData[0]);
				UInt16 destPort = ReadMUInt16(&ipData[2]);
				UInt16 udpLeng = ReadMUInt16(&ipData[4]);
				//UInt16 checksum = ReadMUInt16(&ipData[6]);
				valid = false;
				if (udpLeng <= ipDataSize)
				{
					if (srcPort == 53) //DNS Reply
					{
						if (this->atype & (AT_DNSREQ | AT_DNSTARGET))
						{
							Data::ArrayList<Net::DNSClient::RequestAnswer*> answers;
							Net::DNSClient::RequestAnswer *answer;
							Net::DNSClient::ParseAnswers(&ipData[8], ipDataSize - 8, &answers);
							if (answers.GetCount() > 0)
							{
								DNSRequestResult *req;
								const UTF8Char *reqName;
								Data::DateTime currTime;
								currTime.SetCurrTimeUTC();
								reqName = answers.GetItem(0)->name;
								answer = answers.GetItem(answers.GetCount() - 1);
								if (answer->recType == 1)
								{
									if (this->atype & AT_DNSREQ)
									{
										Sync::MutexUsage mutUsage(this->dnsReqv4Mut);
										req = this->dnsReqv4Map->Get(reqName);
										if (req)
										{
											Sync::MutexUsage mutUsage(req->mut);
											req->status = 0;
											req->recSize = ipDataSize - 8;
											MemCopyNO(req->recBuff, &ipData[8], ipDataSize - 8);
											req->reqTime = currTime.ToTicks();
											req->ttl = Net::DNSClient::GetResponseTTL(req->recBuff, req->recSize);
											mutUsage.EndUse();
										}
										else
										{
											req = MemAlloc(DNSRequestResult, 1);
											NEW_CLASS(req->mut, Sync::Mutex());
											req->status = 0;
											req->recSize = ipDataSize - 8;
											MemCopyNO(req->recBuff, &ipData[8], ipDataSize - 8);
											req->reqTime = currTime.ToTicks();
											req->ttl = Net::DNSClient::GetResponseTTL(req->recBuff, req->recSize);
											this->dnsReqv4Map->Put(reqName, req);
										}
										mutUsage.EndUse();
									}
									if (this->atype & AT_DNSTARGET)
									{
										UOSInt i;
										UOSInt j;
										UInt32 resIP;
										UInt32 sortIP;
										DNSTargetInfo *dnsTarget;
										i = answers.GetCount();
										while (i-- > 0)
										{
											answer = answers.GetItem(i);
											if (answer->recType == 1)
											{
												resIP = ReadNUInt32(answer->addr.addr);
												sortIP = ReadMUInt32(answer->addr.addr);
												Sync::MutexUsage dnsTargetMutUsage(this->dnsTargetMut);
												dnsTarget = this->dnsTargetMap->Get(sortIP);
												if (dnsTarget == 0)
												{
													dnsTarget = MemAlloc(DNSTargetInfo, 1);
													dnsTarget->ip = resIP;
													NEW_CLASS(dnsTarget->mut, Sync::Mutex());
													NEW_CLASS(dnsTarget->addrList, Data::ArrayListICaseStrUTF8());
													this->dnsTargetMap->Put(sortIP, dnsTarget);
												}
												Sync::MutexUsage mutUsage(dnsTarget->mut);
												dnsTargetMutUsage.EndUse();
												if (dnsTarget->addrList->SortedIndexOf(answer->name) < 0)
												{
													dnsTarget->addrList->SortedInsert(Text::StrCopyNew(answer->name));
												}
												j = i;
												while (j-- > 0)
												{
													answer = answers.GetItem(j);
													if (answer->recType == 5)
													{
														if (dnsTarget->addrList->SortedIndexOf(answer->name) < 0)
														{
															dnsTarget->addrList->SortedInsert(Text::StrCopyNew(answer->name));
														}
													}
												}
												mutUsage.EndUse();
											}
										}													
									}
								}
								else if (answer->recType == 28)
								{
									if (this->atype & AT_DNSREQ)
									{
										Sync::MutexUsage mutUsage(this->dnsReqv6Mut);
										req = this->dnsReqv6Map->Get(reqName);
										if (req)
										{
											Sync::MutexUsage mutUsage(req->mut);
											req->status = 0;
											req->recSize = ipDataSize - 8;
											MemCopyNO(req->recBuff, &ipData[8], ipDataSize - 8);
											req->reqTime = currTime.ToTicks();
											req->ttl = Net::DNSClient::GetResponseTTL(req->recBuff, req->recSize);
											mutUsage.EndUse();
										}
										else
										{
											req = MemAlloc(DNSRequestResult, 1);
											NEW_CLASS(req->mut, Sync::Mutex());
											req->status = 0;
											req->recSize = ipDataSize - 8;
											MemCopyNO(req->recBuff, &ipData[8], ipDataSize - 8);
											req->reqTime = currTime.ToTicks();
											req->ttl = Net::DNSClient::GetResponseTTL(req->recBuff, req->recSize);
											this->dnsReqv6Map->Put(reqName, req);
										}
										mutUsage.EndUse();
									}
								}
								else
								{
									if (this->atype & AT_DNSREQ)
									{
										Sync::MutexUsage mutUsage(this->dnsReqOthMut);
										req = this->dnsReqOthMap->Get(reqName);
										if (req)
										{
											Sync::MutexUsage mutUsage(req->mut);
											req->status = 0;
											req->recSize = ipDataSize - 8;
											MemCopyNO(req->recBuff, &ipData[8], ipDataSize - 8);
											req->reqTime = currTime.ToTicks();
											req->ttl = Net::DNSClient::GetResponseTTL(req->recBuff, req->recSize);
											mutUsage.EndUse();
										}
										else
										{
											req = MemAlloc(DNSRequestResult, 1);
											NEW_CLASS(req->mut, Sync::Mutex());
											req->status = 0;
											req->recSize = ipDataSize - 8;
											MemCopyNO(req->recBuff, &ipData[8], ipDataSize - 8);
											req->reqTime = currTime.ToTicks();
											req->ttl = Net::DNSClient::GetResponseTTL(req->recBuff, req->recSize);
											this->dnsReqOthMap->Put(reqName, req);
										}
										mutUsage.EndUse();
									}
								}
								Net::DNSClient::FreeAnswers(&answers);
							}
						}
						valid = true;
					}
					else if (destPort == 53) //DNS Request
					{
						if (this->atype & AT_DNSCLI)
						{
							DNSClientInfo *dnsCli;
							UInt32 cliId = ReadMUInt32(&packet[12]);
							Sync::MutexUsage dnsCliInfoMutUsage(this->dnsCliInfoMut);
							dnsCli = this->dnsCliInfos->Get(cliId);
							if (dnsCli == 0)
							{
								dnsCli = MemAlloc(DNSClientInfo, 1);
								dnsCli->cliId = cliId;
								dnsCli->addr.addrType = Net::SocketUtil::AT_IPV4;
								WriteMUInt32(dnsCli->addr.addr, cliId);
								NEW_CLASS(dnsCli->mut, Sync::Mutex());
								NEW_CLASS(dnsCli->hourInfos, Data::ArrayList<DNSCliHourInfo*>());
								this->dnsCliInfos->Put(cliId, dnsCli);
							}
							dnsCliInfoMutUsage.EndUse();
							Data::DateTime dt;
							DNSCliHourInfo *hInfo;
							dt.SetCurrTimeUTC();
							Sync::MutexUsage mutUsage(dnsCli->mut);
							hInfo = dnsCli->hourInfos->GetItem(0);
							if (hInfo != 0 && hInfo->year == dt.GetYear() && hInfo->month == dt.GetMonth() && hInfo->day == dt.GetDay() && hInfo->hour == dt.GetHour())
							{
								hInfo->reqCount++;
							}
							else
							{
								if (dnsCli->hourInfos->GetCount() >= 72)
								{
									hInfo = dnsCli->hourInfos->RemoveAt(71);
								}
								else
								{
									hInfo = MemAlloc(DNSCliHourInfo, 1);
								}
								hInfo->year = dt.GetYear();
								hInfo->month = dt.GetMonth();
								hInfo->day = dt.GetDay();
								hInfo->hour = dt.GetHour();
								hInfo->reqCount = 1;
								dnsCli->hourInfos->Insert(0, hInfo);
							}
							mutUsage.EndUse();
						}
						valid = true;
					}
					else if (srcPort == 67 || destPort == 67) //DHCP Request/Reply
					{
						UInt8 macBuff[8];
						UInt64 iMAC;
						DHCPInfo *dhcp;
						if (ipDataSize >= 248 && ReadMUInt32(&ipData[244]) == 0x63825363)
						{
							macBuff[0] = 0;
							macBuff[1] = 0;
							macBuff[2] = ipData[36];
							macBuff[3] = ipData[37];
							macBuff[4] = ipData[38];
							macBuff[5] = ipData[39];
							macBuff[6] = ipData[40];
							macBuff[7] = ipData[41];
							iMAC = ReadMUInt64(macBuff);
							Sync::MutexUsage dhcpMutUsage(this->dhcpMut);
							dhcp = this->dhcpMap->Get(iMAC);
							if (dhcp == 0)
							{
								dhcp = MemAlloc(DHCPInfo, 1);
								MemClear(dhcp, sizeof(DHCPInfo));
								dhcp->iMAC = iMAC;
								NEW_CLASS(dhcp->mut, Sync::Mutex());
								this->dhcpMap->Put(iMAC, dhcp);
							}
							dhcpMutUsage.EndUse();
							const UInt8 *currPtr = &ipData[248];
							const UInt8 *endPtr = &ipData[ipDataSize];
							UInt8 msgType = 0;
							UInt8 t;
							UInt8 len;
							UTF8Char *sptr;
							Sync::MutexUsage mutUsage(dhcp->mut);
							dhcp->updated = true;
							while (currPtr < endPtr)
							{
								t = *currPtr++;
								if (t == 255)
								{
									break;
								}
								if (currPtr >= endPtr)
									break;
								len = *currPtr++;
								if (currPtr + len > endPtr)
									break;
								if (t == 53 && len == 1)
								{
									msgType = currPtr[0];
								}
								else if (t == 12 && len >= 1 && msgType == 1)
								{
									if (dhcp->hostName == 0)
									{
										sptr = MemAlloc(UTF8Char, (UOSInt)len + 1);
										MemCopyNO(sptr, currPtr, len);
										sptr[len] = 0;
										dhcp->hostName = sptr;
									}
								}
								else if (t == 60 && len >= 1 && msgType == 1)
								{
									if (dhcp->vendorClass == 0)
									{
										sptr = MemAlloc(UTF8Char, (UOSInt)len + 1);
										MemCopyNO(sptr, currPtr, len);
										sptr[len] = 0;
										dhcp->vendorClass = sptr;
									}
								}
								else if (t == 1 && len == 4 && (msgType == 2 || msgType == 5))
								{
									dhcp->subnetMask = ReadNUInt32(currPtr);
								}
								else if (t == 3 && len == 4 && (msgType == 2 || msgType == 5))
								{
									dhcp->router = ReadNUInt32(currPtr);
								}
								else if (t == 6 && len > 0 && (len & 3) == 0 && (msgType == 2 || msgType == 5))
								{
									OSInt i = 0;
									OSInt cnt = len >> 2;
									if (cnt > 4)
										cnt = 4;
									while (i < cnt)
									{
										dhcp->dns[i] = ReadNUInt32(&currPtr[i * 4]);
										i++;
									}
									while (cnt < 4)
									{
										dhcp->dns[i] = 0;
										cnt++;
									}
								}
								else if (t == 54 && len == 4 && (msgType == 2 || msgType == 5))
								{
									dhcp->dhcpServer = ReadNUInt32(currPtr);
								}
								else if (t == 51 && len == 4 && (msgType == 2 || msgType == 5))
								{
									dhcp->ipAddr = ReadNUInt32(&ipData[24]);
									dhcp->gwAddr = ReadNUInt32(&ipData[32]);
									dhcp->ipAddrLease = ReadMUInt32(currPtr);
									Data::DateTime dt;
									dt.SetCurrTimeUTC();
									dhcp->ipAddrTime = dt.ToTicks();
								}
								else if (t == 58 && len == 4 && (msgType == 2 || msgType == 5))
								{
									dhcp->renewTime = ReadMUInt32(currPtr);
								}
								else if (t == 59 && len == 4 && (msgType == 2 || msgType == 5))
								{
									dhcp->rebindTime = ReadMUInt32(currPtr);
								}
								currPtr += len;
							}
							mutUsage.EndUse();
						}
						valid = true;
					}
					else if (srcPort == 123 || destPort == 123) // NTP
					{
						if (this->atype & AT_IPLOG)
						{
							if (destPort == 123)
							{
								if ((ipData[8] & 7) == 3) //client
								{
									Text::StringBuilderUTF8 sb;
									IPLogInfo *ipLog;
									UInt32 sortableIP = ReadMUInt32(&packet[12]);
									Sync::MutexUsage ipLogMutUsage(this->ipLogMut);
									ipLog = this->ipLogMap->Get(sortableIP);
									if (ipLog == 0)
									{
										ipLog = MemAlloc(IPLogInfo, 1);
										ipLog->ip = ip->srcIP;
										NEW_CLASS(ipLog->mut, Sync::Mutex());
										NEW_CLASS(ipLog->logList, Data::ArrayList<const UTF8Char*>());
										this->ipLogMap->Put(sortableIP, ipLog);
									}
									ipLogMutUsage.EndUse();
									Data::DateTime dt;
									dt.SetCurrTime();
									sb.AppendDate(&dt);
									sb.Append((const UTF8Char*)" UDP Port ");
									sb.AppendU16(srcPort);
									sb.Append((const UTF8Char*)" NTP request to ");
									Net::SocketUtil::GetIPv4Name(sbuff, ip->destIP);
									sb.Append(sbuff);
									Sync::MutexUsage mutUsage(ipLog->mut);
									while (ipLog->logList->GetCount() >= IPLOGCNT)
									{
										Text::StrDelNew(ipLog->logList->RemoveAt(0));
									}
									ipLog->logList->Add(Text::StrCopyNew(sb.ToString()));
									mutUsage.EndUse();
								}
							}
							else
							{
								if ((ipData[8] & 7) == 4) //server
								{
									Text::StringBuilderUTF8 sb;
									IPLogInfo *ipLog;
									UInt32 sortableIP = ReadMUInt32(&packet[16]);
									Sync::MutexUsage ipLogMutUsage(this->ipLogMut);
									ipLog = this->ipLogMap->Get(sortableIP);
									if (ipLog == 0)
									{
										ipLog = MemAlloc(IPLogInfo, 1);
										ipLog->ip = ip->destIP;
										NEW_CLASS(ipLog->mut, Sync::Mutex());
										NEW_CLASS(ipLog->logList, Data::ArrayList<const UTF8Char*>());
										this->ipLogMap->Put(sortableIP, ipLog);
									}
									ipLogMutUsage.EndUse();
									Data::DateTime dt;
									dt.SetCurrTime();
									sb.AppendDate(&dt);
									sb.Append((const UTF8Char*)" UDP Port ");
									sb.AppendU16(destPort);
									sb.Append((const UTF8Char*)" NTP reply from ");
									Net::SocketUtil::GetIPv4Name(sbuff, ip->srcIP);
									sb.Append(sbuff);
									sb.Append((const UTF8Char*)", time = ");
									Net::NTPServer::ReadTime(&ipData[40], &dt);
									dt.ToString(sbuff, "yyyy-MM-dd HH:mm:ss.fff");
									sb.Append(sbuff);
									Sync::MutexUsage mutUsage(ipLog->mut);
									while (ipLog->logList->GetCount() >= IPLOGCNT)
									{
										Text::StrDelNew(ipLog->logList->RemoveAt(0));
									}
									ipLog->logList->Add(Text::StrCopyNew(sb.ToString()));
									mutUsage.EndUse();
								}
							}
						}
						valid = true;
					}
					else if (srcPort == 137 && destPort == 137) // NetBIOS-NS (RFC 1002)
					{
						//UInt16 name_trn_id = ReadMUInt16(&ipData[8]);
//									Bool isResponse = (ipData[10] & 0x80) != 0;
						UInt8 opcode = (ipData[10] & 0x78) >> 3;
						if (opcode == 5)
						{
/*
FF FF FF FF FF FF 88 B1 11 39 8B 9F 08 00 45 00 
00 60 17 FC 00 00 80 11 9D B4 C0 A8 01 8D C0 A8 
01 FF 00 89 00 89 00 4C 2A 1E AC 87 29 10 00 01 
00 00 00 00 00 01 20 46 44 45 49 45 46 46 43 45 
50 45 4F 43 41 43 41 43 41 43 41 43 41 43 41 43 
41 43 41 43 41 41 41 00 00 20 00 01 C0 0C 00 20 
00 01 00 04 93 E0 00 06 00 00 C0 A8 01 8D
*/
//////////////////////////////////////
							valid = true;
						}
						else
						{
							valid = true;
						}
					}
					else if (srcPort == 138 && destPort == 138) // NetBIOS-DGM
					{
/*
FF FF FF FF FF FF 00 11 32 0A AB 9C 08 00 45 00 
00 E5 00 00 40 00 40 11 B7 A4 C0 A8 00 14 C0 A8 
00 FF 00 8A 00 8A 00 D1 43 0B 11 0A 36 CC C0 A8 
00 14 00 8A 00 BB 00 00 20 46 44 46 44 46 48 46 
43 46 50 45 45 46 44 44 43 44 42 44 42 45 4B 43 
41 43 41 43 41 43 41 41 41 00 20 46 48 45 50 45 
4F 45 48 43 41 43 41 43 41 43 41 43 41 43 41 43 
41 43 41 43 41 43 41 43 41 42 4E 00 FF 53 4D 42 
25 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
00 00 00 00 00 00 00 00 00 00 00 00 11 00 00 21 
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
00 00 00 21 00 56 00 03 00 01 00 01 00 02 00 32 
00 5C 4D 41 49 4C 53 4C 4F 54 5C 42 52 4F 57 53 
45 00 01 01 80 FC 0A 00 53 53 57 52 5F 44 53 32 
31 31 4A 00 00 00 00 00 06 01 03 9A 80 00 0F 01 
55 AA 00 
*/
						UInt8 msgType = ipData[8];
						//UInt8 flags = ipData[9];
						//UInt16 dgm_id = ReadMInt16(&ipData[10]);
						//UInt32 source_ip = ReadNInt32(&ipData[12]);
						//UInt16 source_port = ReadMInt16(&ipData[16]);
						//UInt16 dgm_length = ReadMInt16(&ipData[18]);
						//UInt16 packet_offset = ReadMInt16(&ipData[20]);
						switch (msgType)
						{
						case 0x10: //Direct Unique Datagram
						case 0x11: //Direct Group Datagram
						case 0x12: //Broadcast Datagram
							if (ipData[22] == 0x20 && ipData[55] == 0 && ipData[56] == 0x20 && ipData[89] == 0)
							{
								if (this->atype & AT_DEVICE)
								{
									Sync::MutexUsage mutUsage(this->macMut);
									mac = this->MACGet(srcMAC);
									if (mac->name == 0)
									{
										MemCopyNO(sbuff, &ipData[23], 32);
										NetBIOSDecName(sbuff, 32);
										Text::StrRTrim(sbuff);
										mac->name = Text::StrCopyNew(sbuff);
/*										NetBIOSDecName(&ipData[57], 32);
										console->Write(&ipData[23]);
										console->Write((const UTF8Char*)", ");
										console->Write(&ipData[57]);
										console->Write((const UTF8Char*)", ");
										Net::SocketUtil::GetIPv4Name(&ipData[23], ip->srcIP);
										console->Write(&ipData[23]);
										console->Write((const UTF8Char*)", ");
										Net::SocketUtil::GetIPv4Name(&ipData[23], ip->destIP);
										console->WriteLine(&ipData[23]);*/
									}
									mutUsage.EndUse();
								}
								valid = true;
							}
							break;
						case 0x13: //Datagram Error
							//UInt16 error_code = ReadMInt16(&ipData[22]);
							valid = true;
							break;
						case 0x14: //Datagram Query Request
						case 0x15: //Datagram Positive Query Response
						case 0x16: //Datagram Negative Query Response
							break;
						}
						//////////////////////////////
					}
					else if (srcPort == 161 || destPort == 161) // SNMP
					{
						////////////////////////////
						valid = true;
					}
					else if (destPort == 1900) //UPnP Discovery / SSDP
					{
						if (this->atype & AT_IPLOG)
						{
							Text::StringBuilderUTF8 sb;
							IPLogInfo *ipLog;
							UInt32 sortableIP = ReadMUInt32(&packet[12]);
							Sync::MutexUsage ipLogMutUsage(this->ipLogMut);
							ipLog = this->ipLogMap->Get(sortableIP);
							if (ipLog == 0)
							{
								ipLog = MemAlloc(IPLogInfo, 1);
								ipLog->ip = ip->srcIP;
								NEW_CLASS(ipLog->mut, Sync::Mutex());
								NEW_CLASS(ipLog->logList, Data::ArrayList<const UTF8Char*>());
								this->ipLogMap->Put(sortableIP, ipLog);
							}
							ipLogMutUsage.EndUse();
							Data::DateTime dt;
							dt.SetCurrTime();
							sb.AppendDate(&dt);
							sb.Append((const UTF8Char*)" UDP Port ");
							sb.AppendU16(srcPort);
							sb.Append((const UTF8Char*)" SSDP to ");
							Net::SocketUtil::GetIPv4Name(sbuff, ip->destIP);
							sb.Append(sbuff);
//							ipData[udpLeng] = 0;
							UOSInt i = Text::StrIndexOf(&ipData[8], (const UTF8Char*)" * ");
							if (i != INVALID_INDEX && i > 0)
							{
								sb.Append((const UTF8Char*)", method = ");
								sb.AppendC(&ipData[8], i);
							}
							Sync::MutexUsage mutUsage(ipLog->mut);
							while (ipLog->logList->GetCount() >= IPLOGCNT)
							{
								Text::StrDelNew(ipLog->logList->RemoveAt(0));
							}
							ipLog->logList->Add(Text::StrCopyNew(sb.ToString()));
							mutUsage.EndUse();
						}
						valid = true;
//									ipData[udpLeng] = 0;
//									console->WriteLine(&ipData[8]);
					}
					else if (srcPort == 3702 || destPort == 3702) //WS-Discovery
					{
/*
<?xml version="1.0" encoding="utf-8"?>
<soap:Envelope xmlns:soap="http://www.w3.org/2003/05/soap-envelope" xmlns:wsa="http://schemas.xmlsoap.org/ws/2004/08/addressing" xmlns:wsd="http://schemas.xmlsoap.org/ws/2005/04/discovery" xmlns:wsdp="http://schemas.xmlsoap.org/ws/2006/02/devprof">
<soap:Header>
<wsa:To>urn:schemas-xmlsoap-org:ws:2005:04:discovery</wsa:To>
<wsa:Action>http://schemas.xmlsoap.org/ws/2005/04/discovery/Probe</wsa:Action>
<wsa:MessageID>urn:uuid:404c5a42-ff6c-47d7-b3cd-02be780cc179</wsa:MessageID>
</soap:Header>
<soap:Body>
<wsd:Probe>
<wsd:Types>wsdp:Device</wsd:Types>
</wsd:Probe>
</soap:Body>
</soap:Envelope>
*/
//									ipData[udpLeng] = 0;
//									console->WriteLine(&ipData[8]);
						valid = true;
					}
					else if (srcPort == 5353 && destPort == 5353) //mDNS
					{
						////////////////////////////
						valid = true;
					}
					else if (destPort == 5355) //LLMNR
					{
/*
01 00 5E 00 00 FC 6C 4B 90 24 5C 52 08 00 45 00 
00 3A 13 A2 00 00 01 11 04 3B C0 A8 00 32 E0 00 
00 FC D3 1E 14 EB 00 26 77 8F 63 88 00 00 00 01 
00 00 00 00 00 00 0C 4C 49 4E 4B 53 59 53 31 32 
36 33 39 00 00 01 00 01 
*/
						if (this->atype & AT_DEVICE)
						{
							Sync::MutexUsage mutUsage(this->macMut);
							mac = this->MACGet(srcMAC);
							if (mac->name == 0)
							{
								Text::StrConcatC(sbuff, &ipData[21], ipData[20]);
								mac->name = Text::StrCopyNew(sbuff);
							}
							mutUsage.EndUse();
						}
						valid = true;
					}
					else if ((srcPort > 0 && srcPort < 10000) || (destPort > 0 && destPort < 10000))
					{
					}
					else
					{
						valid = true;
					}
					
				}
			}
			else
			{
				valid = false;
			}
			
			break;
		default:
			ip->otherCnt++;
			ip->otherSize += ipDataSize;
			valid = false;
			break;
		}
		ipTranMutUsage.EndUse();
	}
	else
	{
		valid = false;
	}
	return valid;
}

Bool Net::EthernetAnalyzer::PacketIPv6(const UInt8 *packet, UOSInt packetSize, UInt64 srcMAC, UInt64 destMAC)
{
	MACStatus *mac;
	Bool valid = true;
	if ((packet[0] & 0xf0) == 0x60 && packetSize >= 40)
	{
		if (this->atype & AT_DEVICE)
		{
			Sync::MutexUsage mutUsage(this->macMut);
			mac = this->MACGet(srcMAC);
			if (mac->ipv6Addr.addrType == Net::SocketUtil::AT_UNKNOWN)
			{
				mac->ipv6Addr.addrType = Net::SocketUtil::AT_IPV6;
				MemCopyNO(mac->ipv6Addr.addr, &packet[8], 16);
			}
			mac->ipv6SrcCnt++;

			mac = this->MACGet(destMAC);
			if (mac->ipv6Addr.addrType == Net::SocketUtil::AT_UNKNOWN)
			{
				mac->ipv6Addr.addrType = Net::SocketUtil::AT_IPV6;
				MemCopyNO(mac->ipv6Addr.addr, &packet[24], 16);
			}
			mac->ipv6DestCnt++;
			mutUsage.EndUse();
		}
	}
	else
	{
		valid = false;
	}		
	return valid;
}

Bool Net::EthernetAnalyzer::PacketARP(const UInt8 *packet, UOSInt packetSize, UInt64 srcMAC, UInt64 destMAC)
{
	Bool valid = true;
	MACStatus *mac;
	if (packetSize >= 28)
	{
		UInt16 htype = ReadMUInt16(&packet[0]);
		UInt16 ptype = ReadMUInt16(&packet[2]);
		if (htype == 1 && ptype == 0x0800 && packet[4] == 6 && packet[5] == 4)
		{
			//UInt16 oper = ReadMUInt16(&packet[6]); //1 = request, 2 = reply
			//MemCopyNO(senderHWAddr, &packet[8], 6);
			//MemCopyNO(senderProtoAddr, &packet[14], 4);
			//MemCopyNO(targetHWAddr, &packet[18], 6);
			//MemCopyNO(targetProtoAddr, &packet[24], 4);

			if (this->atype & AT_DEVICE)
			{
				Sync::MutexUsage mutUsage(this->macMut);
				mac = this->MACGet(srcMAC);
				mac->othSrcCnt++;

				mac = this->MACGet(destMAC);
				mac->othDestCnt++;
				mutUsage.EndUse();
			}
		}
		else
		{
			valid = false;
		}
	}
	else
	{
		valid = false;
	}
	return valid;
}

Net::EthernetAnalyzer::AnalyzeType Net::EthernetAnalyzer::GetAnalyzeType()
{
	return this->atype;
}

void Net::EthernetAnalyzer::HandlePingv4Request(Pingv4Handler pingv4Hdlr, void *userObj)
{
	this->pingv4ReqObj = userObj;
	this->pingv4ReqHdlr = pingv4Hdlr;
}

Bool Net::EthernetAnalyzer::PacketDataGetName(UInt32 linkType, const UInt8 *packet, UOSInt packetSize, Text::StringBuilderUTF *sb)
{
	switch (linkType)
	{
	case 0:
		return PacketNullGetName(packet, packetSize, sb);
	case 1:
		return PacketEthernetGetName(packet, packetSize, sb);
	case 101:
		return PacketIPv4GetName(packet, packetSize, sb);
	case 113:
		return PacketLinuxGetName(packet, packetSize, sb);
	}
	return false;
}

Bool Net::EthernetAnalyzer::PacketNullGetName(const UInt8 *packet, UOSInt packetSize, Text::StringBuilderUTF *sb)
{
	UInt32 packetType = ReadMUInt32(packet);
	switch (packetType)
	{
		case 2:
			return PacketIPv4GetName(&packet[4], packetSize - 4, sb);
		case 24:
		case 28:
		case 30:
			return PacketIPv6GetName(&packet[4], packetSize - 4, sb);
	}
	return false;
}

Bool Net::EthernetAnalyzer::PacketEthernetGetName(const UInt8 *packet, UOSInt packetSize, Text::StringBuilderUTF *sb)
{
	return PacketEthernetDataGetName(ReadMUInt16(&packet[12]), &packet[14], packetSize - 14, sb);
}

Bool Net::EthernetAnalyzer::PacketLinuxGetName(const UInt8 *packet, UOSInt packetSize, Text::StringBuilderUTF *sb)
{
	return PacketEthernetDataGetName(ReadMUInt16(&packet[14]), &packet[16], packetSize - 16, sb);
}

Bool Net::EthernetAnalyzer::PacketEthernetDataGetName(UInt16 etherType, const UInt8 *packet, UOSInt packetSize, Text::StringBuilderUTF *sb)
{
	switch (etherType)
	{
	case 0x0004: //IEEE 802.2 LLC
		sb->Append((const UTF8Char*)"IEEE802.2 LLC");
		return true;
	case 0x0006: //ARP
		sb->Append((const UTF8Char*)"ARP");
		return true;
	case 0x26:
		sb->Append((const UTF8Char*)"IEEE802.2 LLC");
		return true;
	case 0x0800: //IPv4
		return PacketIPv4GetName(packet, packetSize, sb);
	case 0x0806: //ARP
		sb->Append((const UTF8Char*)"ARP");
		return true;
	case 0x86DD: //IPv6
		return PacketIPv6GetName(packet, packetSize, sb);
	default:
		return false;
	}
}

Bool Net::EthernetAnalyzer::PacketIPv4GetName(const UInt8 *packet, UOSInt packetSize, Text::StringBuilderUTF *sb)
{
	UTF8Char sbuff[32];
	if ((packet[0] & 0xf0) != 0x40)
	{
		return false;
	}

	UInt32 srcIP = ReadNUInt32(&packet[12]);
	UInt32 destIP = ReadNUInt32(&packet[16]);
	const UInt8 *ipData;
	UOSInt ipDataSize;

	if ((packet[0] & 0xf) <= 5)
	{
		ipData = &packet[20];
		ipDataSize = packetSize - 20;
	}
	else
	{
		ipData = &packet[(packet[0] & 0xf) << 2];
		ipDataSize = packetSize - ((packet[0] & 0xf) << 2);
	}

	Net::SocketUtil::GetIPv4Name(sbuff, srcIP);
	sb->Append(sbuff);
	sb->Append((const UTF8Char*)" -> ");
	Net::SocketUtil::GetIPv4Name(sbuff, destIP);
	sb->Append(sbuff);
	sb->AppendChar(' ', 1);
	return PacketIPDataGetName(packet[9], ipData, ipDataSize, sb);
}

Bool Net::EthernetAnalyzer::PacketIPv6GetName(const UInt8 *packet, UOSInt packetSize, Text::StringBuilderUTF *sb)
{
	UTF8Char sbuff[64];
	if ((packet[0] & 0xf0) != 0x60 || packetSize < 40)
	{
		return false;
	}

	Net::SocketUtil::AddressInfo srcAddr;
	Net::SocketUtil::AddressInfo destAddr;
	Net::SocketUtil::SetAddrInfoV6(&srcAddr, &packet[8], 0);
	Net::SocketUtil::SetAddrInfoV6(&destAddr, &packet[24], 0);
	Net::SocketUtil::GetAddrName(sbuff, &srcAddr);
	sb->Append(sbuff);
	sb->Append((const UTF8Char*)" -> ");
	Net::SocketUtil::GetAddrName(sbuff, &destAddr);
	sb->Append(sbuff);
	sb->AppendChar(' ', 1);
	return PacketIPDataGetName(packet[6], &packet[40], packetSize - 40, sb);
}

Bool Net::EthernetAnalyzer::PacketIPDataGetName(UInt8 protocol, const UInt8 *packet, UOSInt packetSize, Text::StringBuilderUTF *sb)
{
	switch (protocol)
	{
	case 0:
		sb->Append((const UTF8Char*)"HOPOPT");
		return true;
	case 1:
		sb->Append((const UTF8Char*)"ICMP");
		return true;
	case 2:
		sb->Append((const UTF8Char*)"IGMP");
		return true;
	case 3:
		sb->Append((const UTF8Char*)"GGP");
		return true;
	case 4:
		sb->Append((const UTF8Char*)"IP-in-IP");
		return true;
	case 5:
		sb->Append((const UTF8Char*)"ST");
		return true;
	case 6:
		sb->Append((const UTF8Char*)"TCP");
		return true;
	case 17:
	{
		sb->Append((const UTF8Char*)"UDP");
		if (packetSize >= 4)
		{
			UInt16 destPort = 0;
			const UTF8Char *csptr;
			destPort = ReadMUInt16(&packet[2]);
			csptr = Net::EthernetAnalyzer::UDPPortGetName(destPort);
			sb->AppendChar(' ', 1);
			if (csptr)
			{
				sb->Append(csptr);
			}
			else
			{
				sb->AppendU16(destPort);
			}
		}
		return true;
	}
	case 58:
		sb->Append((const UTF8Char*)"ICMPv6");
		return true;
	default:
		return false;
	}
}

void Net::EthernetAnalyzer::PacketDataGetDetail(UInt32 linkType, const UInt8 *packet, UOSInt packetSize, Text::StringBuilderUTF *sb)
{
	switch (linkType)
	{
	case 0:
		PacketNullGetDetail(packet, packetSize, sb);
		break;
	case 1:
		PacketEthernetGetDetail(packet, packetSize, sb);
		break;
	case 101:
		PacketIPv4GetDetail(packet, packetSize, sb);
		break;
	case 113:
		PacketLinuxGetDetail(packet, packetSize, sb);
		break;
	default:
		sb->Append((const UTF8Char*)"\r\n");
		sb->AppendHexBuff(packet, packetSize, ' ', Text::LBT_CRLF);
	}
}

void Net::EthernetAnalyzer::PacketNullGetDetail(const UInt8 *packet, UOSInt packetSize, Text::StringBuilderUTF *sb)
{
	UInt32 packetType = ReadMUInt32(packet);
	sb->Append((const UTF8Char*)"\r\nPacket Type=");
	sb->AppendU32(packetType);
	switch (packetType)
	{
	case 2:
		PacketIPv4GetDetail(&packet[4], packetSize - 4, sb);
		break;
	case 24:
	case 28:
	case 30:
		PacketIPv6GetDetail(&packet[4], packetSize - 4, sb);
		break;
	}
}

void Net::EthernetAnalyzer::PacketEthernetGetDetail(const UInt8 *packet, UOSInt packetSize, Text::StringBuilderUTF *sb)
{
	const Net::MACInfo::MACEntry *mac;
	sb->Append((const UTF8Char*)"\r\nSrcMAC=");
	sb->AppendHexBuff(&packet[6], 6, ':', Text::LBT_NONE);
	mac = Net::MACInfo::GetMACInfoBuff(&packet[6]);
	sb->Append((const UTF8Char*)" (");
	sb->Append((const UTF8Char*)mac->name);
	sb->Append((const UTF8Char*)")");
	sb->Append((const UTF8Char*)"\r\nDestMAC=");
	sb->AppendHexBuff(&packet[0], 6, ':', Text::LBT_NONE);
	mac = Net::MACInfo::GetMACInfoBuff(&packet[0]);
	sb->Append((const UTF8Char*)" (");
	sb->Append((const UTF8Char*)mac->name);
	sb->Append((const UTF8Char*)")");
	PacketEthernetDataGetDetail(ReadMUInt16(&packet[12]), &packet[14], packetSize - 14, sb);
}

void Net::EthernetAnalyzer::PacketLinuxGetDetail(const UInt8 *packet, UOSInt packetSize, Text::StringBuilderUTF *sb)
{
	sb->Append((const UTF8Char*)"\r\nPacket Type=");
	sb->AppendU16(ReadMUInt16(&packet[0]));
	switch (ReadMUInt16(&packet[0]))
	{
	case 0:
		sb->Append((const UTF8Char*)" (Unicast to us)");
		break;
	case 1:
		sb->Append((const UTF8Char*)" (Broadcast to us)");
		break;
	case 2:
		sb->Append((const UTF8Char*)" (Multicast to us)");
		break;
	case 3:
		sb->Append((const UTF8Char*)" (Sent by somebody else to somebody else)");
		break;
	case 4:
		sb->Append((const UTF8Char*)" (Sent by us)");
		break;
	}
	sb->Append((const UTF8Char*)"\r\nLink-Layer Device Type=");
	sb->AppendU16(ReadMUInt16(&packet[2]));
	switch (ReadMUInt16(&packet[2]))
	{
	case 772:
		sb->Append((const UTF8Char*)" (Link Layer Address Type)");
		break;
	case 778:
		sb->Append((const UTF8Char*)" (IP GRE Protocol Type)");
		break;
	case 803:
		sb->Append((const UTF8Char*)" (IEEE802.11)");
		break;
	}
	sb->Append((const UTF8Char*)"\r\nLink-Layer Address Length=");
	UInt16 len = ReadMUInt16(&packet[4]);
	sb->AppendU16(len);
	if (len > 0)
	{
		sb->Append((const UTF8Char*)"\r\nLink-Layer Address=");
		sb->AppendHexBuff(&packet[6], (len > 8)?8:len, ':', Text::LBT_NONE);
		if (len == 6)
		{
			const Net::MACInfo::MACEntry *macInfo = Net::MACInfo::GetMACInfoBuff(&packet[6]);
			sb->Append((const UTF8Char*)" (");
			sb->Append((const UTF8Char*)macInfo->name);
			sb->Append((const UTF8Char*)")");
		}
	}
	PacketEthernetDataGetDetail(ReadMUInt16(&packet[14]), &packet[16], packetSize - 16, sb);
}

void Net::EthernetAnalyzer::PacketEthernetDataGetDetail(UInt16 etherType, const UInt8 *packet, UOSInt packetSize, Text::StringBuilderUTF *sb)
{
	sb->Append((const UTF8Char*)"\r\nEtherType=0x");
	sb->AppendHex16(etherType);
	switch (etherType)
	{
	case 0x0004: //IEEE802.2 LLC
		sb->Append((const UTF8Char*)" (IEEE802.2 LLC)");
		PacketIEEE802_2LLCGetDetail(packet, packetSize, sb);
		return;
	case 0x0006: //ARP
		sb->Append((const UTF8Char*)" (ARP)");
		sb->Append((const UTF8Char*)"\r\n");
		sb->Append((const UTF8Char*)"\r\nARP:");
		sb->Append((const UTF8Char*)"\r\n");
		sb->AppendHexBuff(packet, packetSize, ' ', Text::LBT_CRLF);
		return;
	case 0x26: //Legnth = 0x26 (IEEE802.2 LLC)
		PacketIEEE802_2LLCGetDetail(packet, packetSize, sb);
		return;
	case 0x0800: //IPv4
		sb->Append((const UTF8Char*)" (IPv4)");
		sb->Append((const UTF8Char*)"\r\n");
		sb->Append((const UTF8Char*)"\r\nIPv4:");
		PacketIPv4GetDetail(packet, packetSize, sb);
		return;
	case 0x0806: //ARP
		sb->Append((const UTF8Char*)" (ARP)");
		sb->Append((const UTF8Char*)"\r\n");
		sb->Append((const UTF8Char*)"\r\nARP:");
		if (packetSize < 22)
		{
			sb->Append((const UTF8Char*)"\r\n");
			sb->AppendHexBuff(packet, packetSize, ' ', Text::LBT_CRLF);
		}
		else
		{
			UInt8 sbuff[32];
			UInt16 htype = ReadMUInt16(&packet[0]);
			UInt16 ptype = ReadMUInt16(&packet[2]);
			UInt8 hlen = packet[4];
			UInt8 plen = packet[5];
			UInt16 oper = ReadMUInt16(&packet[6]);
			sb->Append((const UTF8Char*)"\r\nHardware Type (HTYPE)=");
			sb->AppendU16(htype);
			sb->Append((const UTF8Char*)"\r\nProtocol Type (PTYPE)=0x");
			sb->AppendHex16(ptype);
			sb->Append((const UTF8Char*)"\r\nHardware address length (HLEN)=");
			sb->AppendU16(hlen);
			sb->Append((const UTF8Char*)"\r\nProtocol address length (PLEN)=");
			sb->AppendU16(plen);
			sb->Append((const UTF8Char*)"\r\nOperation (OPER)=");
			sb->AppendU16(oper);
			switch (oper)
			{
			case 1:
				sb->Append((const UTF8Char*)" (Request)");
				break;
			case 2:
				sb->Append((const UTF8Char*)" (Reply)");
				break;
			}
			if (htype == 1 && ptype == 0x0800 && hlen == 6 && plen == 4 && packetSize >= 42)
			{
				sb->Append((const UTF8Char*)"\r\nSender hardware address (SHA)=");
				sb->AppendHexBuff(&packet[8], 6, ':', Text::LBT_NONE);
				sb->Append((const UTF8Char*)"\r\nSender protocol address (SPA)=");
				Net::SocketUtil::GetIPv4Name(sbuff, ReadNUInt32(&packet[14]));
				sb->Append(sbuff);
				sb->Append((const UTF8Char*)"\r\nTarget hardware address (THA)=");
				sb->AppendHexBuff(&packet[18], 6, ':', Text::LBT_NONE);
				sb->Append((const UTF8Char*)"\r\nTarget protocol address (TPA)=");
				Net::SocketUtil::GetIPv4Name(sbuff, ReadNUInt32(&packet[24]));
				sb->Append(sbuff);
				if (packetSize > 28)
				{
					sb->Append((const UTF8Char*)"\r\n");
					sb->Append((const UTF8Char*)"\r\n");
					sb->AppendHexBuff(&packet[28], packetSize - 28, ' ', Text::LBT_CRLF);
				}
			}
			else
			{
				sb->Append((const UTF8Char*)"\r\n");
				sb->AppendHexBuff(packet, packetSize, ' ', Text::LBT_CRLF);
			}
		}
		return;
	case 0x86DD: //IPv6
		sb->Append((const UTF8Char*)" (IPv6)");
		sb->Append((const UTF8Char*)"\r\n");
		sb->Append((const UTF8Char*)"\r\nIPv6:");
		PacketIPv6GetDetail(packet, packetSize, sb);
		return;
	default:
		sb->Append((const UTF8Char*)"\r\n");
		sb->Append((const UTF8Char*)"\r\n");
		sb->AppendHexBuff(packet, packetSize, ' ', Text::LBT_CRLF);
		return;
	}
}

void Net::EthernetAnalyzer::PacketIEEE802_2LLCGetDetail(const UInt8 *packet, UOSInt packetSize, Text::StringBuilderUTF *sb)
{
	const UTF8Char *csptr;
	sb->Append((const UTF8Char*)"\r\n");
	sb->Append((const UTF8Char*)"\r\nIEEE802.2 LLC:");
	sb->Append((const UTF8Char*)"\r\nDSAP Address=0x");
	sb->AppendHex8(packet[0]);
	csptr = LSAPGetName(packet[0]);
	if (csptr)
	{
		sb->Append((const UTF8Char*)" (");
		sb->Append(csptr);
		sb->Append((const UTF8Char*)")");
	}
	sb->Append((const UTF8Char*)"\r\nSSAP Address=0x");
	sb->AppendHex8(packet[1]);
	csptr = LSAPGetName(packet[1]);
	if (csptr)
	{
		sb->Append((const UTF8Char*)" (");
		sb->Append(csptr);
		sb->Append((const UTF8Char*)")");
	}
	sb->Append((const UTF8Char*)"\r\n");
	switch (packet[1])
	{
	case 0x42: //Spanning Tree Protocol (STP)
		if (packetSize >= 38)
		{
			UInt16 protoId;
			sb->Append((const UTF8Char*)"\r\nSpanning Tree Protocol:");
			sb->Append((const UTF8Char*)"\r\nControl=0x");
			sb->AppendHex8(packet[2]);
			sb->Append((const UTF8Char*)"\r\nProtocol ID=0x");
			protoId = ReadMUInt16(&packet[3]);
			sb->AppendHex16(protoId);
			switch (protoId)
			{
			case 0:
				sb->Append((const UTF8Char*)" (IEEE 802.1D)");
				break;
			}
			sb->Append((const UTF8Char*)"\r\nVersion ID=0x");
			sb->AppendHex8(packet[5]);
			switch (packet[5])
			{
			case 0:
				sb->Append((const UTF8Char*)" (Config & TCN)");
				break;
			case 2:
				sb->Append((const UTF8Char*)" (RST)");
				break;
			case 3:
				sb->Append((const UTF8Char*)" (MST)");
				break;
			case 4:
				sb->Append((const UTF8Char*)" (SPT)");
				break;
			}
			sb->Append((const UTF8Char*)"\r\nBPDU Type=0x");
			sb->AppendHex8(packet[6]);
			switch (packet[6])
			{
			case 0:
				sb->Append((const UTF8Char*)" (STP Config BPDU)");
				break;
			case 2:
				sb->Append((const UTF8Char*)" (RST/MST Config BPDU)");
				break;
			case 0x80:
				sb->Append((const UTF8Char*)" (TCN BPDU)");
				break;
			}
			sb->Append((const UTF8Char*)"\r\nFlags=0x");
			sb->AppendHex8(packet[7]);
			sb->Append((const UTF8Char*)"\r\nRoot Bridge Priority=");
			sb->AppendU16((UInt16)(packet[8] >> 4));
			sb->Append((const UTF8Char*)"\r\nRoot Bridge System ID Extension=");
			sb->AppendU16(ReadMUInt16(&packet[8]) & 0xfff);
			sb->Append((const UTF8Char*)"\r\nRoot Bridge MAC Address=");
			sb->AppendHexBuff(&packet[10], 6, ':', Text::LBT_NONE);
			sb->Append((const UTF8Char*)"\r\nRoot Path Cost=");
			sb->AppendU32(ReadMUInt32(&packet[16]));
			sb->Append((const UTF8Char*)"\r\nBridge Priority=");
			sb->AppendU16((UInt16)(packet[20] >> 4));
			sb->Append((const UTF8Char*)"\r\nBridge System ID Extension=");
			sb->AppendU16(ReadMUInt16(&packet[20]) & 0xfff);
			sb->Append((const UTF8Char*)"\r\nBridge MAC Address=");
			sb->AppendHexBuff(&packet[22], 6, ':', Text::LBT_NONE);
			sb->Append((const UTF8Char*)"\r\nPort ID=");
			sb->AppendU16(ReadMUInt16(&packet[28]));
			sb->Append((const UTF8Char*)"\r\nMessage Age=");
			Text::SBAppendF64(sb, ReadMUInt16(&packet[30]) / 256.0);
			sb->Append((const UTF8Char*)"\r\nMax Age=");
			Text::SBAppendF64(sb, ReadMUInt16(&packet[32]) / 256.0);
			sb->Append((const UTF8Char*)"\r\nHello Time=");
			Text::SBAppendF64(sb, ReadMUInt16(&packet[34]) / 256.0);
			sb->Append((const UTF8Char*)"\r\nForward Delay=");
			Text::SBAppendF64(sb, ReadMUInt16(&packet[36]) / 256.0);
			if (packetSize > 38)
			{
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"\r\n");
				sb->AppendHexBuff(&packet[38], packetSize - 38, ' ', Text::LBT_CRLF);
			}
		}
		break;
	default:
		sb->Append((const UTF8Char*)"\r\n");
		sb->AppendHexBuff(&packet[3], packetSize - 3, ' ', Text::LBT_CRLF);
		break;
	}

}

void Net::EthernetAnalyzer::PacketIPv4GetDetail(const UInt8 *packet, UOSInt packetSize, Text::StringBuilderUTF *sb)
{
	if ((packet[0] & 0xf0) != 0x40 || packetSize < 20)
	{
		sb->Append((const UTF8Char*)"\r\nNot IPv4 Packet");
		sb->Append((const UTF8Char*)"\r\n");
		sb->AppendHexBuff(packet, packetSize, ' ', Text::LBT_CRLF);
		return;
	}

	const UInt8 *ipData;
	UOSInt ipDataSize;
	ipDataSize = HeaderIPv4GetDetail(packet, packetSize, sb);
	ipData = &packet[ipDataSize];
	ipDataSize = packetSize - ipDataSize;

	sb->Append((const UTF8Char*)"\r\n");
	PacketIPDataGetDetail(packet[9], ipData, ipDataSize, sb);
}

void Net::EthernetAnalyzer::PacketIPv6GetDetail(const UInt8 *packet, UOSInt packetSize, Text::StringBuilderUTF *sb)
{
	UTF8Char sbuff[64];
	if ((packet[0] & 0xf0) != 0x60 || packetSize < 40)
	{
		sb->Append((const UTF8Char*)"\r\nNot IPv6 Packet");
		return;
	}

	sb->Append((const UTF8Char*)"\r\nVersion=6");
	sb->Append((const UTF8Char*)"\r\nDS=");
	sb->AppendU16((UInt16)(((packet[0] & 0xf) << 2) | (packet[1] >> 6)));
	sb->Append((const UTF8Char*)"\r\nECN=");
	sb->AppendU16((UInt16)((packet[1] & 0x30) >> 4));
	sb->Append((const UTF8Char*)"\r\nFlow Label=");
	sb->AppendU16((UInt16)(((packet[1] & 0xf) << 16) | ReadMUInt16(&packet[2])));
	sb->Append((const UTF8Char*)"\r\nPayload Length=");
	sb->AppendU16(ReadMUInt16(&packet[4]));
	sb->Append((const UTF8Char*)"\r\nNext Header=");
	sb->AppendU16(packet[6]);
	sb->Append((const UTF8Char*)"\r\nHop Limit=");
	sb->AppendU16(packet[7]);
	Net::SocketUtil::AddressInfo addr;
	sb->Append((const UTF8Char*)"\r\nSource Address=");
	Net::SocketUtil::SetAddrInfoV6(&addr, &packet[8], 0);
	Net::SocketUtil::GetAddrName(sbuff, &addr);
	sb->Append(sbuff);
	sb->Append((const UTF8Char*)"\r\nDestination Address=");
	Net::SocketUtil::SetAddrInfoV6(&addr, &packet[24], 0);
	Net::SocketUtil::GetAddrName(sbuff, &addr);
	sb->Append(sbuff);
	sb->Append((const UTF8Char*)"\r\n");
	PacketIPDataGetDetail(packet[6], &packet[40], packetSize - 40, sb);
}

void Net::EthernetAnalyzer::PacketIPDataGetDetail(UInt8 protocol, const UInt8 *packet, UOSInt packetSize, Text::StringBuilderUTF *sb)
{
	UTF8Char sbuff[64];
	switch (protocol)
	{
	case 1: //ICMP
		sb->Append((const UTF8Char*)"\r\nICMP:");
		if (packetSize >= 4)
		{
			UOSInt i = 4;
			sb->Append((const UTF8Char*)"\r\nType=");
			sb->AppendU16(packet[0]);
			switch (packet[0])
			{
			case 0:
				sb->Append((const UTF8Char*)" (Echo Reply)");
				break;
			case 3:
				sb->Append((const UTF8Char*)" (Destination Unreachable)");
				break;
			case 4:
				sb->Append((const UTF8Char*)" (Source Quench)");
				break;
			case 5:
				sb->Append((const UTF8Char*)" (Redirect Message)");
				break;
			case 6:
				sb->Append((const UTF8Char*)" (Alternate Host Address)");
				break;
			case 8:
				sb->Append((const UTF8Char*)" (Echo Request)");
				break;
			case 9:
				sb->Append((const UTF8Char*)" (Router Advertisement)");
				break;
			case 10:
				sb->Append((const UTF8Char*)" (Router Solicitation)");
				break;
			case 11:
				sb->Append((const UTF8Char*)" (Time Exceeded)");
				break;
			case 12:
				sb->Append((const UTF8Char*)" (Parameter Problem: Bad IP header)");
				break;
			case 13:
				sb->Append((const UTF8Char*)" (Timestamp)");
				break;
			case 14:
				sb->Append((const UTF8Char*)" (Timestamp Reply)");
				break;
			case 15:
				sb->Append((const UTF8Char*)" (Information Request)");
				break;
			case 16:
				sb->Append((const UTF8Char*)" (Information Reply)");
				break;
			case 17:
				sb->Append((const UTF8Char*)" (Address Mask Request)");
				break;
			case 18:
				sb->Append((const UTF8Char*)" (Address Mask Reply)");
				break;
			case 30:
				sb->Append((const UTF8Char*)" (Traceroute)");
				break;
			case 42:
				sb->Append((const UTF8Char*)" (Extended Echo Request)");
				break;
			case 43:
				sb->Append((const UTF8Char*)" (Extended Echo Reply)");
				break;
			}
			sb->Append((const UTF8Char*)"\r\nCode=");
			sb->AppendU16(packet[1]);
			switch (packet[0])
			{
			case 3:
				switch (packet[1])
				{
				case 0:
					sb->Append((const UTF8Char*)" (Destination network unreachable)");
					break;
				case 1:
					sb->Append((const UTF8Char*)" (Destination host unreachable)");
					break;
				case 2:
					sb->Append((const UTF8Char*)" (Destination protocol unreachable)");
					break;
				case 3:
					sb->Append((const UTF8Char*)" (Destination port unreachable)");
					break;
				case 4:
					sb->Append((const UTF8Char*)" (Fragmentation required)");
					break;
				case 5:
					sb->Append((const UTF8Char*)" (Source route failed)");
					break;
				case 6:
					sb->Append((const UTF8Char*)" (Destination network unknown)");
					break;
				case 7:
					sb->Append((const UTF8Char*)" (Destination host unknown)");
					break;
				case 8:
					sb->Append((const UTF8Char*)" (Source host isolated)");
					break;
				case 9:
					sb->Append((const UTF8Char*)" (Network administratively prohibited)");
					break;
				case 10:
					sb->Append((const UTF8Char*)" (Host administratively prohibited)");
					break;
				case 11:
					sb->Append((const UTF8Char*)" (Network unreachable for ToS)");
					break;
				case 12:
					sb->Append((const UTF8Char*)" (Host unreachable for ToS)");
					break;
				case 13:
					sb->Append((const UTF8Char*)" (Communication administratively prohibited)");
					break;
				case 14:
					sb->Append((const UTF8Char*)" (Host Precedence Violation)");
					break;
				case 15:
					sb->Append((const UTF8Char*)" (Precedence cutoff in effect)");
					break;
				}
				break;
			case 5:
				switch (packet[1])
				{
				case 0:
					sb->Append((const UTF8Char*)" (Redirect Datagram for the Network)");
					break;
				case 1:
					sb->Append((const UTF8Char*)" (Redirect Datagram for the Host)");
					break;
				case 2:
					sb->Append((const UTF8Char*)" (Redirect Datagram for the ToS & network)");
					break;
				case 3:
					sb->Append((const UTF8Char*)" (Redirect Datagram for the ToS & host)");
					break;
				}
				break;
			case 11:
				switch (packet[1])
				{
				case 0:
					sb->Append((const UTF8Char*)" (TTL expired in transit)");
					break;
				case 1:
					sb->Append((const UTF8Char*)" (Fragment reassembly time exceeded)");
					break;
				}
				break;
			case 12:
				switch (packet[1])
				{
				case 0:
					sb->Append((const UTF8Char*)" (Pointer indicates the error)");
					break;
				case 1:
					sb->Append((const UTF8Char*)" (Missing a required option)");
					break;
				case 2:
					sb->Append((const UTF8Char*)" (Bad length)");
					break;
				}
				break;
			case 43:
				switch (packet[1])
				{
				case 0:
					sb->Append((const UTF8Char*)" (No Error)");
					break;
				case 1:
					sb->Append((const UTF8Char*)" (Malformed Query)");
					break;
				case 2:
					sb->Append((const UTF8Char*)" (No Such Interface)");
					break;
				case 3:
					sb->Append((const UTF8Char*)" (No Such Table Entry)");
					break;
				case 4:
					sb->Append((const UTF8Char*)" (Multiple Interfaces Satisfy Query)");
					break;
				}
				break;
			}
			sb->Append((const UTF8Char*)"\r\nChecksum=0x");
			sb->AppendHex16(ReadMUInt16(&packet[2]));
			switch (packet[0])
			{
			case 3:
				sb->Append((const UTF8Char*)"\r\nNext-hop MTU=");
				sb->AppendU16(ReadMUInt16(&packet[4]));
				i = 8;
				sb->Append((const UTF8Char*)"\r\nOriginal IP Header:");
				i += HeaderIPv4GetDetail(&packet[i], packetSize - i, sb);
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"\r\n");
				sb->AppendHexBuff(&packet[i], packetSize - i, ' ', Text::LBT_CRLF);
				i = packetSize;
				break;
			}
			if (i < packetSize)
			{
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"\r\n");
				sb->AppendHexBuff(&packet[i], packetSize - i, ' ', Text::LBT_CRLF);
			}
		}
		else
		{
			sb->Append((const UTF8Char*)"\r\n");
			sb->AppendHexBuff(packet, packetSize, ' ', Text::LBT_CRLF);
		}
		return;
	case 2: //IGMP
		sb->Append((const UTF8Char*)"\r\nIGMP:");
		if (packet[0] == 0x11)
		{
			sb->Append((const UTF8Char*)"\r\nType=0x11 (IGMPv3 membership query)");
			if (packetSize >= 8)
			{
				UInt16 n;
				sb->Append((const UTF8Char*)"\r\nMax Resp Time=");
				sb->AppendU16(packet[1]);
				sb->Append((const UTF8Char*)"\r\nChecksum=0x");
				sb->AppendHex16(ReadMUInt16(&packet[2]));
				Net::SocketUtil::GetIPv4Name(sbuff, ReadNUInt32(&packet[4]));
				sb->Append((const UTF8Char*)"\r\nGroup Address=");
				sb->Append(sbuff);
				if (packetSize >= 16)
				{
					sb->Append((const UTF8Char*)"\r\nFlags=0x");
					sb->AppendHex8(packet[8]);
					sb->Append((const UTF8Char*)"\r\nQQIC=");
					sb->AppendU16(packet[9]);
					sb->Append((const UTF8Char*)"\r\nQQIC=");
					n = ReadMUInt16(&packet[10]);
					sb->AppendU16(n);
					if (packetSize >= 12 + (UOSInt)n * 4)
					{
						UInt16 i = 0;
						while (i < n)
						{
							sb->Append((const UTF8Char*)"\r\nSource Address[");
							sb->AppendU16(i);
							sb->Append((const UTF8Char*)"]=");
							Net::SocketUtil::GetIPv4Name(sbuff, ReadNUInt32(&packet[12 + i * 4]));
							sb->Append(sbuff);
							i++;
						}
						if (packetSize > 12 + (UOSInt)n * 4)
						{
							sb->Append((const UTF8Char*)"\r\n");
							sb->Append((const UTF8Char*)"\r\n");
							sb->AppendHexBuff(&packet[12 + n * 4], packetSize - 12 - (UOSInt)n * 4, ' ', Text::LBT_CRLF);
						}
					}
				}
				else
				{
					sb->Append((const UTF8Char*)"\r\n");
					sb->Append((const UTF8Char*)"\r\n");
					sb->AppendHexBuff(&packet[8], packetSize - 8, ' ', Text::LBT_CRLF);
				}
			}
			else
			{
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"\r\n");
				sb->AppendHexBuff(&packet[1], packetSize - 1, ' ', Text::LBT_CRLF);
			}
		}
		else if (packet[0] == 0x16)
		{
			sb->Append((const UTF8Char*)"\r\nType=0x16 (IGMPv2 Membership Report)");
			if (packetSize >= 8)
			{
				sb->Append((const UTF8Char*)"\r\nMax Resp Time=");
				sb->AppendU16(packet[1]);
				sb->Append((const UTF8Char*)"\r\nChecksum=0x");
				sb->AppendHex16(ReadMUInt16(&packet[2]));
				Net::SocketUtil::GetIPv4Name(sbuff, ReadNUInt32(&packet[4]));
				sb->Append((const UTF8Char*)"\r\nGroup Address=");
				sb->Append(sbuff);
				if (packetSize > 8)
				{
					sb->Append((const UTF8Char*)"\r\n");
					sb->Append((const UTF8Char*)"\r\n");
					sb->AppendHexBuff(&packet[8], packetSize - 8, ' ', Text::LBT_CRLF);
				}
			}
		}
		else
		{
			sb->Append((const UTF8Char*)"\r\nType=0x");
			sb->AppendHex8(packet[0]);
			sb->Append((const UTF8Char*)"\r\n");
			sb->AppendHexBuff(packet, packetSize, ' ', Text::LBT_CRLF);
		}
		return;
	case 6:
	{
		sb->Append((const UTF8Char*)"\r\nTCP:");
		if (packetSize < 20)
		{
			sb->Append((const UTF8Char*)"\r\n");
			sb->AppendHexBuff(packet, packetSize, ' ', Text::LBT_CRLF);
		}
		else
		{
			sb->Append((const UTF8Char*)"\r\nSource Port=");
			sb->AppendU16(ReadMUInt16(&packet[0]));
			sb->Append((const UTF8Char*)"\r\nDestination Port=");
			sb->AppendU16(ReadMUInt16(&packet[2]));
			sb->Append((const UTF8Char*)"\r\nSequence Number=");
			sb->AppendU32(ReadMUInt32(&packet[4]));
			sb->Append((const UTF8Char*)"\r\nAcknowledgment Number=");
			sb->AppendU32(ReadMUInt32(&packet[8]));
			sb->Append((const UTF8Char*)"\r\nData Offset=");
			sb->AppendU16((UInt16)(packet[12] >> 4));
			sb->Append((const UTF8Char*)"\r\nNS=");
			sb->AppendU16(packet[12] & 1);
			sb->Append((const UTF8Char*)"\r\nCWR=");
			sb->AppendU16((UInt16)((packet[13] >> 7) & 1));
			sb->Append((const UTF8Char*)"\r\nECE=");
			sb->AppendU16((UInt16)((packet[13] >> 6) & 1));
			sb->Append((const UTF8Char*)"\r\nURG=");
			sb->AppendU16((UInt16)((packet[13] >> 5) & 1));
			sb->Append((const UTF8Char*)"\r\nACK=");
			sb->AppendU16((UInt16)((packet[13] >> 4) & 1));
			sb->Append((const UTF8Char*)"\r\nPSH=");
			sb->AppendU16((UInt16)((packet[13] >> 3) & 1));
			sb->Append((const UTF8Char*)"\r\nRST=");
			sb->AppendU16((UInt16)((packet[13] >> 2) & 1));
			sb->Append((const UTF8Char*)"\r\nSYN=");
			sb->AppendU16((UInt16)((packet[13] >> 1) & 1));
			sb->Append((const UTF8Char*)"\r\nFIN=");
			sb->AppendU16(packet[13] & 1);
			sb->Append((const UTF8Char*)"\r\nWindow Size=");
			sb->AppendU16(ReadMUInt16(&packet[14]));
			sb->Append((const UTF8Char*)"\r\nChecksum=0x");
			sb->AppendHex16(ReadMUInt16(&packet[16]));
			sb->Append((const UTF8Char*)"\r\nUrgent Pointer=0x");
			sb->AppendHex16(ReadMUInt16(&packet[18]));
			UOSInt headerLen = (UOSInt)(packet[12] >> 4) * 4;
			if (headerLen > 20)
			{
				sb->Append((const UTF8Char*)"\r\nOptions:");
				UOSInt i = 20;
				while (i < headerLen)
				{
					sb->Append((const UTF8Char*)"\r\nKind=");
					sb->AppendU16(packet[i]);
					switch (packet[i])
					{
					case 0:
						sb->Append((const UTF8Char*)" (End of option list)");
						i = headerLen - 1;
						break;
					case 1:
						sb->Append((const UTF8Char*)" (No operation)");
						break;
					case 2:
						sb->Append((const UTF8Char*)" (No operation)");
						sb->Append((const UTF8Char*)", Length=");
						sb->AppendU16(packet[i + 1]);
						if (packet[i + 1] == 4)
						{
							sb->Append((const UTF8Char*)", Value=");
							sb->AppendU16(ReadMUInt16(&packet[i + 2]));
						}
						i += (UOSInt)packet[i + 1] - 1;
						break;
					case 3:
						sb->Append((const UTF8Char*)" (Window scale)");
						sb->Append((const UTF8Char*)", Length=");
						sb->AppendU16(packet[i + 1]);
						if (packet[i + 1] == 3)
						{
							sb->Append((const UTF8Char*)", Value=");
							sb->AppendU16(packet[i + 2]);
						}
						i += (UOSInt)packet[i + 1] - 1;
						break;
					case 4:
						sb->Append((const UTF8Char*)" (SACK permitted)");
						sb->Append((const UTF8Char*)", Length=");
						sb->AppendU16(packet[i + 1]);
						i += (UOSInt)packet[i + 1] - 1;
						break;
					default:
						sb->Append((const UTF8Char*)", Length=");
						sb->AppendU16(packet[i + 1]);
						i += (UOSInt)packet[i + 1] - 1;
						break;
					}
					i++;
				}
			}
			if (packetSize > headerLen)
			{
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"\r\nData:");
				sb->Append((const UTF8Char*)"\r\n");
				sb->AppendHexBuff(&packet[headerLen], packetSize - headerLen, ' ', Text::LBT_CRLF);
			}
		}
		return;
	}
	case 17:
	{
		UInt16 srcPort = 0;
		UInt16 destPort = 0;
		const UTF8Char *csptr;
		UOSInt udpLen = packetSize;
		sb->Append((const UTF8Char*)"\r\nUDP:");
		
		if (packetSize >= 2)
		{
			sb->Append((const UTF8Char*)"\r\nSrcPort=");
			srcPort = ReadMUInt16(&packet[0]);
			sb->AppendU16(srcPort);
			csptr = UDPPortGetName(srcPort);
			if (csptr)
			{
				sb->Append((const UTF8Char*)" (");
				sb->Append(csptr);
				sb->Append((const UTF8Char*)")");
			}
		}
		if (packetSize >= 4)
		{
			sb->Append((const UTF8Char*)"\r\nDestPort=");
			destPort = ReadMUInt16(&packet[2]);
			sb->AppendU16(destPort);
			csptr = UDPPortGetName(destPort);
			if (csptr)
			{
				sb->Append((const UTF8Char*)" (");
				sb->Append(csptr);
				sb->Append((const UTF8Char*)")");
			}
		}
		if (packetSize >= 6)
		{
			udpLen = ReadMUInt16(&packet[4]);
			sb->Append((const UTF8Char*)"\r\nLength=");
			sb->AppendUOSInt(udpLen);
			if (packetSize < udpLen)
				udpLen = packetSize;
		}
		if (packetSize >= 8)
		{
			sb->Append((const UTF8Char*)"\r\nChecksum=0x");
			sb->AppendHex16(ReadMUInt16(&packet[6]));
		}
		if (packetSize > 8)
		{
			sb->Append((const UTF8Char*)"\r\n");
			PacketUDPGetDetail(srcPort, destPort, &packet[8], udpLen - 8, sb);
			if (packetSize > udpLen)
			{
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"\r\nPadding:");
				sb->Append((const UTF8Char*)"\r\n");
				sb->AppendHexBuff(&packet[udpLen], packetSize - udpLen, ' ', Text::LBT_CRLF);
			}
		}
		return;
	}
	case 58:
		sb->Append((const UTF8Char*)"\r\nICMPv6:");
		if (packetSize >= 4)
		{
			sb->Append((const UTF8Char*)"\r\nType=");
			sb->AppendU16(packet[0]);
			switch (packet[0])
			{
			case 1:
				sb->Append((const UTF8Char*)" (Destination unreachable)");
				break;
			case 2:
				sb->Append((const UTF8Char*)" (Packet Too Big)");
				break;
			case 3:
				sb->Append((const UTF8Char*)" (Time exceeded)");
				break;
			case 4:
				sb->Append((const UTF8Char*)" (Parameter problem)");
				break;
			case 100:
				sb->Append((const UTF8Char*)" (Private experimentation)");
				break;
			case 101:
				sb->Append((const UTF8Char*)" (Private experimentation)");
				break;
			case 127:
				sb->Append((const UTF8Char*)" (Reserved for expansion of ICMPv6 error messages)");
				break;
			case 128:
				sb->Append((const UTF8Char*)" (Echo Request)");
				break;
			case 129:
				sb->Append((const UTF8Char*)" (Echo Reply)");
				break;
			case 130:
				sb->Append((const UTF8Char*)" (Multicast Listener Query)");
				break;
			case 131:
				sb->Append((const UTF8Char*)" (Multicast Listener Report)");
				break;
			case 132:
				sb->Append((const UTF8Char*)" (Multicast Listener Done)");
				break;
			case 133:
				sb->Append((const UTF8Char*)" (Router Solicitation)");
				break;
			case 134:
				sb->Append((const UTF8Char*)" (Router Advertisement)");
				break;
			case 135:
				sb->Append((const UTF8Char*)" (Neighbor Solicitation)");
				break;
			case 136:
				sb->Append((const UTF8Char*)" (Neighbor Advertisement)");
				break;
			case 137:
				sb->Append((const UTF8Char*)" (Redirect Message)");
				break;
			case 138:
				sb->Append((const UTF8Char*)" (Router Renumbering)");
				break;
			case 139:
				sb->Append((const UTF8Char*)" (ICMP Node Information Query)");
				break;
			case 140:
				sb->Append((const UTF8Char*)" (ICMP Node Information Response)");
				break;
			case 141:
				sb->Append((const UTF8Char*)" (Inverse Neighbor Discovery Solicitation Message)");
				break;
			case 142:
				sb->Append((const UTF8Char*)" (Inverse Neighbor Discovery Advertisement Message)");
				break;
			case 143:
				sb->Append((const UTF8Char*)" (Multicast Listener Discovery (MLDv2) reports)");
				break;
			case 144:
				sb->Append((const UTF8Char*)" (Home Agent Address Discovery Request Message)");
				break;
			case 145:
				sb->Append((const UTF8Char*)" (Home Agent Address Discovery Reply Message)");
				break;
			case 146:
				sb->Append((const UTF8Char*)" (Mobile Prefix Solicitation)");
				break;
			case 147:
				sb->Append((const UTF8Char*)" (Mobile Prefix Advertisement)");
				break;
			case 148:
				sb->Append((const UTF8Char*)" (Certification Path Solicitation)");
				break;
			case 149:
				sb->Append((const UTF8Char*)" (Certification Path Advertisement)");
				break;
			case 151:
				sb->Append((const UTF8Char*)" (Multicast Router Advertisement)");
				break;
			case 152:
				sb->Append((const UTF8Char*)" (Multicast Router Solicitation)");
				break;
			case 153:
				sb->Append((const UTF8Char*)" (Multicast Router Termination)");
				break;
			case 155:
				sb->Append((const UTF8Char*)" (RPL Control Message)");
				break;
			case 200:
				sb->Append((const UTF8Char*)" (Private experimentation)");
				break;
			case 201:
				sb->Append((const UTF8Char*)" (Private experimentation)");
				break;
			case 255:
				sb->Append((const UTF8Char*)" (Reserved for expansion of ICMPv6 informational messages)");
				break;
			}
			sb->Append((const UTF8Char*)"\r\nCode=");
			sb->AppendU16(packet[1]);
			switch (packet[0])
			{
			case 1: //Destination unreachable
				switch (packet[1])
				{
				case 0:
					sb->Append((const UTF8Char*)" (no route to destination)");
					break;
				case 1:
					sb->Append((const UTF8Char*)" (communication with destination administratively prohibited)");
					break;
				case 2:
					sb->Append((const UTF8Char*)" (beyond scope of source address)");
					break;
				case 3:
					sb->Append((const UTF8Char*)" (address unreachable)");
					break;
				case 4:
					sb->Append((const UTF8Char*)" (port unreachable)");
					break;
				case 5:
					sb->Append((const UTF8Char*)" (source address failed ingress/egress policy)");
					break;
				case 6:
					sb->Append((const UTF8Char*)" (reject route to destination)");
					break;
				case 7:
					sb->Append((const UTF8Char*)" (Error in Source Routing Header)");
					break;
				}
				break;
			case 3: //Time exceeded
				switch (packet[1])
				{
				case 0:
					sb->Append((const UTF8Char*)" (hop limit exceeded in transit)");
					break;
				case 1:
					sb->Append((const UTF8Char*)" (fragment reassembly time exceeded)");
					break;
				}
				break;
			case 4: //Parameter problem
				switch (packet[1])
				{
				case 0:
					sb->Append((const UTF8Char*)" (erroneous header field encountered)");
					break;
				case 1:
					sb->Append((const UTF8Char*)" (unrecognized Next Header type encountered)");
					break;
				case 2:
					sb->Append((const UTF8Char*)" (unrecognized IPv6 option encountered)");
					break;
				}
				break;
			case 138: //Router Renumbering
				switch (packet[1])
				{
				case 0:
					sb->Append((const UTF8Char*)" (Router Renumbering Command)");
					break;
				case 1:
					sb->Append((const UTF8Char*)" (Router Renumbering Result)");
					break;
				case 255:
					sb->Append((const UTF8Char*)" (Sequence Number Reset)");
					break;
				}
				break;
			case 139: //ICMP Node Information Query
				switch (packet[1])
				{
				case 0:
					sb->Append((const UTF8Char*)" (The Data field contains an IPv6 address which is the Subject of this Query)");
					break;
				case 1:
					sb->Append((const UTF8Char*)" (The Data field contains a name which is the Subject of this Query, or is empty, as in the case of a NOOP)");
					break;
				case 2:
					sb->Append((const UTF8Char*)" (The Data field contains an IPv4 address which is the Subject of this Query)");
					break;
				}
				break;
			case 140: //ICMP Node Information Response
				switch (packet[1])
				{
				case 0:
					sb->Append((const UTF8Char*)" (A successful reply. The Reply Data field may or may not be empty)");
					break;
				case 1:
					sb->Append((const UTF8Char*)" (The Responder refuses to supply the answer. The Reply Data field will be empty.)");
					break;
				case 2:
					sb->Append((const UTF8Char*)" (The Qtype of the Query is unknown to the Responder. The Reply Data field will be empty)");
					break;
				}
				break;
			}
			sb->Append((const UTF8Char*)"\r\nChecksum=0x");
			sb->AppendHex16(ReadMUInt16(&packet[2]));
			switch (packet[0])
			{
			case 134:
				if (packetSize >= 24)
				{
					sb->Append((const UTF8Char*)"\r\nHop Limit=");
					sb->AppendU16(packet[4]);
					sb->Append((const UTF8Char*)"\r\nFlags=0x");
					sb->AppendHex8(packet[5]);
					sb->Append((const UTF8Char*)"\r\nRouter Lifetime=");
					sb->AppendU16(ReadMUInt16(&packet[6]));
					sb->Append((const UTF8Char*)"\r\nReachable Timer=");
					sb->AppendU32(ReadMUInt32(&packet[8]));
					sb->Append((const UTF8Char*)"\r\nRetains Timer=");
					sb->AppendU32(ReadMUInt32(&packet[12]));
					UOSInt i = 16;
					while (i + 7 < packetSize)
					{
						sb->Append((const UTF8Char*)"\r\nType=");
						sb->AppendU16(packet[i]);
						switch (packet[i])
						{
						case 1:
							sb->Append((const UTF8Char *)" (Source Link-layer Address)");
							break;
						case 2:
							sb->Append((const UTF8Char *)" (Target Link-layer Address)");
							break;
						}
						sb->Append((const UTF8Char*)"\r\nLength=");
						sb->AppendU16(packet[i + 1]);
						switch (packet[i])
						{
						case 1:
						case 2:
							sb->Append((const UTF8Char*)"\r\nAddress=");
							sb->AppendHexBuff(&packet[i + 2], 6, ':', Text::LBT_NONE);
							break;
						case 3:
							sb->Append((const UTF8Char*)"\r\nPrefix Length=");
							sb->AppendU16(packet[i + 2]);
							sb->Append((const UTF8Char*)"\r\nFlags=");
							sb->AppendU16(packet[i + 3]);
							sb->Append((const UTF8Char*)"\r\nValid Lifetime=");
							sb->AppendU32(ReadMUInt32(&packet[i + 4]));
							sb->Append((const UTF8Char*)"\r\nPreferred Lifetime=");
							sb->AppendU32(ReadMUInt32(&packet[i + 8]));
							sb->Append((const UTF8Char*)"\r\nReserved=");
							sb->AppendU32(ReadMUInt32(&packet[i + 12]));
							sb->Append((const UTF8Char*)"\r\nPrefix=");
							sb->AppendHexBuff(&packet[i + 16], (UOSInt)packet[i + 1] - 16, ' ', Text::LBT_NONE);
							break;
						case 5:
							sb->Append((const UTF8Char*)"\r\nPrefix Length=");
							sb->AppendU16(ReadMUInt16(&packet[i + 2]));
							sb->Append((const UTF8Char*)"\r\nMTU=");
							sb->AppendU32(ReadMUInt32(&packet[i + 4]));
							break;
						}
						i += (UOSInt)packet[i + 1] * 8;
					}
				}
				else
				{
					sb->Append((const UTF8Char*)"\r\n");
					sb->AppendHexBuff(&packet[4], packetSize - 4, ' ', Text::LBT_CRLF);
				}
				break;
			case 135:
				if (packetSize >= 24)
				{
					Net::SocketUtil::AddressInfo addr;
					sb->Append((const UTF8Char*)"\r\nReserved=0x");
					sb->AppendHex32(ReadMUInt32(&packet[4]));
					Net::SocketUtil::SetAddrInfoV6(&addr, &packet[8], 0);
					Net::SocketUtil::GetAddrName(sbuff, &addr);
					sb->Append((const UTF8Char*)"\r\nTarget Address=");
					sb->Append(sbuff);
					UOSInt i = 24;
					while (i < packetSize)
					{
						sb->Append((const UTF8Char*)"\r\nType=");
						sb->AppendU16(packet[i]);
						sb->Append((const UTF8Char*)"\r\nLength=");
						sb->AppendU16(packet[i + 1]);
						switch (packet[i])
						{
						case 1:
							sb->Append((const UTF8Char *)" (Source Link-layer Address)");
							break;
						case 2:
							sb->Append((const UTF8Char *)" (Target Link-layer Address)");
							break;
						}
						sb->Append((const UTF8Char*)"\r\nAddress=");
						sb->AppendHexBuff(&packet[i + 2], 6, ':', Text::LBT_NONE);
						i += 8;
					}
				}
				else
				{
					sb->Append((const UTF8Char*)"\r\n");
					sb->AppendHexBuff(&packet[4], packetSize - 4, ' ', Text::LBT_CRLF);
				}
				break;
			default:
				sb->Append((const UTF8Char*)"\r\n");
				sb->AppendHexBuff(&packet[4], packetSize - 4, ' ', Text::LBT_CRLF);
				break;
			}
		}
		return;
	default:
		sb->Append((const UTF8Char*)"\r\n");
		sb->AppendHexBuff(packet, packetSize, ' ', Text::LBT_CRLF);
		return;
	}
}

void Net::EthernetAnalyzer::PacketUDPGetDetail(UInt16 srcPort, UInt16 destPort, const UInt8 *packet, UOSInt packetSize, Text::StringBuilderUTF *sb)
{
	UTF8Char sbuff[64];
	UTF8Char sbuff2[64];
	UTF8Char *sptr;
	if (destPort == 53)
	{
		sb->Append((const UTF8Char*)"\r\nDNS Request:");
		if (packetSize < 12)
		{
			sb->Append((const UTF8Char*)"\r\n");
			sb->AppendHexBuff(packet, packetSize, ' ', Text::LBT_CRLF);
		}
		else
		{
			PacketDNSGetDetail(packet, packetSize, sb);
		}
	}
	else if (srcPort == 53)
	{
		sb->Append((const UTF8Char*)"\r\nDNS Reply:");
		if (packetSize < 12)
		{
			sb->Append((const UTF8Char*)"\r\n");
			sb->AppendHexBuff(packet, packetSize, ' ', Text::LBT_CRLF);
		}
		else
		{
			PacketDNSGetDetail(packet, packetSize, sb);
		}
	}
	else if (srcPort == 67 || destPort == 67)
	{
		sb->Append((const UTF8Char*)"\r\nBOOTP (DHCP):");
		if (packetSize < 240)
		{
			sb->Append((const UTF8Char*)"\r\n");
			sb->AppendHexBuff(packet, packetSize, ' ', Text::LBT_CRLF);
		}
		else if (ReadMUInt32(&packet[236]) == 0x63825363)
		{
			sb->Append((const UTF8Char*)"\r\nOP=");
			sb->AppendU16(packet[0]);
			switch (packet[0])
			{
			case 1:
				sb->Append((const UTF8Char*)" (Request)");
				break;
			case 2:
				sb->Append((const UTF8Char*)" (Reply)");
				break;
			}
			sb->Append((const UTF8Char*)"\r\nHardware Type (HTYPE)=");
			sb->AppendU16(packet[1]);
			sb->Append((const UTF8Char*)"\r\nHardware Address Length (HLEN)=");
			sb->AppendU16(packet[2]);
			sb->Append((const UTF8Char*)"\r\nHOPS=");
			sb->AppendU16(packet[3]);
			sb->Append((const UTF8Char*)"\r\nTransaction ID=0x");
			sb->AppendHex32(ReadMUInt32(&packet[4]));
			sb->Append((const UTF8Char*)"\r\nSeconds Elapsed (SECS)=");
			sb->AppendU16(ReadMUInt16(&packet[8]));
			sb->Append((const UTF8Char*)"\r\nFlags=0x");
			sb->AppendHex16(ReadMUInt16(&packet[10]));
			sb->Append((const UTF8Char*)"\r\nClient IP Address=");
			Net::SocketUtil::GetIPv4Name(sbuff, ReadNUInt32(&packet[12]));
			sb->Append(sbuff);
			sb->Append((const UTF8Char*)"\r\nYour IP Address=");
			Net::SocketUtil::GetIPv4Name(sbuff, ReadNUInt32(&packet[16]));
			sb->Append(sbuff);
			sb->Append((const UTF8Char*)"\r\nServer IP Address=");
			Net::SocketUtil::GetIPv4Name(sbuff, ReadNUInt32(&packet[20]));
			sb->Append(sbuff);
			sb->Append((const UTF8Char*)"\r\nGateway IP Address=");
			Net::SocketUtil::GetIPv4Name(sbuff, ReadNUInt32(&packet[24]));
			sb->Append(sbuff);
			sb->Append((const UTF8Char*)"\r\nClient Hardware Address=");
			sb->AppendHexBuff(&packet[28], 6, ':', Text::LBT_NONE);
			sb->Append((const UTF8Char*)"\r\nPadding=");
			sb->AppendHexBuff(&packet[34], 10, ' ', Text::LBT_NONE);
			sb->Append((const UTF8Char*)"\r\nServer Host Name=");
			sb->AppendS(&packet[44], 64);
			sb->Append((const UTF8Char*)"\r\nBoot File Name=");
			sb->AppendS(&packet[108], 128);
			sb->Append((const UTF8Char*)"\r\nDHCP Magic=0x");
			sb->AppendHex32(ReadMUInt32(&packet[236]));
			const UInt8 *currPtr = &packet[240];
			const UInt8 *endPtr = &packet[packetSize];
			const UTF8Char *csptr;
			UInt8 t;
			UInt8 len;
			while (currPtr < endPtr)
			{
				t = *currPtr++;
				sb->Append((const UTF8Char*)"\r\nOption Type=");
				sb->AppendU16(t);
				csptr = DHCPOptionGetName(t);
				if (csptr)
				{
					sb->Append((const UTF8Char*)" (");
					sb->Append(csptr);
					sb->Append((const UTF8Char*)")");
				}
				if (t == 255)
				{
					if (currPtr < endPtr)
					{
						sb->Append((const UTF8Char*)"\r\nPadding:");
						sb->Append((const UTF8Char*)"\r\n");
						sb->AppendHexBuff(currPtr, (UOSInt)(endPtr - currPtr), ' ', Text::LBT_CRLF);
					}
					break;
				}
				if (currPtr >= endPtr)
				{
					break;
				}
				len = *currPtr++;
				sb->Append((const UTF8Char*)"\r\nOption Length=");
				sb->AppendU16(len);
				if (t == 1 && len == 4)
				{
					sb->Append((const UTF8Char*)"\r\nSubnet Mask=");
					Net::SocketUtil::GetIPv4Name(sbuff, ReadNUInt32(currPtr));
					sb->Append(sbuff);
				}
				else if (t == 3 && len == 4)
				{
					sb->Append((const UTF8Char*)"\r\nRouter=");
					Net::SocketUtil::GetIPv4Name(sbuff, ReadNUInt32(currPtr));
					sb->Append(sbuff);
				}
				else if (t == 6 && len > 0 && (len & 3) == 0)
				{
					sb->Append((const UTF8Char*)"\r\nDNS=");
					OSInt i = 0;
					while (i < len)
					{
						if (i > 0)
						{
							sb->Append((const UTF8Char*)", ");
						}
						Net::SocketUtil::GetIPv4Name(sbuff, ReadNUInt32(&currPtr[i]));
						sb->Append(sbuff);
						i += 4;
					}
				}
				else if (t == 12 && len > 0)
				{
					sb->Append((const UTF8Char*)"\r\nHost Name=");
					sb->AppendC(currPtr, len);
				}
				else if (t == 15 && len > 0)
				{
					sb->Append((const UTF8Char*)"\r\nDomain Name=");
					sb->AppendC(currPtr, len);
				}
				else if (t == 50 && len == 4)
				{
					sb->Append((const UTF8Char*)"\r\nRequested IP Address=");
					Net::SocketUtil::GetIPv4Name(sbuff, ReadNUInt32(currPtr));
					sb->Append(sbuff);
				}
				else if (t == 51 && len == 4)
				{
					sb->Append((const UTF8Char*)"\r\nIP Address Lease Time=");
					sb->AppendU32(ReadMUInt32(currPtr));
				}
				else if (t == 53 && len == 1)
				{
					sb->Append((const UTF8Char*)"\r\nDHCP Type=");
					sb->AppendU16(currPtr[0]);
					switch (currPtr[0])
					{
					case 1:
						sb->Append((const UTF8Char*)" (Discover)");
						break;
					case 2:
						sb->Append((const UTF8Char*)" (Offer)");
						break;
					case 3:
						sb->Append((const UTF8Char*)" (Request)");
						break;
					case 5:
						sb->Append((const UTF8Char*)" (ACK)");
						break;
					}
				}
				else if (t == 54 && len == 4)
				{
					sb->Append((const UTF8Char*)"\r\nDHCP Server=");
					Net::SocketUtil::GetIPv4Name(sbuff, ReadNUInt32(currPtr));
					sb->Append(sbuff);
				}
				else if (t == 55 && len > 0)
				{
					OSInt i;
					sb->Append((const UTF8Char*)"\r\nParameter Request List:");
					i = 0;
					while (i < len)
					{
						sb->Append((const UTF8Char*)"\r\n-");
						sb->AppendU16(currPtr[i]);
						csptr = DHCPOptionGetName(currPtr[i]);
						if (csptr)
						{
							sb->Append((const UTF8Char*)" (");
							sb->Append(csptr);
							sb->Append((const UTF8Char*)")");
						}
						i++;
					}
				}
				else if (t == 57 && len == 2)
				{
					sb->Append((const UTF8Char*)"\r\nMax DHCP Message Size=");
					sb->AppendU16(ReadMUInt16(currPtr));
				}
				else if (t == 58 && len == 4)
				{
					sb->Append((const UTF8Char*)"\r\nRenew Time=");
					sb->AppendU32(ReadMUInt32(currPtr));
				}
				else if (t == 59 && len == 4)
				{
					sb->Append((const UTF8Char*)"\r\nRebinding Time=");
					sb->AppendU32(ReadMUInt32(currPtr));
				}
				else if (t == 60 && len >= 1)
				{
					sb->Append((const UTF8Char*)"\r\nVendor Class ID=");
					sb->AppendC(currPtr, len);
				}
				else if (t == 61 && len >= 1)
				{
					sb->Append((const UTF8Char*)"\r\nClient ID Type=");
					sb->AppendU16(currPtr[0]);
					if (len > 1)
					{
						sb->Append((const UTF8Char*)"\r\nClient ID=");
						sb->AppendHexBuff(&currPtr[1], (UOSInt)len - 1, ':', Text::LBT_NONE);
					}
				}
				else if (t == 66 && len >= 1)
				{
					sb->Append((const UTF8Char*)"\r\nTFTP Server Name=");
					sb->AppendC(currPtr, len);
				}
				else if (t == 81 && len >= 3)
				{
					sb->Append((const UTF8Char*)"\r\nFlags=0x");
					sb->AppendHex8(currPtr[0]);
					sb->Append((const UTF8Char*)"\r\nRCODE1=");
					sb->AppendU16(currPtr[1]);
					sb->Append((const UTF8Char*)"\r\nRCODE2=");
					sb->AppendU16(currPtr[1]);
					if (len > 3)
					{
						sb->Append((const UTF8Char*)"\r\nDomain Name=");
						sb->AppendC(&currPtr[3], (UOSInt)len - 3);
					}
				}
				else if (t == 120 && len >= 1)
				{
					sb->Append((const UTF8Char*)"\r\nSIP Server Encoding=");
					sb->AppendU16(currPtr[0]);
					if (currPtr[0] == 1 && len == 5)
					{
						sb->Append((const UTF8Char*)"\r\nSIP Server Address=");
						Net::SocketUtil::GetIPv4Name(sbuff, ReadNUInt32(&currPtr[1]));
						sb->Append(sbuff);
					}
					else if (len > 1)
					{
						sb->Append((const UTF8Char*)"\r\n");
						sb->AppendHexBuff(&currPtr[1], (UOSInt)len - 1, ' ', Text::LBT_CRLF);
					}
				}
				else
				{
					sb->Append((const UTF8Char*)"\r\n");
					sb->AppendHexBuff(currPtr, len, ' ', Text::LBT_CRLF);
				}

				currPtr += len;
			}
		}
		else
		{
			sb->Append((const UTF8Char*)"\r\n");
			sb->AppendHexBuff(packet, packetSize, ' ', Text::LBT_CRLF);
		}
	}
	else if (srcPort == 69 || destPort == 69)
	{
		UInt16 opcode = ReadMUInt16(packet);
		UOSInt i = 2;
		UOSInt len;
		sb->Append((const UTF8Char*)"\r\nTFTP:");
		sb->Append((const UTF8Char*)"\r\nOpcode=");
		sb->AppendU16(opcode);
		switch (opcode)
		{
		case 1:
			sb->Append((const UTF8Char*)" (Read request)");
			break;
		case 2:
			sb->Append((const UTF8Char*)" (Write request)");
			break;
		case 3:
			sb->Append((const UTF8Char*)" (Data)");
			break;
		case 4:
			sb->Append((const UTF8Char*)" (Acknowledgment)");
			break;
		case 5:
			sb->Append((const UTF8Char*)" (Error)");
			break;
		case 6:
			sb->Append((const UTF8Char*)" (Options Acknowledgment)");
			break;
		}
		if (opcode == 1 || opcode == 2)
		{
			if (packet[packetSize - 1] == 0)
			{
				len = Text::StrCharCnt(&packet[2]);
				sb->Append((const UTF8Char*)"\r\nFilename=");
				sb->Append(&packet[2]);
				i += len + 1;
				if (i < packetSize)
				{
					len = Text::StrCharCnt(&packet[i]);
					sb->Append((const UTF8Char*)"\r\nMode=");
					sb->Append(&packet[i]);
					i += len + 1;
				}
				OSInt optId = 0;
				while (i < packetSize)
				{
					len = Text::StrCharCnt(&packet[i]);
					if (optId & 1)
					{
						sb->Append((const UTF8Char*)"\r\nValue");
					}
					else
					{
						sb->Append((const UTF8Char*)"\r\nOption");
					}
					sb->AppendOSInt(1 + (optId >> 1));
					sb->Append((const UTF8Char*)"=");
					sb->Append(&packet[i]);
					i += len + 1;
					optId++;
				}
			}
		}
		if (packetSize > i)
		{
			sb->Append((const UTF8Char*)"\r\n");
			sb->AppendHexBuff(&packet[i], packetSize - i, ' ', Text::LBT_CRLF);
		}
	}
	else if (srcPort == 123 || destPort == 123) //RFC 5905
	{
		if (destPort == 123)
		{
			sb->Append((const UTF8Char*)"\r\nNTP Request:");
		}
		else
		{
			sb->Append((const UTF8Char*)"\r\nNTP Reply:");
		}
		
		if (packetSize < 48)
		{
			sb->Append((const UTF8Char*)"\r\n");
			sb->AppendHexBuff(packet, packetSize, ' ', Text::LBT_CRLF);
		}
		else
		{
			sb->Append((const UTF8Char*)"\r\nLeap Indicator=");
			sb->AppendU16((UInt16)(packet[0] >> 6));
			switch (packet[0] >> 6)
			{
			case 0:
				sb->Append((const UTF8Char*)" (No warning)");
				break;
			case 1:
				sb->Append((const UTF8Char*)" (Last minute of the day has 61 seconds)");
				break;
			case 2:
				sb->Append((const UTF8Char*)" (Last minute of the day has 59 seconds)");
				break;
			case 3:
				sb->Append((const UTF8Char*)" (Unknown)");
				break;
			}
			sb->Append((const UTF8Char*)"\r\nVersion Number=");
			sb->AppendU16((packet[0] >> 3) & 7);
			sb->Append((const UTF8Char*)"\r\nMode=");
			sb->AppendU16(packet[0] & 7);
			switch (packet[0] & 7)
			{
			case 0:
				sb->Append((const UTF8Char*)" (Reserved)");
				break;
			case 1:
				sb->Append((const UTF8Char*)" (Symmetric active)");
				break;
			case 2:
				sb->Append((const UTF8Char*)" (Symmetric passive)");
				break;
			case 3:
				sb->Append((const UTF8Char*)" (Client)");
				break;
			case 4:
				sb->Append((const UTF8Char*)" (Server)");
				break;
			case 5:
				sb->Append((const UTF8Char*)" (Broadcast)");
				break;
			case 6:
				sb->Append((const UTF8Char*)" (NTP Control Message)");
				break;
			case 7:
				sb->Append((const UTF8Char*)" (Reserved for private use)");
				break;
			}
			sb->Append((const UTF8Char*)"\r\nStratum=");
			sb->AppendU16(packet[1]);
			if (packet[1] == 0)
			{
				sb->Append((const UTF8Char*)" (Unspecified or invalid)");
			}
			else if (packet[1] == 1)
			{
				sb->Append((const UTF8Char*)" (Primary Server)");
			}
			else if (packet[1] < 16)
			{
				sb->Append((const UTF8Char*)" (Secondary Server)");
			}
			else if (packet[1] == 16)
			{
				sb->Append((const UTF8Char*)" (Unsynchronized)");
			}
			else
			{
				sb->Append((const UTF8Char*)" (Reserved)");
			}
			sb->Append((const UTF8Char*)"\r\nPoll=");
			sb->AppendU16(packet[2]);
			sb->Append((const UTF8Char*)"\r\nPrecision=");
			sb->AppendI16((Int8)packet[3]);
			sb->Append((const UTF8Char*)"\r\nRoot Delay=");
			Text::SBAppendF64(sb, ReadMUInt32(&packet[4]) / 65536.0);
			sb->Append((const UTF8Char*)"\r\nRoot Dispersion=");
			Text::SBAppendF64(sb, ReadMUInt32(&packet[8]) / 65536.0);
			sb->Append((const UTF8Char*)"\r\nReference ID=");
			sb->AppendHexBuff(&packet[12], 4, ' ', Text::LBT_NONE);
			Data::DateTime dt;
			sb->Append((const UTF8Char*)"\r\nReference Timestamp=");
			if (ReadNInt64(&packet[16]) == 0)
				sb->Append((const UTF8Char*)"0");
			else
			{
				Net::NTPServer::ReadTime(&packet[16], &dt);
				dt.ToLocalTime();
				dt.ToString(sbuff, "yyyy-MM-dd HH:mm:ss.fff");
				sb->Append(sbuff);
			}
			sb->Append((const UTF8Char*)"\r\nOrigin Timestamp=");
			if (ReadNInt64(&packet[24]) == 0)
				sb->Append((const UTF8Char*)"0");
			else
			{
				Net::NTPServer::ReadTime(&packet[24], &dt);
				dt.ToLocalTime();
				dt.ToString(sbuff, "yyyy-MM-dd HH:mm:ss.fff");
				sb->Append(sbuff);
			}
			sb->Append((const UTF8Char*)"\r\nReceive Timestamp=");
			if (ReadNInt64(&packet[32]) == 0)
				sb->Append((const UTF8Char*)"0");
			else
			{
				Net::NTPServer::ReadTime(&packet[32], &dt);
				dt.ToLocalTime();
				dt.ToString(sbuff, "yyyy-MM-dd HH:mm:ss.fff");
				sb->Append(sbuff);
			}
			sb->Append((const UTF8Char*)"\r\nTransmit Timestamp=");
			if (ReadNInt64(&packet[40]) == 0)
				sb->Append((const UTF8Char*)"0");
			else
			{
				Net::NTPServer::ReadTime(&packet[40], &dt);
				dt.ToLocalTime();
				dt.ToString(sbuff, "yyyy-MM-dd HH:mm:ss.fff");
				sb->Append(sbuff);
			}
			if (packetSize > 48)
			{
				sb->Append((const UTF8Char*)"\r\n");
				sb->AppendHexBuff(&packet[48], packetSize - 48, ' ', Text::LBT_CRLF);
			}
		}
	}
	else if (destPort == 137 && packetSize >= 12)
	{
		UInt16 qdcount = ReadMUInt16(&packet[4]);
		UInt16 ancount = ReadMUInt16(&packet[6]);
		UInt16 nscount = ReadMUInt16(&packet[8]);
		UInt16 arcount = ReadMUInt16(&packet[10]);
		sb->Append((const UTF8Char*)"\r\nNetBIOS-NS:");
		sb->Append((const UTF8Char*)"\r\nNAME_TRN_ID=0x");
		sb->AppendHex16(ReadMUInt16(&packet[0]));
		sb->Append((const UTF8Char*)"\r\nResponse=");
		sb->AppendU16((UInt16)(packet[1] >> 7));
		sb->Append((const UTF8Char*)"\r\nOPCODE=");
		sb->AppendU16((UInt16)((packet[1] & 0x78) >> 3));
		sb->Append((const UTF8Char*)"\r\nNMFLAGS=0x");
		sb->AppendHex16((ReadMUInt16(&packet[1]) & 0x7F0) >> 4);
		sb->Append((const UTF8Char*)"\r\nRCODE=");
		sb->AppendU16(packet[3] & 0xf);
		sb->Append((const UTF8Char*)"\r\nQDCOUNT=");
		sb->AppendU16(qdcount);
		sb->Append((const UTF8Char*)"\r\nANCOUNT=");
		sb->AppendU16(ancount);
		sb->Append((const UTF8Char*)"\r\nNSCOUNT=");
		sb->AppendU16(nscount);
		sb->Append((const UTF8Char*)"\r\nARCOUNT=");
		sb->AppendU16(arcount);
		UOSInt i;
		UInt8 j;
		UInt16 qType;
		UInt16 qClass;
		UInt16 rrType;
		UInt16 rrClass;
		UInt16 rdLength;
		i = 12;
		j = 0;
		while (j < qdcount)
		{
			i = Net::DNSClient::ParseString(sbuff, packet, i, packetSize);
			sb->Append((const UTF8Char*)"\r\nQUESTION_NAME=");
			sb->Append(sbuff);
			if ((sptr = NetBIOSGetName(sbuff2, sbuff)) != 0)
			{
				sptr[-1] = 0;
				Text::StrRTrim(sbuff2);
				sb->Append((const UTF8Char*)" (");
				sb->Append(sbuff2);
				sb->Append((const UTF8Char*)")");
			}
			sb->Append((const UTF8Char*)"\r\nQUESTION_TYPE=");
			qType = ReadMUInt16(&packet[i]);
			sb->AppendU16(qType);
			switch (qType)
			{
			case 0x20:
				sb->Append((const UTF8Char*)" (NB)");
				break;
			case 0x21:
				sb->Append((const UTF8Char*)" (NBSTAT)");
				break;
			}
			sb->Append((const UTF8Char*)"\r\nQUESTION_CLASS=");
			qClass = ReadMUInt16(&packet[2 + i]);
			sb->AppendU16(qClass);
			if (qClass == 1)
			{
				sb->Append((const UTF8Char*)" (IN)");
			}
			i += 4;
			j++;
		}
		j = 0;
		ancount = (UInt16)(ancount + nscount + arcount);
		while (j < ancount)
		{
			i = Net::DNSClient::ParseString(sbuff, packet, i, packetSize);
			sb->Append((const UTF8Char*)"\r\nRR_NAME=");
			sb->Append(sbuff);
			if ((sptr = NetBIOSGetName(sbuff2, sbuff)) != 0)
			{
				sptr[-1] = 0;
				Text::StrRTrim(sbuff2);
				sb->Append((const UTF8Char*)" (");
				sb->Append(sbuff2);
				sb->Append((const UTF8Char*)")");
			}
			sb->Append((const UTF8Char*)"\r\nRR_TYPE=");
			rrType = ReadMUInt16(&packet[i]);
			sb->AppendU16(rrType);
			switch (rrType)
			{
			case 0x20:
				sb->Append((const UTF8Char*)" (NB)");
				break;
			case 0x21:
				sb->Append((const UTF8Char*)" (NBSTAT)");
				break;
			}
			sb->Append((const UTF8Char*)"\r\nRR_CLASS=");
			rrClass = ReadMUInt16(&packet[2 + i]);
			sb->AppendU16(rrClass);
			if (rrClass == 1)
			{
				sb->Append((const UTF8Char*)" (IN)");
			}
			sb->Append((const UTF8Char*)"\r\nTTL=");
			sb->AppendU32(ReadMUInt32(&packet[4 + i]));
			rdLength = ReadMUInt16(&packet[8 + i]);
			sb->Append((const UTF8Char*)"\r\nRD_LENGTH=");
			sb->AppendU16(rdLength);
			i += 10;
			if (rrType == 0x20 && rdLength == 6)
			{
				sb->Append((const UTF8Char*)"\r\nNB_FLAGS=0x");
				sb->AppendHex16(ReadMUInt16(&packet[i]));
				sb->Append((const UTF8Char*)"\r\nNB_ADDRESS=");
				Net::SocketUtil::GetIPv4Name(sbuff, ReadNUInt32(&packet[i + 2]));
				sb->Append(sbuff);
			}
			else
			{
				sb->Append((const UTF8Char*)"\r\nRDATA=");
				sb->AppendHexBuff(&packet[i], rdLength, ' ', Text::LBT_NONE);
			}
			i += rdLength;
			j++;
		}
		if (packetSize > i)
		{
			sb->Append((const UTF8Char*)"\r\n");
			sb->AppendHexBuff(&packet[i], packetSize - i, ' ', Text::LBT_CRLF);
		}
	}
	else if (destPort == 138 && packetSize >= 10)
	{
		UInt8 msgType = packet[0];
		sb->Append((const UTF8Char*)"\r\nNetBIOS-DS:");
		sb->Append((const UTF8Char*)"\r\nMSG_TYPE=0x");
		sb->AppendHex8(msgType);
		switch (msgType)
		{
		case 0x10:
			sb->Append((const UTF8Char*)" (Direct Unique Datagram)");
			break;
		case 0x11:
			sb->Append((const UTF8Char*)" (Direct Group Datagram)");
			break;
		case 0x12:
			sb->Append((const UTF8Char*)" (Broadcast Datagram)");
			break;
		case 0x13:
			sb->Append((const UTF8Char*)" (Datagram Error)");
			break;
		case 0x14:
			sb->Append((const UTF8Char*)" (Datagram Query Request)");
			break;
		case 0x15:
			sb->Append((const UTF8Char*)" (Datagram Positive Query Response)");
			break;
		case 0x16:
			sb->Append((const UTF8Char*)" (Datagram Negative Query Response)");
			break;
		}
		sb->Append((const UTF8Char*)"\r\nFLAGS=0x");
		sb->AppendHex8(packet[1]);
		sb->Append((const UTF8Char*)"\r\nDGM_ID=");
		sb->AppendU16(ReadMUInt16(&packet[2]));
		sb->Append((const UTF8Char*)"\r\nSOURCE_IP=");
		Net::SocketUtil::GetIPv4Name(sbuff, ReadNUInt32(&packet[4]));
		sb->Append(sbuff);
		sb->Append((const UTF8Char*)"\r\nSOURCE_PORT=");
		sb->AppendU16(ReadMUInt16(&packet[8]));
		UOSInt i;
		i = 10;
		switch (msgType)
		{
		case 0x10:
		case 0x11:
		case 0x12:
			sb->Append((const UTF8Char*)"\r\nDGM_LENGTH=");
			sb->AppendU16(ReadMUInt16(&packet[10]));
			sb->Append((const UTF8Char*)"\r\nPACKET_OFFSET=");
			sb->AppendU16(ReadMUInt16(&packet[12]));
			i = 14;
			i = Net::DNSClient::ParseString(sbuff, packet, i, packetSize);
			sb->Append((const UTF8Char*)"\r\nSOURCE_NAME=");
			sb->Append(sbuff);
			if ((sptr = NetBIOSGetName(sbuff2, sbuff)) != 0)
			{
				sptr[-1] = 0;
				Text::StrRTrim(sbuff2);
				sb->Append((const UTF8Char*)" (");
				sb->Append(sbuff2);
				sb->Append((const UTF8Char*)")");
			}
			i = Net::DNSClient::ParseString(sbuff, packet, i, packetSize);
			sb->Append((const UTF8Char*)"\r\nDESTINATION_NAME=");
			sb->Append(sbuff);
			if ((sptr = NetBIOSGetName(sbuff2, sbuff)) != 0)
			{
				sptr[-1] = 0;
				Text::StrRTrim(sbuff2);
				sb->Append((const UTF8Char*)" (");
				sb->Append(sbuff2);
				sb->Append((const UTF8Char*)")");
			}
			break;
		case 0x13:
			if (packetSize >= 19)
			{
				sb->Append((const UTF8Char*)"\r\nERROR_CODE=0x");
				sb->AppendHex8(packet[10]);
				switch (packet[10])
				{
				case 0x82:
					sb->Append((const UTF8Char*)" (Destination Name Not Present)");
					break;
				case 0x83:
					sb->Append((const UTF8Char*)" (Invalid Source Name Format)");
					break;
				case 0x84:
					sb->Append((const UTF8Char*)" (Invalid Destination Name Format)");
					break;
				}
				i = 11;
			}
			break;
		case 0x14:
		case 0x15:
		case 0x16:
			i = Net::DNSClient::ParseString(sbuff, packet, i, packetSize);
			sb->Append((const UTF8Char*)"\r\nDESTINATION_NAME=");
			sb->Append(sbuff);
			if ((sptr = NetBIOSGetName(sbuff2, sbuff)) != 0)
			{
				sptr[-1] = 0;
				Text::StrRTrim(sbuff2);
				sb->Append((const UTF8Char*)" (");
				sb->Append(sbuff2);
				sb->Append((const UTF8Char*)")");
			}
			break;
		}

		if (packetSize > i)
		{
			sb->Append((const UTF8Char*)"\r\n");
			sb->AppendHexBuff(&packet[i], packetSize - i, ' ', Text::LBT_CRLF);
		}
	}
	else if (srcPort == 161 || destPort == 161 || destPort == 162 || srcPort == 162)
	{
		if (packet[0] == 0x30)
		{
			sb->Append((const UTF8Char*)"\r\nSNMP:");
			sb->Append((const UTF8Char*)"\r\n");
			Net::SNMPInfo snmp;
			UOSInt i = snmp.PDUGetDetail((const UTF8Char*)"Message", packet, packetSize, 0, sb);
			if (packetSize > i)
			{
				sb->Append((const UTF8Char*)"\r\n");
				sb->AppendHexBuff(&packet[i], packetSize - i, ' ', Text::LBT_CRLF);
			}
		}
	}
	else if (srcPort == 427 || destPort == 427) //RFC 2165/2608
	{
		UOSInt i;
		sb->Append((const UTF8Char*)"\r\nService Location Protocol:");
		sb->Append((const UTF8Char*)"\r\nVersion=");
		sb->AppendU16(packet[0]);
		sb->Append((const UTF8Char*)"\r\nFunction-ID=");
		sb->AppendU16(packet[1]);
		switch (packet[1])
		{
		case 1:
			sb->Append((const UTF8Char*)" (Service Request)");
			break;
		case 2:
			sb->Append((const UTF8Char*)" (Service Reply)");
			break;
		case 3:
			sb->Append((const UTF8Char*)" (Service Registration)");
			break;
		case 4:
			sb->Append((const UTF8Char*)" (Service Deregister)");
			break;
		case 5:
			sb->Append((const UTF8Char*)" (Service Acknowledge)");
			break;
		case 6:
			sb->Append((const UTF8Char*)" (Attribute Request)");
			break;
		case 7:
			sb->Append((const UTF8Char*)" (Attribute Reply)");
			break;
		case 8:
			sb->Append((const UTF8Char*)" (DA Advertisement)");
			break;
		case 9:
			sb->Append((const UTF8Char*)" (Service Type Request)");
			break;
		case 10:
			sb->Append((const UTF8Char*)" (Service Type Reply)");
			break;
		case 11:
			sb->Append((const UTF8Char*)" (SA Advertisement)");
			break;
		}
		i = 2;
		UInt16 len;
		UInt16 len2;
		if (packet[0] == 1)
		{
			len = ReadMUInt16(&packet[2]);
			sb->Append((const UTF8Char*)"\r\nLength=");
			sb->AppendU16(len);
			sb->Append((const UTF8Char*)"\r\nFlags=0x");
			sb->AppendHex8(packet[4]);
			sb->Append((const UTF8Char*)"\r\nDialect=");
			sb->AppendU16(packet[5]);
			sb->Append((const UTF8Char*)"\r\nLanguage Code=");
			sb->AppendC(&packet[6], 2);
			UInt16 enc;
			sb->Append((const UTF8Char*)"\r\nCharacter Encoding=");
			enc = ReadMUInt16(&packet[8]);
			sb->AppendU16(enc);
			sb->Append((const UTF8Char*)"\r\nTransaction Identifier=");
			sb->AppendU16(ReadMUInt16(&packet[10]));
			if (packet[1] == 1)
			{
				i = 12;
				if (i + 2 <= packetSize)
				{
					len2 = ReadMUInt16(&packet[i]);
					i += 2;
					sb->Append((const UTF8Char*)"\r\nPrevious Responders=");
					if (len2 == 0)
					{
					
					}
					else if (len2 + i <= len)
					{
						sb->AppendC(&packet[i], len2);
						i += len2;
					}
					else
					{
						sb->AppendC(&packet[i], len - i);
						i = len;
					}
				}
				if (i + 2 <= packetSize)
				{
					len2 = ReadMUInt16(&packet[i]);
					i += 2;
					sb->Append((const UTF8Char*)"\r\nService Request=");
					if (len2 == 0)
					{
					
					}
					else if (len2 + i <= len)
					{
						sb->AppendC(&packet[i], len2);
						i += len2;
					}
					else
					{
						sb->AppendC(&packet[i], len - i);
						i = len;
					}
				}
			}
		}

		if (i < packetSize)
		{
			sb->Append((const UTF8Char*)"\r\n");
			sb->Append((const UTF8Char*)"\r\n");
			sb->AppendHexBuff(&packet[i], packetSize - i, ' ', Text::LBT_CRLF);
		}
	}
	else if (destPort == 1700)
	{
		if (packetSize >= 12 && packet[3] < 6)
		{
			sb->Append((const UTF8Char*)"\r\nLoRa Gateway:");
			sb->Append((const UTF8Char*)"\r\nProtocol Version=");
			sb->AppendU16(packet[0]);
			sb->Append((const UTF8Char*)"\r\nRandom Token=");
			sb->AppendU16(ReadMUInt16(&packet[1]));
			sb->Append((const UTF8Char*)"\r\nIdentifier=");
			sb->AppendU16(packet[3]);
			switch (packet[3])
			{
			case 0:
				sb->Append((const UTF8Char*)" (PUSH_DATA)");
				break;
			case 1:
				sb->Append((const UTF8Char*)" (PUSH_ACK)");
				break;
			case 2:
				sb->Append((const UTF8Char*)" (PULL_DATA)");
				break;
			case 3:
				sb->Append((const UTF8Char*)" (PULL_RESP)");
				break;
			case 4:
				sb->Append((const UTF8Char*)" (PULL_ACK)");
				break;
			case 5:
				sb->Append((const UTF8Char*)" (TX_ACK)");
				break;
			}
			sb->Append((const UTF8Char*)"\r\nGateway UID=");
			sb->AppendHexBuff(&packet[4], 8, 0, Text::LBT_NONE);

			if (packetSize > 12)
			{
				if (packet[12] == 0x7B)
				{
					sb->Append((const UTF8Char*)"\r\nContent:");
					sb->Append((const UTF8Char*)"\r\n");
					Text::JSText::JSONWellFormat(&packet[12], packetSize - 12, 0, sb);
					Text::JSONBase *json = Text::JSONBase::ParseJSONStrLen(&packet[12], packetSize - 12);
					if (json)
					{
						if (json->GetJSType() == Text::JSONBase::JST_OBJECT)
						{
							Text::JSONObject *jobj = (Text::JSONObject*)json;
							Text::JSONBase *jbase = jobj->GetObjectValue((const UTF8Char*)"rxpk");
							if (jbase && jbase->GetJSType() == Text::JSONBase::JST_ARRAY)
							{
								Text::TextBinEnc::Base64Enc b64;
								Text::JSONArray *jarr = (Text::JSONArray*)jbase;
								UOSInt i;
								UOSInt j;
								i = 0;
								j = jarr->GetArrayLength();
								while (i < j)
								{
									jbase = jarr->GetArrayValue(i);
									if (jbase && jbase->GetJSType() == Text::JSONBase::JST_OBJECT)
									{
										jobj = (Text::JSONObject*)jbase;
										jbase = jobj->GetObjectValue((const UTF8Char*)"data");
										if (jbase && jbase->GetJSType() == Text::JSONBase::JST_STRINGUTF8)
										{
											Text::JSONStringUTF8 *jstr = (Text::JSONStringUTF8*)jbase;
											UOSInt dataLen;
											UInt8 *dataBuff;
											const UTF8Char *dataStr = jstr->GetValue();
											sb->Append((const UTF8Char*)"\r\n");
											sb->Append((const UTF8Char*)"\r\n");
											sb->Append(dataStr);
											sb->Append((const UTF8Char*)":");
											dataLen = b64.CalcBinSize(dataStr);
											dataBuff = MemAlloc(UInt8, dataLen);
											if (b64.DecodeBin(dataStr, dataBuff) == dataLen)
											{
												PacketLoRaMACGetDetail(dataBuff, dataLen, sb);
											}
											else
											{
												sb->Append((const UTF8Char*)"\r\nNot base64 encoding");
											}
											MemFree(dataBuff);
										}
									}
									i++;
								}
							}
						}
						json->EndUse();
					}
				}
				else
				{
					sb->Append((const UTF8Char*)"\r\n");
					sb->Append((const UTF8Char*)"\r\n");
					sb->AppendHexBuff(&packet[12], packetSize - 12, ' ', Text::LBT_CRLF);
				}
			}
		}
		else
		{
			sb->Append((const UTF8Char*)"\r\n");
			sb->Append((const UTF8Char*)"\r\n");
			sb->AppendHexBuff(packet, packetSize, ' ', Text::LBT_CRLF);
		}
	}
	else if (srcPort == 1700)
	{
		if (packetSize >= 4 && packet[3] < 5)
		{
			sb->Append((const UTF8Char*)"\r\nLoRa Gateway PUSH_ACK:");
			sb->Append((const UTF8Char*)"\r\nProtocol Version=");
			sb->AppendU16(packet[0]);
			sb->Append((const UTF8Char*)"\r\nRandom Token=");
			sb->AppendU16(ReadMUInt16(&packet[1]));
			sb->Append((const UTF8Char*)"\r\nIdentifier=");
			sb->AppendU16(packet[3]);
			switch (packet[3])
			{
			case 0:
				sb->Append((const UTF8Char*)" (PUSH_DATA)");
				break;
			case 1:
				sb->Append((const UTF8Char*)" (PUSH_ACK)");
				break;
			case 2:
				sb->Append((const UTF8Char*)" (PULL_DATA)");
				break;
			case 3:
				sb->Append((const UTF8Char*)" (PULL_RESP)");
				break;
			case 4:
				sb->Append((const UTF8Char*)" (PULL_ACK)");
				break;
			case 5:
				sb->Append((const UTF8Char*)" (TX_ACK)");
				break;
			}
			if (packetSize > 4)
			{
				sb->Append((const UTF8Char*)"\r\n");
				sb->Append((const UTF8Char*)"\r\n");
				sb->AppendHexBuff(&packet[4], packetSize - 4, ' ', Text::LBT_CRLF);
			}
		}
		else
		{
			sb->Append((const UTF8Char*)"\r\n");
			sb->Append((const UTF8Char*)"\r\n");
			sb->AppendHexBuff(packet, packetSize, ' ', Text::LBT_CRLF);
		}
	}
	else if (destPort == 1900)
	{
		sb->Append((const UTF8Char*)"\r\nSSDP Request:");
		sb->Append((const UTF8Char*)"\r\n");
		sb->AppendC(packet, packetSize);
	}
	else if (srcPort == 1900)
	{
		sb->Append((const UTF8Char*)"\r\nSSDP Reply:");
		sb->Append((const UTF8Char*)"\r\n");
		sb->AppendC(packet, packetSize);
	}
	else if (destPort == 3702)
	{
		sb->Append((const UTF8Char*)"\r\nWS-Discovery Request:");
		sb->Append((const UTF8Char*)"\r\n");
		XMLWellFormat(packet, packetSize, sb);
	}
	else if (srcPort == 3702)
	{
		sb->Append((const UTF8Char*)"\r\nWS-Discovery Reply:");
		sb->Append((const UTF8Char*)"\r\n");
		XMLWellFormat(packet, packetSize, sb);
	}
	else if (destPort == 5353)
	{
		sb->Append((const UTF8Char*)"\r\nmDNS:");
		if (packetSize < 12)
		{
			sb->Append((const UTF8Char*)"\r\n");
			sb->AppendHexBuff(packet, packetSize, ' ', Text::LBT_CRLF);
		}
		else
		{
			PacketDNSGetDetail(packet, packetSize, sb);
		}
	}
	else if (srcPort == 17500 && destPort == 17500)
	{
		sb->Append((const UTF8Char*)"\r\nDropbox LAN Sync Discovery:");
		sb->Append((const UTF8Char*)"\r\n");
		Text::JSText::JSONWellFormat(packet, packetSize, 0, sb);
	}
	else
	{
		sb->Append((const UTF8Char*)"\r\nUnknown Data:");
		sb->Append((const UTF8Char*)"\r\n");
		sb->AppendHexBuff(packet, packetSize, ' ', Text::LBT_CRLF);
	}
}

void Net::EthernetAnalyzer::PacketDNSGetDetail(const UInt8 *packet, UOSInt packetSize, Text::StringBuilderUTF *sb)
{
	UTF8Char sbuff[128];
	const UTF8Char *csptr;
	sb->Append((const UTF8Char*)"\r\nID=");
	sb->AppendU16(ReadMUInt16(&packet[0]));
	sb->Append((const UTF8Char*)"\r\nQR=");
	sb->Append((packet[2] & 0x80)?(const UTF8Char*)"1 (Response)":(const UTF8Char*)"0 (Request)");
	sb->Append((const UTF8Char*)"\r\nOPCODE=");
	UInt8 opcode = (packet[2] & 0x78) >> 3;
	sb->AppendU16(opcode);
	switch (opcode)
	{
	case 0:
		sb->Append((const UTF8Char*)" (QUERY)");
		break;
	case 1:
		sb->Append((const UTF8Char*)" (IQUERY)");
		break;
	case 2:
		sb->Append((const UTF8Char*)" (STATUS)");
		break;
	}
	sb->Append((const UTF8Char*)"\r\nAA=");
	sb->AppendU16((packet[2] & 4) >> 2);
	sb->Append((const UTF8Char*)"\r\nTC=");
	sb->AppendU16((packet[2] & 2) >> 1);
	sb->Append((const UTF8Char*)"\r\nRD=");
	sb->AppendU16((packet[2] & 1));
	sb->Append((const UTF8Char*)"\r\nRA=");
	sb->AppendU16((packet[3] & 0x80) >> 7);
	sb->Append((const UTF8Char*)"\r\nZ=");
	sb->AppendU16((packet[3] & 0x70) >> 4);
	sb->Append((const UTF8Char*)"\r\nRCODE=");
	UInt8 rcode = packet[3] & 0xf;
	sb->AppendU16(rcode);
	switch (rcode)
	{
	case 0:
		sb->Append((const UTF8Char*)" (No error)");
		break;
	case 1:
		sb->Append((const UTF8Char*)" (Format error)");
		break;
	case 2:
		sb->Append((const UTF8Char*)" (Server failure)");
		break;
	case 3:
		sb->Append((const UTF8Char*)" (Name Error)");
		break;
	case 4:
		sb->Append((const UTF8Char*)" (Not Implemented)");
		break;
	case 5:
		sb->Append((const UTF8Char*)" (Refused)");
		break;
	}
	UInt16 qdcount = ReadMUInt16(&packet[4]);
	UInt16 ancount = ReadMUInt16(&packet[6]);
	UInt16 nscount = ReadMUInt16(&packet[8]);
	UInt16 arcount = ReadMUInt16(&packet[10]);
	sb->Append((const UTF8Char*)"\r\nQDCOUNT=");
	sb->AppendU16(qdcount);
	sb->Append((const UTF8Char*)"\r\nANCOUNT=");
	sb->AppendU16(ancount);
	sb->Append((const UTF8Char*)"\r\nNSCOUNT=");
	sb->AppendU16(nscount);
	sb->Append((const UTF8Char*)"\r\nARCOUNT=");
	sb->AppendU16(arcount);
	UOSInt i = 12;
	UInt16 j;
	UInt16 t;
	j = 0;
	while (j < qdcount)
	{
		i = Net::DNSClient::ParseString(sbuff, packet, i, packetSize);
		sb->Append((const UTF8Char*)"\r\nQNAME=");
		sb->Append(sbuff);
		sb->Append((const UTF8Char*)"\r\nQTYPE=");
		t = ReadMUInt16(&packet[i]);
		sb->AppendU16(t);
		csptr = Net::DNSClient::TypeGetID(t);
		if (csptr)
		{
			sb->Append((const UTF8Char*)" (");
			sb->Append(csptr);
			sb->Append((const UTF8Char*)")");
		}
		sb->Append((const UTF8Char*)"\r\nQCLASS=");
		sb->AppendU16(ReadMUInt16(&packet[i + 2]));

		i += 4;
		j++;
	}
	ancount = (UInt16)(ancount + nscount + arcount);
	j = 0;
	while (j < ancount)
	{
		UInt16 rrType;
		UInt16 rrClass;
		UInt16 rdLength;
		UOSInt k;

		i = Net::DNSClient::ParseString(sbuff, packet, i, packetSize);
		sb->Append((const UTF8Char*)"\r\nNAME=");
		sb->Append(sbuff);
		rrType = ReadMUInt16(&packet[i]);
		rrClass = ReadMUInt16(&packet[i + 2]);
		rdLength = ReadMUInt16(&packet[i + 8]);
		sb->Append((const UTF8Char*)"\r\nTYPE=");
		sb->AppendU16(rrType);
		csptr = Net::DNSClient::TypeGetID(rrType);
		if (csptr)
		{
			sb->Append((const UTF8Char*)" (");
			sb->Append(csptr);
			sb->Append((const UTF8Char*)")");
		}
		sb->Append((const UTF8Char*)"\r\nCLASS=");
		sb->AppendU16(rrClass);
		sb->Append((const UTF8Char*)"\r\nTTL=");
		sb->AppendU32(ReadMUInt32(&packet[i + 4]));
		sb->Append((const UTF8Char*)"\r\nRDLENGTH=");
		sb->AppendU16(rdLength);
		sb->Append((const UTF8Char*)"\r\nRDATA=");
		i += 10;
		switch (rrType)
		{
		case 1: // A - a host address
			Net::SocketUtil::GetIPv4Name(sbuff, ReadNUInt32(&packet[i]));
			sb->Append(sbuff);
			break;
		case 2: // NS - an authoritative name server
		case 5: // CNAME - the canonical name for an alias
		case 12: // PTR - a domain name pointer
			Net::DNSClient::ParseString(sbuff, packet, i, i + rdLength);
			sb->Append(sbuff);
			break;
		case 6:
			k = Net::DNSClient::ParseString(sbuff, packet, i, i + rdLength);
			sb->Append(sbuff);
			sb->Append((const UTF8Char*)"\r\n-MailAddr=");
			k = Net::DNSClient::ParseString(sbuff, packet, k, i + rdLength);
			sb->Append(sbuff);
			if (k + 20 <= i + rdLength)
			{
				sb->Append((const UTF8Char*)"\r\n-SN=");
				sb->AppendU32(ReadMUInt32(&packet[k]));
				sb->Append((const UTF8Char*)"\r\n-Refresh=");
				sb->AppendU32(ReadMUInt32(&packet[k + 4]));
				sb->Append((const UTF8Char*)"\r\n-Retry=");
				sb->AppendU32(ReadMUInt32(&packet[k + 8]));
				sb->Append((const UTF8Char*)"\r\n-Expire=");
				sb->AppendU32(ReadMUInt32(&packet[k + 12]));
				sb->Append((const UTF8Char*)"\r\n-DefTTL=");
				sb->AppendU32(ReadMUInt32(&packet[k + 16]));
				k += 20;
				if (k < i + rdLength)
				{
					sb->Append((const UTF8Char*)"\r\n");
					sb->AppendHexBuff(&packet[k], i + rdLength - k, ' ', Text::LBT_CRLF);
				}
			}
			else if (k < i + rdLength)
			{
				sb->Append((const UTF8Char*)"\r\n");
				sb->AppendHexBuff(&packet[k], i + rdLength - k, ' ', Text::LBT_CRLF);
			}
			break;
		case 15: // MX - mail exchange
			sb->Append((const UTF8Char*)"Priority=");
			sb->AppendU16(ReadMUInt16(&packet[i]));
			sb->Append((const UTF8Char*)", ");
			Net::DNSClient::ParseString(sbuff, packet, i + 2, i + rdLength);
			sb->Append(sbuff);
			break;
		case 16: // TXT - text strings
			{
				UOSInt k = 0;
				while (k < rdLength)
				{
					if ((UOSInt)packet[i + k] + 1 + k > rdLength)
					{
						sb->AppendHexBuff(&packet[i + k], rdLength - k, ' ', Text::LBT_NONE);
						break;
					}
					if (k > 0)
					{
						sb->Append((const UTF8Char*)", ");
					}
					sb->AppendC(&packet[i + k + 1], packet[i + k]);
					k += (UOSInt)packet[i + k] + 1;
				}
			}
			break;
		case 28: // AAAA
			{
				Net::SocketUtil::AddressInfo addr;
				Net::SocketUtil::SetAddrInfoV6(&addr, &packet[i], 0);
				Net::SocketUtil::GetAddrName(sbuff, &addr);
				sb->Append(sbuff);
			}
			break;
		case 33: // SRV - 
			{
				sb->Append((const UTF8Char*)"Priority=");
				sb->AppendU16(ReadMUInt16(&packet[i]));
				sb->Append((const UTF8Char*)", Weight=");
				sb->AppendU16(ReadMUInt16(&packet[i + 2]));
				sb->Append((const UTF8Char*)", Port=");
				sb->AppendU16(ReadMUInt16(&packet[i + 4]));
				sb->Append((const UTF8Char*)", Target=");
				Net::DNSClient::ParseString(sbuff, packet, i + 6, i + rdLength);
				sb->Append(sbuff);
			}
			break;
		case 41: // OPT - 
			{
				sb->Append((const UTF8Char*)"OPTION-CODE=");
				sb->AppendU16(ReadMUInt16(&packet[i]));
				sb->Append((const UTF8Char*)", OPTION-LENGTH=");
				sb->AppendU16(ReadMUInt16(&packet[i + 2]));
				sb->Append((const UTF8Char*)", OPTION-DATA=");
				sb->AppendHexBuff(&packet[i + 4], (UOSInt)rdLength - 4, ' ', Text::LBT_NONE);
			}
			break;
		case 43: // DS - Delegation signer
			{
				sb->Append((const UTF8Char*)"Key Tag=");
				sb->AppendU16(ReadMUInt16(&packet[i]));
				sb->Append((const UTF8Char*)", Algorithm=");
				sb->AppendU16(packet[i + 2]);
				sb->Append((const UTF8Char*)", Digest Type=");
				sb->AppendU16(packet[i + 3]);
				sb->Append((const UTF8Char*)", Digest=");
				sb->AppendHexBuff(&packet[i + 4], (UOSInt)rdLength - 4, ' ', Text::LBT_NONE);
			}
			break;
		case 46: // RRSIG - DNSSEC signature
			{
				sb->Append((const UTF8Char*)"Type Covered=");
				sb->AppendU16(ReadMUInt16(&packet[i]));
				sb->Append((const UTF8Char*)", Algorithm=");
				sb->AppendU16(packet[i + 2]);
				sb->Append((const UTF8Char*)", Labels=");
				sb->AppendU16(packet[i + 3]);
				sb->Append((const UTF8Char*)", Original TTL=");
				sb->AppendU32(ReadMUInt32(&packet[i + 4]));
				sb->Append((const UTF8Char*)", Signature Expiration=");
				sb->AppendU32(ReadMUInt32(&packet[i + 8]));
				sb->Append((const UTF8Char*)", Signature Inception=");
				sb->AppendU32(ReadMUInt32(&packet[i + 12]));
				sb->Append((const UTF8Char*)", Key Tag=");
				sb->AppendU16(ReadMUInt16(&packet[i + 16]));
				sb->Append((const UTF8Char*)", Signer's Name=");
				sb->Append(&packet[i + 18]);
				sb->Append((const UTF8Char*)", Signature=");
				UOSInt nameLen = Text::StrCharCnt(&packet[i + 18]);
				sb->AppendHexBuff(&packet[i + 19 + nameLen], (UOSInt)rdLength - 19 - nameLen, ' ', Text::LBT_NONE);
			}
			break;
		case 47: // NSEC - Next Secure record
			{
				UOSInt k = Net::DNSClient::ParseString(sbuff, packet, i, i + rdLength);
				sb->Append((const UTF8Char*)"Next Domain Name=");
				sb->Append(sbuff);
				if (k < i + rdLength)
				{
					sb->Append((const UTF8Char*)", ");
					sb->AppendHexBuff(&packet[k], i + rdLength - k, ' ', Text::LBT_NONE);
				}
			}
			break;
		case 48: // DNSKEY - DNS Key record
			{
				sb->Append((const UTF8Char*)"Flags=");
				sb->AppendU16(ReadMUInt16(&packet[i]));
				sb->Append((const UTF8Char*)", Protocol=");
				sb->AppendU16(packet[i + 2]);
				sb->Append((const UTF8Char*)", Algorithm=");
				sb->AppendU16(packet[i + 3]);
				sb->Append((const UTF8Char*)", Public Key=");
				sb->AppendHexBuff(&packet[i + 4], (UOSInt)rdLength - 4, ' ', Text::LBT_NONE);
			}
			break;
		case 250: // TSIG
			{
				UOSInt k = Net::DNSClient::ParseString(sbuff, packet, i, i + rdLength);
				sb->Append((const UTF8Char*)"\r\n-Algorithm=");
				sb->Append(sbuff);
				if (k + 10 < i + rdLength)
				{
					Data::DateTime dt;
					dt.SetUnixTimestamp((Int64)(((UInt64)(ReadMUInt16(&packet[k])) << 32) | ReadMUInt32(&packet[k + 2])));
					sb->Append((const UTF8Char*)"\r\n-Time Signed=");
					dt.ToLocalTime();
					dt.ToString(sbuff, "yyyy-MM-dd HH:mm:ss zzz");
					sb->Append(sbuff);
					sb->Append((const UTF8Char*)"\r\n-Fudge=");
					sb->AppendU16(ReadMUInt16(&packet[k + 6]));
					sb->Append((const UTF8Char*)"\r\n-MAC Size=");
					UOSInt macSize = ReadMUInt16(&packet[k + 8]);
					sb->AppendUOSInt(macSize);
					k += 10;
					if (macSize > 0 && k + macSize <= i + rdLength)
					{
						sb->Append((const UTF8Char*)"\r\n-MAC=");
						sb->AppendHexBuff(&packet[k], macSize, ' ', Text::LBT_NONE);
						k += macSize;
					}
					if (k + 6 <= i + rdLength)
					{
						sb->Append((const UTF8Char*)"\r\n-Original Id=");
						sb->AppendU16(ReadMUInt16(&packet[k]));
						sb->Append((const UTF8Char*)"\r\n-Error=");
						sb->AppendU16(ReadMUInt16(&packet[k + 2]));
						sb->Append((const UTF8Char*)"\r\n-Other Len=");
						sb->AppendU16(ReadMUInt16(&packet[k + 4]));
						k += 6;
					}
					if (k < i + rdLength)
					{
						sb->Append((const UTF8Char*)"\r\n-Other=");
						sb->AppendHexBuff(&packet[k], i + rdLength - k, ' ', Text::LBT_NONE);
					}
				}
				else
				{
					sb->Append((const UTF8Char*)"\r\n-");
					sb->AppendHexBuff(&packet[k], i + rdLength - k, ' ', Text::LBT_NONE);
				}
				
			}
			break;
		default:
			sb->AppendHexBuff(&packet[i], rdLength, ' ', Text::LBT_NONE);
			break;
		}
		i += rdLength;
		j++;
	}
	if (i < packetSize)
	{
		sb->Append((const UTF8Char*)"\r\n");
		sb->AppendHexBuff(&packet[i], packetSize - i, ' ', Text::LBT_CRLF);
	}
}

void Net::EthernetAnalyzer::PacketLoRaMACGetDetail(const UInt8 *packet, UOSInt packetSize, Text::StringBuilderUTF *sb)
{
	sb->Append((const UTF8Char*)"\r\nMessage type (MType)=");
	sb->AppendU16((UInt16)(packet[0] >> 5));
	switch (packet[0] >> 5)
	{
	case 0:
		sb->Append((const UTF8Char*)" (Join-request)");
		break;
	case 1:
		sb->Append((const UTF8Char*)" (Join-accept)");
		break;
	case 2:
		sb->Append((const UTF8Char*)" (Unconfirmed Data Up)");
		break;
	case 3:
		sb->Append((const UTF8Char*)" (Unconfirmed Data Down)");
		break;
	case 4:
		sb->Append((const UTF8Char*)" (Confirmed Data Up)");
		break;
	case 5:
		sb->Append((const UTF8Char*)" (Confirmed Data Down)");
		break;
	case 6:
		sb->Append((const UTF8Char*)" (Rejoin-request)");
		break;
	case 7:
		sb->Append((const UTF8Char*)" (Proprietary)");
		break;
	}
	sb->Append((const UTF8Char*)"\r\nRFU=");
	sb->AppendU16((packet[0] >> 2) & 7);
	sb->Append((const UTF8Char*)"\r\nMajor=");
	sb->AppendU16(packet[0] & 3);
	UInt8 mType = (UInt8)(packet[0] >> 5);
	if (mType == 0 || mType == 1 || mType == 6)
	{
		if (packetSize == 23)
		{
			sb->Append((const UTF8Char*)"\r\nJoinEUI=");
			sb->AppendHex64(ReadUInt64(&packet[1]));
			sb->Append((const UTF8Char*)"\r\nDevEUI=");
			sb->AppendHex64(ReadUInt64(&packet[9]));
			sb->Append((const UTF8Char*)"\r\nDevNonce=");
			sb->AppendU16(ReadUInt16(&packet[17]));
		}
		else
		{
			sb->Append((const UTF8Char*)"\r\nMACPayload:");
			sb->Append((const UTF8Char*)"\r\n");
			sb->AppendHexBuff(&packet[1], packetSize - 5, ' ', Text::LBT_CRLF);
		}
	}
	else
	{
		sb->Append((const UTF8Char*)"\r\nMACPayload:");
		sb->Append((const UTF8Char*)"\r\n");
		sb->AppendHexBuff(&packet[1], packetSize - 5, ' ', Text::LBT_CRLF);
	}

	sb->Append((const UTF8Char*)"\r\nMIC=");
	sb->AppendHexBuff(&packet[packetSize - 4], 4, ' ', Text::LBT_NONE);
}

UOSInt Net::EthernetAnalyzer::HeaderIPv4GetDetail(const UInt8 *packet, UOSInt packetSize, Text::StringBuilderUTF *sb)
{
	UTF8Char sbuff[32];
	sb->Append((const UTF8Char*)"\r\nVersion=4");
	sb->Append((const UTF8Char*)"\r\nInternet Header Length=");
	sb->AppendU16((UInt16)packet[0] & 0xf);
	sb->Append((const UTF8Char*)"\r\nDSCP=");
	sb->AppendU16((UInt16)(packet[1] >> 2));
	sb->Append((const UTF8Char*)"\r\nECN=");
	sb->AppendU16((UInt16)packet[1] & 0x3);
	sb->Append((const UTF8Char*)"\r\nTotal Size=");
	sb->AppendU16(ReadMUInt16(&packet[2]));
	sb->Append((const UTF8Char*)"\r\nIdentification=");
	sb->AppendU16(ReadMUInt16(&packet[4]));
	sb->Append((const UTF8Char*)"\r\nFlags=");
	sb->AppendU16((UInt16)(packet[6] >> 5));
	sb->Append((const UTF8Char*)"\r\nFragment Offset=");
	sb->AppendU16(ReadMUInt16(&packet[6]) & 0x1fff);
	sb->Append((const UTF8Char*)"\r\nTTL=");
	sb->AppendU16((UInt16)packet[8]);
	sb->Append((const UTF8Char*)"\r\nProtocol=");
	sb->AppendU16((UInt16)packet[9]);
	sb->Append((const UTF8Char*)"\r\nHeader Checksum=0x");
	sb->AppendHex16(ReadMUInt16(&packet[10]));
	Net::SocketUtil::GetIPv4Name(sbuff, ReadNUInt32(&packet[12]));
	sb->Append((const UTF8Char*)"\r\nSrcIP=");
	sb->Append(sbuff);
	Net::SocketUtil::GetIPv4Name(sbuff, ReadNUInt32(&packet[16]));
	sb->Append((const UTF8Char*)"\r\nDestIP=");
	sb->Append(sbuff);

	if ((packet[0] & 0xf) <= 5)
	{
		return 20;
	}
	else
	{
		sb->Append((const UTF8Char*)"\r\nOptions:\r\n");
		sb->AppendHexBuff(&packet[20], (UOSInt)((packet[0] & 0xf) << 2) - 20, ' ', Text::LBT_CRLF);
		return (packet[0] & 0xf) << 2;
	}
}

void Net::EthernetAnalyzer::XMLWellFormat(const UTF8Char *buff, UOSInt buffSize, Text::StringBuilderUTF *sb)
{
	UOSInt startOfst = 0;
	UOSInt lev = 0;
	UOSInt currOfst = 0;
	UTF8Char lastC = 0;
	UTF8Char c;
	UInt8 startType = 0;
	while (currOfst < buffSize)
	{
		c = buff[currOfst];
		if (c == '<')
		{
			if (startType == 0)
			{
				if (startOfst < currOfst)
				{
					sb->AppendC(&buff[startOfst], currOfst - startOfst);
				}
				if (buff[currOfst + 1] == '?')
				{
					startOfst = currOfst;
					startType = 1;
				}
				else if (buff[currOfst + 1] == '/')
				{
					startOfst = currOfst;
					startType = 3;
				}
				else
				{
					startOfst = currOfst;
					startType = 2;
				}
			}
			else if (startType == 4 && buff[currOfst + 1] == '/')
			{
				startType = 5;
			}
		}
		else if (c == '>')
		{
			if (startType == 1)
			{
				if (lev > 0)
				{
					sb->AppendChar(' ', lev << 1);
				}
				sb->AppendC(&buff[startOfst], currOfst - startOfst + 1);
				sb->Append((const UTF8Char*)"\r\n");
				startType = 0;
				startOfst = currOfst + 1;
			}
			else if (startType == 2)
			{
				if (lastC == '/')
				{
					if (lev > 0)
					{
						sb->AppendChar(' ', lev << 1);
					}
					sb->AppendC(&buff[startOfst], currOfst - startOfst + 1);
					sb->Append((const UTF8Char*)"\r\n");
					startType = 0;
					startOfst = currOfst + 1;
				}
				else if (buff[currOfst + 1] == '<' && buff[currOfst + 2] == '/')
				{
					startType = 5;
				}
				else if (buff[currOfst + 1] == '<')
				{
					if (lev > 0)
					{
						sb->AppendChar(' ', lev << 1);
					}
					lev++;
					sb->AppendC(&buff[startOfst], currOfst - startOfst + 1);
					sb->Append((const UTF8Char*)"\r\n");
					startType = 0;
					startOfst = currOfst + 1;
				}
				else
				{
					startType = 4;
				}
			}
			else if (startType == 5)
			{
				if (lev > 0)
				{
					sb->AppendChar(' ', lev << 1);
				}
				sb->AppendC(&buff[startOfst], currOfst - startOfst + 1);
				sb->Append((const UTF8Char*)"\r\n");
				startType = 0;
				startOfst = currOfst + 1;
			}
			else if (startType == 3)
			{
				if (lev > 0)
				{
					lev--;
					if (lev > 0)
					{
						sb->AppendChar(' ', lev << 1);
					}
				}
				sb->AppendC(&buff[startOfst], currOfst - startOfst + 1);
				sb->Append((const UTF8Char*)"\r\n");
				startType = 0;
				startOfst = currOfst + 1;
			}
		}

		currOfst++;
		lastC = c;
	}
	if (startOfst < buffSize)
	{
		sb->AppendC(&buff[startOfst], buffSize - startOfst);
	}
}

UTF8Char *Net::EthernetAnalyzer::NetBIOSGetName(UTF8Char *sbuff, const UTF8Char *nbName)
{
	UTF8Char c1;
	UTF8Char c2;
	while (true)
	{
		c1 = *nbName++;
		if (c1 == 0)
			break;
		c2 = *nbName++;
		if (c1 < 65 || c1 >= 81 || c2 < 65 || c2 >= 81)
		{
			return 0;
		}
		*sbuff++ = (UTF8Char)(((c1 - 65) << 4) | (c2 - 65));
	}
	*sbuff = 0;
	return sbuff;
}

const UTF8Char *Net::EthernetAnalyzer::TCPPortGetName(UInt16 port)
{
	switch (port)
	{
	case 21:
		return (const UTF8Char*)"FTP";
	case 22:
		return (const UTF8Char*)"SSH";
	case 23:
		return (const UTF8Char*)"Telnet";
	case 25:
		return (const UTF8Char*)"SMTP";
	case 43:
		return (const UTF8Char*)"WHOIS";
	case 53:
		return (const UTF8Char*)"DNS";
	case 80:
		return (const UTF8Char*)"HTTP";
	case 110:
		return (const UTF8Char*)"POP3";
	case 135:
		return (const UTF8Char*)"Microsoft EPMAP";
	case 139:
		return (const UTF8Char*)"NetBIOS-SSN";
	case 443:
		return (const UTF8Char*)"HTTPS";
	case 445:
		return (const UTF8Char*)"Microsoft-DS";
	case 538:
		return (const UTF8Char*)"GDOMAP";
	case 902:
		return (const UTF8Char*)"VMWare ESX";
	case 912:
		return (const UTF8Char*)"VMWare ESX";
	case 3306:
		return (const UTF8Char*)"MySQL";
	case 3389:
		return (const UTF8Char*)"RDP";
	}
	return 0;
}
const UTF8Char *Net::EthernetAnalyzer::UDPPortGetName(UInt16 port)
{
	switch (port)
	{
	case 53:
		return (const UTF8Char*)"DNS";
	case 67:
		return (const UTF8Char*)"DHCP Server";
	case 68:
		return (const UTF8Char*)"DHCP Client";
	case 69:
		return (const UTF8Char*)"TFTP";
	case 123:
		return (const UTF8Char*)"NTP";
	case 137:
		return (const UTF8Char*)"NetBIOS-NS";
	case 138:
		return (const UTF8Char*)"NetBIOS-DS";
	case 139:
		return (const UTF8Char*)"NetBIOS-SS";
	case 161:
		return (const UTF8Char*)"SNMP";
	case 162:
		return (const UTF8Char*)"SNMP-TRAP";
	case 427:
		return (const UTF8Char*)"SLP";
	case 546:
		return (const UTF8Char*)"DHCPv6 client";
	case 547:
		return (const UTF8Char*)"DHCPv6 server";
	case 1700:
		return (const UTF8Char*)"LoRa Gateway";
	case 1900:
		return (const UTF8Char*)"SSDP";
	case 3702:
		return (const UTF8Char*)"WS-Discovery";
	case 5353:
		return (const UTF8Char*)"mDNS";
	case 17500:
		return (const UTF8Char*)"Dropbox LAN Sync Discovery";
	case 55208:
		return (const UTF8Char*)"JavaME Device Detection";
	case 55209:
		return (const UTF8Char*)"JavaME Device Detection";
	}
	return 0;
}

const UTF8Char *Net::EthernetAnalyzer::LinkTypeGetName(UInt32 linkType)
{
	switch (linkType)
	{
	case 0:
		return (const UTF8Char*)"Null";
	case 1:
		return (const UTF8Char*)"Ethernet";
	case 3:
		return (const UTF8Char*)"AX 25";
	case 6:
		return (const UTF8Char*)"IEEE802.5";
	case 7:
		return (const UTF8Char*)"ARCNET";
	case 8:
		return (const UTF8Char*)"SLIP";
	case 9:
		return (const UTF8Char*)"PPP";
	}
	return 0;
}

const UTF8Char *Net::EthernetAnalyzer::LSAPGetName(UInt8 lsap)
{
	switch (lsap)
	{
	case 0:
		return (const UTF8Char*)"Null LSAP";
	case 2:
		return (const UTF8Char*)"Individual LLC Sublayer Mgt";
	case 4:
		return (const UTF8Char*)"SNA Path Control (individual)";
	case 6:
		return (const UTF8Char*)"Reserved for DoD";
	case 0x0E:
		return (const UTF8Char*)"ProWay-LAN";
	case 0x18:
		return (const UTF8Char*)"Texas Instruments";
	case 0x42:
		return (const UTF8Char*)"IEEE 802.1 Bridge Spanning Tree Protocol";
	case 0x4E:
		return (const UTF8Char*)"EIA-RS 511";
	case 0x5E:
		return (const UTF8Char*)"ISI IP";
	case 0x7E:
		return (const UTF8Char*)"ISO 8208 (X.25 over IEEE 802.2 Type LLC)";
	case 0x80:
		return (const UTF8Char*)"Xerox Network Systems (XNS)";
	case 0x82:
		return (const UTF8Char*)"BACnet/Ethernet";
	case 0x86:
		return (const UTF8Char*)"Nestar";
	case 0x8E:
		return (const UTF8Char*)"ProWay-LAN (IEC 955)";
	case 0x98:
		return (const UTF8Char*)"ARPANET Address Resolution Protocol (ARP)";
	case 0xA6:
		return (const UTF8Char*)"RDE (route determination entity)";
	case 0xAA:
		return (const UTF8Char*)"SNAP Extension Used";
	case 0xBC:
		return (const UTF8Char*)"Banyan Vines";
	case 0xE0:
		return (const UTF8Char*)"Novell NetWare";
	case 0xF0:
		return (const UTF8Char*)"IBM NetBIOS";
	case 0xF4:
		return (const UTF8Char*)"IBM LAN Management (individual)";
	case 0xF8:
		return (const UTF8Char*)"IBM Remote Program Load (RPL)";
	case 0xFA:
		return (const UTF8Char*)"Ungermann-Bass";
	case 0xFE:
		return (const UTF8Char*)"OSI protocols ISO CLNS IS 8473";
	case 0x03:
		return (const UTF8Char*)"Group LLC Sublayer Mgt";
	case 0x05:
		return (const UTF8Char*)"SNA Path Control (group)";
	case 0xF5:
		return (const UTF8Char*)"IBM LAN Management (group)";
	case 0xFF:
		return (const UTF8Char*)"Global DSAP (broadcast to all)";
	}
	return 0;
}

const UTF8Char *Net::EthernetAnalyzer::DHCPOptionGetName(UInt8 t)
{
	switch (t)
	{
	case 1:
		return (const UTF8Char*)"Subnet Mask";
	case 2:
		return (const UTF8Char*)"Time Offset";
	case 3:
		return (const UTF8Char*)"Router";
	case 4:
		return (const UTF8Char*)"Time Server";
	case 5:
		return (const UTF8Char*)"Name Server";
	case 6:
		return (const UTF8Char*)"Domain Name Server";
	case 7:
		return (const UTF8Char*)"Log Server";
	case 8:
		return (const UTF8Char*)"Cookie Server";
	case 9:
		return (const UTF8Char*)"LPR Server";
	case 10:
		return (const UTF8Char*)"Impress server";
	case 11:
		return (const UTF8Char*)"Resource location server";
	case 12:
		return (const UTF8Char*)"Host name";
	case 13:
		return (const UTF8Char*)"Boot file size";
	case 14:
		return (const UTF8Char*)"Merit dump file";
	case 15:
		return (const UTF8Char*)"Domain name";
	case 16:
		return (const UTF8Char*)"Swap server";
	case 17:
		return (const UTF8Char*)"Root path";
	case 18:
		return (const UTF8Char*)"Extensions path";
	case 19:
		return (const UTF8Char*)"Forward On/Off";
	case 20:
		return (const UTF8Char*)"SrcRte On/Off";
	case 21:
		return (const UTF8Char*)"Policy Filter";
	case 22:
		return (const UTF8Char*)"Max DG Assembly";
	case 23:
		return (const UTF8Char*)"Default IP TTL";
	case 24:
		return (const UTF8Char*)"MTU Timeout";
	case 25:
		return (const UTF8Char*)"MTU Plateau";
	case 26:
		return (const UTF8Char*)"MTU Interface";
	case 27:
		return (const UTF8Char*)"MTU Subnet";
	case 28:
		return (const UTF8Char*)"Broadcast Address";
	case 29:
		return (const UTF8Char*)"Mask Discovery";
	case 30:
		return (const UTF8Char*)"Mask Supplier";
	case 31:
		return (const UTF8Char*)"Router Discovery";
	case 32:
		return (const UTF8Char*)"Router Request";
	case 33:
		return (const UTF8Char*)"Static Route";
	case 34:
		return (const UTF8Char*)"Trailers";
	case 35:
		return (const UTF8Char*)"ARP Timeout";
	case 36:
		return (const UTF8Char*)"Ethernet";
	case 37:
		return (const UTF8Char*)"Default TCP TTL";
	case 38:
		return (const UTF8Char*)"Keepalive Time";
	case 39:
		return (const UTF8Char*)"Keepalive Data";
	case 40:
		return (const UTF8Char*)"NIS Domain";
	case 41:
		return (const UTF8Char*)"NIS Servers";
	case 42:
		return (const UTF8Char*)"NTP Servers";
	case 43:
		return (const UTF8Char*)"Vendor Specific";
	case 44:
		return (const UTF8Char*)"NETBIOS Name Srv";
	case 45:
		return (const UTF8Char*)"NETBIOS Dist Srv";
	case 46:
		return (const UTF8Char*)"NETBIOS Node Type";
	case 47:
		return (const UTF8Char*)"NETBIOS Scope";
	case 48:
		return (const UTF8Char*)"X Window Font";
	case 49:
		return (const UTF8Char*)"X Window Manager";
	case 50:
		return (const UTF8Char*)"Requested IP Address";
	case 51:
		return (const UTF8Char*)"IP Address Least Time";
	case 52:
		return (const UTF8Char*)"Option overload";
	case 53:
		return (const UTF8Char*)"DHCP Message Type";
	case 54:
		return (const UTF8Char*)"DHCP Server";
	case 55:
		return (const UTF8Char*)"Parameter Request List";
	case 56:
		return (const UTF8Char*)"Message";
	case 57:
		return (const UTF8Char*)"Maximum DHCP message size";
	case 58:
		return (const UTF8Char*)"Renew Time Value";
	case 59:
		return (const UTF8Char*)"Rebinding Time Value";
	case 60:
		return (const UTF8Char*)"Vendor class identifier";
	case 61:
		return (const UTF8Char*)"Client ID";
	case 66:
		return (const UTF8Char*)"TFTP Server Name";
	case 67:
		return (const UTF8Char*)"Bootfile name";
	case 81:
		return (const UTF8Char*)"Client FQDN";
	case 82:
		return (const UTF8Char*)"Agent Information Option";
	case 90:
		return (const UTF8Char*)"Authentication";
	case 116:
		return (const UTF8Char*)"Auto-Config";
	case 120:
		return (const UTF8Char*)"SIP Servers";
	case 249:
		return (const UTF8Char*)"Microsoft Classless Static Route";
	case 255:
		return (const UTF8Char*)"End";
	}
	return 0;
}
