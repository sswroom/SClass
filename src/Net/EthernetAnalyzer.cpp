#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "Net/EthernetAnalyzer.h"
#include "Net/NTPServer.h"
#include "Sync/Interlocked.h"
#include "Text/StringBuilderUTF8.h"

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

void Net::EthernetAnalyzer::NetBIOSDecName(UnsafeArray<UTF8Char> nameBuff, UOSInt nameSize)
{
	UnsafeArray<UTF8Char> destPtr = nameBuff;
	while (nameSize >= 2)
	{
		*destPtr++ = (UTF8Char)(((nameBuff[0] - 'A') << 4) | (nameBuff[1] - 'A'));
		nameBuff += 2;
		nameSize -= 2;
	}
	*destPtr = 0;
}

NN<Net::EthernetAnalyzer::MACStatus> Net::EthernetAnalyzer::MACGet(UInt64 macAddr)
{
	NN<MACStatus> mac;
	if (this->macMap.Get(macAddr).SetTo(mac))
		return mac;
	mac = MemAllocNN(MACStatus);
	mac.ZeroContent();
	mac->macAddr = macAddr;
	this->macMap.Put(macAddr, mac);
	return mac;
}

void Net::EthernetAnalyzer::MDNSAdd(NN<Net::DNSClient::RequestAnswer> ans)
{
	Sync::MutexUsage mutUsage(this->mdnsMut);
	NN<Net::DNSClient::RequestAnswer> rans;
	OSInt i = 0;
	OSInt j = (OSInt)this->mdnsList.GetCount() - 1;
	OSInt k;
	OSInt l;
	while (i <= j)
	{
		k = (i + j) >> 1;
		rans = this->mdnsList.GetItemNoCheck((UOSInt)k);
		l = Text::StrCompare(rans->name->v, ans->name->v);
		if (l < 0)
		{
			i = k + 1;
		}
		else if (l > 0)
		{
			j = k - 1;
		}
		else if (rans->recType < ans->recType)
		{
			i = k + 1;
		}
		else if (rans->recType > ans->recType)
		{
			j = k - 1;
		}
		else
		{
			Net::DNSClient::FreeAnswer(ans);
			return;
		}
	}
	this->mdnsList.Insert((UOSInt)i, ans);
}

Net::EthernetAnalyzer::EthernetAnalyzer(Optional<IO::Writer> errWriter, AnalyzeType aType, NN<Text::String> name) : IO::ParsedObject(name), tcp4synList(128)
{
	this->atype = aType;
	this->packetCnt = 0;
	this->packetTotalSize = 0;
	this->errWriter = errWriter;
	this->isFirst = true;
	this->pingv4ReqHdlr = 0;
}

Net::EthernetAnalyzer::EthernetAnalyzer(Optional<IO::Writer> errWriter, AnalyzeType aType, Text::CStringNN name) : IO::ParsedObject(name), tcp4synList(128)
{
	this->atype = aType;
	this->packetCnt = 0;
	this->packetTotalSize = 0;
	this->errWriter = errWriter;
	this->isFirst = true;
	this->pingv4ReqHdlr = 0;
}

Net::EthernetAnalyzer::~EthernetAnalyzer()
{
	UOSInt i;
	UOSInt j;
	UnsafeArray<UInt8> packetData;
	NN<MACStatus> mac;
	i = this->macMap.GetCount();
	while (i-- > 0)
	{
		mac = this->macMap.GetItemNoCheck(i);
		OPTSTR_DEL(mac->name);
		j = 16;
		while (j-- > 0)
		{
			if (mac->packetData[j].SetTo(packetData))
				MemFreeArr(packetData);
		}
		MemFreeNN(mac);
	}

	NN<IPTranStatus> ipTran;
	i = this->ipTranMap.GetCount();
	while (i-- > 0)
	{
		ipTran = this->ipTranMap.GetItemNoCheck(i);
		MemFreeNN(ipTran);
	}

	NN<DNSClientInfo> dnsCli;
	i = this->dnsCliInfos.GetCount();
	while (i-- > 0)
	{
		dnsCli = this->dnsCliInfos.GetItemNoCheck(i);
		j = dnsCli->hourInfos.GetCount();
		while (j-- > 0)
		{
			MemFreeNN(dnsCli->hourInfos.GetItemNoCheck(j));
		}
		dnsCli.Delete();
	}

	NN<const Data::ArrayListNN<Net::EthernetAnalyzer::DNSRequestResult>> dnsReqList;
	NN<Net::EthernetAnalyzer::DNSRequestResult> req;
	dnsReqList = this->dnsReqv4Map.GetValues();
	i = dnsReqList->GetCount();
	while (i-- > 0)
	{
		req = dnsReqList->GetItemNoCheck(i);
		req.Delete();
	}

	dnsReqList = this->dnsReqv6Map.GetValues();
	i = dnsReqList->GetCount();
	while (i-- > 0)
	{
		req = dnsReqList->GetItemNoCheck(i);
		req.Delete();
	}

	dnsReqList = this->dnsReqOthMap.GetValues();
	i = dnsReqList->GetCount();
	while (i-- > 0)
	{
		req = dnsReqList->GetItemNoCheck(i);
		req.Delete();
	}
	
	NN<Net::EthernetAnalyzer::DNSTargetInfo> target;
	i = this->dnsTargetMap.GetCount();
	while (i-- > 0)
	{
		target = this->dnsTargetMap.GetItemNoCheck(i);
		target->addrList.FreeAll();
		target.Delete();
	}

	NN<Net::EthernetAnalyzer::IPLogInfo> ipLog;
	i = this->ipLogMap.GetCount();
	while (i-- > 0)
	{
		ipLog = this->ipLogMap.GetItemNoCheck(i);
		j = ipLog->logList.GetCount();
		while (j-- > 0)
		{
			OPTSTR_DEL(ipLog->logList.GetItem(j));
		}
		ipLog.Delete();
	}

	NN<DHCPInfo> dhcp;
	i = this->dhcpMap.GetCount();
	while (i-- > 0)
	{
		dhcp = this->dhcpMap.GetItemNoCheck(i);
		OPTSTR_DEL(dhcp->vendorClass);
		OPTSTR_DEL(dhcp->hostName);
		dhcp.Delete();
	}
	this->mdnsList.FreeAll(Net::DNSClient::FreeAnswer);
	this->bandwidthMap.MemFreeAll();
}

