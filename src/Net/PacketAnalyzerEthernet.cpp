#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "Net/DNSClient.h"
#include "Net/MACInfo.h"
#include "Net/NetBIOSUtil.h"
#include "Net/NTPServer.h"
#include "Net/PacketAnalyzerEthernet.h"
#include "Net/SNMPInfo.h"
#include "Net/SocketUtil.h"
#include "Text/JSON.h"
#include "Text/JSText.h"
#include "Text/HTMLUtil.h"
#include "Text/MyStringFloat.h"
#include "Text/TextBinEnc/Base64Enc.h"

Bool Net::PacketAnalyzerEthernet::PacketNullGetName(const UInt8 *packet, UOSInt packetSize, Text::StringBuilderUTF *sb)
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

Bool Net::PacketAnalyzerEthernet::PacketEthernetGetName(const UInt8 *packet, UOSInt packetSize, Text::StringBuilderUTF *sb)
{
	return PacketEthernetDataGetName(ReadMUInt16(&packet[12]), &packet[14], packetSize - 14, sb);
}

Bool Net::PacketAnalyzerEthernet::PacketLinuxGetName(const UInt8 *packet, UOSInt packetSize, Text::StringBuilderUTF *sb)
{
	return PacketEthernetDataGetName(ReadMUInt16(&packet[14]), &packet[16], packetSize - 16, sb);
}

Bool Net::PacketAnalyzerEthernet::PacketEthernetDataGetName(UInt16 etherType, const UInt8 *packet, UOSInt packetSize, Text::StringBuilderUTF *sb)
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

Bool Net::PacketAnalyzerEthernet::PacketIPv4GetName(const UInt8 *packet, UOSInt packetSize, Text::StringBuilderUTF *sb)
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

Bool Net::PacketAnalyzerEthernet::PacketIPv6GetName(const UInt8 *packet, UOSInt packetSize, Text::StringBuilderUTF *sb)
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

Bool Net::PacketAnalyzerEthernet::PacketIPDataGetName(UInt8 protocol, const UInt8 *packet, UOSInt packetSize, Text::StringBuilderUTF *sb)
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
			csptr = UDPPortGetName(destPort);
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


void Net::PacketAnalyzerEthernet::PacketNullGetDetail(const UInt8 *packet, UOSInt packetSize, Text::StringBuilderUTF *sb)
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

void Net::PacketAnalyzerEthernet::PacketEthernetGetDetail(const UInt8 *packet, UOSInt packetSize, Text::StringBuilderUTF *sb)
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

void Net::PacketAnalyzerEthernet::PacketLinuxGetDetail(const UInt8 *packet, UOSInt packetSize, Text::StringBuilderUTF *sb)
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

void Net::PacketAnalyzerEthernet::PacketEthernetDataGetDetail(UInt16 etherType, const UInt8 *packet, UOSInt packetSize, Text::StringBuilderUTF *sb)
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

void Net::PacketAnalyzerEthernet::PacketIEEE802_2LLCGetDetail(const UInt8 *packet, UOSInt packetSize, Text::StringBuilderUTF *sb)
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

void Net::PacketAnalyzerEthernet::PacketIPv4GetDetail(const UInt8 *packet, UOSInt packetSize, Text::StringBuilderUTF *sb)
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

void Net::PacketAnalyzerEthernet::PacketIPv6GetDetail(const UInt8 *packet, UOSInt packetSize, Text::StringBuilderUTF *sb)
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

void Net::PacketAnalyzerEthernet::PacketIPDataGetDetail(UInt8 protocol, const UInt8 *packet, UOSInt packetSize, Text::StringBuilderUTF *sb)
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

