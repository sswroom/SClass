#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "IO/FileAnalyse/SBFrameDetail.h"
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
#include "Text/StringBuilderUTF8.h"
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
	IO::FileAnalyse::SBFrameDetail frame(sb);
	PacketNullGetDetail(packet, packetSize, 0, &frame);
}

void Net::PacketAnalyzerEthernet::PacketEthernetGetDetail(const UInt8 *packet, UOSInt packetSize, Text::StringBuilderUTF *sb)
{
	IO::FileAnalyse::SBFrameDetail frame(sb);
	PacketNullGetDetail(packet, packetSize, 0, &frame);
}

void Net::PacketAnalyzerEthernet::PacketLinuxGetDetail(const UInt8 *packet, UOSInt packetSize, Text::StringBuilderUTF *sb)
{
	IO::FileAnalyse::SBFrameDetail frame(sb);
	PacketNullGetDetail(packet, packetSize, 0, &frame);
}

void Net::PacketAnalyzerEthernet::PacketEthernetDataGetDetail(UInt16 etherType, const UInt8 *packet, UOSInt packetSize, Text::StringBuilderUTF *sb)
{
	IO::FileAnalyse::SBFrameDetail frame(sb);
	PacketEthernetDataGetDetail(etherType, packet, packetSize, 0, &frame);
}

void Net::PacketAnalyzerEthernet::PacketIEEE802_2LLCGetDetail(const UInt8 *packet, UOSInt packetSize, Text::StringBuilderUTF *sb)
{
	IO::FileAnalyse::SBFrameDetail frame(sb);
	PacketIEEE802_2LLCGetDetail(packet, packetSize, 0, &frame);
}

void Net::PacketAnalyzerEthernet::PacketIPv4GetDetail(const UInt8 *packet, UOSInt packetSize, Text::StringBuilderUTF *sb)
{
	IO::FileAnalyse::SBFrameDetail frame(sb);
	PacketIPv4GetDetail(packet, packetSize, 0, &frame);
}

void Net::PacketAnalyzerEthernet::PacketIPv6GetDetail(const UInt8 *packet, UOSInt packetSize, Text::StringBuilderUTF *sb)
{
	IO::FileAnalyse::SBFrameDetail frame(sb);
	PacketIPv6GetDetail(packet, packetSize, 0, &frame);
}

void Net::PacketAnalyzerEthernet::PacketIPDataGetDetail(UInt8 protocol, const UInt8 *packet, UOSInt packetSize, Text::StringBuilderUTF *sb)
{
	IO::FileAnalyse::SBFrameDetail frame(sb);
	PacketIPDataGetDetail(protocol, packet, packetSize, 0, &frame);
}

void Net::PacketAnalyzerEthernet::PacketUDPGetDetail(UInt16 srcPort, UInt16 destPort, const UInt8 *packet, UOSInt packetSize, Text::StringBuilderUTF *sb)
{
	IO::FileAnalyse::SBFrameDetail frame(sb);
	PacketUDPGetDetail(srcPort, destPort, packet, packetSize, 0, &frame);
}

void Net::PacketAnalyzerEthernet::PacketDNSGetDetail(const UInt8 *packet, UOSInt packetSize, Text::StringBuilderUTF *sb)
{
	IO::FileAnalyse::SBFrameDetail frame(sb);
	PacketDNSGetDetail(packet, packetSize, 0, &frame);
}

void Net::PacketAnalyzerEthernet::PacketLoRaMACGetDetail(const UInt8 *packet, UOSInt packetSize, Text::StringBuilderUTF *sb)
{
	IO::FileAnalyse::SBFrameDetail frame(sb);
	PacketLoRaMACGetDetail(packet, packetSize, 0, &frame);
}

UOSInt Net::PacketAnalyzerEthernet::HeaderIPv4GetDetail(const UInt8 *packet, UOSInt packetSize, Text::StringBuilderUTF *sb)
{
	IO::FileAnalyse::SBFrameDetail frame(sb);
	return HeaderIPv4GetDetail(packet, packetSize, 0, &frame);
}

void Net::PacketAnalyzerEthernet::PacketNullGetDetail(const UInt8 *packet, UOSInt packetSize, UInt32 frameOfst, IO::FileAnalyse::FrameDetailHandler *frame)
{
	UInt32 packetType = ReadMUInt32(packet);
	frame->AddUInt(frameOfst, 4, "Packet Type", packetType);
	switch (packetType)
	{
	case 2:
		PacketIPv4GetDetail(&packet[4], packetSize - 4, frameOfst + 4, frame);
		break;
	case 24:
	case 28:
	case 30:
		PacketIPv6GetDetail(&packet[4], packetSize - 4, frameOfst + 4, frame);
		break;
	}
}

void Net::PacketAnalyzerEthernet::PacketEthernetGetDetail(const UInt8 *packet, UOSInt packetSize, UInt32 frameOfst, IO::FileAnalyse::FrameDetailHandler *frame)
{
	frame->AddMACAddr(frameOfst + 0, "DestMAC", &packet[0], true);
	frame->AddMACAddr(frameOfst + 6, "SrcMAC", &packet[6], true);
	UInt16 etherType = ReadMUInt16(&packet[12]);
	frame->AddHex16Name(frameOfst + 12, "EtherType", etherType, EtherTypeGetName(etherType));
	PacketEthernetDataGetDetail(etherType, &packet[14], packetSize - 14, frameOfst + 14, frame);
}

void Net::PacketAnalyzerEthernet::PacketLinuxGetDetail(const UInt8 *packet, UOSInt packetSize, UInt32 frameOfst, IO::FileAnalyse::FrameDetailHandler *frame)
{
	const Char *vName;
	UInt16 v;
	v = ReadMUInt16(&packet[0]);
	vName = 0;
	switch (v)
	{
	case 0:
		vName = "Unicast to us";
		break;
	case 1:
		vName = "Broadcast to us";
		break;
	case 2:
		vName = "Multicast to us";
		break;
	case 3:
		vName = "Sent by somebody else to somebody else";
		break;
	case 4:
		vName = "Sent by us";
		break;
	}
	frame->AddUIntName(frameOfst + 0, 2, "Packet Type", v, (const UTF8Char*)vName);
	vName = 0;
	v = ReadMUInt16(&packet[2]);
	switch (v)
	{
	case 772:
		vName = "Link Layer Address Type";
		break;
	case 778:
		vName = "IP GRE Protocol Type";
		break;
	case 803:
		vName = "IEEE802.11";
		break;
	}
	frame->AddUIntName(frameOfst + 2, 2, "Link-Layer Device Type", v, (const UTF8Char *)vName);
	UInt16 len = ReadMUInt16(&packet[4]);
	frame->AddUInt(frameOfst + 4, 2, "Link-Layer Address Length", len);
	if (len > 0)
	{
		if (len == 6)
		{
			frame->AddMACAddr(frameOfst + 6, "Link-Layer Address", &packet[6], true);
		}
		else
		{
			if (len > 8)
			{
				frame->AddHexBuff(frameOfst + 6, 8, "Link-Layer Address", &packet[6], false);
			}
			else
			{
				frame->AddHexBuff(frameOfst + 6, len, "Link-Layer Address", &packet[6], false);
			}
		}
	}
	UInt16 etherType = ReadMUInt16(&packet[14]);
	frame->AddHex16Name(frameOfst + 14, "EtherType", etherType, EtherTypeGetName(etherType));
	PacketEthernetDataGetDetail(etherType, &packet[16], packetSize - 16, frameOfst + 16, frame);
}

void Net::PacketAnalyzerEthernet::PacketEthernetDataGetDetail(UInt16 etherType, const UInt8 *packet, UOSInt packetSize, UInt32 frameOfst, IO::FileAnalyse::FrameDetailHandler *frame)
{
	switch (etherType)
	{
	case 0x0004: //IEEE802.2 LLC
		frame->AddFieldSeperstor(frameOfst, (const UTF8Char*)"IEEE802.2 LLC:");
		PacketIEEE802_2LLCGetDetail(packet, packetSize, frameOfst, frame);
		return;
	case 0x26: //Legnth = 0x26 (IEEE802.2 LLC)
		frame->AddFieldSeperstor(frameOfst, (const UTF8Char*)"IEEE802.2 LLC:");
		PacketIEEE802_2LLCGetDetail(packet, packetSize, frameOfst, frame);
		return;
	case 0x0800: //IPv4
		frame->AddFieldSeperstor(frameOfst, (const UTF8Char*)"IPv4:");
		PacketIPv4GetDetail(packet, packetSize, frameOfst, frame);
		return;
	case 0x0806: //ARP
		frame->AddFieldSeperstor(frameOfst, (const UTF8Char*)"ARP:");
		PacketARPGetDetail(packet, packetSize, frameOfst, frame);
		return;
	case 0x86DD: //IPv6
		frame->AddFieldSeperstor(frameOfst, (const UTF8Char*)"IPv6:");
		PacketIPv6GetDetail(packet, packetSize, frameOfst, frame);
		return;
	default:
		{
			Text::StringBuilderUTF8 sb;
			sb.AppendHexBuff(packet, packetSize, ' ', Text::LineBreakType::CRLF);
			frame->AddFieldSeperstor(frameOfst, sb.ToString());
		}
		return;
	}
}

void Net::PacketAnalyzerEthernet::PacketIEEE802_2LLCGetDetail(const UInt8 *packet, UOSInt packetSize, UInt32 frameOfst, IO::FileAnalyse::FrameDetailHandler *frame)
{
	const Char *vName;
	UTF8Char sbuff[32];
	frame->AddHex8Name(frameOfst + 0, "DSAP Address", packet[0], LSAPGetName(packet[0]));
	frame->AddHex8Name(frameOfst + 1, "SSAP Address", packet[1], LSAPGetName(packet[1]));
	switch (packet[1])
	{
	case 0x42: //Spanning Tree Protocol (STP)
		if (packetSize >= 38)
		{
			UInt16 protoId;
			frame->AddFieldSeperstor(frameOfst + 2, (const UTF8Char*)"Spanning Tree Protocol:");
			frame->AddHex8(frameOfst + 2, "Control", packet[2]);
			protoId = ReadMUInt16(&packet[3]);
			vName = 0;
			switch (protoId)
			{
			case 0:
				vName = "IEEE 802.1D";
				break;
			}
			frame->AddHex16Name(frameOfst + 3, "Protocol ID", protoId, (const UTF8Char*)vName);
			vName = 0;
			switch (packet[5])
			{
			case 0:
				vName = "Config & TCN";
				break;
			case 2:
				vName = "RST";
				break;
			case 3:
				vName = "MST";
				break;
			case 4:
				vName = "SPT";
				break;
			}
			frame->AddHex8Name(frameOfst + 5, "Version ID", packet[5], (const UTF8Char*)vName);
			vName = 0;
			switch (packet[6])
			{
			case 0:
				vName = "STP Config BPDU";
				break;
			case 2:
				vName = "RST/MST Config BPDU";
				break;
			case 0x80:
				vName = "TCN BPDU";
				break;
			}
			frame->AddHex8Name(frameOfst + 6, "BPDU Type", packet[6], (const UTF8Char*)vName);
			frame->AddHex8(frameOfst + 7, "Flags", packet[7]);
			frame->AddHex16(frameOfst + 8, "Root Bridge", ReadMUInt16(&packet[8]));
			Text::StrUInt16(sbuff, (UInt16)(packet[8] >> 4));
			frame->AddSubfield(frameOfst + 8, 2, (const UTF8Char*)"Root Bridge Priority", sbuff);
			Text::StrUInt16(sbuff, ReadMUInt16(&packet[8]) & 0xfff);
			frame->AddSubfield(frameOfst + 8, 2, (const UTF8Char*)"Root Bridge System ID Extension", sbuff);
			frame->AddMACAddr(frameOfst + 10, "Root Bridge MAC Address", &packet[10], false);
			frame->AddUInt(frameOfst + 16, 4, "Root Path Cost", ReadMUInt32(&packet[16]));
			frame->AddHex16(frameOfst + 20, "Bridge", ReadMUInt16(&packet[20]));
			Text::StrUInt16(sbuff, (UInt16)(packet[20] >> 4));
			frame->AddSubfield(frameOfst + 20, 2, (const UTF8Char*)"Bridge Priority", sbuff);
			Text::StrUInt16(sbuff, ReadMUInt16(&packet[20]) & 0xfff);
			frame->AddSubfield(frameOfst + 20, 2, (const UTF8Char*)"Bridge System ID Extension", sbuff);
			frame->AddMACAddr(frameOfst + 22, "Bridge MAC Address", &packet[22], false);
			frame->AddUInt(frameOfst + 28, 2, "Port ID", ReadMUInt16(&packet[28]));
			frame->AddFloat(frameOfst + 30, 2, "Message Age", ReadMUInt16(&packet[30]) / 256.0);
			frame->AddFloat(frameOfst + 32, 2, "Max Age", ReadMUInt16(&packet[32]) / 256.0);
			frame->AddFloat(frameOfst + 34, 2, "Hello Time", ReadMUInt16(&packet[34]) / 256.0);
			frame->AddFloat(frameOfst + 36, 2, "Forward Delay", ReadMUInt16(&packet[36]) / 256.0);
			if (packetSize > 38)
			{
				Text::StringBuilderUTF8 sb;
				sb.AppendHexBuff(&packet[38], packetSize - 38, ' ', Text::LineBreakType::CRLF);
				frame->AddFieldSeperstor(frameOfst + 38, sb.ToString());
			}
		}
		break;
	default:
		{
			Text::StringBuilderUTF8 sb;
			sb.AppendHexBuff(&packet[2], packetSize - 2, ' ', Text::LineBreakType::CRLF);
			frame->AddFieldSeperstor(frameOfst + 2, sb.ToString());
		}
		break;
	}

}