IO::ParserType Net::EthernetAnalyzer::GetParserType() const
{
	return IO::ParserType::EthernetAnalyzer;
}

UInt64 Net::EthernetAnalyzer::GetPacketCnt() const
{
	return this->packetCnt;
}

UInt64 Net::EthernetAnalyzer::GetPacketTotalSize() const
{
	return this->packetTotalSize;
}

void Net::EthernetAnalyzer::UseIPTran(NN<Sync::MutexUsage> mutUsage)
{
	mutUsage->ReplaceMutex(this->ipTranMut);
}

NN<const Data::ReadingListNN<Net::EthernetAnalyzer::IPTranStatus>> Net::EthernetAnalyzer::IPTranGetList() const
{
	return this->ipTranMap;
}

UOSInt Net::EthernetAnalyzer::IPTranGetCount() const
{
	return this->ipTranMap.GetCount();
}

void Net::EthernetAnalyzer::UseMAC(NN<Sync::MutexUsage> mutUsage)
{
	mutUsage->ReplaceMutex(this->macMut);
}

NN<const Data::ReadingListNN<Net::EthernetAnalyzer::MACStatus>> Net::EthernetAnalyzer::MACGetList() const
{
	return this->macMap;
}

void Net::EthernetAnalyzer::UseDNSCli(NN<Sync::MutexUsage> mutUsage)
{
	mutUsage->ReplaceMutex(this->dnsCliInfoMut);
}

NN<const Data::ReadingListNN<Net::EthernetAnalyzer::DNSClientInfo>> Net::EthernetAnalyzer::DNSCliGetList() const
{
	return this->dnsCliInfos;
}

UOSInt Net::EthernetAnalyzer::DNSCliGetCount()
{
	return this->dnsCliInfos.GetCount();
}

UOSInt Net::EthernetAnalyzer::DNSReqv4GetList(NN<Data::ArrayListNN<Text::String>> reqList)
{
	Sync::MutexUsage mutUsage(this->dnsReqv4Mut);
	return reqList->AddAllOpt(this->dnsReqv4Map.GetKeys());
}

UOSInt Net::EthernetAnalyzer::DNSReqv4GetCount()
{
	return this->dnsReqv4Map.GetCount();
}

Bool Net::EthernetAnalyzer::DNSReqv4GetInfo(Text::CStringNN req, NN<Data::ArrayListNN<Net::DNSClient::RequestAnswer>> ansList, NN<Data::DateTime> reqTime, OutParam<UInt32> ttl)
{
	NN<Net::EthernetAnalyzer::DNSRequestResult> result;
	Sync::MutexUsage mutUsage(this->dnsReqv4Mut);
	if (this->dnsReqv4Map.GetC(req).SetTo(result))
	{
		mutUsage.EndUse();
		Sync::MutexUsage mutUsage(result->mut);
		Net::DNSClient::ParseAnswers(result->recBuff, result->recSize, ansList);
		reqTime->SetInstant(result->reqTime.inst);
		ttl.Set(result->ttl);
		return true;
	}
	else
	{
		return false;
	}
}

UOSInt Net::EthernetAnalyzer::DNSReqv6GetList(NN<Data::ArrayListNN<Text::String>> reqList)
{
	Sync::MutexUsage mutUsage(this->dnsReqv6Mut);
	return reqList->AddAllOpt(this->dnsReqv6Map.GetKeys());
}

UOSInt Net::EthernetAnalyzer::DNSReqv6GetCount()
{
	return this->dnsReqv6Map.GetCount();
}

Bool Net::EthernetAnalyzer::DNSReqv6GetInfo(Text::CStringNN req, NN<Data::ArrayListNN<Net::DNSClient::RequestAnswer>> ansList, NN<Data::DateTime> reqTime, OutParam<UInt32> ttl)
{
	NN<Net::EthernetAnalyzer::DNSRequestResult> result;
	Sync::MutexUsage mutUsage(this->dnsReqv6Mut);
	if (this->dnsReqv6Map.GetC(req).SetTo(result))
	{
		mutUsage.EndUse();
		Sync::MutexUsage mutUsage(result->mut);
		Net::DNSClient::ParseAnswers(result->recBuff, result->recSize, ansList);
		reqTime->SetInstant(result->reqTime.inst);
		ttl.Set(result->ttl);
		return true;
	}
	else
	{
		return false;
	}
}

UOSInt Net::EthernetAnalyzer::DNSReqOthGetList(NN<Data::ArrayListNN<Text::String>> reqList)
{
	Sync::MutexUsage mutUsage(this->dnsReqOthMut);
	return reqList->AddAllOpt(this->dnsReqOthMap.GetKeys());
}

UOSInt Net::EthernetAnalyzer::DNSReqOthGetCount()
{
	return this->dnsReqOthMap.GetCount();
}

Bool Net::EthernetAnalyzer::DNSReqOthGetInfo(Text::CStringNN req, NN<Data::ArrayListNN<Net::DNSClient::RequestAnswer>> ansList, NN<Data::DateTime> reqTime, OutParam<UInt32> ttl)
{
	NN<Net::EthernetAnalyzer::DNSRequestResult> result;
	Sync::MutexUsage mutUsage(this->dnsReqOthMut);
	if (this->dnsReqOthMap.GetC(req).SetTo(result))
	{
		mutUsage.EndUse();
		Sync::MutexUsage mutUsage(result->mut);
		Net::DNSClient::ParseAnswers(result->recBuff, result->recSize, ansList);
		reqTime->SetInstant(result->reqTime.inst);
		ttl.Set(result->ttl);
		return true;
	}
	else
	{
		return false;
	}
}

UOSInt Net::EthernetAnalyzer::DNSTargetGetList(NN<Data::ArrayListNN<Net::EthernetAnalyzer::DNSTargetInfo>> targetList)
{
	Sync::MutexUsage mutUsage(this->dnsTargetMut);
	return targetList->AddAll(this->dnsTargetMap);
}

UOSInt Net::EthernetAnalyzer::DNSTargetGetCount()
{
	return this->dnsTargetMap.GetCount();
}

Optional<Text::String> Net::EthernetAnalyzer::DNSTargetGetName(UInt32 ip) const
{
	Sync::MutexUsage mutUsage(this->dnsTargetMut);
	NN<DNSTargetInfo> target;
#if IS_BYTEORDER_LE
	ip = BSWAPU32(ip);
#endif
	if (this->dnsTargetMap.Get(ip).SetTo(target))
	{
		return target->currName;
	}
	return 0;
}