void Net::PacketAnalyzerEthernet::PacketUDPGetDetail(UInt16 srcPort, UInt16 destPort, const UInt8 *packet, UOSInt packetSize, Text::StringBuilderUTF *sb)
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
	else if ((srcPort == 137 || destPort == 137) && packetSize >= 12)
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
			if ((sptr = Net::NetBIOSUtil::GetName(sbuff2, sbuff)) != 0)
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
			if ((sptr = Net::NetBIOSUtil::GetName(sbuff2, sbuff)) != 0)
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
			else if (rrType == 0x21 && rdLength >= 1)
			{
				UOSInt nName = packet[i];
				if (nName * 18 + 43 <= rdLength)
				{
					UOSInt k;
					sb->Append((const UTF8Char*)"\r\nNumber_of_name=");
					sb->AppendUOSInt(nName);
					k = 0;
					while (k < nName)
					{
						sb->Append((const UTF8Char*)"\r\nName");
						sb->AppendUOSInt(k);
						sb->AppendChar('=', 1);
						MemCopyNO(sbuff, &packet[i + 1 + k * 18], 15);
						sbuff[15] = 0;
						Text::StrRTrim(sbuff);
						sb->Append(sbuff);
						sb->Append((const UTF8Char*)", Type=");
						sb->AppendU16(packet[i + 1 + k * 18 + 15]);
						sb->Append((const UTF8Char*)" (");
						sb->Append(Net::NetBIOSUtil::NameTypeGetName(packet[i + 1 + k * 18 + 15]));
						sb->Append((const UTF8Char*)"), Flags=0x");
						sb->AppendHex16(ReadMUInt16(&packet[i + 1 + k * 18 + 16]));
						k++;
					}
					sb->Append((const UTF8Char*)"\r\nUnit ID=");
					sb->AppendHexBuff(&packet[i + 1 + nName * 18], 6, ':', Text::LBT_NONE);
					sb->Append((const UTF8Char*)" (");
					sb->Append((const UTF8Char*)Net::MACInfo::GetMACInfoBuff(&packet[i + 1 + nName * 18])->name);
					sb->AppendChar(')', 1);
					sb->Append((const UTF8Char*)"\r\nJumpers=0x");
					sb->AppendHex8(packet[i + 7 + nName * 18]);
					sb->Append((const UTF8Char*)"\r\nTest Result=0x");
					sb->AppendHex8(packet[i + 8 + nName * 18]);
					sb->Append((const UTF8Char*)"\r\nVersion number=0x");
					sb->AppendHex16(ReadMUInt16(&packet[i + 9 + nName * 18]));
					sb->Append((const UTF8Char*)"\r\nPeriod of statistics=0x");
					sb->AppendHex16(ReadMUInt16(&packet[i + 11 + nName * 18]));
					sb->Append((const UTF8Char*)"\r\nNumber of CRCs=");
					sb->AppendU16(ReadMUInt16(&packet[i + 13 + nName * 18]));
					sb->Append((const UTF8Char*)"\r\nNumber of alignment errors=");
					sb->AppendU16(ReadMUInt16(&packet[i + 15 + nName * 18]));
					sb->Append((const UTF8Char*)"\r\nNumber of collision=");
					sb->AppendU16(ReadMUInt16(&packet[i + 17 + nName * 18]));
					sb->Append((const UTF8Char*)"\r\nNumber of send aborts=");
					sb->AppendU16(ReadMUInt16(&packet[i + 19 + nName * 18]));
					sb->Append((const UTF8Char*)"\r\nNumber of good sends=");
					sb->AppendU32(ReadMUInt32(&packet[i + 21 + nName * 18]));
					sb->Append((const UTF8Char*)"\r\nNumber of good receives=");
					sb->AppendU32(ReadMUInt32(&packet[i + 25 + nName * 18]));
					sb->Append((const UTF8Char*)"\r\nNumber of retransmits=");
					sb->AppendU16(ReadMUInt16(&packet[i + 29 + nName * 18]));
					sb->Append((const UTF8Char*)"\r\nNumber of no resource conditions=");
					sb->AppendU16(ReadMUInt16(&packet[i + 31 + nName * 18]));
					sb->Append((const UTF8Char*)"\r\nNumber of command blocks=");
					sb->AppendU16(ReadMUInt16(&packet[i + 33 + nName * 18]));
					sb->Append((const UTF8Char*)"\r\nNumber of pending sessions=");
					sb->AppendU16(ReadMUInt16(&packet[i + 35 + nName * 18]));
					sb->Append((const UTF8Char*)"\r\nMax number of pending sessions=");
					sb->AppendU16(ReadMUInt16(&packet[i + 37 + nName * 18]));
					sb->Append((const UTF8Char*)"\r\nMax total sessions possible=");
					sb->AppendU16(ReadMUInt16(&packet[i + 39 + nName * 18]));
					sb->Append((const UTF8Char*)"\r\nSesison data packet size=");
					sb->AppendU16(ReadMUInt16(&packet[i + 41 + nName * 18]));
					if (nName * 18 + 43 < rdLength)
					{
						sb->Append((const UTF8Char*)"\r\nUnknown: ");
						sb->AppendHexBuff(&packet[i + nName * 18 + 43], rdLength - nName * 18 - 43, ' ', Text::LBT_NONE);
					}
				}
				else
				{
					sb->Append((const UTF8Char*)"\r\nRDATA=");
					sb->AppendHexBuff(&packet[i], rdLength, ' ', Text::LBT_NONE);
				}
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
			if ((sptr = Net::NetBIOSUtil::GetName(sbuff2, sbuff)) != 0)
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
			if ((sptr = Net::NetBIOSUtil::GetName(sbuff2, sbuff)) != 0)
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
			if ((sptr = Net::NetBIOSUtil::GetName(sbuff2, sbuff)) != 0)
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
		Text::HTMLUtil::XMLWellFormat(packet, packetSize, 0, sb);
	}
	else if (srcPort == 3702)
	{
		sb->Append((const UTF8Char*)"\r\nWS-Discovery Reply:");
		sb->Append((const UTF8Char*)"\r\n");
		Text::HTMLUtil::XMLWellFormat(packet, packetSize, 0, sb);
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

void Net::PacketAnalyzerEthernet::PacketDNSGetDetail(const UInt8 *packet, UOSInt packetSize, Text::StringBuilderUTF *sb)
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
					dt.ToString(sbuff, "yyyy-MM-dd HH:mm:ss zzzz");
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

void Net::PacketAnalyzerEthernet::PacketLoRaMACGetDetail(const UInt8 *packet, UOSInt packetSize, Text::StringBuilderUTF *sb)
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


UOSInt Net::PacketAnalyzerEthernet::HeaderIPv4GetDetail(const UInt8 *packet, UOSInt packetSize, Text::StringBuilderUTF *sb)
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

const UTF8Char *Net::PacketAnalyzerEthernet::TCPPortGetName(UInt16 port)
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
	case 69:
		return (const UTF8Char*)"TFTP";
	case 80:
		return (const UTF8Char*)"HTTP";
	case 110:
		return (const UTF8Char*)"POP3";
	case 135:
		return (const UTF8Char*)"Microsoft EPMAP";
	case 139:
		return (const UTF8Char*)"NetBIOS-SSN";
	case 143:
		return (const UTF8Char*)"IMAP";
	case 427:
		return (const UTF8Char*)"SvrLoc";
	case 443:
		return (const UTF8Char*)"HTTPS";
	case 445:
		return (const UTF8Char*)"Microsoft-DS";
	case 465:
		return (const UTF8Char*)"SMTPS";
	case 538:
		return (const UTF8Char*)"GDOMAP";
	case 587:
		return (const UTF8Char*)"SMTP STARTTLS";
	case 902:
		return (const UTF8Char*)"VMWare ESX";
	case 912:
		return (const UTF8Char*)"VMWare ESX";
	case 993:
		return (const UTF8Char*)"IMAPS";
	case 995:
		return (const UTF8Char*)"POP3S";
	case 1022:
		return (const UTF8Char*)"RFC3692-style Experiment 2";
	case 3306:
		return (const UTF8Char*)"MySQL";
	case 3389:
		return (const UTF8Char*)"RDP";
	}
	return 0;
}
const UTF8Char *Net::PacketAnalyzerEthernet::UDPPortGetName(UInt16 port)
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

const UTF8Char *Net::PacketAnalyzerEthernet::LSAPGetName(UInt8 lsap)
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

const UTF8Char *Net::PacketAnalyzerEthernet::DHCPOptionGetName(UInt8 t)
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