void Net::PacketAnalyzerEthernet::PacketARPGetDetail(const UInt8 *packet, UOSInt packetSize, UInt32 frameOfst, IO::FileAnalyse::FrameDetailHandler *frame)
{
	if (packetSize < 22)
	{
		Text::StringBuilderUTF8 sb;
		sb.AppendHexBuff(packet, packetSize, ' ', Text::LineBreakType::CRLF);
		frame->AddText(frameOfst, sb.ToString());
	}
	else
	{
		const Char *vName;
		UInt16 htype = ReadMUInt16(&packet[0]);
		UInt16 ptype = ReadMUInt16(&packet[2]);
		UInt8 hlen = packet[4];
		UInt8 plen = packet[5];
		UInt16 oper = ReadMUInt16(&packet[6]);
		frame->AddUInt(frameOfst + 0, 2, "Hardware Type (HTYPE)", htype);
		frame->AddHex16(frameOfst + 2, "Protocol Type (PTYPE)", ptype);
		frame->AddUInt(frameOfst + 4, 1, "Hardware address length (HLEN)", hlen);
		frame->AddUInt(frameOfst + 5, 1, "Protocol address length (PLEN)", plen);
		vName = 0;
		switch (oper)
		{
		case 1:
			vName = "Request";
			break;
		case 2:
			vName = "Reply";
			break;
		}
		frame->AddUIntName(frameOfst + 6, 2, "Operation (OPER)", oper, (const UTF8Char*)vName);
		if (htype == 1 && ptype == 0x0800 && hlen == 6 && plen == 4 && packetSize >= 28)
		{
			frame->AddMACAddr(frameOfst + 8, "Sender hardware address (SHA)", &packet[8], true);
			frame->AddIPv4(frameOfst + 14, "Sender protocol address (SPA)", &packet[14]);
			frame->AddMACAddr(frameOfst + 18, "Target hardware address (THA)", &packet[18], true);
			frame->AddIPv4(frameOfst + 24, "Target protocol address (TPA)", &packet[24]);
			if (packetSize > 28)
			{
				frame->AddHexBuff(frameOfst + 28, packetSize - 28, "Trailer", &packet[28], true);
			}
		}
		else
		{
			frame->AddTextHexBuff(frameOfst + 8, packetSize - 8, &packet[8], true);
		}
	}
}

void Net::PacketAnalyzerEthernet::PacketIPv4GetDetail(const UInt8 *packet, UOSInt packetSize, UInt32 frameOfst, IO::FileAnalyse::FrameDetailHandler *frame)
{
	if ((packet[0] & 0xf0) != 0x40 || packetSize < 20)
	{
		frame->AddText(frameOfst, (const UTF8Char*)"Not IPv4 Packet");
		frame->AddTextHexBuff(frameOfst, packetSize, packet, true);
		return;
	}

	const UInt8 *ipData;
	UOSInt ipDataSize;
	ipDataSize = HeaderIPv4GetDetail(packet, packetSize, frameOfst, frame);
	ipData = &packet[ipDataSize];
	frameOfst += (UInt32)ipDataSize;
	ipDataSize = packetSize - ipDataSize;

	PacketIPDataGetDetail(packet[9], ipData, ipDataSize, frameOfst, frame);
}

void Net::PacketAnalyzerEthernet::PacketIPv6GetDetail(const UInt8 *packet, UOSInt packetSize, UInt32 frameOfst, IO::FileAnalyse::FrameDetailHandler *frame)
{
	UTF8Char sbuff[64];
	if ((packet[0] & 0xf0) != 0x60 || packetSize < 40)
	{
		frame->AddText(frameOfst, (const UTF8Char*)"Not IPv6 Packet");
		frame->AddTextHexBuff(frameOfst, packetSize, packet, true);
		return;
	}

	frame->AddHexBuff(frameOfst, 4, "Header Misc", packet, false);
	frame->AddSubfield(frameOfst, 1, (const UTF8Char*)"Version", (const UTF8Char*)"6");
	Text::StrUInt16(sbuff, (UInt16)(((packet[0] & 0xf) << 2) | (packet[1] >> 6)));
	frame->AddSubfield(frameOfst, 2, (const UTF8Char*)"DS", sbuff);
	Text::StrUInt16(sbuff, (UInt16)((packet[1] & 0x30) >> 4));
	frame->AddSubfield(frameOfst, 2, (const UTF8Char*)"ECN", sbuff);
	Text::StrUInt32(sbuff, (UInt32)(((packet[1] & 0xf) << 16) | ReadMUInt16(&packet[2])));
	frame->AddSubfield(frameOfst + 1, 3, (const UTF8Char*)"Flow Label", sbuff);
	frame->AddUInt(frameOfst + 4, 2, "Payload Length", ReadMUInt16(&packet[4]));
	frame->AddUInt(frameOfst + 6, 1, "Next Header", packet[6]);
	frame->AddUInt(frameOfst + 7, 1, "Hop Limit", packet[7]);
	frame->AddIPv6(frameOfst + 8, "Source Address", &packet[8]);
	frame->AddIPv6(frameOfst + 24, "Destination Address", &packet[24]);
	PacketIPDataGetDetail(packet[6], &packet[40], packetSize - 40, frameOfst + 40, frame);
}