UOSInt Net::EthernetAnalyzer::MDNSGetList(NN<Data::ArrayListNN<Net::DNSClient::RequestAnswer>> mdnsList)
{
	Sync::MutexUsage mutUsage(this->mdnsMut);
	return mdnsList->AddAll(this->mdnsList);
}

UOSInt Net::EthernetAnalyzer::MDNSGetCount()
{
	return this->mdnsList.GetCount();
}

void Net::EthernetAnalyzer::UseDHCP(NN<Sync::MutexUsage> mutUsage)
{
	mutUsage->ReplaceMutex(this->dhcpMut);
}

NN<const Data::ReadingListNN<Net::EthernetAnalyzer::DHCPInfo>> Net::EthernetAnalyzer::DHCPGetList() const
{
	return this->dhcpMap;
}

void Net::EthernetAnalyzer::UseIPLog(NN<Sync::MutexUsage> mutUsage)
{
	mutUsage->ReplaceMutex(this->ipLogMut);
}

NN<const Data::ReadingListNN<Net::EthernetAnalyzer::IPLogInfo>> Net::EthernetAnalyzer::IPLogGetList() const
{
	return this->ipLogMap;
}

UOSInt Net::EthernetAnalyzer::IPLogGetCount() const
{
	return this->ipLogMap.GetCount();
}

Bool Net::EthernetAnalyzer::TCP4SYNIsDiff(UOSInt lastIndex) const
{
	return this->tcp4synList.GetPutIndex() != lastIndex;
}

UOSInt Net::EthernetAnalyzer::TCP4SYNGetList(NN<Data::ArrayList<TCP4SYNInfo>> synList, OptOut<UOSInt> thisIndex) const
{
	Sync::MutexUsage mutUsage(this->tcp4synMut);
	thisIndex.Set(this->tcp4synList.GetPutIndex());
	return this->tcp4synList.GetItems(synList);
}

NN<Data::FastMapNN<UInt32, Net::EthernetAnalyzer::BandwidthStat>> Net::EthernetAnalyzer::BandwidthGetAll(NN<Sync::MutexUsage> mutUsage)
{
	mutUsage->ReplaceMutex(this->bandwidthMut);
	return this->bandwidthMap;
}

Bool Net::EthernetAnalyzer::PacketData(IO::PacketAnalyse::LinkType linkType, UnsafeArray<const UInt8> packet, UOSInt packetSize)
{
	switch (linkType)
	{
	case IO::PacketAnalyse::LinkType::Null:
		return PacketNull(packet, packetSize);
	case IO::PacketAnalyse::LinkType::Ethernet:
		return PacketEthernet(packet, packetSize);
	case IO::PacketAnalyse::LinkType::Linux:
		return PacketLinux(packet, packetSize);
	case IO::PacketAnalyse::LinkType::Bluetooth:
		break;
	}
	return false;
}

Bool Net::EthernetAnalyzer::PacketNull(UnsafeArray<const UInt8> packet, UOSInt packetSize)
{
	UInt32 packetType;
	Bool valid;
	if (packetSize < 4)
	{
		return false;
	}

	packetType = ReadMUInt32(&packet[0]);
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
		NN<IO::Writer> errWriter;
		if (this->isFirst && this->errWriter.SetTo(errWriter))
		{
			this->isFirst = false;
			Text::StringBuilderUTF8 sb;
			sb.AppendHexBuff(packet, packetSize, ' ', Text::LineBreakType::CRLF);
			errWriter->WriteLine(sb.ToCString());
		}
	}
	Sync::Interlocked::IncrementU64(this->packetCnt);
	Sync::Interlocked::AddU64(this->packetTotalSize, packetSize);
	return valid;
}

Bool Net::EthernetAnalyzer::PacketEthernet(UnsafeArray<const UInt8> packet, UOSInt packetSize)
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
		NN<IO::Writer> errWriter;
		if (this->isFirst && this->errWriter.SetTo(errWriter))
		{
			this->isFirst = false;
			Text::StringBuilderUTF8 sb;
			sb.AppendHexBuff(packet, packetSize, ' ', Text::LineBreakType::CRLF);
			errWriter->WriteLine(sb.ToCString());
		}
	}
	Sync::Interlocked::IncrementU64(this->packetCnt);
	Sync::Interlocked::AddU64(this->packetTotalSize, packetSize);
	return valid;
}

Bool Net::EthernetAnalyzer::PacketLinux(UnsafeArray<const UInt8> packet, UOSInt packetSize)
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
		NN<IO::Writer> errWriter;
		if (this->isFirst && this->errWriter.SetTo(errWriter))
		{
			this->isFirst = false;
			Text::StringBuilderUTF8 sb;
			sb.AppendHexBuff(packet, packetSize, ' ', Text::LineBreakType::CRLF);
			errWriter->WriteLine(sb.ToCString());
		}
	}
	Sync::Interlocked::IncrementU64(this->packetCnt);
	Sync::Interlocked::AddU64(this->packetTotalSize, packetSize);
	return valid;
}