void Net::PacketAnalyzerEthernet::PacketIPDataGetDetail(UInt8 protocol, const UInt8 *packet, UOSInt packetSize, UInt32 frameOfst, IO::FileAnalyse::FrameDetailHandler *frame)
{
	UTF8Char sbuff[64];
	const Char *vName;
	switch (protocol)
	{
	case 1: //ICMP
		frame->AddText(frameOfst, (const UTF8Char*)"ICMP:");
		if (packetSize >= 4)
		{
			UOSInt i = 4;
			vName = 0;
			switch (packet[0])
			{
			case 0:
				vName = "Echo Reply";
				break;
			case 3:
				vName = "Destination Unreachable";
				break;
			case 4:
				vName = "Source Quench";
				break;
			case 5:
				vName = "Redirect Message";
				break;
			case 6:
				vName = "Alternate Host Address";
				break;
			case 8:
				vName = "Echo Request";
				break;
			case 9:
				vName = "Router Advertisement";
				break;
			case 10:
				vName = "Router Solicitation";
				break;
			case 11:
				vName = "Time Exceeded";
				break;
			case 12:
				vName = "Parameter Problem: Bad IP header";
				break;
			case 13:
				vName = "Timestamp";
				break;
			case 14:
				vName = "Timestamp Reply";
				break;
			case 15:
				vName = "Information Request";
				break;
			case 16:
				vName = "Information Reply";
				break;
			case 17:
				vName = "Address Mask Request";
				break;
			case 18:
				vName = "Address Mask Reply";
				break;
			case 30:
				vName = "Traceroute";
				break;
			case 42:
				vName = "Extended Echo Request";
				break;
			case 43:
				vName = "Extended Echo Reply";
				break;
			}
			frame->AddUIntName(frameOfst + 0, 1, "Type", packet[0], (const UTF8Char*)vName);

			switch (packet[0])
			{
			case 3:
				switch (packet[1])
				{
				case 0:
					vName = "Destination network unreachable";
					break;
				case 1:
					vName = "Destination host unreachable";
					break;
				case 2:
					vName = "Destination protocol unreachable";
					break;
				case 3:
					vName = "Destination port unreachable";
					break;
				case 4:
					vName = "Fragmentation required";
					break;
				case 5:
					vName = "Source route failed";
					break;
				case 6:
					vName = "Destination network unknown";
					break;
				case 7:
					vName = "Destination host unknown";
					break;
				case 8:
					vName = "Source host isolated";
					break;
				case 9:
					vName = "Network administratively prohibited";
					break;
				case 10:
					vName = "Host administratively prohibited";
					break;
				case 11:
					vName = "Network unreachable for ToS";
					break;
				case 12:
					vName = "Host unreachable for ToS";
					break;
				case 13:
					vName = "Communication administratively prohibited";
					break;
				case 14:
					vName = "Host Precedence Violation";
					break;
				case 15:
					vName = "Precedence cutoff in effect";
					break;
				}
				break;
			case 5:
				switch (packet[1])
				{
				case 0:
					vName = "Redirect Datagram for the Network";
					break;
				case 1:
					vName = "Redirect Datagram for the Host";
					break;
				case 2:
					vName = "Redirect Datagram for the ToS & network";
					break;
				case 3:
					vName = "Redirect Datagram for the ToS & host";
					break;
				}
				break;
			case 11:
				switch (packet[1])
				{
				case 0:
					vName = "TTL expired in transit";
					break;
				case 1:
					vName = "Fragment reassembly time exceeded";
					break;
				}
				break;
			case 12:
				switch (packet[1])
				{
				case 0:
					vName = "Pointer indicates the error";
					break;
				case 1:
					vName = "Missing a required option";
					break;
				case 2:
					vName = "Bad length";
					break;
				}
				break;
			case 43:
				switch (packet[1])
				{
				case 0:
					vName = "No Error";
					break;
				case 1:
					vName = "Malformed Query";
					break;
				case 2:
					vName = "No Such Interface";
					break;
				case 3:
					vName = "No Such Table Entry";
					break;
				case 4:
					vName = "Multiple Interfaces Satisfy Query";
					break;
				}
				break;
			}
			frame->AddUIntName(frameOfst + 1, 1, "Code", packet[1], (const UTF8Char*)vName);
			frame->AddHex16(frameOfst + 2, "Checksum", ReadMUInt16(&packet[2]));
			switch (packet[0])
			{
			case 3:
				{
					frame->AddUInt(frameOfst + 4, 2, "Next-hop MTU", ReadMUInt16(&packet[4]));
					frame->AddHexBuff(frameOfst + 6, 2, "Unused", &packet[6], false);
					frame->AddText(frameOfst + 8, (const UTF8Char*)"Original IP Header:");
					i = 8;
					UInt8 protocol = packet[i + 9];
					i += HeaderIPv4GetDetail(&packet[i], packetSize - i, frameOfst + (UInt32)i, frame);
					PacketIPDataGetDetail(protocol, &packet[i], packetSize - i, frameOfst + (UInt32)i, frame);
				}
				break;
			case 0:
			case 8:
				frame->AddUInt(frameOfst + 4, 2, "Identifier", ReadMUInt16(&packet[4]));
				frame->AddUInt(frameOfst + 6, 2, "Sequence Number", ReadMUInt16(&packet[6]));
				frame->AddHexBuff(frameOfst + 8, packetSize - 8, "Data", &packet[8], true);
				i = packetSize;
				break;
			}
			if (i < packetSize)
			{
				frame->AddTextHexBuff(frameOfst + i, packetSize - i, &packet[i], true);
			}
		}
		else
		{
			frame->AddTextHexBuff(frameOfst, packetSize, packet, true);
		}
		return;
	case 2: //IGMP
		frame->AddText(frameOfst, (const UTF8Char*)"IGMP:");
		if (packet[0] == 0x11)
		{
			frame->AddField(frameOfst + 0, 1, (const UTF8Char*)"Type", (const UTF8Char*)"0x11 (IGMPv3 membership query)");
			if (packetSize >= 8)
			{
				UInt16 n;
				frame->AddUInt(frameOfst + 1, 1, "Max Resp Time", packet[1]);
				frame->AddHex16(frameOfst + 2, "Checksum", ReadMUInt16(&packet[2]));
				frame->AddIPv4(frameOfst + 4, "Group Address", &packet[4]);
				if (packetSize >= 16)
				{
					frame->AddHex8(frameOfst + 8, "Flags", packet[8]);
					frame->AddUInt(frameOfst + 9, 1, "QQIC", packet[9]);
					n = ReadMUInt16(&packet[10]);
					frame->AddUInt(frameOfst + 10, 2, "QQIC", n);
					if (packetSize >= 12 + (UOSInt)n * 4)
					{
						UInt16 i = 0;
						while (i < n)
						{
							Text::StrConcat(Text::StrUInt16(Text::StrConcat(sbuff, (const UTF8Char*)"Source Address["), i), (const UTF8Char*)"]");
							frame->AddIPv4(frameOfst + 12 + (UOSInt)i * 4, (const Char*)sbuff, &packet[12 + i * 4]);
							i++;
						}
						if (packetSize > 12 + (UOSInt)n * 4)
						{
							frame->AddTextHexBuff(frameOfst + 12 + (UOSInt)n * 4, packetSize - 12 - (UInt32)n * 4, &packet[12 + n * 4], true);
						}
					}
				}
				else
				{
					frame->AddTextHexBuff(frameOfst + 8, packetSize - 8, &packet[8], true);
				}
			}
			else
			{
				frame->AddTextHexBuff(frameOfst + 1, packetSize - 1, &packet[1], true);
			}
		}
		else if (packet[0] == 0x16)
		{
			frame->AddField(frameOfst + 0, 1, (const UTF8Char*)"Type", (const UTF8Char*)"0x16 (IGMPv2 Membership Report)");
			if (packetSize >= 8)
			{
				frame->AddUInt(frameOfst + 1, 1, "Max Resp Time", packet[1]);
				frame->AddHex16(frameOfst + 2, "Checksum", ReadMUInt16(&packet[2]));
				frame->AddIPv4(frameOfst + 4, "Group Address", &packet[4]);
				if (packetSize > 8)
				{
					frame->AddTextHexBuff(frameOfst + 8, packetSize - 8, &packet[8], true);
				}
			}
		}
		else
		{
			frame->AddHex8(frameOfst + 0, "Type", packet[0]);
			frame->AddTextHexBuff(frameOfst + 1, packetSize - 1, &packet[1], true);
		}
		return;
	case 6:
	{
		frame->AddText(frameOfst, (const UTF8Char*)"TCP:");
		if (packetSize < 20)
		{
			frame->AddTextHexBuff(frameOfst, packetSize, packet, true);
		}
		else
		{
			frame->AddUInt(frameOfst + 0, 2, "Source Port", ReadMUInt16(&packet[0]));
			frame->AddUInt(frameOfst + 2, 2, "Destination Port", ReadMUInt16(&packet[2]));
			frame->AddUInt(frameOfst + 4, 4, "Sequence Number", ReadMUInt32(&packet[4]));
			frame->AddUInt(frameOfst + 8, 4, "Acknowledgment Number", ReadMUInt32(&packet[8]));
			frame->AddUInt(frameOfst + 12, 1, "Data Offset", (UInt16)(packet[12] >> 4));
			Text::StrUInt16(sbuff, packet[12] & 1);
			frame->AddSubfield(frameOfst + 12, 1, (const UTF8Char*)"NS", sbuff);
			frame->AddHex8(frameOfst + 13, "Flags", packet[13]);
			Text::StrUInt16(sbuff, (UInt16)((packet[13] >> 7) & 1));
			frame->AddSubfield(frameOfst + 13, 1, (const UTF8Char*)"CWR", sbuff);
			Text::StrUInt16(sbuff, (UInt16)((packet[13] >> 6) & 1));
			frame->AddSubfield(frameOfst + 13, 1, (const UTF8Char*)"ECE", sbuff);
			Text::StrUInt16(sbuff, (UInt16)((packet[13] >> 5) & 1));
			frame->AddSubfield(frameOfst + 13, 1, (const UTF8Char*)"URG", sbuff);
			Text::StrUInt16(sbuff, (UInt16)((packet[13] >> 4) & 1));
			frame->AddSubfield(frameOfst + 13, 1, (const UTF8Char*)"ACK", sbuff);
			Text::StrUInt16(sbuff, (UInt16)((packet[13] >> 3) & 1));
			frame->AddSubfield(frameOfst + 13, 1, (const UTF8Char*)"PSH", sbuff);
			Text::StrUInt16(sbuff, (UInt16)((packet[13] >> 2) & 1));
			frame->AddSubfield(frameOfst + 13, 1, (const UTF8Char*)"RST", sbuff);
			Text::StrUInt16(sbuff, (UInt16)((packet[13] >> 1) & 1));
			frame->AddSubfield(frameOfst + 13, 1, (const UTF8Char*)"SYN", sbuff);
			Text::StrUInt16(sbuff, packet[13] & 1);
			frame->AddSubfield(frameOfst + 13, 1, (const UTF8Char*)"FIN", sbuff);
			frame->AddUInt(frameOfst + 14, 2, "Window Size", ReadMUInt16(&packet[14]));
			frame->AddHex16(frameOfst + 16, "Checksum", ReadMUInt16(&packet[16]));
			frame->AddHex16(frameOfst + 18, "Urgent Pointer", ReadMUInt16(&packet[18]));
			UOSInt headerLen = (UOSInt)(packet[12] >> 4) * 4;
			if (headerLen > 20)
			{
				frame->AddText(frameOfst + 20, (const UTF8Char*)"Options:");
				UOSInt i = 20;
				while (i < headerLen)
				{
					vName = 0;
					switch (packet[i])
					{
					case 0:
						vName = "End of option list";
						break;
					case 1:
						vName = "No operation";
						break;
					case 2:
						vName = "No operation";
						break;
					case 3:
						vName = "Window scale";
						break;
					case 4:
						vName = "SACK permitted";
						break;
					case 5:
						vName = "SACK";
						break;
					case 8:
						vName = "Time Stamp Option";
						break;
					}
					frame->AddUIntName(frameOfst + i, 1, "Kind", packet[i], (const UTF8Char*)vName);

					switch (packet[i])
					{
					case 0:
						i = headerLen - 1;
						break;
					case 1:
						break;
					case 2:
						frame->AddUInt(frameOfst + i + 1, 1, "Length", packet[i + 1]);
						if (packet[i + 1] == 4)
						{
							frame->AddUInt(frameOfst + i + 2, 2, "Value", ReadMUInt16(&packet[i + 2]));
						}
						i += (UOSInt)packet[i + 1] - 1;
						break;
					case 3:
						frame->AddUInt(frameOfst + i + 1, 1, "Length", packet[i + 1]);
						if (packet[i + 1] == 3)
						{
							frame->AddUInt(frameOfst + i + 2, 1, "Value", packet[i + 2]);
						}
						i += (UOSInt)packet[i + 1] - 1;
						break;
					case 4:
						frame->AddUInt(frameOfst + i + 1, 1, "Length", packet[i + 1]);
						i += (UOSInt)packet[i + 1] - 1;
						break;
					case 5:
						frame->AddUInt(frameOfst + i + 1, 1, "Length", packet[i + 1]);
						if (packet[i + 1] == 10)
						{
							frame->AddUInt(frameOfst + i + 2, 4, "Left Edge", ReadMUInt32(&packet[i + 2]));
							frame->AddUInt(frameOfst + i + 6, 4, "Right Edge", ReadMUInt32(&packet[i + 6]));
						}
						i += (UOSInt)packet[i + 1] - 1;
						break;
					case 8:
						frame->AddUInt(frameOfst + i + 1, 1, "Length", packet[i + 1]);
						if (packet[i + 1] == 10)
						{
							frame->AddUInt(frameOfst + i + 2, 4, "Timestamp value", ReadMUInt32(&packet[i + 2]));
							frame->AddUInt(frameOfst + i + 6, 4, "Timestamp echo reply", ReadMUInt32(&packet[i + 6]));
						}
						i += (UOSInt)packet[i + 1] - 1;
						break;
					default:
						frame->AddUInt(frameOfst + i + 1, 1, "Length", packet[i + 1]);
						i += (UOSInt)packet[i + 1] - 1;
						break;
					}
					i++;
				}
			}
			if (packetSize > headerLen)
			{
				frame->AddHexBuff(frameOfst + headerLen, packetSize - headerLen, "Data", &packet[headerLen], true);
			}
		}
		return;
	}
	case 17:
	{
		UInt16 srcPort = 0;
		UInt16 destPort = 0;
		UOSInt udpLen = packetSize;
		frame->AddText(frameOfst, (const UTF8Char*)"UDP:");
		
		if (packetSize >= 2)
		{
			srcPort = ReadMUInt16(&packet[0]);
			frame->AddUIntName(frameOfst + 0, 2, "SrcPort", srcPort, UDPPortGetName(srcPort));
		}
		if (packetSize >= 4)
		{
			destPort = ReadMUInt16(&packet[2]);
			frame->AddUIntName(frameOfst + 2, 2, "DestPort", destPort, UDPPortGetName(destPort));
		}
		if (packetSize >= 6)
		{
			udpLen = ReadMUInt16(&packet[4]);
			frame->AddUInt(frameOfst + 4, 2, "Length", udpLen);
			if (packetSize < udpLen)
				udpLen = packetSize;
		}
		if (packetSize >= 8)
		{
			frame->AddHex16(frameOfst + 6, "Checksum", ReadMUInt16(&packet[6]));
		}
		if (packetSize > 8)
		{
			PacketUDPGetDetail(srcPort, destPort, &packet[8], udpLen - 8, frameOfst + 8, frame);
			if (packetSize > udpLen)
			{
				frame->AddHexBuff(frameOfst + udpLen, packetSize - udpLen, "Padding", &packet[udpLen], true);
			}
		}
		return;
	}
	case 58:
		frame->AddText(frameOfst, (const UTF8Char*)"ICMPv6:");
		if (packetSize >= 4)
		{
			vName = 0;
			switch (packet[0])
			{
			case 1:
				vName = "Destination unreachable";
				break;
			case 2:
				vName = "Packet Too Big";
				break;
			case 3:
				vName = "Time exceeded";
				break;
			case 4:
				vName = "Parameter problem";
				break;
			case 100:
				vName = "Private experimentation";
				break;
			case 101:
				vName = "Private experimentation";
				break;
			case 127:
				vName = "Reserved for expansion of ICMPv6 error messages";
				break;
			case 128:
				vName = "Echo Request";
				break;
			case 129:
				vName = "Echo Reply";
				break;
			case 130:
				vName = "Multicast Listener Query";
				break;
			case 131:
				vName = "Multicast Listener Report";
				break;
			case 132:
				vName = "Multicast Listener Done";
				break;
			case 133:
				vName = "Router Solicitation";
				break;
			case 134:
				vName = "Router Advertisement";
				break;
			case 135:
				vName = "Neighbor Solicitation";
				break;
			case 136:
				vName = "Neighbor Advertisement";
				break;
			case 137:
				vName = "Redirect Message";
				break;
			case 138:
				vName = "Router Renumbering";
				break;
			case 139:
				vName = "ICMP Node Information Query";
				break;
			case 140:
				vName = "ICMP Node Information Response";
				break;
			case 141:
				vName = "Inverse Neighbor Discovery Solicitation Message";
				break;
			case 142:
				vName = "Inverse Neighbor Discovery Advertisement Message";
				break;
			case 143:
				vName = "Multicast Listener Discovery (MLDv2) reports";
				break;
			case 144:
				vName = "Home Agent Address Discovery Request Message";
				break;
			case 145:
				vName = "Home Agent Address Discovery Reply Message";
				break;
			case 146:
				vName = "Mobile Prefix Solicitation";
				break;
			case 147:
				vName = "Mobile Prefix Advertisement";
				break;
			case 148:
				vName = "Certification Path Solicitation";
				break;
			case 149:
				vName = "Certification Path Advertisement";
				break;
			case 151:
				vName = "Multicast Router Advertisement";
				break;
			case 152:
				vName = "Multicast Router Solicitation";
				break;
			case 153:
				vName = "Multicast Router Termination";
				break;
			case 155:
				vName = "RPL Control Message";
				break;
			case 200:
				vName = "Private experimentation";
				break;
			case 201:
				vName = "Private experimentation";
				break;
			case 255:
				vName = "Reserved for expansion of ICMPv6 informational messages";
				break;
			}
			frame->AddUIntName(frameOfst + 0, 1, "Type", packet[0], (const UTF8Char*)vName);

			switch (packet[0])
			{
			case 1: //Destination unreachable
				switch (packet[1])
				{
				case 0:
					vName = "no route to destination";
					break;
				case 1:
					vName = "communication with destination administratively prohibited";
					break;
				case 2:
					vName = "beyond scope of source address";
					break;
				case 3:
					vName = "address unreachable";
					break;
				case 4:
					vName = "port unreachable";
					break;
				case 5:
					vName = "source address failed ingress/egress policy";
					break;
				case 6:
					vName = "reject route to destination";
					break;
				case 7:
					vName = "Error in Source Routing Header";
					break;
				}
				break;
			case 3: //Time exceeded
				switch (packet[1])
				{
				case 0:
					vName = "hop limit exceeded in transit";
					break;
				case 1:
					vName = "fragment reassembly time exceeded";
					break;
				}
				break;
			case 4: //Parameter problem
				switch (packet[1])
				{
				case 0:
					vName = "erroneous header field encountered";
					break;
				case 1:
					vName = "unrecognized Next Header type encountered";
					break;
				case 2:
					vName = "unrecognized IPv6 option encountered";
					break;
				}
				break;
			case 138: //Router Renumbering
				switch (packet[1])
				{
				case 0:
					vName = "Router Renumbering Command";
					break;
				case 1:
					vName = "Router Renumbering Result";
					break;
				case 255:
					vName = "Sequence Number Reset";
					break;
				}
				break;
			case 139: //ICMP Node Information Query
				switch (packet[1])
				{
				case 0:
					vName = "The Data field contains an IPv6 address which is the Subject of this Query";
					break;
				case 1:
					vName = "The Data field contains a name which is the Subject of this Query, or is empty, as in the case of a NOOP";
					break;
				case 2:
					vName = "The Data field contains an IPv4 address which is the Subject of this Query";
					break;
				}
				break;
			case 140: //ICMP Node Information Response
				switch (packet[1])
				{
				case 0:
					vName = "A successful reply. The Reply Data field may or may not be empty";
					break;
				case 1:
					vName = "The Responder refuses to supply the answer. The Reply Data field will be empty.";
					break;
				case 2:
					vName = "The Qtype of the Query is unknown to the Responder. The Reply Data field will be empty";
					break;
				}
				break;
			}
			frame->AddUIntName(frameOfst + 1, 1, "Code", packet[1], (const UTF8Char*)vName);
			frame->AddHex16(frameOfst + 2, "Checksum", ReadMUInt16(&packet[2]));
			switch (packet[0])
			{
			case 134:
				if (packetSize >= 24)
				{
					frame->AddUInt(frameOfst + 4, 1, "Hop Limit", packet[4]);
					frame->AddHex8(frameOfst + 5, "Flags", packet[5]);
					frame->AddUInt(frameOfst + 6, 2, "Router Lifetime", ReadMUInt16(&packet[6]));
					frame->AddUInt(frameOfst + 8, 4, "Reachable Timer", ReadMUInt32(&packet[8]));
					frame->AddUInt(frameOfst + 12, 4, "Retains Timer", ReadMUInt32(&packet[12]));
					UOSInt i = 16;
					while (i + 7 < packetSize)
					{
						vName = 0;
						switch (packet[i])
						{
						case 1:
							vName = "Source Link-layer Address";
							break;
						case 2:
							vName = "Target Link-layer Address";
							break;
						}
						frame->AddUIntName(frameOfst + i, 1, "Type", packet[i], (const UTF8Char*)vName);
						frame->AddUInt(frameOfst + i + 1, 1, "Length", packet[i + 1]);
						switch (packet[i])
						{
						case 1:
						case 2:
							frame->AddMACAddr(frameOfst + i + 2, "Address", &packet[i + 2], true);
							break;
						case 3:
							frame->AddUInt(frameOfst + i + 2, 1, "Prefix Length", packet[i + 2]);
							frame->AddHex8(frameOfst + i + 3, "Flags", packet[i + 3]);
							frame->AddUInt(frameOfst + i + 4, 4, "Valid Lifetime", ReadMUInt32(&packet[i + 4]));
							frame->AddUInt(frameOfst + i + 8, 4, "Preferred Lifetime", ReadMUInt32(&packet[i + 8]));
							frame->AddUInt(frameOfst + i + 12, 4, "Reserved", ReadMUInt32(&packet[i + 12]));
							frame->AddHexBuff(frameOfst + i + 16, (UInt32)packet[i + 1] - 16, "Prefix", &packet[i + 16], false);
							break;
						case 5:
							frame->AddUInt(frameOfst + i + 2, 2, "Prefix Length", ReadMUInt16(&packet[i + 2]));
							frame->AddUInt(frameOfst + i + 4, 4, "MTU", ReadMUInt32(&packet[i + 4]));
							break;
						}
						i += (UOSInt)packet[i + 1] * 8;
					}
				}
				else
				{
					frame->AddTextHexBuff(frameOfst + 4, packetSize - 4, &packet[4], true);
				}
				break;
			case 135:
				if (packetSize >= 24)
				{
					frame->AddHex32(frameOfst + 4, "Reserved", ReadMUInt32(&packet[4]));
					frame->AddIPv4(frameOfst + 8, "Target Address", &packet[8]);
					UOSInt i = 24;
					while (i < packetSize)
					{
						vName = 0;
						switch (packet[i])
						{
						case 1:
							vName = "Source Link-layer Address";
							break;
						case 2:
							vName = "Target Link-layer Address";
							break;
						}
						frame->AddUIntName(frameOfst + i, 1, "Type", packet[i], (const UTF8Char*)vName);
						frame->AddUInt(frameOfst + i + 1, 1, "Length", packet[i + 1]);
						frame->AddMACAddr(frameOfst + i + 2, "Address", &packet[i + 2], true);
						i += 8;
					}
				}
				else
				{
					frame->AddTextHexBuff(frameOfst + 4, packetSize - 4, &packet[4], true);
				}
				break;
			default:
				frame->AddTextHexBuff(frameOfst + 4, packetSize - 4, &packet[4], true);
				break;
			}
		}
		return;
	default:
		frame->AddTextHexBuff(frameOfst, packetSize, packet, true);
		return;
	}
}