Bool Net::EthernetAnalyzer::PacketEthernetData(UnsafeArray<const UInt8> packet, UOSInt packetSize, UInt16 etherType, UInt64 srcMAC, UInt64 destMAC)
{
	NN<MACStatus> mac;
	Bool valid = true;
	Data::DateTime dt;
	dt.SetCurrTimeUTC();
	if (etherType == packetSize)
	{
		return true;
	}
	Sync::MutexUsage mutUsage(this->macMut);
	UnsafeArray<UInt8> packetData;
	UOSInt cnt;
	mac = this->MACGet(srcMAC);
	cnt = (UOSInt)(mac->ipv4SrcCnt + mac->ipv6SrcCnt + mac->othSrcCnt);
	mac->packetTime[cnt & 15] = dt.ToInstant();
	mac->packetSize[cnt & 15] = packetSize;
	mac->packetDestMAC[cnt & 15] = destMAC;
	mac->packetEtherType[cnt & 15] = etherType;
	if (mac->packetData[cnt & 15].SetTo(packetData))
	{
		MemFreeArr(packetData);
	}
	mac->packetData[cnt & 15] = packetData = MemAllocArr(UInt8, packetSize);
	MemCopyNO(packetData.Ptr(), packet.Ptr(), packetSize);
	mutUsage.EndUse();

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

Bool Net::EthernetAnalyzer::PacketIPv4(UnsafeArray<const UInt8> packet, UOSInt packetSize, UInt64 srcMAC, UInt64 destMAC)
{
//	Net::SocketUtil::AddressInfo addr;
//	UInt16 port;
//	UInt16 etherType;
//	Int64 destAddr;
//	Int64 srcAddr;
	UInt32 ipAddr;
//	UInt8 tmpBuff[8];
	UTF8Char sbuff[32];
	UnsafeArray<UTF8Char> sptr;
	NN<MACStatus> mac;
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

		{
			UInt32 sortIP;
			Int64 currTime = Data::DateTimeUtil::GetCurrTimeMillis() / 1000;
			NN<BandwidthStat> stat;
			Sync::MutexUsage mutUsage(this->bandwidthMut);
			sortIP = ReadMUInt32(&packet[12]);
			if (!this->bandwidthMap.Get(sortIP).SetTo(stat))
			{
				stat = BandwidthStatCreate(sortIP, currTime);
				this->bandwidthMap.Put(sortIP, stat);
			}
			if (stat->currStat.time != currTime)
			{
				stat->lastStat = stat->currStat;
				stat->currStat.time = currTime;
				stat->currStat.recvBytes = 0;
				stat->currStat.recvCnt = 0;
				stat->currStat.sendBytes = 0;
				stat->currStat.sendCnt = 0;
			}
			stat->currStat.recvBytes += packetSize + 14;
			stat->currStat.recvCnt++;

			sortIP = ReadMUInt32(&packet[16]);
			if (!this->bandwidthMap.Get(sortIP).SetTo(stat))
			{
				stat = BandwidthStatCreate(sortIP, currTime);
				this->bandwidthMap.Put(sortIP, stat);
			}
			if (stat->currStat.time != currTime)
			{
				stat->lastStat = stat->currStat;
				stat->currStat.time = currTime;
				stat->currStat.recvBytes = 0;
				stat->currStat.recvCnt = 0;
				stat->currStat.sendBytes = 0;
				stat->currStat.sendCnt = 0;
			}
			stat->currStat.sendBytes += packetSize + 14;
			stat->currStat.sendCnt++;
		}

		NN<IPTranStatus> ip;
		Int64 addr = ReadMInt64(&packet[12]);
		Sync::MutexUsage ipTranMutUsage(this->ipTranMut);
		if (!this->ipTranMap.Get(addr).SetTo(ip))
		{
			ip = MemAllocNN(IPTranStatus);
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
			this->ipTranMap.Put(addr, ip);
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
				NN<IPLogInfo> ipLog;
				Optional<IPLogInfo> optipLog = 0;
				Data::Timestamp ts = Data::Timestamp::Now();
				UInt32 sortableIP = ReadMUInt32(&packet[12]);
				Sync::MutexUsage mutUsage(this->ipLogMut);
				if (!this->ipLogMap.Get(sortableIP).SetTo(ipLog))
				{
					Net::IPType itype = Net::SocketUtil::GetIPv4Type(ip->srcIP);
					if (itype == Net::IPType::Local || itype == Net::IPType::Private)
					{
						NEW_CLASSNN(ipLog, IPLogInfo());
						ipLog->ip = ip->srcIP;
						this->ipLogMap.Put(sortableIP, ipLog);
						optipLog = ipLog;
					}
				}
				else
				{
					optipLog = ipLog;
				}
				if (optipLog.SetTo(ipLog))
				{
					sb.ClearStr();
					sb.AppendTSNoZone(ts);
					sb.AppendC(UTF8STRC(" ICMP "));
					switch (ipData[0])
					{
					case 0:
						sb.AppendC(UTF8STRC("Ping Reply to "));
						break;
					case 3:
						sb.AppendC(UTF8STRC("Destination unreachable ("));
						switch (ipData[1])
						{
						case 0:
							sb.AppendC(UTF8STRC("Network unreachable error"));
							break;
						case 1:
							sb.AppendC(UTF8STRC("Protocol unreachable error"));
							break;
						case 2:
							sb.AppendC(UTF8STRC("Host unreachable error"));
							break;
						case 3:
							sb.AppendC(UTF8STRC("Port unreachable error"));
							break;
						case 4:
							sb.AppendC(UTF8STRC("The datagram is too big"));
							break;
						case 5:
							sb.AppendC(UTF8STRC("Source route failed error"));
							break;
						case 6:
							sb.AppendC(UTF8STRC("Destination network unknown error"));
							break;
						case 7:
							sb.AppendC(UTF8STRC("Destination host unknown error"));
							break;
						case 8:
							sb.AppendC(UTF8STRC("Source host isolated error"));
							break;
						case 9:
							sb.AppendC(UTF8STRC("The destination network is administratively prohibited"));
							break;
						case 10:
							sb.AppendC(UTF8STRC("The destination host is administratively prohibited"));
							break;
						case 11:
							sb.AppendC(UTF8STRC("The network is unreachable for Type Of Service"));
							break;
						case 12:
							sb.AppendC(UTF8STRC("The host is unreachable for Type Of Service"));
							break;
						case 13:
							sb.AppendC(UTF8STRC("Communication Administratively Prohibited"));
							break;
						case 14:
							sb.AppendC(UTF8STRC("Host precedence violation"));
							break;
						case 15:
							sb.AppendC(UTF8STRC("Precedence cutoff in effect"));
							break;
						default:
							sb.AppendU16(ipData[1]);
							break;
						}
						sb.AppendC(UTF8STRC(") to "));
						break;
					case 8:
						sb.AppendC(UTF8STRC("Ping Request to "));
						break;
					case 11:
						sb.AppendC(UTF8STRC("Time Exceeded to "));
						break;
					default:
						sb.AppendC(UTF8STRC("Unknown ("));
						sb.AppendU16(ipData[0]);
						sb.AppendC(UTF8STRC(") to "));
						break;
					}
					sptr = Net::SocketUtil::GetIPv4Name(sbuff, ip->destIP);
					sb.AppendC(sbuff, (UOSInt)(sptr - sbuff));
					sb.AppendC(UTF8STRC(", ttl = "));
					sb.AppendU16(packet[8]);
					sb.AppendC(UTF8STRC(", size = "));
					sb.AppendUOSInt(ipDataSize);
					Sync::MutexUsage mutUsage(ipLog->mut);
					while (ipLog->logList.GetCount() >= IPLOGCNT)
					{
						OPTSTR_DEL(ipLog->logList.RemoveAt(0));
					}
					ipLog->logList.Add(Text::String::New(sb.ToCString()));
					mutUsage.EndUse();
				}

				sortableIP = ReadMUInt32(&packet[16]);
				optipLog = 0;
				if (!this->ipLogMap.Get(sortableIP).SetTo(ipLog))
				{
					Net::IPType itype = Net::SocketUtil::GetIPv4Type(ip->destIP);
					if (itype == Net::IPType::Local || itype == Net::IPType::Private)
					{
						NEW_CLASSNN(ipLog, IPLogInfo());
						ipLog->ip = ip->destIP;
						this->ipLogMap.Put(sortableIP, ipLog);
						optipLog = ipLog;
					}
				}
				else
				{
					optipLog = ipLog;
				}
				if (optipLog.SetTo(ipLog))
				{
					sb.ClearStr();
					sb.AppendTSNoZone(ts);
					sb.AppendC(UTF8STRC(" ICMP "));
					switch (ipData[0])
					{
					case 0:
						sb.AppendC(UTF8STRC("Ping Reply from "));
						break;
					case 3:
						sb.AppendC(UTF8STRC("Destination unreachable ("));
						switch (ipData[1])
						{
						case 0:
							sb.AppendC(UTF8STRC("Network unreachable error"));
							break;
						case 1:
							sb.AppendC(UTF8STRC("Protocol unreachable error"));
							break;
						case 2:
							sb.AppendC(UTF8STRC("Host unreachable error"));
							break;
						case 3:
							sb.AppendC(UTF8STRC("Port unreachable error"));
							break;
						case 4:
							sb.AppendC(UTF8STRC("The datagram is too big"));
							break;
						case 5:
							sb.AppendC(UTF8STRC("Source route failed error"));
							break;
						case 6:
							sb.AppendC(UTF8STRC("Destination network unknown error"));
							break;
						case 7:
							sb.AppendC(UTF8STRC("Destination host unknown error"));
							break;
						case 8:
							sb.AppendC(UTF8STRC("Source host isolated error"));
							break;
						case 9:
							sb.AppendC(UTF8STRC("The destination network is administratively prohibited"));
							break;
						case 10:
							sb.AppendC(UTF8STRC("The destination host is administratively prohibited"));
							break;
						case 11:
							sb.AppendC(UTF8STRC("The network is unreachable for Type Of Service"));
							break;
						case 12:
							sb.AppendC(UTF8STRC("The host is unreachable for Type Of Service"));
							break;
						case 13:
							sb.AppendC(UTF8STRC("Communication Administratively Prohibited"));
							break;
						case 14:
							sb.AppendC(UTF8STRC("Host precedence violation"));
							break;
						case 15:
							sb.AppendC(UTF8STRC("Precedence cutoff in effect"));
							break;
						default:
							sb.AppendU16(ipData[1]);
							break;
						}
						sb.AppendC(UTF8STRC(") from "));
						break;
					case 8:
						sb.AppendC(UTF8STRC("Ping Request from "));
						break;
					case 11:
						sb.AppendC(UTF8STRC("Time Exceeded from "));
						break;
					default:
						sb.AppendC(UTF8STRC("Unknown ("));
						sb.AppendU16(ipData[0]);
						sb.AppendC(UTF8STRC(") from "));
						break;
					}
					sptr = Net::SocketUtil::GetIPv4Name(sbuff, ip->srcIP);
					sb.AppendC(sbuff, (UOSInt)(sptr - sbuff));
					sb.AppendC(UTF8STRC(", ttl = "));
					sb.AppendU16(packet[8]);
					sb.AppendC(UTF8STRC(", size = "));
					sb.AppendUOSInt(ipDataSize);
					Sync::MutexUsage mutUsage(ipLog->mut);
					while (ipLog->logList.GetCount() >= IPLOGCNT)
					{
						OPTSTR_DEL(ipLog->logList.RemoveAt(0));
					}
					ipLog->logList.Add(Text::String::New(sb.ToString(), sb.GetLength()));
					mutUsage.EndUse();
				}
				mutUsage.EndUse();

				if (ipData[0] == 8)
				{
					if (this->pingv4ReqHdlr.func)
					{
						this->pingv4ReqHdlr.func(this->pingv4ReqHdlr.userObj, ip->srcIP, ip->destIP, packet[8], ipDataSize);
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
			if (ipDataSize > 13 && ipData[13] == 2)
			{
				TCP4SYNInfo syn;
				syn.srcAddr = ReadNUInt32(&packet[12]);
				syn.destAddr = ReadNUInt32(&packet[16]);
				syn.srcPort = ReadMUInt16(&ipData[0]);
				syn.destPort = ReadMUInt16(&ipData[2]);
				syn.reqTime = Data::Timestamp::UtcNow();
				Sync::MutexUsage mutUsage(this->tcp4synMut);
				this->tcp4synList.PutAndLoop(syn);
			}
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
							Data::ArrayListNN<Net::DNSClient::RequestAnswer> answers;
							NN<Net::DNSClient::RequestAnswer> answer;
							Net::DNSClient::ParseAnswers(&ipData[8], ipDataSize - 8, answers);
							if (answers.GetCount() > 0)
							{
								NN<DNSRequestResult> req;
								NN<Text::String> reqName;
								Data::Timestamp currTime = Data::Timestamp::UtcNow();
								reqName = answers.GetItemNoCheck(0)->name;
								answer = answers.GetItemNoCheck(answers.GetCount() - 1);
								if (answer->recType == 1)
								{
									if (this->atype & AT_DNSREQ)
									{
										Sync::MutexUsage mutUsage(this->dnsReqv4Mut);
										if (this->dnsReqv4Map.GetNN(reqName).SetTo(req))
										{
											Sync::MutexUsage mutUsage(req->mut);
											req->status = 0;
											req->recSize = ipDataSize - 8;
											MemCopyNO(req->recBuff, &ipData[8], ipDataSize - 8);
											req->reqTime = currTime;
											req->ttl = Net::DNSClient::GetResponseTTL(req->recBuff, req->recSize);
										}
										else
										{
											NEW_CLASSNN(req, DNSRequestResult());
											req->status = 0;
											req->recSize = ipDataSize - 8;
											MemCopyNO(req->recBuff, &ipData[8], ipDataSize - 8);
											req->reqTime = currTime;
											req->ttl = Net::DNSClient::GetResponseTTL(req->recBuff, req->recSize);
											this->dnsReqv4Map.PutNN(reqName, req);
										}
									}
									if (this->atype & AT_DNSTARGET)
									{
										UOSInt i;
										UOSInt j;
										UInt32 resIP;
										UInt32 sortIP;
										NN<DNSTargetInfo> dnsTarget;
										i = answers.GetCount();
										while (i-- > 0)
										{
											answer = answers.GetItemNoCheck(i);
											if (answer->recType == 1)
											{
												resIP = ReadNUInt32(answer->addr.addr);
												sortIP = ReadMUInt32(answer->addr.addr);
												Sync::MutexUsage dnsTargetMutUsage(this->dnsTargetMut);
												if (!this->dnsTargetMap.Get(sortIP).SetTo(dnsTarget))
												{
													NEW_CLASSNN(dnsTarget, DNSTargetInfo());
													dnsTarget->ip = resIP;
													this->dnsTargetMap.Put(sortIP, dnsTarget);
												}
												Sync::MutexUsage mutUsage(dnsTarget->mut);
												dnsTargetMutUsage.EndUse();
												OSInt si;
												if ((si = dnsTarget->addrList.SortedIndexOf(answer->name)) < 0)
												{
													dnsTarget->addrList.SortedInsert(dnsTarget->currName = answer->name->Clone());
												}
												else
												{
													dnsTarget->currName = dnsTarget->addrList.GetItemNoCheck((UOSInt)si);
												}
												j = i;
												while (j-- > 0)
												{
													answer = answers.GetItemNoCheck(j);
													if (answer->recType == 5)
													{
														if ((si = dnsTarget->addrList.SortedIndexOf(answer->name)) < 0)
														{
															dnsTarget->addrList.SortedInsert(dnsTarget->currName = answer->name->Clone());
														}
														else
														{
															dnsTarget->currName = dnsTarget->addrList.GetItemNoCheck((UOSInt)si);
														}
													}
												}
											}
										}													
									}
								}
								else if (answer->recType == 28)
								{
									if (this->atype & AT_DNSREQ)
									{
										Sync::MutexUsage mutUsage(this->dnsReqv6Mut);
										if (this->dnsReqv6Map.GetNN(reqName).SetTo(req))
										{
											Sync::MutexUsage mutUsage(req->mut);
											req->status = 0;
											req->recSize = ipDataSize - 8;
											MemCopyNO(req->recBuff, &ipData[8], ipDataSize - 8);
											req->reqTime = currTime;
											req->ttl = Net::DNSClient::GetResponseTTL(req->recBuff, req->recSize);
										}
										else
										{
											NEW_CLASSNN(req, DNSRequestResult());
											req->status = 0;
											req->recSize = ipDataSize - 8;
											MemCopyNO(req->recBuff, &ipData[8], ipDataSize - 8);
											req->reqTime = currTime;
											req->ttl = Net::DNSClient::GetResponseTTL(req->recBuff, req->recSize);
											this->dnsReqv6Map.PutNN(reqName, req);
										}
									}
								}
								else
								{
									if (this->atype & AT_DNSREQ)
									{
										Sync::MutexUsage mutUsage(this->dnsReqOthMut);
										if (this->dnsReqOthMap.GetNN(reqName).SetTo(req))
										{
											Sync::MutexUsage mutUsage(req->mut);
											req->status = 0;
											req->recSize = ipDataSize - 8;
											MemCopyNO(req->recBuff, &ipData[8], ipDataSize - 8);
											req->reqTime = currTime;
											req->ttl = Net::DNSClient::GetResponseTTL(req->recBuff, req->recSize);
										}
										else
										{
											NEW_CLASSNN(req, DNSRequestResult());
											req->status = 0;
											req->recSize = ipDataSize - 8;
											MemCopyNO(req->recBuff, &ipData[8], ipDataSize - 8);
											req->reqTime = currTime;
											req->ttl = Net::DNSClient::GetResponseTTL(req->recBuff, req->recSize);
											this->dnsReqOthMap.PutNN(reqName, req);
										}
									}
								}
								Net::DNSClient::FreeAnswers(answers);
							}
						}
						valid = true;
					}
					else if (destPort == 53) //DNS Request
					{
						if (this->atype & AT_DNSCLI)
						{
							NN<DNSClientInfo> dnsCli;
							UInt32 cliId = ReadMUInt32(&packet[12]);
							Sync::MutexUsage dnsCliInfoMutUsage(this->dnsCliInfoMut);
							if (!this->dnsCliInfos.Get(cliId).SetTo(dnsCli))
							{
								NEW_CLASSNN(dnsCli, DNSClientInfo());
								dnsCli->cliId = cliId;
								dnsCli->addr.addrType = Net::AddrType::IPv4;
								WriteMUInt32(dnsCli->addr.addr, cliId);
								this->dnsCliInfos.Put(cliId, dnsCli);
							}
							dnsCliInfoMutUsage.EndUse();
							Data::DateTime dt;
							NN<DNSCliHourInfo> hInfo;
							dt.SetCurrTimeUTC();
							Sync::MutexUsage mutUsage(dnsCli->mut);
							if (dnsCli->hourInfos.GetItem(0).SetTo(hInfo) && hInfo->year == dt.GetYear() && hInfo->month == dt.GetMonth() && hInfo->day == dt.GetDay() && hInfo->hour == dt.GetHour())
							{
								hInfo->reqCount++;
							}
							else
							{
								if (dnsCli->hourInfos.GetCount() >= 72 && dnsCli->hourInfos.RemoveAt(71).SetTo(hInfo))
								{
								}
								else
								{
									hInfo = MemAllocNN(DNSCliHourInfo);
								}
								hInfo->year = dt.GetYear();
								hInfo->month = dt.GetMonth();
								hInfo->day = dt.GetDay();
								hInfo->hour = dt.GetHour();
								hInfo->reqCount = 1;
								dnsCli->hourInfos.Insert(0, hInfo);
							}
							mutUsage.EndUse();
						}
						valid = true;
					}
					else if (srcPort == 67 || destPort == 67) //DHCP Request/Reply
					{
						UInt8 macBuff[8];
						UInt64 iMAC;
						NN<DHCPInfo> dhcp;
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
							if (!this->dhcpMap.Get(iMAC).SetTo(dhcp))
							{
								NEW_CLASSNN(dhcp, DHCPInfo());
								dhcp->iMAC = iMAC;
								dhcp->updated = false;
								dhcp->ipAddrTime = 0;
								dhcp->ipAddr = 0;
								dhcp->ipAddrLease = 0;
								dhcp->subnetMask = 0;
								dhcp->gwAddr = 0;
								dhcp->dhcpServer = 0;
								dhcp->renewTime = 0;
								dhcp->rebindTime = 0;
								dhcp->router = 0;
								dhcp->hostName = 0;
								dhcp->vendorClass = 0;
								this->dhcpMap.Put(iMAC, dhcp);
							}
							dhcpMutUsage.EndUse();
							const UInt8 *currPtr = &ipData[248];
							const UInt8 *endPtr = &ipData[ipDataSize];
							UInt8 msgType = 0;
							UInt8 t;
							UInt8 len;
							NN<Text::String> s;
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
									if (dhcp->hostName.IsNull())
									{
										dhcp->hostName = s = Text::String::New(len);
										MemCopyNO(s->v.Ptr(), currPtr, len);
										s->v[len] = 0;
									}
								}
								else if (t == 60 && len >= 1 && msgType == 1)
								{
									if (dhcp->vendorClass.IsNull())
									{
										dhcp->vendorClass = s = Text::String::New(len);
										MemCopyNO(s->v.Ptr(), currPtr, len);
										s->v[len] = 0;
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
									dhcp->ipAddrTime = Data::Timestamp::UtcNow();
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
									NN<IPLogInfo> ipLog;
									UInt32 sortableIP = ReadMUInt32(&packet[12]);
									Sync::MutexUsage ipLogMutUsage(this->ipLogMut);
									if (!this->ipLogMap.Get(sortableIP).SetTo(ipLog))
									{
										NEW_CLASSNN(ipLog, IPLogInfo());
										ipLog->ip = ip->srcIP;
										this->ipLogMap.Put(sortableIP, ipLog);
									}
									ipLogMutUsage.EndUse();
									sb.AppendTSNoZone(Data::Timestamp::Now());
									sb.AppendC(UTF8STRC(" UDP Port "));
									sb.AppendU16(srcPort);
									sb.AppendC(UTF8STRC(" NTP request to "));
									sptr = Net::SocketUtil::GetIPv4Name(sbuff, ip->destIP);
									sb.AppendC(sbuff, (UOSInt)(sptr - sbuff));
									Sync::MutexUsage mutUsage(ipLog->mut);
									while (ipLog->logList.GetCount() >= IPLOGCNT)
									{
										OPTSTR_DEL(ipLog->logList.RemoveAt(0));
									}
									ipLog->logList.Add(Text::String::New(sb.ToString(), sb.GetLength()));
									mutUsage.EndUse();
								}
							}
							else
							{
								if ((ipData[8] & 7) == 4) //server
								{
									Text::StringBuilderUTF8 sb;
									NN<IPLogInfo> ipLog;
									UInt32 sortableIP = ReadMUInt32(&packet[16]);
									Sync::MutexUsage ipLogMutUsage(this->ipLogMut);
									if (!this->ipLogMap.Get(sortableIP).SetTo(ipLog))
									{
										NEW_CLASSNN(ipLog, IPLogInfo());
										ipLog->ip = ip->destIP;
										this->ipLogMap.Put(sortableIP, ipLog);
									}
									ipLogMutUsage.EndUse();
									sb.AppendTSNoZone(Data::Timestamp::Now());
									sb.AppendC(UTF8STRC(" UDP Port "));
									sb.AppendU16(destPort);
									sb.AppendC(UTF8STRC(" NTP reply from "));
									sptr = Net::SocketUtil::GetIPv4Name(sbuff, ip->srcIP);
									sb.AppendC(sbuff, (UOSInt)(sptr - sbuff));
									sb.AppendC(UTF8STRC(", time = "));
									sptr = Net::NTPServer::ReadTime(&ipData[40]).ToStringNoZone(sbuff);
									sb.AppendC(sbuff, (UOSInt)(sptr - sbuff));
									Sync::MutexUsage mutUsage(ipLog->mut);
									while (ipLog->logList.GetCount() >= IPLOGCNT)
									{
										OPTSTR_DEL(ipLog->logList.RemoveAt(0));
									}
									ipLog->logList.Add(Text::String::New(sb.ToString(), sb.GetLength()));
									mutUsage.EndUse();
								}
							}
						}
						valid = true;
					}
					else if (srcPort == 137 || destPort == 137) // NetBIOS-NS (RFC 1002)
					{
						//UInt16 name_trn_id = ReadMUInt16(&ipData[8]);
//									Bool isResponse = (ipData[10] & 0x80) != 0;
						UInt8 opcode = (ipData[10] & 0x78) >> 3;
						if (opcode == 0) //query
						{
							UInt16 nQuestion = ReadMUInt16(&ipData[12]);
							UInt16 nAnswer = ReadMUInt16(&ipData[14]);
							if (nQuestion == 0 && nAnswer == 1 && ipDataSize >= 64)
							{
								UInt16 questType = ReadMUInt16(&ipData[54]);
								UInt16 leng = ReadMUInt16(&ipData[62]);
								if (ipDataSize >= 64 + (UOSInt)leng)
								{
									if (questType == 33) //NBSTAT
									{
										UInt8 nName = ipData[64];
										if (nName * 18 + 1 <= leng)
										{
											UOSInt i = 0;
											while (i < nName)
											{
												UInt16 flags = ReadMUInt16(&ipData[65 + i * 18 + 16]);
												if ((flags & 0x8000) == 0 && ipData[65 + i * 18 + 15] == 0)
												{
													Sync::MutexUsage mutUsage(this->macMut);
													mac = this->MACGet(srcMAC);
													if (mac->name.IsNull())
													{
														MemCopyNO(sbuff, &ipData[65 + i * 18], 15);
														sbuff[15] = 0;
														sptr = Text::StrRTrim(sbuff);
														mac->name = Text::String::New(sbuff, (UOSInt)(sptr - sbuff));
													}
													mutUsage.EndUse();
													break;
												}
												i++;
											}
										}
									}
								}
							}
							valid = true;
						}
						else if (opcode == 5)
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
					else if (srcPort == 138 || destPort == 138) // NetBIOS-DGM
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
									if (mac->name.IsNull())
									{
										MemCopyNO(sbuff, &ipData[23], 32);
										NetBIOSDecName(sbuff, 32);
										sptr = Text::StrRTrim(sbuff);
										mac->name = Text::String::New(sbuff, (UOSInt)(sptr - sbuff));
/*										NetBIOSDecName(&ipData[57], 32);
										console->Write(&ipData[23]);
										console->Write(CSTR(", ");
										console->Write(&ipData[57]);
										console->Write(CSTR(", ");
										Net::SocketUtil::GetIPv4Name(&ipData[23], ip->srcIP);
										console->Write(&ipData[23]);
										console->Write(CSTR(", ");
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
							NN<IPLogInfo> ipLog;
							UInt32 sortableIP = ReadMUInt32(&packet[12]);
							Sync::MutexUsage ipLogMutUsage(this->ipLogMut);
							if (!this->ipLogMap.Get(sortableIP).SetTo(ipLog))
							{
								NEW_CLASSNN(ipLog, IPLogInfo());
								ipLog->ip = ip->srcIP;
								this->ipLogMap.Put(sortableIP, ipLog);
							}
							ipLogMutUsage.EndUse();
							sb.AppendTSNoZone(Data::Timestamp::Now());
							sb.AppendC(UTF8STRC(" UDP Port "));
							sb.AppendU16(srcPort);
							sb.AppendC(UTF8STRC(" SSDP to "));
							sptr = Net::SocketUtil::GetIPv4Name(sbuff, ip->destIP);
							sb.AppendC(sbuff, (UOSInt)(sptr - sbuff));
//							ipData[udpLeng] = 0;
							UOSInt i = Text::StrIndexOf(&ipData[8], (const UTF8Char*)" * ");
							if (i != INVALID_INDEX && i > 0)
							{
								sb.AppendC(UTF8STRC(", method = "));
								sb.AppendC(&ipData[8], i);
							}
							Sync::MutexUsage mutUsage(ipLog->mut);
							while (ipLog->logList.GetCount() >= IPLOGCNT)
							{
								OPTSTR_DEL(ipLog->logList.RemoveAt(0));
							}
							ipLog->logList.Add(Text::String::New(sb.ToString(), sb.GetLength()));
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
						UInt16 flags = ReadMUInt16(&ipData[10]);
						if ((flags & 0xf800) == 0x8000) //Response query
						{
							UInt16 nAns = ReadMUInt16(&ipData[14]);
							UOSInt i = 12;
							UOSInt j = 0;
							NN<Net::DNSClient::RequestAnswer> ans;
							while (j < nAns)
							{
								ans = Net::DNSClient::ParseAnswer(&ipData[8], ipDataSize - 8, i);
								this->MDNSAdd(ans);
								j++;
							}
						}
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
							if (mac->name.IsNull())
							{
								sptr = Text::StrConcatC(sbuff, &ipData[21], ipData[20]);
								mac->name = Text::String::New(sbuff, (UOSInt)(sptr - sbuff));
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

Bool Net::EthernetAnalyzer::PacketIPv6(UnsafeArray<const UInt8> packet, UOSInt packetSize, UInt64 srcMAC, UInt64 destMAC)
{
	NN<MACStatus> mac;
	Bool valid = true;
	if ((packet[0] & 0xf0) == 0x60 && packetSize >= 40)
	{
		if (this->atype & AT_DEVICE)
		{
			Sync::MutexUsage mutUsage(this->macMut);
			mac = this->MACGet(srcMAC);
			if (mac->ipv6Addr.addrType == Net::AddrType::Unknown)
			{
				mac->ipv6Addr.addrType = Net::AddrType::IPv6;
				MemCopyNO(mac->ipv6Addr.addr, &packet[8], 16);
			}
			mac->ipv6SrcCnt++;

			mac = this->MACGet(destMAC);
			if (mac->ipv6Addr.addrType == Net::AddrType::Unknown)
			{
				mac->ipv6Addr.addrType = Net::AddrType::IPv6;
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

Bool Net::EthernetAnalyzer::PacketARP(UnsafeArray<const UInt8> packet, UOSInt packetSize, UInt64 srcMAC, UInt64 destMAC)
{
	Bool valid = true;
	NN<MACStatus> mac;
	UOSInt i;
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
				UInt32 ipAddr = ReadNUInt32(&packet[14]);

				Sync::MutexUsage mutUsage(this->macMut);
				mac = this->MACGet(srcMAC);
				mac->othSrcCnt++;
				i = 0;
				while (i < 4)
				{
					if (mac->ipv4Addr[i] == ipAddr)
					{
						mac->ipv4ByARP = true;
						if (i != 0)
						{
							mac->ipv4Addr[i] = mac->ipv4Addr[0];
							mac->ipv4Addr[0] = ipAddr;
						}
						break;
					}
					else if (mac->ipv4Addr[i] == 0)
					{
						mac->ipv4ByARP = true;
						if (i == 0)
						{
							mac->ipv4Addr[i] = ipAddr;
						}
						else
						{
							mac->ipv4Addr[i] = mac->ipv4Addr[0];
							mac->ipv4Addr[0] = ipAddr;
						}
						break;
					}
					i++;
				}
				if (!mac->ipv4ByARP)
				{
					mac->ipv4Addr[0] = ipAddr;
					mac->ipv4ByARP = true;
				}

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

void Net::EthernetAnalyzer::HandlePingv4Request(Pingv4Handler pingv4Hdlr, AnyType userObj)
{
	this->pingv4ReqHdlr = {pingv4Hdlr, userObj};
}

NN<Net::EthernetAnalyzer::BandwidthStat> Net::EthernetAnalyzer::BandwidthStatCreate(UInt32 sortIP, Int64 time)
{
	NN<BandwidthStat> stat;
	stat = MemAllocNN(BandwidthStat);
	stat->ip = sortIP;
	stat->displayFlags = 0;
	stat->displayTime = 0;
	stat->lastStat.time = time - 1;
	stat->lastStat.recvBytes = 0;
	stat->lastStat.recvCnt = 0;
	stat->lastStat.sendBytes = 0;
	stat->lastStat.sendCnt = 0;
	stat->currStat.time = time;
	stat->currStat.recvBytes = 0;
	stat->currStat.recvCnt = 0;
	stat->currStat.sendBytes = 0;
	stat->currStat.sendCnt = 0;
	return stat;
}