void Net::PacketAnalyzerEthernet::PacketUDPGetDetail(UInt16 srcPort, UInt16 destPort, const UInt8 *packet, UOSInt packetSize, UInt32 frameOfst, IO::FileAnalyse::FrameDetailHandler *frame)
{
	const Char *vName;
	UTF8Char sbuff[64];
	UTF8Char sbuff2[64];
	if (destPort == 53)
	{
		frame->AddText(frameOfst, (const UTF8Char*)"DNS Request:");
		if (packetSize < 12)
		{
			frame->AddTextHexBuff(frameOfst, packetSize, packet, true);
		}
		else
		{
			PacketDNSGetDetail(packet, packetSize, frameOfst, frame);
		}
	}
	else if (srcPort == 53)
	{
		frame->AddText(frameOfst, (const UTF8Char*)"DNS Reply:");
		if (packetSize < 12)
		{
			frame->AddTextHexBuff(frameOfst, packetSize, packet, true);
		}
		else
		{
			PacketDNSGetDetail(packet, packetSize, frameOfst, frame);
		}
	}
	else if (srcPort == 67 || destPort == 67)
	{
		frame->AddText(frameOfst, (const UTF8Char*)"BOOTP (DHCP):");
		if (packetSize < 240)
		{
			frame->AddTextHexBuff(frameOfst, packetSize, packet, true);
		}
		else if (ReadMUInt32(&packet[236]) == 0x63825363)
		{
			vName = 0;
			switch (packet[0])
			{
			case 1:
				vName = "Request";
				break;
			case 2:
				vName = "Reply";
				break;
			}
			frame->AddUIntName(frameOfst + 0, 1, "OP", packet[0], (const UTF8Char*)vName);
			frame->AddUInt(frameOfst + 1, 1, "Hardware Type (HTYPE)", packet[1]);
			frame->AddUInt(frameOfst + 2, 1, "Hardware Address Length (HLEN)", packet[2]);
			frame->AddUInt(frameOfst + 3, 1, "HOPS", packet[3]);
			frame->AddHex32(frameOfst + 4, "Transaction ID", ReadMUInt32(&packet[4]));
			frame->AddUInt(frameOfst + 8, 2, "Seconds Elapsed (SECS)", ReadMUInt16(&packet[8]));
			frame->AddHex16(frameOfst + 10, "Flags", ReadMUInt16(&packet[10]));
			frame->AddIPv4(frameOfst + 12, "Client IP Address", &packet[12]);
			frame->AddIPv4(frameOfst + 16, "Your IP Address", &packet[16]);
			frame->AddIPv4(frameOfst + 20, "Server IP Address", &packet[20]);
			frame->AddIPv4(frameOfst + 24, "Gateway IP Address", &packet[24]);
			frame->AddMACAddr(frameOfst + 28, "Client Hardware Address", &packet[28], true);
			frame->AddHexBuff(frameOfst + 34, 10, "Padding", &packet[34], false);
			frame->AddStrS(frameOfst + 44, 64, "Server Host Name", &packet[44]);
			frame->AddStrS(frameOfst + 108, 128, "Server Host Name", &packet[108]);
			frame->AddHex32(frameOfst + 236, "DHCP Magic", ReadMUInt32(&packet[236]));
			const UInt8 *currPtr = &packet[240];
			const UInt8 *endPtr = &packet[packetSize];
			UInt8 t;
			UInt8 len;
			while (currPtr < endPtr)
			{
				t = *currPtr++;
				frame->AddUIntName(frameOfst + (UInt32)(currPtr - packet - 1), 1, "Option Type", t, DHCPOptionGetName(t));
				if (t == 255)
				{
					if (currPtr < endPtr)
					{
						frame->AddHexBuff(frameOfst + (UInt32)(currPtr - packet), (UInt32)(endPtr - currPtr), "Padding", currPtr, true);
					}
					break;
				}
				if (currPtr >= endPtr)
				{
					break;
				}
				len = *currPtr++;
				frame->AddUInt(frameOfst + (UInt32)(currPtr - packet - 1), 1, "Option Length", len);
				if (t == 1 && len == 4)
				{
					frame->AddIPv4(frameOfst + (UInt32)(currPtr - packet), "Subnet Mask", currPtr);
				}
				else if (t == 3 && len == 4)
				{
					frame->AddIPv4(frameOfst + (UInt32)(currPtr - packet), "Router", currPtr);
				}
				else if (t == 6 && len > 0 && (len & 3) == 0)
				{

					OSInt i = 0;
					while (i < len)
					{
						Text::StrOSInt(Text::StrConcat(sbuff, (const UTF8Char*)"DNS"), i >> 2);
						frame->AddIPv4(frameOfst + (UInt32)(currPtr - packet), (const Char*)sbuff, &currPtr[i]);
						i += 4;
					}
				}
				else if (t == 12 && len > 0)
				{
					frame->AddStrC(frameOfst + (UInt32)(currPtr - packet), len, "Host Name", currPtr);
				}
				else if (t == 15 && len > 0)
				{
					frame->AddStrC(frameOfst + (UInt32)(currPtr - packet), len, "Domain Name", currPtr);
				}
				else if (t == 50 && len == 4)
				{
					frame->AddIPv4(frameOfst + (UInt32)(currPtr - packet), "Requested IP Address", currPtr);
				}
				else if (t == 51 && len == 4)
				{
					frame->AddUInt(frameOfst + (UInt32)(currPtr - packet), 4, "IP Address Lease Time", ReadMUInt32(currPtr));
				}
				else if (t == 53 && len == 1)
				{
					vName = 0;
					switch (currPtr[0])
					{
					case 1:
						vName = "Discover";
						break;
					case 2:
						vName = "Offer";
						break;
					case 3:
						vName = "Request";
						break;
					case 5:
						vName = "ACK";
						break;
					}
					frame->AddUIntName(frameOfst + (UInt32)(currPtr - packet), 1, "DHCP Type", currPtr[0], (const UTF8Char*)vName);
				}
				else if (t == 54 && len == 4)
				{
					frame->AddIPv4(frameOfst + (UInt32)(currPtr - packet), "DHCP Server", currPtr);
				}
				else if (t == 55 && len > 0)
				{
					OSInt i;
					frame->AddText(frameOfst + (UInt32)(currPtr - packet), (const UTF8Char*)"\r\nParameter Request List:");
					i = 0;
					while (i < len)
					{
						Text::StrOSInt(Text::StrConcat(sbuff, (const UTF8Char*)"Parameter Request "), i);
						frame->AddUIntName(frameOfst + (UInt32)(currPtr - packet + i), 1, (const Char*)sbuff, currPtr[i], DHCPOptionGetName(currPtr[i]));
						i++;
					}
				}
				else if (t == 57 && len == 2)
				{
					frame->AddUInt(frameOfst + (UInt32)(currPtr - packet), 2, "Max DHCP Message Size", ReadMUInt16(currPtr));
				}
				else if (t == 58 && len == 4)
				{
					frame->AddUInt(frameOfst + (UInt32)(currPtr - packet), 4, "Renew Time", ReadMUInt32(currPtr));
				}
				else if (t == 59 && len == 4)
				{
					frame->AddUInt(frameOfst + (UInt32)(currPtr - packet), 4, "Rebinding Time", ReadMUInt32(currPtr));
				}
				else if (t == 60 && len >= 1)
				{
					frame->AddStrC(frameOfst + (UInt32)(currPtr - packet), len, "Vendor Class ID", currPtr);
				}
				else if (t == 61 && len >= 1)
				{
					frame->AddUInt(frameOfst + (UInt32)(currPtr - packet), 1, "Client ID Type", currPtr[0]);
					if (len > 1)
					{
						frame->AddHexBuff(frameOfst + (UInt32)(currPtr - packet) + 1, (UOSInt)len - 1, "Client ID", &currPtr[1], ':', false);
					}
				}
				else if (t == 66 && len >= 1)
				{
					frame->AddStrC(frameOfst + (UInt32)(currPtr - packet), len, "TFTP Server Name", currPtr);
				}
				else if (t == 81 && len >= 3)
				{
					frame->AddHex8(frameOfst + (UInt32)(currPtr - packet), "Frags", currPtr[0]);
					frame->AddUInt(frameOfst + (UInt32)(currPtr - packet) + 1, 1, "RCODE1", currPtr[1]);
					frame->AddUInt(frameOfst + (UInt32)(currPtr - packet) + 2, 1, "RCODE2", currPtr[2]);
					if (len > 3)
					{
						frame->AddStrC(frameOfst + (UInt32)(currPtr - packet + 3), (UOSInt)len - 3, "Domain Name", &currPtr[3]);
					}
				}
				else if (t == 120 && len >= 1)
				{
					frame->AddUInt(frameOfst + (UInt32)(currPtr - packet), 1, "SIP Server Encoding", currPtr[0]);
					if (currPtr[0] == 1 && len == 5)
					{
						frame->AddIPv4(frameOfst + (UInt32)(currPtr - packet) + 1, "SIP Server Address", &currPtr[1]);
					}
					else if (len > 1)
					{
						frame->AddHexBuff(frameOfst + (UInt32)(currPtr - packet) + 1, (UOSInt)len - 1, "Unknown", &currPtr[1], true);
					}
				}
				else
				{
					frame->AddHexBuff(frameOfst + (UInt32)(currPtr - packet), len, "Unknown", currPtr, true);
				}

				currPtr += len;
			}
		}
		else
		{
			frame->AddTextHexBuff(frameOfst, packetSize, packet, true);
		}
	}
	else if (srcPort == 69 || destPort == 69)
	{
		UInt16 opcode = ReadMUInt16(packet);
		UOSInt i = 2;
		UOSInt len;
		frame->AddText(frameOfst, (const UTF8Char*)"TFTP:");
		vName = 0;
		switch (opcode)
		{
		case 1:
			vName = "Read request";
			break;
		case 2:
			vName = "Write request";
			break;
		case 3:
			vName = "Data";
			break;
		case 4:
			vName = "Acknowledgment";
			break;
		case 5:
			vName = "Error";
			break;
		case 6:
			vName = "Options Acknowledgment";
			break;
		}
		frame->AddUIntName(frameOfst, 2, "Opcode", opcode, (const UTF8Char*)"vName");

		if (opcode == 1 || opcode == 2)
		{
			if (packet[packetSize - 1] == 0)
			{
				len = Text::StrCharCnt(&packet[2]);
				frame->AddField(frameOfst + 2, (UInt32)len + 1, (const UTF8Char*)"Filename", &packet[2]);
				i += len + 1;
				if (i < packetSize)
				{
					len = Text::StrCharCnt(&packet[i]);
					frame->AddField(frameOfst + (UInt32)i, (UInt32)len + 1, (const UTF8Char*)"Mode", &packet[i]);
					i += len + 1;
				}
				OSInt optId = 0;
				while (i < packetSize)
				{
					len = Text::StrCharCnt(&packet[i]);
					if (optId & 1)
					{
						Text::StrOSInt(Text::StrConcat(sbuff, (const UTF8Char*)"Value"), 1 + (optId >> 1));
					}
					else
					{
						Text::StrOSInt(Text::StrConcat(sbuff, (const UTF8Char*)"Option"), 1 + (optId >> 1));
					}
					frame->AddField(frameOfst + (UInt32)i, (UInt32)len + 1, sbuff, &packet[i]);
					i += len + 1;
					optId++;
				}
			}
		}
		if (packetSize > i)
		{
			frame->AddTextHexBuff(frameOfst + i, packetSize - i, &packet[i], true);
		}
	}
	else if (srcPort == 123 || destPort == 123) //RFC 5905
	{
		if (destPort == 123)
		{
			frame->AddText(frameOfst, (const UTF8Char*)"NTP Request:");
		}
		else
		{
			frame->AddText(frameOfst, (const UTF8Char*)"NTP Reply:");
		}
		
		if (packetSize < 48)
		{
			frame->AddTextHexBuff(frameOfst, packetSize, packet, true);
		}
		else
		{
			vName = 0;
			switch (packet[0] >> 6)
			{
			case 0:
				vName = "No warning";
				break;
			case 1:
				vName = "Last minute of the day has 61 seconds";
				break;
			case 2:
				vName = "Last minute of the day has 59 seconds";
				break;
			case 3:
				vName = "Unknown";
				break;
			}
			frame->AddUIntName(frameOfst + 0, 1, "Leap Indicator", (UOSInt)packet[0] >> 6, (const UTF8Char*)vName);
			frame->AddUInt(frameOfst + 0, 1, "Version Number", (packet[0] >> 3) & 7);
			vName = 0;
			switch (packet[0] & 7)
			{
			case 0:
				vName = "Reserved";
				break;
			case 1:
				vName = "Symmetric active";
				break;
			case 2:
				vName = "Symmetric passive";
				break;
			case 3:
				vName = "Client";
				break;
			case 4:
				vName = "Server";
				break;
			case 5:
				vName = "Broadcast";
				break;
			case 6:
				vName = "NTP Control Message";
				break;
			case 7:
				vName = "Reserved for private use";
				break;
			}
			frame->AddUIntName(frameOfst + 0, 1, "Mode", packet[0] & 7, (const UTF8Char*)vName);
			if (packet[1] == 0)
			{
				vName = "Unspecified or invalid";
			}
			else if (packet[1] == 1)
			{
				vName = "Primary Server";
			}
			else if (packet[1] < 16)
			{
				vName = "Secondary Server";
			}
			else if (packet[1] == 16)
			{
				vName = "Unsynchronized";
			}
			else
			{
				vName = "Reserved";
			}
			frame->AddUIntName(frameOfst + 1, 1, "Stratum", packet[1], (const UTF8Char*)vName);
			frame->AddUInt(frameOfst + 2, 1, "Poll", packet[2]);
			frame->AddInt(frameOfst + 3, 1, "Precision", (Int8)packet[3]);
			frame->AddFloat(frameOfst + 4, 4, "Root Delay", ReadMUInt32(&packet[4]) / 65536.0);
			frame->AddFloat(frameOfst + 8, 4, "Root Dispersion", ReadMUInt32(&packet[8]) / 65536.0);
			frame->AddHexBuff(frameOfst + 12, 4, "Reference ID", &packet[12], false);
			Data::DateTime dt;
			if (ReadNInt64(&packet[16]) == 0)
				Text::StrConcat(sbuff, (const UTF8Char*)"0");
			else
			{
				Net::NTPServer::ReadTime(&packet[16], &dt);
				dt.ToLocalTime();
				dt.ToString(sbuff, "yyyy-MM-dd HH:mm:ss.fff");
			}
			frame->AddField(frameOfst + 16, 8, (const UTF8Char*)"Reference Timestamp", sbuff);
			if (ReadNInt64(&packet[24]) == 0)
				Text::StrConcat(sbuff, (const UTF8Char*)"0");
			else
			{
				Net::NTPServer::ReadTime(&packet[24], &dt);
				dt.ToLocalTime();
				dt.ToString(sbuff, "yyyy-MM-dd HH:mm:ss.fff");
			}
			frame->AddField(frameOfst + 24, 8, (const UTF8Char*)"Origin Timestamp", sbuff);
			if (ReadNInt64(&packet[32]) == 0)
				Text::StrConcat(sbuff, (const UTF8Char*)"0");
			else
			{
				Net::NTPServer::ReadTime(&packet[32], &dt);
				dt.ToLocalTime();
				dt.ToString(sbuff, "yyyy-MM-dd HH:mm:ss.fff");
			}
			frame->AddField(frameOfst + 32, 8, (const UTF8Char*)"Receive Timestamp", sbuff);
			if (ReadNInt64(&packet[40]) == 0)
				Text::StrConcat(sbuff, (const UTF8Char*)"0");
			else
			{
				Net::NTPServer::ReadTime(&packet[40], &dt);
				dt.ToLocalTime();
				dt.ToString(sbuff, "yyyy-MM-dd HH:mm:ss.fff");
			}
			frame->AddField(frameOfst + 40, 8, (const UTF8Char*)"Transmit Timestamp", sbuff);
			if (packetSize > 48)
			{
				frame->AddTextHexBuff(frameOfst + 48, packetSize - 48, &packet[48], true);
			}
		}
	}
	else if ((srcPort == 137 || destPort == 137) && packetSize >= 12)
	{
		UInt16 qdcount = ReadMUInt16(&packet[4]);
		UInt16 ancount = ReadMUInt16(&packet[6]);
		UInt16 nscount = ReadMUInt16(&packet[8]);
		UInt16 arcount = ReadMUInt16(&packet[10]);
		frame->AddText(frameOfst, (const UTF8Char*)"NetBIOS-NS:");
		frame->AddHex16(frameOfst + 0, "NAME_TRN_ID", ReadMUInt16(&packet[0]));
		frame->AddHex16(frameOfst + 2, "Flags", ReadMUInt16(&packet[2]));
		Text::StrUInt16(sbuff, (UInt16)(packet[2] >> 7));
		frame->AddSubfield(frameOfst + 2, 2, (const UTF8Char*)"Response", sbuff);
		Text::StrUInt16(sbuff, (UInt16)((packet[2] & 0x78) >> 3));
		frame->AddSubfield(frameOfst + 2, 2, (const UTF8Char*)"OPCODE", sbuff);
		Text::StrHexByte(Text::StrConcat(sbuff, (const UTF8Char*)"0x"), (UInt8)((ReadMUInt16(&packet[2]) & 0x7F0) >> 4));
		frame->AddSubfield(frameOfst + 2, 2, (const UTF8Char*)"NMFLAGS", sbuff);
		Text::StrUInt16(sbuff, packet[3] & 0xf);
		frame->AddSubfield(frameOfst + 2, 2, (const UTF8Char*)"RCODE", sbuff);
		frame->AddUInt(frameOfst + 4, 2, "QDCOUNT", qdcount);
		frame->AddUInt(frameOfst + 6, 2, "ANCOUNT", ancount);
		frame->AddUInt(frameOfst + 8, 2, "NSCOUNT", nscount);
		frame->AddUInt(frameOfst + 10, 2, "ARCOUNT", arcount);
		UOSInt i;
		UOSInt k;
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
			k = Net::DNSClient::ParseString(sbuff, packet, i, packetSize);
			frame->AddNetBIOSName(frameOfst + i, k - i, "QUESTION_NAME", sbuff);
			i = k;
			qType = ReadMUInt16(&packet[i]);
			vName = 0;
			switch (qType)
			{
			case 0x20:
				vName = "NB";
				break;
			case 0x21:
				vName = "NBSTAT";
				break;
			}
			frame->AddUIntName(frameOfst + i, 2, "QUESTION_TYPE", qType, (const UTF8Char*)vName);

			qClass = ReadMUInt16(&packet[2 + i]);
			vName = 0;
			if (qClass == 1)
			{
				vName = "IN";
			}
			frame->AddUIntName(frameOfst + i + 2, 2, "QUESTION_CLASS", qClass, (const UTF8Char*)vName);
			i += 4;
			j++;
		}
		j = 0;
		ancount = (UInt16)(ancount + nscount + arcount);
		while (j < ancount)
		{
			k = Net::DNSClient::ParseString(sbuff, packet, i, packetSize);
			frame->AddNetBIOSName(frameOfst + i, k - i, "RR_NAME", sbuff);
			i = k;
			rrType = ReadMUInt16(&packet[i]);
			vName = 0;
			switch (rrType)
			{
			case 0x20:
				vName = "NB";
				break;
			case 0x21:
				vName = "NBSTAT";
				break;
			}
			frame->AddUIntName(frameOfst + i, 2, "RR_TYPE", rrType, (const UTF8Char*)vName);

			rrClass = ReadMUInt16(&packet[2 + i]);
			vName = 0;
			if (rrClass == 1)
			{
				vName = "IN";
			}
			frame->AddUIntName(frameOfst + i + 2, 2, "RR_CLASS", rrClass, (const UTF8Char*)vName);
			frame->AddUInt(frameOfst + i + 4, 4, "TTL", ReadMUInt32(&packet[4 + i]));
			rdLength = ReadMUInt16(&packet[8 + i]);
			frame->AddUInt(frameOfst + i + 8, 2, "RD_LENGTH", rdLength);
			i += 10;
			if (rrType == 0x20 && rdLength == 6)
			{
				frame->AddHex16(frameOfst + i, "NB_FLAGS", ReadMUInt16(&packet[i]));
				frame->AddIPv4(frameOfst + i + 2, "NB_ADDRESS", &packet[i + 2]);
			}
			else if (rrType == 0x21 && rdLength >= 1)
			{
				UOSInt nName = packet[i];
				if (nName * 18 + 43 <= rdLength)
				{
					UOSInt k;
					frame->AddUInt(frameOfst + i, 1, "Number_of_name", nName);
					k = 0;
					while (k < nName)
					{
						Text::StrUOSInt(Text::StrConcat(sbuff, (const UTF8Char*)"Name"), k);
						MemCopyNO(sbuff2, &packet[i + 1 + k * 18], 15);
						sbuff2[15] = 0;
						Text::StrRTrim(sbuff2);
						frame->AddField(frameOfst + (UInt32)i + 1 + (UInt32)k * 18, 15, sbuff, sbuff2);
						Text::StrUOSInt(Text::StrConcat(sbuff, (const UTF8Char*)"Type"), k);
						frame->AddUIntName(frameOfst + i + 1 + k * 18 + 15, 1, (const Char*)sbuff, packet[i + 1 + k * 18 + 15], Net::NetBIOSUtil::NameTypeGetName(packet[i + 1 + k * 18 + 15]));
						Text::StrUOSInt(Text::StrConcat(sbuff, (const UTF8Char*)"Flags"), k);
						frame->AddHex16(frameOfst + i + 1 + k * 18 + 16, (const Char*)sbuff, ReadMUInt16(&packet[i + 1 + k * 18 + 16]));
						k++;
					}
					frame->AddMACAddr(frameOfst + i + 1 + nName * 18, "Unit ID", &packet[i + 1 + nName * 18], true);
					frame->AddHex8(frameOfst + i + 7 + nName * 18, "Jumpers", packet[i + 7 + nName * 18]);
					frame->AddHex8(frameOfst + i + 8 + nName * 18, "Test Result", packet[i + 8 + nName * 18]);
					frame->AddHex16(frameOfst + i + 9 + nName * 18, "Version number", ReadMUInt16(&packet[i + 9 + nName * 18]));
					frame->AddHex16(frameOfst + i + 11 + nName * 18, "Period of statistics", ReadMUInt16(&packet[i + 11 + nName * 18]));
					frame->AddUInt(frameOfst + i + 13 + nName * 18, 2, "Number of CRCs", ReadMUInt16(&packet[i + 13 + nName * 18]));
					frame->AddUInt(frameOfst + i + 15 + nName * 18, 2, "Number of alignment errors", ReadMUInt16(&packet[i + 15 + nName * 18]));
					frame->AddUInt(frameOfst + i + 17 + nName * 18, 2, "Number of collision", ReadMUInt16(&packet[i + 17 + nName * 18]));
					frame->AddUInt(frameOfst + i + 19 + nName * 18, 2, "Number of send aborts", ReadMUInt16(&packet[i + 19 + nName * 18]));
					frame->AddUInt(frameOfst + i + 21 + nName * 18, 4, "Number of good sends", ReadMUInt32(&packet[i + 21 + nName * 18]));
					frame->AddUInt(frameOfst + i + 25 + nName * 18, 4, "Number of good receives", ReadMUInt32(&packet[i + 25 + nName * 18]));
					frame->AddUInt(frameOfst + i + 29 + nName * 18, 2, "Number of retransmits", ReadMUInt16(&packet[i + 29 + nName * 18]));
					frame->AddUInt(frameOfst + i + 31 + nName * 18, 2, "Number of no resource conditions", ReadMUInt16(&packet[i + 31 + nName * 18]));
					frame->AddUInt(frameOfst + i + 33 + nName * 18, 2, "Number of command blocks", ReadMUInt16(&packet[i + 33 + nName * 18]));
					frame->AddUInt(frameOfst + i + 35 + nName * 18, 2, "Number of pending sessions", ReadMUInt16(&packet[i + 35 + nName * 18]));
					frame->AddUInt(frameOfst + i + 37 + nName * 18, 2, "Max number of pending sessions", ReadMUInt16(&packet[i + 37 + nName * 18]));
					frame->AddUInt(frameOfst + i + 39 + nName * 18, 2, "Max total sessions possible", ReadMUInt16(&packet[i + 39 + nName * 18]));
					frame->AddUInt(frameOfst + i + 41 + nName * 18, 2, "Sesison data packet size", ReadMUInt16(&packet[i + 41 + nName * 18]));
					if (nName * 18 + 43 < rdLength)
					{
						frame->AddHexBuff(frameOfst + i + nName * 18 + 43, rdLength - nName * 18 - 43, "Unknown", &packet[i + nName * 18 + 43], false);
					}
				}
				else
				{
					frame->AddHexBuff(frameOfst + i, rdLength, "RDATA", &packet[i], false);
				}
			}
			else
			{
				frame->AddHexBuff(frameOfst + i, rdLength, "RDATA", &packet[i], false);
			}
			i += rdLength;
			j++;
		}
		if (packetSize > i)
		{
			frame->AddTextHexBuff(frameOfst + i, packetSize - i, &packet[i], true);
		}
	}
	else if (destPort == 138 && packetSize >= 10)
	{
		UInt8 msgType = packet[0];
		frame->AddText(frameOfst, (const UTF8Char*)"NetBIOS-DS:");
		vName = 0;
		switch (msgType)
		{
		case 0x10:
			vName = "Direct Unique Datagram";
			break;
		case 0x11:
			vName = "Direct Group Datagram";
			break;
		case 0x12:
			vName = "Broadcast Datagram";
			break;
		case 0x13:
			vName = "Datagram Error";
			break;
		case 0x14:
			vName = "Datagram Query Request";
			break;
		case 0x15:
			vName = "Datagram Positive Query Response";
			break;
		case 0x16:
			vName = "Datagram Negative Query Response";
			break;
		}
		frame->AddHex8Name(frameOfst + 0, "MSG_TYPE", msgType, (const UTF8Char*)vName);
		frame->AddHex8(frameOfst + 1, "FLAGS", packet[1]);
		frame->AddUInt(frameOfst + 2, 2, "DGM_ID", ReadMUInt16(&packet[2]));
		frame->AddIPv4(frameOfst + 4, "SOURCE_IP", &packet[4]);
		frame->AddUInt(frameOfst + 8, 2, "SOURCE_PORT", ReadMUInt16(&packet[8]));
		UOSInt i;
		UOSInt j;
		i = 10;
		switch (msgType)
		{
		case 0x10:
		case 0x11:
		case 0x12:
			frame->AddUInt(frameOfst + 10, 2, "DGM_LENGTH", ReadMUInt16(&packet[10]));
			frame->AddUInt(frameOfst + 12, 2, "PACKET_OFFSET", ReadMUInt16(&packet[12]));
			i = 14;
			j = Net::DNSClient::ParseString(sbuff, packet, i, packetSize);
			frame->AddNetBIOSName(frameOfst + i, (UInt32)(j - i), "SOURCE_NAME", sbuff);
			i = j;
			j = Net::DNSClient::ParseString(sbuff, packet, i, packetSize);
			frame->AddNetBIOSName(frameOfst + i, (UInt32)(j - i), "DESTINATION_NAME", sbuff);
			i = j;
			break;
		case 0x13:
			if (packetSize >= 19)
			{
				vName = 0;
				switch (packet[10])
				{
				case 0x82:
					vName = "Destination Name Not Present";
					break;
				case 0x83:
					vName = "Invalid Source Name Format";
					break;
				case 0x84:
					vName = "Invalid Destination Name Format";
					break;
				}
				frame->AddHex8Name(frameOfst + 10, "ERROR_CODE", packet[10], (const UTF8Char*)vName);
				i = 11;
			}
			break;
		case 0x14:
		case 0x15:
		case 0x16:
			j = Net::DNSClient::ParseString(sbuff, packet, i, packetSize);
			frame->AddNetBIOSName(frameOfst + i, (UInt32)(j - i), "DESTINATION_NAME", sbuff);
			i = j;
			break;
		}

		if (packetSize > i)
		{
			frame->AddTextHexBuff(frameOfst + i, packetSize - i, &packet[i], true);
		}
	}
	else if (srcPort == 161 || destPort == 161 || destPort == 162 || srcPort == 162)
	{
		if (packet[0] == 0x30)
		{
			frame->AddText(frameOfst, (const UTF8Char*)"SNMP:");
			Net::SNMPInfo snmp;
			Text::StringBuilderUTF8 sb;
			UOSInt i = snmp.PDUGetDetail((const UTF8Char*)"Message", packet, packetSize, 0, &sb);
			frame->AddField(frameOfst, (UInt32)i, sb.ToString(), 0);
			if (packetSize > i)
			{
				frame->AddTextHexBuff(frameOfst + i, packetSize - i, &packet[i], true);
			}
		}
	}
	else if (srcPort == 427 || destPort == 427) //RFC 2165/2608
	{
		UOSInt i;
		frame->AddText(frameOfst, (const UTF8Char*)"Service Location Protocol:");
		frame->AddUInt(frameOfst + 0, 1, "Version", packet[0]);
		vName = 0;
		switch (packet[1])
		{
		case 1:
			vName = "Service Request";
			break;
		case 2:
			vName = "Service Reply";
			break;
		case 3:
			vName = "Service Registration";
			break;
		case 4:
			vName = "Service Deregister";
			break;
		case 5:
			vName = "Service Acknowledge";
			break;
		case 6:
			vName = "Attribute Request";
			break;
		case 7:
			vName = "Attribute Reply";
			break;
		case 8:
			vName = "DA Advertisement";
			break;
		case 9:
			vName = "Service Type Request";
			break;
		case 10:
			vName = "Service Type Reply";
			break;
		case 11:
			vName = "SA Advertisement";
			break;
		}
		frame->AddUIntName(frameOfst + 1, 1, "Function-ID", packet[1], (const UTF8Char*)vName);
		i = 2;
		UInt16 len;
		UInt16 len2;
		if (packet[0] == 1)
		{
			len = ReadMUInt16(&packet[2]);
			frame->AddUInt(frameOfst + 2, 2, "Length", len);
			frame->AddHex8(frameOfst + 4, "Flags", packet[4]);
			frame->AddUInt(frameOfst + 5, 1, "Dialect", packet[5]);
			frame->AddStrC(frameOfst + 6, 2, "Language Code", &packet[6]);
			UInt16 enc;
			enc = ReadMUInt16(&packet[8]);
			frame->AddUInt(frameOfst + 8, 2, "Character Encoding", enc);
			frame->AddUInt(frameOfst + 10, 2, "Transaction Identifier", ReadMUInt16(&packet[10]));
			if (packet[1] == 1)
			{
				i = 12;
				if (i + 2 <= packetSize)
				{
					len2 = ReadMUInt16(&packet[i]);
					i += 2;
					if (len2 == 0)
					{
						frame->AddField(frameOfst + (UInt32)i - 2, 2, (const UTF8Char*)"Previous Responders", (const UTF8Char*)"");
					}
					else if (len2 + i <= len)
					{
						frame->AddStrC(frameOfst + i - 2, 2 + (UOSInt)len2, "Previous Responders", &packet[i]);
						i += len2;
					}
					else
					{
						frame->AddStrC(frameOfst + i - 2, (UOSInt)len - i + 2, "Previous Responders", &packet[i]);
						i = len;
					}
				}
				if (i + 2 <= packetSize)
				{
					len2 = ReadMUInt16(&packet[i]);
					i += 2;
					if (len2 == 0)
					{
						frame->AddField(frameOfst + (UInt32)i - 2, 2, (const UTF8Char*)"Service Request", (const UTF8Char*)"");
					}
					else if (len2 + i <= len)
					{
						frame->AddStrC(frameOfst + i - 2, 2 + (UOSInt)len2, "Service Request", &packet[i]);
						i += len2;
					}
					else
					{
						frame->AddStrC(frameOfst + i - 2, len - i + 2, "Service Request", &packet[i]);
						i = len;
					}
				}
			}
		}

		if (i < packetSize)
		{
			frame->AddTextHexBuff(frameOfst + i, packetSize - i, &packet[i], true);
		}
	}
	else if (destPort == 1700)
	{
		if (packetSize >= 12 && packet[3] < 6)
		{
			frame->AddText(frameOfst, (const UTF8Char*)"LoRa Gateway:");
			frame->AddUInt(frameOfst + 0, 1, "Protocol Version", packet[0]);
			frame->AddUInt(frameOfst + 1, 2, "Random Token", ReadMUInt16(&packet[1]));
			vName = 0;
			switch (packet[3])
			{
			case 0:
				vName = "PUSH_DATA";
				break;
			case 1:
				vName = "PUSH_ACK";
				break;
			case 2:
				vName = "PULL_DATA";
				break;
			case 3:
				vName = "PULL_RESP";
				break;
			case 4:
				vName = "PULL_ACK";
				break;
			case 5:
				vName = "TX_ACK";
				break;
			}
			frame->AddUIntName(frameOfst + 3, 1, "Protocol Version", packet[3], (const UTF8Char*)vName);
			frame->AddHexBuff(frameOfst + 4, 8, "Gateway UID", &packet[4], 0, false);

			if (packetSize > 12)
			{
				if (packet[12] == 0x7B)
				{
					frame->AddText(frameOfst + 12, (const UTF8Char*)"\r\nContent:");
					Text::StringBuilderUTF8 sb;
					Text::JSText::JSONWellFormat(&packet[12], packetSize - 12, 0, &sb);
					frame->AddField(frameOfst + 12, (UInt32)packetSize - 12, sb.ToString(), 0);
					Text::JSONBase *json = Text::JSONBase::ParseJSONStrLen(&packet[12], packetSize - 12);
					if (json)
					{
						if (json->GetType() == Text::JSONType::Object)
						{
							Text::JSONObject *jobj = (Text::JSONObject*)json;
							Text::JSONBase *jbase = jobj->GetObjectValue((const UTF8Char*)"rxpk");
							if (jbase && jbase->GetType() == Text::JSONType::Array)
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
									if (jbase && jbase->GetType() == Text::JSONType::Object)
									{
										jobj = (Text::JSONObject*)jbase;
										jbase = jobj->GetObjectValue((const UTF8Char*)"data");
										if (jbase && jbase->GetType() == Text::JSONType::StringUTF8)
										{
											Text::JSONStringUTF8 *jstr = (Text::JSONStringUTF8*)jbase;
											UOSInt dataLen;
											UInt8 *dataBuff;
											const UTF8Char *dataStr = jstr->GetValue();
											sb.ClearStr();
											sb.Append((const UTF8Char*)"\r\n");
											sb.Append(dataStr);
											sb.Append((const UTF8Char*)":");
											dataLen = b64.CalcBinSize(dataStr);
											dataBuff = MemAlloc(UInt8, dataLen);
											if (b64.DecodeBin(dataStr, dataBuff) == dataLen)
											{
												PacketLoRaMACGetDetail(dataBuff, dataLen, &sb);
											}
											else
											{
												sb.Append((const UTF8Char*)"\r\nNot base64 encoding");
											}
											frame->AddText(frameOfst + 12, sb.ToString());
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
					frame->AddTextHexBuff(frameOfst + 12, packetSize - 12, &packet[12], true);
				}
			}
		}
		else
		{
			frame->AddTextHexBuff(frameOfst, packetSize, packet, true);
		}
	}
	else if (srcPort == 1700)
	{
		if (packetSize >= 4 && packet[3] < 5)
		{
			frame->AddText(frameOfst, (const UTF8Char*)"LoRa Gateway PUSH_ACK:");
			frame->AddUInt(frameOfst + 0, 1, "Protocol Version", packet[0]);
			frame->AddUInt(frameOfst + 1, 2, "Random Token", ReadMUInt16(&packet[1]));
			vName = 0;
			switch (packet[3])
			{
			case 0:
				vName = "PUSH_DATA";
				break;
			case 1:
				vName = "PUSH_ACK";
				break;
			case 2:
				vName = "PULL_DATA";
				break;
			case 3:
				vName = "PULL_RESP";
				break;
			case 4:
				vName = "PULL_ACK";
				break;
			case 5:
				vName = "TX_ACK";
				break;
			}
			frame->AddUIntName(frameOfst + 3, 1, "Protocol Version", packet[3], (const UTF8Char*)vName);
			if (packetSize > 4)
			{
				frame->AddTextHexBuff(frameOfst + 4, packetSize - 4, &packet[4], true);
			}
		}
		else
		{
			frame->AddTextHexBuff(frameOfst, packetSize, packet, true);
		}
	}
	else if (destPort == 1900)
	{
		frame->AddText(frameOfst, (const UTF8Char*)"SSDP Request:");
		const UTF8Char *csptr = Text::StrCopyNewC(packet, packetSize);
		frame->AddField(frameOfst, (UInt32)packetSize, csptr, 0);
		Text::StrDelNew(csptr);
	}
	else if (srcPort == 1900)
	{
		frame->AddText(frameOfst, (const UTF8Char*)"SSDP Reply:");
		const UTF8Char *csptr = Text::StrCopyNewC(packet, packetSize);
		frame->AddField(frameOfst, (UInt32)packetSize, csptr, 0);
		Text::StrDelNew(csptr);
	}
	else if (destPort == 3702)
	{
		frame->AddText(frameOfst, (const UTF8Char*)"WS-Discovery Request:");
		Text::StringBuilderUTF8 sb;
		Text::HTMLUtil::XMLWellFormat(packet, packetSize, 0, &sb);
		frame->AddField(frameOfst, (UInt32)packetSize, sb.ToString(), 0);
	}
	else if (srcPort == 3702)
	{
		frame->AddText(frameOfst, (const UTF8Char*)"WS-Discovery Reply:");
		Text::StringBuilderUTF8 sb;
		Text::HTMLUtil::XMLWellFormat(packet, packetSize, 0, &sb);
		frame->AddField(frameOfst, (UInt32)packetSize, sb.ToString(), 0);
	}
	else if (destPort == 5353)
	{
		frame->AddText(frameOfst, (const UTF8Char*)"mDNS:");
		if (packetSize < 12)
		{
			frame->AddTextHexBuff(frameOfst, packetSize, packet, true);
		}
		else
		{
			PacketDNSGetDetail(packet, packetSize, frameOfst, frame);
		}
	}
	else if (srcPort == 17500 && destPort == 17500)
	{
		frame->AddText(frameOfst, (const UTF8Char*)"Dropbox LAN Sync Discovery:");
		Text::StringBuilderUTF8 sb;
		Text::JSText::JSONWellFormat(packet, packetSize, 0, &sb);
		frame->AddField(frameOfst, (UInt32)packetSize, sb.ToString(), 0);
	}
	else if (srcPort >= 1024 && destPort >= 1024)
	{
		frame->AddHexBuff(frameOfst, packetSize, "Private Packet", packet, true);		
	}
	else
	{
		frame->AddText(frameOfst, (const UTF8Char*)"Unknown Data:");
		frame->AddTextHexBuff(frameOfst, packetSize, packet, true);
	}
}

void Net::PacketAnalyzerEthernet::PacketDNSGetDetail(const UInt8 *packet, UOSInt packetSize, UInt32 frameOfst, IO::FileAnalyse::FrameDetailHandler *frame)
{
	UTF8Char sbuff[128];
	const Char *vName;
	frame->AddUInt(frameOfst + 0, 2, "ID", ReadMUInt16(&packet[0]));
	frame->AddField(frameOfst + 2, 1, (const UTF8Char*)"QR", (packet[2] & 0x80)?(const UTF8Char*)"1 (Response)":(const UTF8Char*)"0 (Request)");
	UInt8 opcode = (packet[2] & 0x78) >> 3;
	vName = 0;
	switch (opcode)
	{
	case 0:
		vName = "QUERY";
		break;
	case 1:
		vName = "IQUERY";
		break;
	case 2:
		vName = "STATUS";
		break;
	}
	frame->AddUIntName(frameOfst + 2, 1, "OPCODE", opcode, (const UTF8Char*)vName);
	frame->AddUInt(frameOfst + 2, 1, "AA", (packet[2] & 4) >> 2);
	frame->AddUInt(frameOfst + 2, 1, "TC", (packet[2] & 2) >> 1);
	frame->AddUInt(frameOfst + 2, 1, "RD", (packet[2] & 1));
	frame->AddUInt(frameOfst + 3, 1, "RA", (packet[3] & 0x80) >> 7);
	frame->AddUInt(frameOfst + 3, 1, "Z", (packet[3] & 0x70) >> 4);
	UInt8 rcode = packet[3] & 0xf;
	vName = 0;
	switch (rcode)
	{
	case 0:
		vName = "No error";
		break;
	case 1:
		vName = "Format error";
		break;
	case 2:
		vName = "Server failure";
		break;
	case 3:
		vName = "Name Error";
		break;
	case 4:
		vName = "Not Implemented";
		break;
	case 5:
		vName = "Refused";
		break;
	}
	frame->AddUIntName(frameOfst + 3, 1, "RCODE", rcode, (const UTF8Char*)vName);

	UInt16 qdcount = ReadMUInt16(&packet[4]);
	UInt16 ancount = ReadMUInt16(&packet[6]);
	UInt16 nscount = ReadMUInt16(&packet[8]);
	UInt16 arcount = ReadMUInt16(&packet[10]);
	frame->AddUInt(frameOfst + 4, 2, "QDCOUNT", qdcount);
	frame->AddUInt(frameOfst + 6, 2, "ANCOUNT", ancount);
	frame->AddUInt(frameOfst + 8, 2, "NSCOUNT", nscount);
	frame->AddUInt(frameOfst + 10, 2, "ARCOUNT", arcount);
	UOSInt i = 12;
	UInt16 j;
	UOSInt k;
	UInt16 t;
	j = 0;
	while (j < qdcount)
	{
		k = Net::DNSClient::ParseString(sbuff, packet, i, packetSize);
		frame->AddField(frameOfst + (UInt32)i, (UInt32)(k - i), (const UTF8Char*)"QNAME", sbuff);
		i = k;
		t = ReadMUInt16(&packet[i]);
		frame->AddUIntName(frameOfst + i, 2, "QTYPE", t, Net::DNSClient::TypeGetID(t));
		frame->AddUInt(frameOfst + i + 2, 2, "QCLASS", ReadMUInt16(&packet[i + 2]));

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
		UOSInt l;

		k = Net::DNSClient::ParseString(sbuff, packet, i, packetSize);
		frame->AddField(frameOfst + (UInt32)i, (UInt32)(k - i), (const UTF8Char*)"NAME", sbuff);
		i = k;
		rrType = ReadMUInt16(&packet[i]);
		rrClass = ReadMUInt16(&packet[i + 2]);
		rdLength = ReadMUInt16(&packet[i + 8]);
		frame->AddUIntName(frameOfst + i, 2, "TYPE", rrType, Net::DNSClient::TypeGetID(rrType));
		frame->AddUInt(frameOfst + i + 2, 2, "CLASS", rrClass);
		frame->AddUInt(frameOfst + i + 4, 4, "TTL", ReadMUInt32(&packet[i + 4]));
		frame->AddUInt(frameOfst + i + 8, 2, "RDLENGTH", rdLength);
		i += 10;
		switch (rrType)
		{
		case 1: // A - a host address
			frame->AddIPv4(frameOfst + i, "RDATA", &packet[i]);
			break;
		case 2: // NS - an authoritative name server
		case 5: // CNAME - the canonical name for an alias
		case 12: // PTR - a domain name pointer
			k = Net::DNSClient::ParseString(sbuff, packet, i, i + rdLength);
			frame->AddField(frameOfst + (UInt32)i, (UInt32)(k - i), (const UTF8Char*)"RDATA", sbuff);
			break;
		case 6:
			k = Net::DNSClient::ParseString(sbuff, packet, i, i + rdLength);
			frame->AddField(frameOfst + (UInt32)i, (UInt32)(k - i), (const UTF8Char*)"RDATA", sbuff);
			l = Net::DNSClient::ParseString(sbuff, packet, k, i + rdLength);
			frame->AddField(frameOfst + (UInt32)k, (UInt32)(l - k), (const UTF8Char*)"-MailAddr", sbuff);
			k = l;
			if (k + 20 <= i + rdLength)
			{
				frame->AddUInt(frameOfst + k, 4, "-SN", ReadMUInt32(&packet[k]));
				frame->AddUInt(frameOfst + k + 4, 4, "-Refresh", ReadMUInt32(&packet[k + 4]));
				frame->AddUInt(frameOfst + k + 8, 4, "-Retry", ReadMUInt32(&packet[k + 8]));
				frame->AddUInt(frameOfst + k + 12, 4, "-Expire", ReadMUInt32(&packet[k + 12]));
				frame->AddUInt(frameOfst + k + 16, 4, "-DefTTL", ReadMUInt32(&packet[k + 16]));
				k += 20;
				if (k < i + rdLength)
				{
					frame->AddTextHexBuff(frameOfst + k , i + rdLength - k, &packet[k], true);
				}
			}
			else if (k < i + rdLength)
			{
				frame->AddTextHexBuff(frameOfst + k , i + (UOSInt)rdLength - k, &packet[k], true);
			}
			break;
		case 15: // MX - mail exchange
			frame->AddUInt(frameOfst + i, 2, "Priority", ReadMUInt16(&packet[i]));
			k = Net::DNSClient::ParseString(sbuff, packet, i + 2, i + rdLength);
			frame->AddField(frameOfst + (UInt32)i + 2, (UInt32)(k - i - 2), (const UTF8Char*)"RDATA", sbuff);
			break;
		case 16: // TXT - text strings
			{
				UOSInt k = 0;
				while (k < rdLength)
				{
					if ((UOSInt)packet[i + k] + 1 + k > rdLength)
					{
						frame->AddTextHexBuff(frameOfst + i + k, rdLength - k, &packet[i + k], false);
						break;
					}
					frame->AddUInt(frameOfst + i + k, 1, "StrLen", packet[i + k]);
					frame->AddStrC(frameOfst + i + k + 1, packet[i + k], "RDATA", &packet[i + k + 1]);
					k += (UOSInt)packet[i + k] + 1;
				}
			}
			break;
		case 28: // AAAA
			{
				frame->AddIPv6(frameOfst + i, "RDATA", &packet[i]);
			}
			break;
		case 33: // SRV - 
			{
				frame->AddUInt(frameOfst + i, 2, "Priority", ReadMUInt16(&packet[i]));
				frame->AddUInt(frameOfst + i + 2, 2, "Weight", ReadMUInt16(&packet[i + 2]));
				frame->AddUInt(frameOfst + i + 4, 2, "Port", ReadMUInt16(&packet[i + 4]));
				k = Net::DNSClient::ParseString(sbuff, packet, i + 6, i + rdLength);
				frame->AddField(frameOfst + (UInt32)i + 6, (UInt32)(k - i - 6), (const UTF8Char*)"Target", sbuff);
			}
			break;
		case 41: // OPT - 
			{
				frame->AddUInt(frameOfst + i, 2, "OPTION-CODE", ReadMUInt16(&packet[i]));
				frame->AddUInt(frameOfst + i + 2, 2, "OPTION-LENGTH", ReadMUInt16(&packet[i + 2]));
				frame->AddHexBuff(frameOfst + i + 4, (UInt32)rdLength - 4, "OPTION-DATA", &packet[i + 4], false);
			}
			break;
		case 43: // DS - Delegation signer
			{
				frame->AddUInt(frameOfst + i, 2, "Key Tag", ReadMUInt16(&packet[i]));
				frame->AddUInt(frameOfst + i + 2, 1, "Algorithm", packet[i + 2]);
				frame->AddUInt(frameOfst + i + 3, 1, "Digest Type", packet[i + 3]);
				frame->AddHexBuff(frameOfst + i + 4, (UInt32)rdLength - 4, "Digest", &packet[i + 4], false);
			}
			break;
		case 46: // RRSIG - DNSSEC signature
			{
				frame->AddUInt(frameOfst + i, 2, "Type Covered", ReadMUInt16(&packet[i]));
				frame->AddUInt(frameOfst + i + 2, 1, "Algorithm", packet[i + 2]);
				frame->AddUInt(frameOfst + i + 3, 1, "Labels", packet[i + 3]);
				frame->AddUInt(frameOfst + i + 4, 4, "Original TTL", ReadMUInt32(&packet[i + 4]));
				frame->AddUInt(frameOfst + i + 8, 4, "Signature Expiration", ReadMUInt32(&packet[i + 8]));
				frame->AddUInt(frameOfst + i + 12, 4, "Signature Inception", ReadMUInt32(&packet[i + 12]));
				frame->AddUInt(frameOfst + i + 16, 2, "Key Tag", ReadMUInt16(&packet[i + 16]));
				UOSInt nameLen = Text::StrCharCnt(&packet[i + 18]);
				frame->AddField(frameOfst + (UInt32)i + 18, (UInt32)nameLen + 1, (const UTF8Char*)"Signer's Name", &packet[i + 18]);
				frame->AddHexBuff(frameOfst + i + 19 + nameLen, (UOSInt)rdLength - 19 - nameLen, "Signature", &packet[i + 19 + nameLen], false);
			}
			break;
		case 47: // NSEC - Next Secure record
			{
				UOSInt k = Net::DNSClient::ParseString(sbuff, packet, i, i + rdLength);
				frame->AddField(frameOfst + (UInt32)i, (UInt32)(k - i), (const UTF8Char*)"Next Domain Name", sbuff);
				if (k < i + rdLength)
				{
					frame->AddHexBuff(frameOfst + k, i + rdLength - k, "RDATA", &packet[k], false);
				}
			}
			break;
		case 48: // DNSKEY - DNS Key record
			{
				frame->AddUInt(frameOfst + i, 2, "Flags", ReadMUInt16(&packet[i]));
				frame->AddUInt(frameOfst + i + 2, 1, "Protocol", packet[i + 2]);
				frame->AddUInt(frameOfst + i + 3, 1, "Algorithm", packet[i + 3]);
				frame->AddHexBuff(frameOfst + i + 4, (UOSInt)rdLength - 4, "Public Key", &packet[i + 4], false);
			}
			break;
		case 250: // TSIG
			{
				UOSInt k = Net::DNSClient::ParseString(sbuff, packet, i, i + rdLength);
				frame->AddField(frameOfst + (UInt32)i, (UInt32)(k - i), (const UTF8Char*)"Algorithm", sbuff);
				if (k + 10 < i + rdLength)
				{
					Data::DateTime dt;
					dt.SetUnixTimestamp((Int64)(((UInt64)(ReadMUInt16(&packet[k])) << 32) | ReadMUInt32(&packet[k + 2])));
					dt.ToLocalTime();
					dt.ToString(sbuff, "yyyy-MM-dd HH:mm:ss zzzz");
					frame->AddField(frameOfst + (UInt32)k, 6, (const UTF8Char*)"Time Signed", sbuff);
					frame->AddUInt(frameOfst + k + 6, 2, "Fudge", ReadMUInt16(&packet[k + 6]));
					UOSInt macSize = ReadMUInt16(&packet[k + 8]);
					frame->AddUInt(frameOfst + k + 8, 2, "MAC Size", macSize);
					k += 10;
					if (macSize > 0 && k + macSize <= i + rdLength)
					{
						frame->AddHexBuff(frameOfst + k, macSize, "MAC", &packet[k], false);
						k += macSize;
					}
					if (k + 6 <= i + rdLength)
					{
						frame->AddUInt(frameOfst + k + 0, 2, "Original Id", ReadMUInt16(&packet[k + 0]));
						frame->AddUInt(frameOfst + k + 2, 2, "Error", ReadMUInt16(&packet[k + 2]));
						frame->AddUInt(frameOfst + k + 4, 2, "Other Len", ReadMUInt16(&packet[k + 4]));
						k += 6;
					}
					if (k < i + rdLength)
					{
						frame->AddHexBuff(frameOfst + k, i + rdLength - k, "Other", &packet[k], false);
					}
				}
				else
				{
					frame->AddText(frameOfst + (UInt32)k, (const UTF8Char*)"RDDATA:");
					frame->AddTextHexBuff(frameOfst + k, i + rdLength - k, &packet[k], false);
				}
				
			}
			break;
		default:
			frame->AddText(frameOfst + (UInt32)i, (const UTF8Char*)"RDDATA:");
			frame->AddTextHexBuff(frameOfst + i, rdLength, &packet[i], false);
			break;
		}
		i += rdLength;
		j++;
	}
	if (i < packetSize)
	{
		frame->AddTextHexBuff(frameOfst + i, packetSize - i, &packet[i], false);
	}
}

void Net::PacketAnalyzerEthernet::PacketLoRaMACGetDetail(const UInt8 *packet, UOSInt packetSize, UInt32 frameOfst, IO::FileAnalyse::FrameDetailHandler *frame)
{
	const Char *vName = 0;
	switch (packet[0] >> 5)
	{
	case 0:
		vName = "Join-request";
		break;
	case 1:
		vName = "Join-accept";
		break;
	case 2:
		vName = "Unconfirmed Data Up";
		break;
	case 3:
		vName = "Unconfirmed Data Down";
		break;
	case 4:
		vName = "Confirmed Data Up";
		break;
	case 5:
		vName = "Confirmed Data Down";
		break;
	case 6:
		vName = "Rejoin-request";
		break;
	case 7:
		vName = "Proprietary";
		break;
	}
	frame->AddUIntName(frameOfst + 0, 1, "Message type (MType)", (UInt16)(packet[0] >> 5), (const UTF8Char*)vName);
	frame->AddUInt(frameOfst + 0, 1, "RFU", (packet[0] >> 2) & 7);
	frame->AddUInt(frameOfst + 0, 1, "Major", packet[0] & 3);
	UInt8 mType = (UInt8)(packet[0] >> 5);
	if (mType == 0 || mType == 1 || mType == 6)
	{
		if (packetSize == 23)
		{
			UInt8 buff[8];
			buff[0] = packet[8];
			buff[1] = packet[7];
			buff[2] = packet[6];
			buff[3] = packet[5];
			buff[4] = packet[4];
			buff[5] = packet[3];
			buff[6] = packet[2];
			buff[7] = packet[1];
			frame->AddHexBuff(frameOfst + 1, 8, "JoinEUI", buff, 0, false);
			buff[0] = packet[16];
			buff[1] = packet[15];
			buff[2] = packet[14];
			buff[3] = packet[13];
			buff[4] = packet[12];
			buff[5] = packet[11];
			buff[6] = packet[10];
			buff[7] = packet[9];
			frame->AddHexBuff(frameOfst + 1, 8, "DevEUI", buff, 0, false);
			frame->AddUInt(frameOfst + 17, 2, "DevNonce", ReadUInt16(&packet[17]));
		}
		else
		{
			frame->AddHexBuff(frameOfst + 1, packetSize - 5, "MACPayload", &packet[1], true);
		}
	}
	else
	{
		frame->AddHexBuff(frameOfst + 1, packetSize - 5, "MACPayload", &packet[1], true);
	}
	frame->AddHexBuff(frameOfst + packetSize - 4, 4, "MIC", &packet[packetSize - 4], false);
}


UOSInt Net::PacketAnalyzerEthernet::HeaderIPv4GetDetail(const UInt8 *packet, UOSInt packetSize, UInt32 frameOfst, IO::FileAnalyse::FrameDetailHandler *frame)
{
	frame->AddField(frameOfst + 0, 1, (const UTF8Char*)"Version", (const UTF8Char*)"4");
	frame->AddUInt(frameOfst + 0, 1, "Internet Header Length", (UInt16)packet[0] & 0xf);
	frame->AddUInt(frameOfst + 1, 1, "DSCP", (UInt16)(packet[1] >> 2));
	frame->AddUInt(frameOfst + 1, 1, "ECN", (UInt16)packet[1] & 0x3);
	frame->AddUInt(frameOfst + 2, 2, "Total Size", ReadMUInt16(&packet[2]));
	frame->AddUInt(frameOfst + 4, 2, "Identification", ReadMUInt16(&packet[4]));
	frame->AddUInt(frameOfst + 6, 2, "Flags", (UInt16)(packet[6] >> 5));
	frame->AddUInt(frameOfst + 6, 2, "Fragment Offset", ReadMUInt16(&packet[6]) & 0x1fff);
	frame->AddUInt(frameOfst + 8, 1, "TTL", packet[8]);
	frame->AddUInt(frameOfst + 9, 1, "Protocol", packet[9]);
	frame->AddHex16(frameOfst + 10, "Header Checksum", ReadMUInt16(&packet[10]));
	frame->AddIPv4(frameOfst + 12, "SrcIP", &packet[12]);
	frame->AddIPv4(frameOfst + 16, "DestIP", &packet[16]);

	if ((packet[0] & 0xf) <= 5)
	{
		return 20;
	}
	else
	{
		frame->AddHexBuff(frameOfst + 20, (UInt32)((packet[0] & 0xf) << 2) - 20, "Options", &packet[20], true);
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

const UTF8Char *Net::PacketAnalyzerEthernet::EtherTypeGetName(UInt16 etherType)
{
	switch (etherType)
	{
	case 0x0004:
		return (const UTF8Char*)"IEEE802.2 LLC";
	case 0x0006: //ARP
		return (const UTF8Char*)"ARP";
	case 0x26: //Legnth = 0x26 (IEEE802.2 LLC)
		return (const UTF8Char*)"IEEE802.2 LLC";
	case 0x0800: //IPv4
		return (const UTF8Char*)"IPv4";
	case 0x0806: //ARP
		return (const UTF8Char*)"ARP";
	case 0x86DD: //IPv6
		return (const UTF8Char*)"IPv6";
	case 0x8874: //broadcom
		return (const UTF8Char*)"Broadcom";
	}
	return 0;
}
