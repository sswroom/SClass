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

Bool Net::PacketAnalyzerEthernet::PacketNullGetName(const UInt8 *packet, UOSInt packetSize, Text::StringBuilderUTF8 *sb)
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

Bool Net::PacketAnalyzerEthernet::PacketEthernetGetName(const UInt8 *packet, UOSInt packetSize, Text::StringBuilderUTF8 *sb)
{
	return PacketEthernetDataGetName(ReadMUInt16(&packet[12]), &packet[14], packetSize - 14, sb);
}

Bool Net::PacketAnalyzerEthernet::PacketLinuxGetName(const UInt8 *packet, UOSInt packetSize, Text::StringBuilderUTF8 *sb)
{
	return PacketEthernetDataGetName(ReadMUInt16(&packet[14]), &packet[16], packetSize - 16, sb);
}

Bool Net::PacketAnalyzerEthernet::PacketEthernetDataGetName(UInt16 etherType, const UInt8 *packet, UOSInt packetSize, Text::StringBuilderUTF8 *sb)
{
	switch (etherType)
	{
	case 0x0004: //IEEE 802.2 LLC
		sb->AppendC(UTF8STRC("IEEE802.2 LLC"));
		return true;
	case 0x0006: //ARP
		sb->AppendC(UTF8STRC("ARP"));
		return true;
	case 0x26:
		sb->AppendC(UTF8STRC("IEEE802.2 LLC"));
		return true;
	case 0x0800: //IPv4
		return PacketIPv4GetName(packet, packetSize, sb);
	case 0x0806: //ARP
		sb->AppendC(UTF8STRC("ARP"));
		return true;
	case 0x86DD: //IPv6
		return PacketIPv6GetName(packet, packetSize, sb);
	default:
		return false;
	}
}

Bool Net::PacketAnalyzerEthernet::PacketIPv4GetName(const UInt8 *packet, UOSInt packetSize, Text::StringBuilderUTF8 *sb)
{
	UTF8Char sbuff[32];
	UTF8Char *sptr;
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

	sptr = Net::SocketUtil::GetIPv4Name(sbuff, srcIP);
	sb->AppendC(sbuff, (UOSInt)(sptr - sbuff));
	sb->AppendC(UTF8STRC(" -> "));
	sptr = Net::SocketUtil::GetIPv4Name(sbuff, destIP);
	sb->AppendC(sbuff, (UOSInt)(sptr - sbuff));
	sb->AppendUTF8Char(' ');
	return PacketIPDataGetName(packet[9], ipData, ipDataSize, sb);
}

Bool Net::PacketAnalyzerEthernet::PacketIPv6GetName(const UInt8 *packet, UOSInt packetSize, Text::StringBuilderUTF8 *sb)
{
	UTF8Char sbuff[64];
	UTF8Char *sptr;
	if ((packet[0] & 0xf0) != 0x60 || packetSize < 40)
	{
		return false;
	}

	Net::SocketUtil::AddressInfo srcAddr;
	Net::SocketUtil::AddressInfo destAddr;
	Net::SocketUtil::SetAddrInfoV6(&srcAddr, &packet[8], 0);
	Net::SocketUtil::SetAddrInfoV6(&destAddr, &packet[24], 0);
	sptr = Net::SocketUtil::GetAddrName(sbuff, &srcAddr);
	sb->AppendC(sbuff, (UOSInt)(sptr - sbuff));
	sb->AppendC(UTF8STRC(" -> "));
	sptr = Net::SocketUtil::GetAddrName(sbuff, &destAddr);
	sb->AppendC(sbuff, (UOSInt)(sptr - sbuff));
	sb->AppendUTF8Char(' ');
	return PacketIPDataGetName(packet[6], &packet[40], packetSize - 40, sb);
}

Bool Net::PacketAnalyzerEthernet::PacketIPDataGetName(UInt8 protocol, const UInt8 *packet, UOSInt packetSize, Text::StringBuilderUTF8 *sb)
{
	switch (protocol)
	{
	case 0:
		sb->AppendC(UTF8STRC("HOPOPT"));
		return true;
	case 1:
		sb->AppendC(UTF8STRC("ICMP"));
		return true;
	case 2:
		sb->AppendC(UTF8STRC("IGMP"));
		return true;
	case 3:
		sb->AppendC(UTF8STRC("GGP"));
		return true;
	case 4:
		sb->AppendC(UTF8STRC("IP-in-IP"));
		return true;
	case 5:
		sb->AppendC(UTF8STRC("ST"));
		return true;
	case 6:
		sb->AppendC(UTF8STRC("TCP"));
		return true;
	case 17:
	{
		sb->AppendC(UTF8STRC("UDP"));
		if (packetSize >= 4)
		{
			UInt16 destPort = 0;
			Text::CString cstr;
			destPort = ReadMUInt16(&packet[2]);
			cstr = UDPPortGetName(destPort);
			sb->AppendUTF8Char(' ');
			if (cstr.v)
			{
				sb->Append(cstr);
			}
			else
			{
				sb->AppendU16(destPort);
			}
		}
		return true;
	}
	case 58:
		sb->AppendC(UTF8STRC("ICMPv6"));
		return true;
	default:
		return false;
	}
}

void Net::PacketAnalyzerEthernet::PacketNullGetDetail(const UInt8 *packet, UOSInt packetSize, Text::StringBuilderUTF8 *sb)
{
	IO::FileAnalyse::SBFrameDetail frame(sb);
	PacketNullGetDetail(packet, packetSize, 0, &frame);
}

void Net::PacketAnalyzerEthernet::PacketEthernetGetDetail(const UInt8 *packet, UOSInt packetSize, Text::StringBuilderUTF8 *sb)
{
	IO::FileAnalyse::SBFrameDetail frame(sb);
	PacketNullGetDetail(packet, packetSize, 0, &frame);
}

void Net::PacketAnalyzerEthernet::PacketLinuxGetDetail(const UInt8 *packet, UOSInt packetSize, Text::StringBuilderUTF8 *sb)
{
	IO::FileAnalyse::SBFrameDetail frame(sb);
	PacketNullGetDetail(packet, packetSize, 0, &frame);
}

void Net::PacketAnalyzerEthernet::PacketEthernetDataGetDetail(UInt16 etherType, const UInt8 *packet, UOSInt packetSize, Text::StringBuilderUTF8 *sb)
{
	IO::FileAnalyse::SBFrameDetail frame(sb);
	PacketEthernetDataGetDetail(etherType, packet, packetSize, 0, &frame);
}

void Net::PacketAnalyzerEthernet::PacketIEEE802_2LLCGetDetail(const UInt8 *packet, UOSInt packetSize, Text::StringBuilderUTF8 *sb)
{
	IO::FileAnalyse::SBFrameDetail frame(sb);
	PacketIEEE802_2LLCGetDetail(packet, packetSize, 0, &frame);
}

void Net::PacketAnalyzerEthernet::PacketIPv4GetDetail(const UInt8 *packet, UOSInt packetSize, Text::StringBuilderUTF8 *sb)
{
	IO::FileAnalyse::SBFrameDetail frame(sb);
	PacketIPv4GetDetail(packet, packetSize, 0, &frame);
}

void Net::PacketAnalyzerEthernet::PacketIPv6GetDetail(const UInt8 *packet, UOSInt packetSize, Text::StringBuilderUTF8 *sb)
{
	IO::FileAnalyse::SBFrameDetail frame(sb);
	PacketIPv6GetDetail(packet, packetSize, 0, &frame);
}

void Net::PacketAnalyzerEthernet::PacketIPDataGetDetail(UInt8 protocol, const UInt8 *packet, UOSInt packetSize, Text::StringBuilderUTF8 *sb)
{
	IO::FileAnalyse::SBFrameDetail frame(sb);
	PacketIPDataGetDetail(protocol, packet, packetSize, 0, &frame);
}

void Net::PacketAnalyzerEthernet::PacketUDPGetDetail(UInt16 srcPort, UInt16 destPort, const UInt8 *packet, UOSInt packetSize, Text::StringBuilderUTF8 *sb)
{
	IO::FileAnalyse::SBFrameDetail frame(sb);
	PacketUDPGetDetail(srcPort, destPort, packet, packetSize, 0, &frame);
}

void Net::PacketAnalyzerEthernet::PacketDNSGetDetail(const UInt8 *packet, UOSInt packetSize, Text::StringBuilderUTF8 *sb)
{
	IO::FileAnalyse::SBFrameDetail frame(sb);
	PacketDNSGetDetail(packet, packetSize, 0, &frame);
}

void Net::PacketAnalyzerEthernet::PacketLoRaMACGetDetail(const UInt8 *packet, UOSInt packetSize, Text::StringBuilderUTF8 *sb)
{
	IO::FileAnalyse::SBFrameDetail frame(sb);
	PacketLoRaMACGetDetail(packet, packetSize, 0, &frame);
}

UOSInt Net::PacketAnalyzerEthernet::HeaderIPv4GetDetail(const UInt8 *packet, UOSInt packetSize, Text::StringBuilderUTF8 *sb)
{
	IO::FileAnalyse::SBFrameDetail frame(sb);
	return HeaderIPv4GetDetail(packet, packetSize, 0, &frame);
}

void Net::PacketAnalyzerEthernet::PacketNullGetDetail(const UInt8 *packet, UOSInt packetSize, UInt32 frameOfst, IO::FileAnalyse::FrameDetailHandler *frame)
{
	UInt32 packetType = ReadMUInt32(packet);
	frame->AddUInt(frameOfst, 4, CSTR("Packet Type"), packetType);
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
	frame->AddMACAddr(frameOfst + 0, CSTR("DestMAC"), &packet[0], true);
	frame->AddMACAddr(frameOfst + 6, CSTR("SrcMAC"), &packet[6], true);
	UInt16 etherType = ReadMUInt16(&packet[12]);
	frame->AddHex16Name(frameOfst + 12, CSTR("EtherType"), etherType, EtherTypeGetName(etherType));
	PacketEthernetDataGetDetail(etherType, &packet[14], packetSize - 14, frameOfst + 14, frame);
}

void Net::PacketAnalyzerEthernet::PacketLinuxGetDetail(const UInt8 *packet, UOSInt packetSize, UInt32 frameOfst, IO::FileAnalyse::FrameDetailHandler *frame)
{
	Text::CString vName;
	UInt16 v;
	v = ReadMUInt16(&packet[0]);
	vName = CSTR_NULL;
	switch (v)
	{
	case 0:
		vName = {UTF8STRC("Unicast to us")};
		break;
	case 1:
		vName = {UTF8STRC("Broadcast to us")};
		break;
	case 2:
		vName = {UTF8STRC("Multicast to us")};
		break;
	case 3:
		vName = {UTF8STRC("Sent by somebody else to somebody else")};
		break;
	case 4:
		vName = {UTF8STRC("Sent by us")};
		break;
	}
	frame->AddUIntName(frameOfst + 0, 2, CSTR("Packet Type"), v, vName);
	vName = CSTR_NULL;
	v = ReadMUInt16(&packet[2]);
	switch (v)
	{
	case 772:
		vName = {UTF8STRC("Link Layer Address Type")};
		break;
	case 778:
		vName = {UTF8STRC("IP GRE Protocol Type")};
		break;
	case 803:
		vName = {UTF8STRC("IEEE802.11")};
		break;
	}
	frame->AddUIntName(frameOfst + 2, 2, CSTR("Link-Layer Device Type"), v, vName);
	UInt16 len = ReadMUInt16(&packet[4]);
	frame->AddUInt(frameOfst + 4, 2, CSTR("Link-Layer Address Length"), len);
	if (len > 0)
	{
		if (len == 6)
		{
			frame->AddMACAddr(frameOfst + 6, CSTR("Link-Layer Address"), &packet[6], true);
		}
		else
		{
			if (len > 8)
			{
				frame->AddHexBuff(frameOfst + 6, 8, CSTR("Link-Layer Address"), &packet[6], false);
			}
			else
			{
				frame->AddHexBuff(frameOfst + 6, len, CSTR("Link-Layer Address"), &packet[6], false);
			}
		}
	}
	UInt16 etherType = ReadMUInt16(&packet[14]);
	frame->AddHex16Name(frameOfst + 14, CSTR("EtherType"), etherType, EtherTypeGetName(etherType));
	PacketEthernetDataGetDetail(etherType, &packet[16], packetSize - 16, frameOfst + 16, frame);
}

void Net::PacketAnalyzerEthernet::PacketEthernetDataGetDetail(UInt16 etherType, const UInt8 *packet, UOSInt packetSize, UInt32 frameOfst, IO::FileAnalyse::FrameDetailHandler *frame)
{
	switch (etherType)
	{
	case 0x0004: //IEEE802.2 LLC
		frame->AddFieldSeperstor(frameOfst, CSTR("IEEE802.2 LLC:"));
		PacketIEEE802_2LLCGetDetail(packet, packetSize, frameOfst, frame);
		return;
	case 0x26: //Legnth = 0x26 (IEEE802.2 LLC)
		frame->AddFieldSeperstor(frameOfst, CSTR("IEEE802.2 LLC:"));
		PacketIEEE802_2LLCGetDetail(packet, packetSize, frameOfst, frame);
		return;
	case 0x0800: //IPv4
		frame->AddFieldSeperstor(frameOfst, CSTR("IPv4:"));
		PacketIPv4GetDetail(packet, packetSize, frameOfst, frame);
		return;
	case 0x0806: //ARP
		frame->AddFieldSeperstor(frameOfst, CSTR("ARP:"));
		PacketARPGetDetail(packet, packetSize, frameOfst, frame);
		return;
	case 0x86DD: //IPv6
		frame->AddFieldSeperstor(frameOfst, CSTR("IPv6:"));
		PacketIPv6GetDetail(packet, packetSize, frameOfst, frame);
		return;
	default:
		{
			Text::StringBuilderUTF8 sb;
			sb.AppendHexBuff(packet, packetSize, ' ', Text::LineBreakType::CRLF);
			frame->AddFieldSeperstor(frameOfst, sb.ToCString());
		}
		return;
	}
}

void Net::PacketAnalyzerEthernet::PacketIEEE802_2LLCGetDetail(const UInt8 *packet, UOSInt packetSize, UInt32 frameOfst, IO::FileAnalyse::FrameDetailHandler *frame)
{
	Text::CString vName;
	UTF8Char sbuff[32];
	UTF8Char *sptr;
	frame->AddHex8Name(frameOfst + 0, CSTR("DSAP Address"), packet[0], LSAPGetName(packet[0]));
	frame->AddHex8Name(frameOfst + 1, CSTR("SSAP Address"), packet[1], LSAPGetName(packet[1]));
	switch (packet[1])
	{
	case 0x42: //Spanning Tree Protocol (STP)
		if (packetSize >= 38)
		{
			UInt16 protoId;
			frame->AddFieldSeperstor(frameOfst + 2, CSTR("Spanning Tree Protocol:"));
			frame->AddHex8(frameOfst + 2, CSTR("Control"), packet[2]);
			protoId = ReadMUInt16(&packet[3]);
			vName = CSTR_NULL;
			switch (protoId)
			{
			case 0:
				vName = {UTF8STRC("IEEE 802.1D")};
				break;
			}
			frame->AddHex16Name(frameOfst + 3, CSTR("Protocol ID"), protoId, vName);
			vName = CSTR_NULL;
			switch (packet[5])
			{
			case 0:
				vName = {UTF8STRC("Config & TCN")};
				break;
			case 2:
				vName = {UTF8STRC("RST")};
				break;
			case 3:
				vName = {UTF8STRC("MST")};
				break;
			case 4:
				vName = {UTF8STRC("SPT")};
				break;
			}
			frame->AddHex8Name(frameOfst + 5, CSTR("Version ID"), packet[5], vName);
			vName = CSTR_NULL;
			switch (packet[6])
			{
			case 0:
				vName = {UTF8STRC("STP Config BPDU")};
				break;
			case 2:
				vName = {UTF8STRC("RST/MST Config BPDU")};
				break;
			case 0x80:
				vName = {UTF8STRC("TCN BPDU")};
				break;
			}
			frame->AddHex8Name(frameOfst + 6, CSTR("BPDU Type"), packet[6], vName);
			frame->AddHex8(frameOfst + 7, CSTR("Flags"), packet[7]);
			frame->AddHex16(frameOfst + 8, CSTR("Root Bridge"), ReadMUInt16(&packet[8]));
			sptr = Text::StrUInt16(sbuff, (UInt16)(packet[8] >> 4));
			frame->AddSubfield(frameOfst + 8, 2, CSTR("Root Bridge Priority"), CSTRP(sbuff, sptr));
			sptr = Text::StrUInt16(sbuff, ReadMUInt16(&packet[8]) & 0xfff);
			frame->AddSubfield(frameOfst + 8, 2, CSTR("Root Bridge System ID Extension"), CSTRP(sbuff, sptr));
			frame->AddMACAddr(frameOfst + 10, CSTR("Root Bridge MAC Address"), &packet[10], false);
			frame->AddUInt(frameOfst + 16, 4, CSTR("Root Path Cost"), ReadMUInt32(&packet[16]));
			frame->AddHex16(frameOfst + 20, CSTR("Bridge"), ReadMUInt16(&packet[20]));
			sptr = Text::StrUInt16(sbuff, (UInt16)(packet[20] >> 4));
			frame->AddSubfield(frameOfst + 20, 2, CSTR("Bridge Priority"), CSTRP(sbuff, sptr));
			sptr = Text::StrUInt16(sbuff, ReadMUInt16(&packet[20]) & 0xfff);
			frame->AddSubfield(frameOfst + 20, 2, CSTR("Bridge System ID Extension"), CSTRP(sbuff, sptr));
			frame->AddMACAddr(frameOfst + 22, CSTR("Bridge MAC Address"), &packet[22], false);
			frame->AddUInt(frameOfst + 28, 2, CSTR("Port ID"), ReadMUInt16(&packet[28]));
			frame->AddFloat(frameOfst + 30, 2, CSTR("Message Age"), ReadMUInt16(&packet[30]) / 256.0);
			frame->AddFloat(frameOfst + 32, 2, CSTR("Max Age"), ReadMUInt16(&packet[32]) / 256.0);
			frame->AddFloat(frameOfst + 34, 2, CSTR("Hello Time"), ReadMUInt16(&packet[34]) / 256.0);
			frame->AddFloat(frameOfst + 36, 2, CSTR("Forward Delay"), ReadMUInt16(&packet[36]) / 256.0);
			if (packetSize > 38)
			{
				Text::StringBuilderUTF8 sb;
				sb.AppendHexBuff(&packet[38], packetSize - 38, ' ', Text::LineBreakType::CRLF);
				frame->AddFieldSeperstor(frameOfst + 38, sb.ToCString());
			}
		}
		break;
	default:
		{
			Text::StringBuilderUTF8 sb;
			sb.AppendHexBuff(&packet[2], packetSize - 2, ' ', Text::LineBreakType::CRLF);
			frame->AddFieldSeperstor(frameOfst + 2, sb.ToCString());
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
		frame->AddText(frameOfst, sb.ToCString());
	}
	else
	{
		Text::CString vName;
		UInt16 htype = ReadMUInt16(&packet[0]);
		UInt16 ptype = ReadMUInt16(&packet[2]);
		UInt8 hlen = packet[4];
		UInt8 plen = packet[5];
		UInt16 oper = ReadMUInt16(&packet[6]);
		frame->AddUInt(frameOfst + 0, 2, CSTR("Hardware Type (HTYPE)"), htype);
		frame->AddHex16(frameOfst + 2, CSTR("Protocol Type (PTYPE)"), ptype);
		frame->AddUInt(frameOfst + 4, 1, CSTR("Hardware address length (HLEN)"), hlen);
		frame->AddUInt(frameOfst + 5, 1, CSTR("Protocol address length (PLEN)"), plen);
		vName = CSTR_NULL;
		switch (oper)
		{
		case 1:
			vName = {UTF8STRC("Request")};
			break;
		case 2:
			vName = {UTF8STRC("Reply")};
			break;
		}
		frame->AddUIntName(frameOfst + 6, 2, CSTR("Operation (OPER)"), oper, vName);
		if (htype == 1 && ptype == 0x0800 && hlen == 6 && plen == 4 && packetSize >= 28)
		{
			frame->AddMACAddr(frameOfst + 8, CSTR("Sender hardware address (SHA)"), &packet[8], true);
			frame->AddIPv4(frameOfst + 14, CSTR("Sender protocol address (SPA)"), &packet[14]);
			frame->AddMACAddr(frameOfst + 18, CSTR("Target hardware address (THA)"), &packet[18], true);
			frame->AddIPv4(frameOfst + 24, CSTR("Target protocol address (TPA)"), &packet[24]);
			if (packetSize > 28)
			{
				frame->AddHexBuff(frameOfst + 28, packetSize - 28, CSTR("Trailer"), &packet[28], true);
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
		frame->AddText(frameOfst, CSTR("Not IPv4 Packet"));
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
	UTF8Char *sptr;
	if ((packet[0] & 0xf0) != 0x60 || packetSize < 40)
	{
		frame->AddText(frameOfst, CSTR("Not IPv6 Packet"));
		frame->AddTextHexBuff(frameOfst, packetSize, packet, true);
		return;
	}

	frame->AddHexBuff(frameOfst, 4, CSTR("Header Misc"), packet, false);
	frame->AddSubfield(frameOfst, 1, CSTR("Version"), CSTR("6"));
	sptr = Text::StrUInt16(sbuff, (UInt16)(((packet[0] & 0xf) << 2) | (packet[1] >> 6)));
	frame->AddSubfield(frameOfst, 2, CSTR("DS"), CSTRP(sbuff, sptr));
	sptr = Text::StrUInt16(sbuff, (UInt16)((packet[1] & 0x30) >> 4));
	frame->AddSubfield(frameOfst, 2, CSTR("ECN"), CSTRP(sbuff, sptr));
	sptr = Text::StrUInt32(sbuff, (UInt32)(((packet[1] & 0xf) << 16) | ReadMUInt16(&packet[2])));
	frame->AddSubfield(frameOfst + 1, 3, CSTR("Flow Label"), CSTRP(sbuff, sptr));
	frame->AddUInt(frameOfst + 4, 2, CSTR("Payload Length"), ReadMUInt16(&packet[4]));
	frame->AddUInt(frameOfst + 6, 1, CSTR("Next Header"), packet[6]);
	frame->AddUInt(frameOfst + 7, 1, CSTR("Hop Limit"), packet[7]);
	frame->AddIPv6(frameOfst + 8, CSTR("Source Address"), &packet[8]);
	frame->AddIPv6(frameOfst + 24, CSTR("Destination Address"), &packet[24]);
	PacketIPDataGetDetail(packet[6], &packet[40], packetSize - 40, frameOfst + 40, frame);
}

void Net::PacketAnalyzerEthernet::PacketIPDataGetDetail(UInt8 protocol, const UInt8 *packet, UOSInt packetSize, UInt32 frameOfst, IO::FileAnalyse::FrameDetailHandler *frame)
{
	UTF8Char sbuff[64];
	UTF8Char *sptr;
	Text::CString vName;
	switch (protocol)
	{
	case 1: //ICMP
		frame->AddText(frameOfst, CSTR("ICMP:"));
		if (packetSize >= 4)
		{
			UOSInt i = 4;
			vName = CSTR_NULL;
			switch (packet[0])
			{
			case 0:
				vName = {UTF8STRC("Echo Reply")};
				break;
			case 3:
				vName = {UTF8STRC("Destination Unreachable")};
				break;
			case 4:
				vName = {UTF8STRC("Source Quench")};
				break;
			case 5:
				vName = {UTF8STRC("Redirect Message")};
				break;
			case 6:
				vName = {UTF8STRC("Alternate Host Address")};
				break;
			case 8:
				vName = {UTF8STRC("Echo Request")};
				break;
			case 9:
				vName = {UTF8STRC("Router Advertisement")};
				break;
			case 10:
				vName = {UTF8STRC("Router Solicitation")};
				break;
			case 11:
				vName = {UTF8STRC("Time Exceeded")};
				break;
			case 12:
				vName = {UTF8STRC("Parameter Problem: Bad IP header")};
				break;
			case 13:
				vName = {UTF8STRC("Timestamp")};
				break;
			case 14:
				vName = {UTF8STRC("Timestamp Reply")};
				break;
			case 15:
				vName = {UTF8STRC("Information Request")};
				break;
			case 16:
				vName = {UTF8STRC("Information Reply")};
				break;
			case 17:
				vName = {UTF8STRC("Address Mask Request")};
				break;
			case 18:
				vName = {UTF8STRC("Address Mask Reply")};
				break;
			case 30:
				vName = {UTF8STRC("Traceroute")};
				break;
			case 42:
				vName = {UTF8STRC("Extended Echo Request")};
				break;
			case 43:
				vName = {UTF8STRC("Extended Echo Reply")};
				break;
			}
			frame->AddUIntName(frameOfst + 0, 1, CSTR("Type"), packet[0], vName);

			vName = CSTR_NULL;
			switch (packet[0])
			{
			case 3:
				switch (packet[1])
				{
				case 0:
					vName = {UTF8STRC("Destination network unreachable")};
					break;
				case 1:
					vName = {UTF8STRC("Destination host unreachable")};
					break;
				case 2:
					vName = {UTF8STRC("Destination protocol unreachable")};
					break;
				case 3:
					vName = {UTF8STRC("Destination port unreachable")};
					break;
				case 4:
					vName = {UTF8STRC("Fragmentation required")};
					break;
				case 5:
					vName = {UTF8STRC("Source route failed")};
					break;
				case 6:
					vName = {UTF8STRC("Destination network unknown")};
					break;
				case 7:
					vName = {UTF8STRC("Destination host unknown")};
					break;
				case 8:
					vName = {UTF8STRC("Source host isolated")};
					break;
				case 9:
					vName = {UTF8STRC("Network administratively prohibited")};
					break;
				case 10:
					vName = {UTF8STRC("Host administratively prohibited")};
					break;
				case 11:
					vName = {UTF8STRC("Network unreachable for ToS")};
					break;
				case 12:
					vName = {UTF8STRC("Host unreachable for ToS")};
					break;
				case 13:
					vName = {UTF8STRC("Communication administratively prohibited")};
					break;
				case 14:
					vName = {UTF8STRC("Host Precedence Violation")};
					break;
				case 15:
					vName = {UTF8STRC("Precedence cutoff in effect")};
					break;
				}
				break;
			case 5:
				switch (packet[1])
				{
				case 0:
					vName = {UTF8STRC("Redirect Datagram for the Network")};
					break;
				case 1:
					vName = {UTF8STRC("Redirect Datagram for the Host")};
					break;
				case 2:
					vName = {UTF8STRC("Redirect Datagram for the ToS & network")};
					break;
				case 3:
					vName = {UTF8STRC("Redirect Datagram for the ToS & host")};
					break;
				}
				break;
			case 11:
				switch (packet[1])
				{
				case 0:
					vName = {UTF8STRC("TTL expired in transit")};
					break;
				case 1:
					vName = {UTF8STRC("Fragment reassembly time exceeded")};
					break;
				}
				break;
			case 12:
				switch (packet[1])
				{
				case 0:
					vName ={UTF8STRC("Pointer indicates the error")};
					break;
				case 1:
					vName = {UTF8STRC("Missing a required option")};
					break;
				case 2:
					vName = {UTF8STRC("Bad length")};
					break;
				}
				break;
			case 43:
				switch (packet[1])
				{
				case 0:
					vName = {UTF8STRC("No Error")};
					break;
				case 1:
					vName = {UTF8STRC("Malformed Query")};
					break;
				case 2:
					vName = {UTF8STRC("No Such Interface")};
					break;
				case 3:
					vName = {UTF8STRC("No Such Table Entry")};
					break;
				case 4:
					vName = {UTF8STRC("Multiple Interfaces Satisfy Query")};
					break;
				}
				break;
			}
			frame->AddUIntName(frameOfst + 1, 1, CSTR("Code"), packet[1], vName);
			frame->AddHex16(frameOfst + 2, CSTR("Checksum"), ReadMUInt16(&packet[2]));
			switch (packet[0])
			{
			case 3:
				{
					frame->AddUInt(frameOfst + 4, 2, CSTR("Next-hop MTU"), ReadMUInt16(&packet[4]));
					frame->AddHexBuff(frameOfst + 6, 2, CSTR("Unused"), &packet[6], false);
					frame->AddText(frameOfst + 8, CSTR("Original IP Header:"));
					i = 8;
					UInt8 protocol = packet[i + 9];
					i += HeaderIPv4GetDetail(&packet[i], packetSize - i, frameOfst + (UInt32)i, frame);
					PacketIPDataGetDetail(protocol, &packet[i], packetSize - i, frameOfst + (UInt32)i, frame);
				}
				break;
			case 0:
			case 8:
				frame->AddUInt(frameOfst + 4, 2, CSTR("Identifier"), ReadMUInt16(&packet[4]));
				frame->AddUInt(frameOfst + 6, 2, CSTR("Sequence Number"), ReadMUInt16(&packet[6]));
				frame->AddHexBuff(frameOfst + 8, packetSize - 8, CSTR("Data"), &packet[8], true);
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
		frame->AddText(frameOfst, CSTR("IGMP:"));
		if (packet[0] == 0x11)
		{
			frame->AddField(frameOfst + 0, 1, CSTR("Type"), CSTR("0x11 (IGMPv3 membership query)"));
			if (packetSize >= 8)
			{
				UInt16 n;
				frame->AddUInt(frameOfst + 1, 1, CSTR("Max Resp Time"), packet[1]);
				frame->AddHex16(frameOfst + 2, CSTR("Checksum"), ReadMUInt16(&packet[2]));
				frame->AddIPv4(frameOfst + 4, CSTR("Group Address"), &packet[4]);
				if (packetSize >= 16)
				{
					frame->AddHex8(frameOfst + 8, CSTR("Flags"), packet[8]);
					frame->AddUInt(frameOfst + 9, 1, CSTR("QQIC"), packet[9]);
					n = ReadMUInt16(&packet[10]);
					frame->AddUInt(frameOfst + 10, 2, CSTR("QQIC"), n);
					if (packetSize >= 12 + (UOSInt)n * 4)
					{
						UInt16 i = 0;
						while (i < n)
						{
							sptr = Text::StrConcatC(Text::StrUInt16(Text::StrConcatC(sbuff, UTF8STRC("Source Address[")), i), UTF8STRC("]"));
							frame->AddIPv4(frameOfst + 12 + (UOSInt)i * 4, CSTRP(sbuff, sptr), &packet[12 + i * 4]);
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
			frame->AddField(frameOfst + 0, 1, CSTR("Type"), CSTR("0x16 (IGMPv2 Membership Report)"));
			if (packetSize >= 8)
			{
				frame->AddUInt(frameOfst + 1, 1, CSTR("Max Resp Time"), packet[1]);
				frame->AddHex16(frameOfst + 2, CSTR("Checksum"), ReadMUInt16(&packet[2]));
				frame->AddIPv4(frameOfst + 4, CSTR("Group Address"), &packet[4]);
				if (packetSize > 8)
				{
					frame->AddTextHexBuff(frameOfst + 8, packetSize - 8, &packet[8], true);
				}
			}
		}
		else
		{
			frame->AddHex8(frameOfst + 0, CSTR("Type"), packet[0]);
			frame->AddTextHexBuff(frameOfst + 1, packetSize - 1, &packet[1], true);
		}
		return;
	case 6:
	{
		frame->AddText(frameOfst, CSTR("TCP:"));
		if (packetSize < 20)
		{
			frame->AddTextHexBuff(frameOfst, packetSize, packet, true);
		}
		else
		{
			frame->AddUInt(frameOfst + 0, 2, CSTR("Source Port"), ReadMUInt16(&packet[0]));
			frame->AddUInt(frameOfst + 2, 2, CSTR("Destination Port"), ReadMUInt16(&packet[2]));
			frame->AddUInt(frameOfst + 4, 4, CSTR("Sequence Number"), ReadMUInt32(&packet[4]));
			frame->AddUInt(frameOfst + 8, 4, CSTR("Acknowledgment Number"), ReadMUInt32(&packet[8]));
			frame->AddUInt(frameOfst + 12, 1, CSTR("Data Offset"), (UInt16)(packet[12] >> 4));
			sptr = Text::StrUInt16(sbuff, packet[12] & 1);
			frame->AddSubfield(frameOfst + 12, 1, CSTR("NS"), CSTRP(sbuff, sptr));
			frame->AddHex8(frameOfst + 13, CSTR("Flags"), packet[13]);
			sptr = Text::StrUInt16(sbuff, (UInt16)((packet[13] >> 7) & 1));
			frame->AddSubfield(frameOfst + 13, 1, CSTR("CWR"), CSTRP(sbuff, sptr));
			sptr = Text::StrUInt16(sbuff, (UInt16)((packet[13] >> 6) & 1));
			frame->AddSubfield(frameOfst + 13, 1, CSTR("ECE"), CSTRP(sbuff, sptr));
			sptr = Text::StrUInt16(sbuff, (UInt16)((packet[13] >> 5) & 1));
			frame->AddSubfield(frameOfst + 13, 1, CSTR("URG"), CSTRP(sbuff, sptr));
			sptr = Text::StrUInt16(sbuff, (UInt16)((packet[13] >> 4) & 1));
			frame->AddSubfield(frameOfst + 13, 1, CSTR("ACK"), CSTRP(sbuff, sptr));
			sptr = Text::StrUInt16(sbuff, (UInt16)((packet[13] >> 3) & 1));
			frame->AddSubfield(frameOfst + 13, 1, CSTR("PSH"), CSTRP(sbuff, sptr));
			sptr = Text::StrUInt16(sbuff, (UInt16)((packet[13] >> 2) & 1));
			frame->AddSubfield(frameOfst + 13, 1, CSTR("RST"), CSTRP(sbuff, sptr));
			sptr = Text::StrUInt16(sbuff, (UInt16)((packet[13] >> 1) & 1));
			frame->AddSubfield(frameOfst + 13, 1, CSTR("SYN"), CSTRP(sbuff, sptr));
			sptr = Text::StrUInt16(sbuff, packet[13] & 1);
			frame->AddSubfield(frameOfst + 13, 1, CSTR("FIN"), CSTRP(sbuff, sptr));
			frame->AddUInt(frameOfst + 14, 2, CSTR("Window Size"), ReadMUInt16(&packet[14]));
			frame->AddHex16(frameOfst + 16, CSTR("Checksum"), ReadMUInt16(&packet[16]));
			frame->AddHex16(frameOfst + 18, CSTR("Urgent Pointer"), ReadMUInt16(&packet[18]));
			UOSInt headerLen = (UOSInt)(packet[12] >> 4) * 4;
			if (headerLen > 20)
			{
				frame->AddText(frameOfst + 20, CSTR("Options:"));
				UOSInt i = 20;
				while (i < headerLen)
				{
					vName = CSTR_NULL;
					switch (packet[i])
					{
					case 0:
						vName = {UTF8STRC("End of option list")};
						break;
					case 1:
						vName = {UTF8STRC("No operation")};
						break;
					case 2:
						vName = {UTF8STRC("No operation")};
						break;
					case 3:
						vName = {UTF8STRC("Window scale")};
						break;
					case 4:
						vName = {UTF8STRC("SACK permitted")};
						break;
					case 5:
						vName = {UTF8STRC("SACK")};
						break;
					case 8:
						vName = {UTF8STRC("Time Stamp Option")};
						break;
					}
					frame->AddUIntName(frameOfst + i, 1, CSTR("Kind"), packet[i], vName);

					switch (packet[i])
					{
					case 0:
						i = headerLen - 1;
						break;
					case 1:
						break;
					case 2:
						frame->AddUInt(frameOfst + i + 1, 1, CSTR("Length"), packet[i + 1]);
						if (packet[i + 1] == 4)
						{
							frame->AddUInt(frameOfst + i + 2, 2, CSTR("Value"), ReadMUInt16(&packet[i + 2]));
						}
						i += (UOSInt)packet[i + 1] - 1;
						break;
					case 3:
						frame->AddUInt(frameOfst + i + 1, 1, CSTR("Length"), packet[i + 1]);
						if (packet[i + 1] == 3)
						{
							frame->AddUInt(frameOfst + i + 2, 1, CSTR("Value"), packet[i + 2]);
						}
						i += (UOSInt)packet[i + 1] - 1;
						break;
					case 4:
						frame->AddUInt(frameOfst + i + 1, 1, CSTR("Length"), packet[i + 1]);
						i += (UOSInt)packet[i + 1] - 1;
						break;
					case 5:
						frame->AddUInt(frameOfst + i + 1, 1, CSTR("Length"), packet[i + 1]);
						if (packet[i + 1] == 10)
						{
							frame->AddUInt(frameOfst + i + 2, 4, CSTR("Left Edge"), ReadMUInt32(&packet[i + 2]));
							frame->AddUInt(frameOfst + i + 6, 4, CSTR("Right Edge"), ReadMUInt32(&packet[i + 6]));
						}
						i += (UOSInt)packet[i + 1] - 1;
						break;
					case 8:
						frame->AddUInt(frameOfst + i + 1, 1, CSTR("Length"), packet[i + 1]);
						if (packet[i + 1] == 10)
						{
							frame->AddUInt(frameOfst + i + 2, 4, CSTR("Timestamp value"), ReadMUInt32(&packet[i + 2]));
							frame->AddUInt(frameOfst + i + 6, 4, CSTR("Timestamp echo reply"), ReadMUInt32(&packet[i + 6]));
						}
						i += (UOSInt)packet[i + 1] - 1;
						break;
					default:
						frame->AddUInt(frameOfst + i + 1, 1, CSTR("Length"), packet[i + 1]);
						i += (UOSInt)packet[i + 1] - 1;
						break;
					}
					i++;
				}
			}
			if (packetSize > headerLen)
			{
				frame->AddHexBuff(frameOfst + headerLen, packetSize - headerLen, CSTR("Data"), &packet[headerLen], true);
			}
		}
		return;
	}
	case 17:
	{
		UInt16 srcPort = 0;
		UInt16 destPort = 0;
		UOSInt udpLen = packetSize;
		frame->AddText(frameOfst, CSTR("UDP:"));
		
		if (packetSize >= 2)
		{
			srcPort = ReadMUInt16(&packet[0]);
			frame->AddUIntName(frameOfst + 0, 2, CSTR("SrcPort"), srcPort, UDPPortGetName(srcPort));
		}
		if (packetSize >= 4)
		{
			destPort = ReadMUInt16(&packet[2]);
			frame->AddUIntName(frameOfst + 2, 2, CSTR("DestPort"), destPort, UDPPortGetName(destPort));
		}
		if (packetSize >= 6)
		{
			udpLen = ReadMUInt16(&packet[4]);
			frame->AddUInt(frameOfst + 4, 2, CSTR("Length"), udpLen);
			if (packetSize < udpLen)
				udpLen = packetSize;
		}
		if (packetSize >= 8)
		{
			frame->AddHex16(frameOfst + 6, CSTR("Checksum"), ReadMUInt16(&packet[6]));
		}
		if (packetSize > 8)
		{
			PacketUDPGetDetail(srcPort, destPort, &packet[8], udpLen - 8, frameOfst + 8, frame);
			if (packetSize > udpLen)
			{
				frame->AddHexBuff(frameOfst + udpLen, packetSize - udpLen, CSTR("Padding"), &packet[udpLen], true);
			}
		}
		return;
	}
	case 58:
		frame->AddText(frameOfst, CSTR("ICMPv6:"));
		if (packetSize >= 4)
		{
			vName = CSTR_NULL;
			switch (packet[0])
			{
			case 1:
				vName = {UTF8STRC("Destination unreachable")};
				break;
			case 2:
				vName = {UTF8STRC("Packet Too Big")};
				break;
			case 3:
				vName = {UTF8STRC("Time exceeded")};
				break;
			case 4:
				vName = {UTF8STRC("Parameter problem")};
				break;
			case 100:
				vName = {UTF8STRC("Private experimentation")};
				break;
			case 101:
				vName = {UTF8STRC("Private experimentation")};
				break;
			case 127:
				vName = {UTF8STRC("Reserved for expansion of ICMPv6 error messages")};
				break;
			case 128:
				vName = {UTF8STRC("Echo Request")};
				break;
			case 129:
				vName = {UTF8STRC("Echo Reply")};
				break;
			case 130:
				vName = {UTF8STRC("Multicast Listener Query")};
				break;
			case 131:
				vName = {UTF8STRC("Multicast Listener Report")};
				break;
			case 132:
				vName = {UTF8STRC("Multicast Listener Done")};
				break;
			case 133:
				vName = {UTF8STRC("Router Solicitation")};
				break;
			case 134:
				vName = {UTF8STRC("Router Advertisement")};
				break;
			case 135:
				vName = {UTF8STRC("Neighbor Solicitation")};
				break;
			case 136:
				vName = {UTF8STRC("Neighbor Advertisement")};
				break;
			case 137:
				vName = {UTF8STRC("Redirect Message")};
				break;
			case 138:
				vName = {UTF8STRC("Router Renumbering")};
				break;
			case 139:
				vName = {UTF8STRC("ICMP Node Information Query")};
				break;
			case 140:
				vName = {UTF8STRC("ICMP Node Information Response")};
				break;
			case 141:
				vName = {UTF8STRC("Inverse Neighbor Discovery Solicitation Message")};
				break;
			case 142:
				vName = {UTF8STRC("Inverse Neighbor Discovery Advertisement Message")};
				break;
			case 143:
				vName = {UTF8STRC("Multicast Listener Discovery (MLDv2) reports")};
				break;
			case 144:
				vName = {UTF8STRC("Home Agent Address Discovery Request Message")};
				break;
			case 145:
				vName = {UTF8STRC("Home Agent Address Discovery Reply Message")};
				break;
			case 146:
				vName = {UTF8STRC("Mobile Prefix Solicitation")};
				break;
			case 147:
				vName = {UTF8STRC("Mobile Prefix Advertisement")};
				break;
			case 148:
				vName = {UTF8STRC("Certification Path Solicitation")};
				break;
			case 149:
				vName = {UTF8STRC("Certification Path Advertisement")};
				break;
			case 151:
				vName = {UTF8STRC("Multicast Router Advertisement")};
				break;
			case 152:
				vName = {UTF8STRC("Multicast Router Solicitation")};
				break;
			case 153:
				vName = {UTF8STRC("Multicast Router Termination")};
				break;
			case 155:
				vName = {UTF8STRC("RPL Control Message")};
				break;
			case 200:
				vName = {UTF8STRC("Private experimentation")};
				break;
			case 201:
				vName = {UTF8STRC("Private experimentation")};
				break;
			case 255:
				vName = {UTF8STRC("Reserved for expansion of ICMPv6 informational messages")};
				break;
			}
			frame->AddUIntName(frameOfst + 0, 1, CSTR("Type"), packet[0], vName);

			vName = CSTR_NULL;
			switch (packet[0])
			{
			case 1: //Destination unreachable
				switch (packet[1])
				{
				case 0:
					vName = {UTF8STRC("no route to destination")};
					break;
				case 1:
					vName = {UTF8STRC("communication with destination administratively prohibited")};
					break;
				case 2:
					vName = {UTF8STRC("beyond scope of source address")};
					break;
				case 3:
					vName = {UTF8STRC("address unreachable")};
					break;
				case 4:
					vName = {UTF8STRC("port unreachable")};
					break;
				case 5:
					vName = {UTF8STRC("source address failed ingress/egress policy")};
					break;
				case 6:
					vName = {UTF8STRC("reject route to destination")};
					break;
				case 7:
					vName = {UTF8STRC("Error in Source Routing Header")};
					break;
				}
				break;
			case 3: //Time exceeded
				switch (packet[1])
				{
				case 0:
					vName = {UTF8STRC("hop limit exceeded in transit")};
					break;
				case 1:
					vName = {UTF8STRC("fragment reassembly time exceeded")};
					break;
				}
				break;
			case 4: //Parameter problem
				switch (packet[1])
				{
				case 0:
					vName = {UTF8STRC("erroneous header field encountered")};
					break;
				case 1:
					vName = {UTF8STRC("unrecognized Next Header type encountered")};
					break;
				case 2:
					vName = {UTF8STRC("unrecognized IPv6 option encountered")};
					break;
				}
				break;
			case 138: //Router Renumbering
				switch (packet[1])
				{
				case 0:
					vName = {UTF8STRC("Router Renumbering Command")};
					break;
				case 1:
					vName = {UTF8STRC("Router Renumbering Result")};
					break;
				case 255:
					vName = {UTF8STRC("Sequence Number Reset")};
					break;
				}
				break;
			case 139: //ICMP Node Information Query
				switch (packet[1])
				{
				case 0:
					vName = {UTF8STRC("The Data field contains an IPv6 address which is the Subject of this Query")};
					break;
				case 1:
					vName = {UTF8STRC("The Data field contains a name which is the Subject of this Query, or is empty, as in the case of a NOOP")};
					break;
				case 2:
					vName = {UTF8STRC("The Data field contains an IPv4 address which is the Subject of this Query")};
					break;
				}
				break;
			case 140: //ICMP Node Information Response
				switch (packet[1])
				{
				case 0:
					vName = {UTF8STRC("A successful reply. The Reply Data field may or may not be empty")};
					break;
				case 1:
					vName = {UTF8STRC("The Responder refuses to supply the answer. The Reply Data field will be empty.")};
					break;
				case 2:
					vName = {UTF8STRC("The Qtype of the Query is unknown to the Responder. The Reply Data field will be empty")};
					break;
				}
				break;
			}
			frame->AddUIntName(frameOfst + 1, 1, CSTR("Code"), packet[1], vName);
			frame->AddHex16(frameOfst + 2, CSTR("Checksum"), ReadMUInt16(&packet[2]));
			switch (packet[0])
			{
			case 134:
				if (packetSize >= 24)
				{
					frame->AddUInt(frameOfst + 4, 1, CSTR("Hop Limit"), packet[4]);
					frame->AddHex8(frameOfst + 5, CSTR("Flags"), packet[5]);
					frame->AddUInt(frameOfst + 6, 2, CSTR("Router Lifetime"), ReadMUInt16(&packet[6]));
					frame->AddUInt(frameOfst + 8, 4, CSTR("Reachable Timer"), ReadMUInt32(&packet[8]));
					frame->AddUInt(frameOfst + 12, 4, CSTR("Retains Timer"), ReadMUInt32(&packet[12]));
					UOSInt i = 16;
					while (i + 7 < packetSize)
					{
						vName = CSTR_NULL;
						switch (packet[i])
						{
						case 1:
							vName = {UTF8STRC("Source Link-layer Address")};
							break;
						case 2:
							vName = {UTF8STRC("Target Link-layer Address")};
							break;
						}
						frame->AddUIntName(frameOfst + i, 1, CSTR("Type"), packet[i], vName);
						frame->AddUInt(frameOfst + i + 1, 1, CSTR("Length"), packet[i + 1]);
						switch (packet[i])
						{
						case 1:
						case 2:
							frame->AddMACAddr(frameOfst + i + 2, CSTR("Address"), &packet[i + 2], true);
							break;
						case 3:
							frame->AddUInt(frameOfst + i + 2, 1, CSTR("Prefix Length"), packet[i + 2]);
							frame->AddHex8(frameOfst + i + 3, CSTR("Flags"), packet[i + 3]);
							frame->AddUInt(frameOfst + i + 4, 4, CSTR("Valid Lifetime"), ReadMUInt32(&packet[i + 4]));
							frame->AddUInt(frameOfst + i + 8, 4, CSTR("Preferred Lifetime"), ReadMUInt32(&packet[i + 8]));
							frame->AddUInt(frameOfst + i + 12, 4, CSTR("Reserved"), ReadMUInt32(&packet[i + 12]));
							frame->AddHexBuff(frameOfst + i + 16, (UInt32)packet[i + 1] - 16, CSTR("Prefix"), &packet[i + 16], false);
							break;
						case 5:
							frame->AddUInt(frameOfst + i + 2, 2, CSTR("Prefix Length"), ReadMUInt16(&packet[i + 2]));
							frame->AddUInt(frameOfst + i + 4, 4, CSTR("MTU"), ReadMUInt32(&packet[i + 4]));
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
					frame->AddHex32(frameOfst + 4, CSTR("Reserved"), ReadMUInt32(&packet[4]));
					frame->AddIPv4(frameOfst + 8, CSTR("Target Address"), &packet[8]);
					UOSInt i = 24;
					while (i < packetSize)
					{
						vName = CSTR_NULL;
						switch (packet[i])
						{
						case 1:
							vName = {UTF8STRC("Source Link-layer Address")};
							break;
						case 2:
							vName = {UTF8STRC("Target Link-layer Address")};
							break;
						}
						frame->AddUIntName(frameOfst + i, 1, CSTR("Type"), packet[i], vName);
						frame->AddUInt(frameOfst + i + 1, 1, CSTR("Length"), packet[i + 1]);
						frame->AddMACAddr(frameOfst + i + 2, CSTR("Address"), &packet[i + 2], true);
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
	Text::CString vName;
	UTF8Char sbuff[64];
	UTF8Char sbuff2[64];
	UTF8Char *sptr;
	UTF8Char *sptr2;
	if (destPort == 53)
	{
		frame->AddText(frameOfst, CSTR("DNS Request:"));
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
		frame->AddText(frameOfst, CSTR("DNS Reply:"));
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
		frame->AddText(frameOfst, CSTR("BOOTP (DHCP):"));
		if (packetSize < 240)
		{
			frame->AddTextHexBuff(frameOfst, packetSize, packet, true);
		}
		else if (ReadMUInt32(&packet[236]) == 0x63825363)
		{
			vName = CSTR_NULL;
			switch (packet[0])
			{
			case 1:
				vName = {UTF8STRC("Request")};
				break;
			case 2:
				vName = {UTF8STRC("Reply")};
				break;
			}
			frame->AddUIntName(frameOfst + 0, 1, CSTR("OP"), packet[0], vName);
			frame->AddUInt(frameOfst + 1, 1, CSTR("Hardware Type (HTYPE)"), packet[1]);
			frame->AddUInt(frameOfst + 2, 1, CSTR("Hardware Address Length (HLEN)"), packet[2]);
			frame->AddUInt(frameOfst + 3, 1, CSTR("HOPS"), packet[3]);
			frame->AddHex32(frameOfst + 4, CSTR("Transaction ID"), ReadMUInt32(&packet[4]));
			frame->AddUInt(frameOfst + 8, 2, CSTR("Seconds Elapsed (SECS)"), ReadMUInt16(&packet[8]));
			frame->AddHex16(frameOfst + 10, CSTR("Flags"), ReadMUInt16(&packet[10]));
			frame->AddIPv4(frameOfst + 12, CSTR("Client IP Address"), &packet[12]);
			frame->AddIPv4(frameOfst + 16, CSTR("Your IP Address"), &packet[16]);
			frame->AddIPv4(frameOfst + 20, CSTR("Server IP Address"), &packet[20]);
			frame->AddIPv4(frameOfst + 24, CSTR("Gateway IP Address"), &packet[24]);
			frame->AddMACAddr(frameOfst + 28, CSTR("Client Hardware Address"), &packet[28], true);
			frame->AddHexBuff(frameOfst + 34, 10, CSTR("Padding"), &packet[34], false);
			frame->AddStrS(frameOfst + 44, 64, CSTR("Server Host Name"), &packet[44]);
			frame->AddStrS(frameOfst + 108, 128, CSTR("Server Host Name"), &packet[108]);
			frame->AddHex32(frameOfst + 236, CSTR("DHCP Magic"), ReadMUInt32(&packet[236]));
			const UInt8 *currPtr = &packet[240];
			const UInt8 *endPtr = &packet[packetSize];
			UInt8 t;
			UInt8 len;
			while (currPtr < endPtr)
			{
				t = *currPtr++;
				frame->AddUIntName(frameOfst + (UInt32)(currPtr - packet - 1), 1, CSTR("Option Type"), t, DHCPOptionGetName(t));
				if (t == 255)
				{
					if (currPtr < endPtr)
					{
						frame->AddHexBuff(frameOfst + (UInt32)(currPtr - packet), (UInt32)(endPtr - currPtr), CSTR("Padding"), currPtr, true);
					}
					break;
				}
				if (currPtr >= endPtr)
				{
					break;
				}
				len = *currPtr++;
				frame->AddUInt(frameOfst + (UInt32)(currPtr - packet - 1), 1, CSTR("Option Length"), len);
				if (t == 1 && len == 4)
				{
					frame->AddIPv4(frameOfst + (UInt32)(currPtr - packet), CSTR("Subnet Mask"), currPtr);
				}
				else if (t == 3 && len == 4)
				{
					frame->AddIPv4(frameOfst + (UInt32)(currPtr - packet), CSTR("Router"), currPtr);
				}
				else if (t == 6 && len > 0 && (len & 3) == 0)
				{

					OSInt i = 0;
					while (i < len)
					{
						sptr = Text::StrOSInt(Text::StrConcatC(sbuff, UTF8STRC("DNS")), i >> 2);
						frame->AddIPv4(frameOfst + (UInt32)(currPtr - packet), CSTRP(sbuff, sptr), &currPtr[i]);
						i += 4;
					}
				}
				else if (t == 12 && len > 0)
				{
					frame->AddStrC(frameOfst + (UInt32)(currPtr - packet), len, CSTR("Host Name"), currPtr);
				}
				else if (t == 15 && len > 0)
				{
					frame->AddStrC(frameOfst + (UInt32)(currPtr - packet), len, CSTR("Domain Name"), currPtr);
				}
				else if (t == 50 && len == 4)
				{
					frame->AddIPv4(frameOfst + (UInt32)(currPtr - packet), CSTR("Requested IP Address"), currPtr);
				}
				else if (t == 51 && len == 4)
				{
					frame->AddUInt(frameOfst + (UInt32)(currPtr - packet), 4, CSTR("IP Address Lease Time"), ReadMUInt32(currPtr));
				}
				else if (t == 53 && len == 1)
				{
					vName = CSTR_NULL;
					switch (currPtr[0])
					{
					case 1:
						vName = {UTF8STRC("Discover")};
						break;
					case 2:
						vName = {UTF8STRC("Offer")};
						break;
					case 3:
						vName = {UTF8STRC("Request")};
						break;
					case 5:
						vName = {UTF8STRC("ACK")};
						break;
					}
					frame->AddUIntName(frameOfst + (UInt32)(currPtr - packet), 1, CSTR("DHCP Type"), currPtr[0], vName);
				}
				else if (t == 54 && len == 4)
				{
					frame->AddIPv4(frameOfst + (UInt32)(currPtr - packet), CSTR("DHCP Server"), currPtr);
				}
				else if (t == 55 && len > 0)
				{
					OSInt i;
					frame->AddText(frameOfst + (UInt32)(currPtr - packet), CSTR("\r\nParameter Request List:"));
					i = 0;
					while (i < len)
					{
						sptr = Text::StrOSInt(Text::StrConcatC(sbuff, UTF8STRC("Parameter Request ")), i);
						frame->AddUIntName(frameOfst + (UInt32)(currPtr - packet + i), 1, CSTRP(sbuff, sptr), currPtr[i], DHCPOptionGetName(currPtr[i]));
						i++;
					}
				}
				else if (t == 57 && len == 2)
				{
					frame->AddUInt(frameOfst + (UInt32)(currPtr - packet), 2, CSTR("Max DHCP Message Size"), ReadMUInt16(currPtr));
				}
				else if (t == 58 && len == 4)
				{
					frame->AddUInt(frameOfst + (UInt32)(currPtr - packet), 4, CSTR("Renew Time"), ReadMUInt32(currPtr));
				}
				else if (t == 59 && len == 4)
				{
					frame->AddUInt(frameOfst + (UInt32)(currPtr - packet), 4, CSTR("Rebinding Time"), ReadMUInt32(currPtr));
				}
				else if (t == 60 && len >= 1)
				{
					frame->AddStrC(frameOfst + (UInt32)(currPtr - packet), len, CSTR("Vendor Class ID"), currPtr);
				}
				else if (t == 61 && len >= 1)
				{
					frame->AddUInt(frameOfst + (UInt32)(currPtr - packet), 1, CSTR("Client ID Type"), currPtr[0]);
					if (len > 1)
					{
						frame->AddHexBuff(frameOfst + (UInt32)(currPtr - packet) + 1, (UOSInt)len - 1, CSTR("Client ID"), &currPtr[1], ':', false);
					}
				}
				else if (t == 66 && len >= 1)
				{
					frame->AddStrC(frameOfst + (UInt32)(currPtr - packet), len, CSTR("TFTP Server Name"), currPtr);
				}
				else if (t == 81 && len >= 3)
				{
					frame->AddHex8(frameOfst + (UInt32)(currPtr - packet), CSTR("Frags"), currPtr[0]);
					frame->AddUInt(frameOfst + (UInt32)(currPtr - packet) + 1, 1, CSTR("RCODE1"), currPtr[1]);
					frame->AddUInt(frameOfst + (UInt32)(currPtr - packet) + 2, 1, CSTR("RCODE2"), currPtr[2]);
					if (len > 3)
					{
						frame->AddStrC(frameOfst + (UInt32)(currPtr - packet + 3), (UOSInt)len - 3, CSTR("Domain Name"), &currPtr[3]);
					}
				}
				else if (t == 120 && len >= 1)
				{
					frame->AddUInt(frameOfst + (UInt32)(currPtr - packet), 1, CSTR("SIP Server Encoding"), currPtr[0]);
					if (currPtr[0] == 1 && len == 5)
					{
						frame->AddIPv4(frameOfst + (UInt32)(currPtr - packet) + 1, CSTR("SIP Server Address"), &currPtr[1]);
					}
					else if (len > 1)
					{
						frame->AddHexBuff(frameOfst + (UInt32)(currPtr - packet) + 1, (UOSInt)len - 1, CSTR("Unknown"), &currPtr[1], true);
					}
				}
				else
				{
					frame->AddHexBuff(frameOfst + (UInt32)(currPtr - packet), len, CSTR("Unknown"), currPtr, true);
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
		frame->AddText(frameOfst, CSTR("TFTP:"));
		vName = CSTR_NULL;
		switch (opcode)
		{
		case 1:
			vName = {UTF8STRC("Read request")};
			break;
		case 2:
			vName = {UTF8STRC("Write request")};
			break;
		case 3:
			vName = {UTF8STRC("Data")};
			break;
		case 4:
			vName = {UTF8STRC("Acknowledgment")};
			break;
		case 5:
			vName = {UTF8STRC("Error")};
			break;
		case 6:
			vName = {UTF8STRC("Options Acknowledgment")};
			break;
		}
		frame->AddUIntName(frameOfst, 2, CSTR("Opcode"), opcode, vName);

		if (opcode == 1 || opcode == 2)
		{
			if (packet[packetSize - 1] == 0)
			{
				len = Text::StrCharCnt(&packet[2]);
				frame->AddField(frameOfst + 2, (UInt32)len + 1, CSTR("Filename"), {&packet[2], len});
				i += len + 1;
				if (i < packetSize)
				{
					len = Text::StrCharCnt(&packet[i]);
					frame->AddField(frameOfst + (UInt32)i, (UInt32)len + 1, CSTR("Mode"), {&packet[i], len});
					i += len + 1;
				}
				OSInt optId = 0;
				while (i < packetSize)
				{
					len = Text::StrCharCnt(&packet[i]);
					if (optId & 1)
					{
						sptr = Text::StrOSInt(Text::StrConcatC(sbuff, UTF8STRC("Value")), 1 + (optId >> 1));
					}
					else
					{
						sptr = Text::StrOSInt(Text::StrConcatC(sbuff, UTF8STRC("Option")), 1 + (optId >> 1));
					}
					frame->AddField(frameOfst + (UInt32)i, (UInt32)len + 1, CSTRP(sbuff, sptr), {&packet[i], len});
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
			frame->AddText(frameOfst, CSTR("NTP Request:"));
		}
		else
		{
			frame->AddText(frameOfst, CSTR("NTP Reply:"));
		}
		
		if (packetSize < 48)
		{
			frame->AddTextHexBuff(frameOfst, packetSize, packet, true);
		}
		else
		{
			vName = CSTR_NULL;
			switch (packet[0] >> 6)
			{
			case 0:
				vName = {UTF8STRC("No warning")};
				break;
			case 1:
				vName = {UTF8STRC("Last minute of the day has 61 seconds")};
				break;
			case 2:
				vName = {UTF8STRC("Last minute of the day has 59 seconds")};
				break;
			case 3:
				vName = {UTF8STRC("Unknown")};
				break;
			}
			frame->AddUIntName(frameOfst + 0, 1, CSTR("Leap Indicator"), (UOSInt)packet[0] >> 6, vName);
			frame->AddUInt(frameOfst + 0, 1, CSTR("Version Number"), (packet[0] >> 3) & 7);
			vName = CSTR_NULL;
			switch (packet[0] & 7)
			{
			case 0:
				vName = {UTF8STRC("Reserved")};
				break;
			case 1:
				vName = {UTF8STRC("Symmetric active")};
				break;
			case 2:
				vName = {UTF8STRC("Symmetric passive")};
				break;
			case 3:
				vName = {UTF8STRC("Client")};
				break;
			case 4:
				vName = {UTF8STRC("Server")};
				break;
			case 5:
				vName = {UTF8STRC("Broadcast")};
				break;
			case 6:
				vName = {UTF8STRC("NTP Control Message")};
				break;
			case 7:
				vName = {UTF8STRC("Reserved for private use")};
				break;
			}
			frame->AddUIntName(frameOfst + 0, 1, CSTR("Mode"), packet[0] & 7, vName);
			if (packet[1] == 0)
			{
				vName = {UTF8STRC("Unspecified or invalid")};
			}
			else if (packet[1] == 1)
			{
				vName = {UTF8STRC("Primary Server")};
			}
			else if (packet[1] < 16)
			{
				vName = {UTF8STRC("Secondary Server")};
			}
			else if (packet[1] == 16)
			{
				vName = {UTF8STRC("Unsynchronized")};
			}
			else
			{
				vName = {UTF8STRC("Reserved")};
			}
			frame->AddUIntName(frameOfst + 1, 1, CSTR("Stratum"), packet[1], vName);
			frame->AddUInt(frameOfst + 2, 1, CSTR("Poll"), packet[2]);
			frame->AddInt(frameOfst + 3, 1, CSTR("Precision"), (Int8)packet[3]);
			frame->AddFloat(frameOfst + 4, 4, CSTR("Root Delay"), ReadMUInt32(&packet[4]) / 65536.0);
			frame->AddFloat(frameOfst + 8, 4, CSTR("Root Dispersion"), ReadMUInt32(&packet[8]) / 65536.0);
			frame->AddHexBuff(frameOfst + 12, 4, CSTR("Reference ID"), &packet[12], false);
			Data::DateTime dt;
			if (ReadNInt64(&packet[16]) == 0)
				sptr = Text::StrConcatC(sbuff, UTF8STRC("0"));
			else
			{
				Net::NTPServer::ReadTime(&packet[16], &dt);
				dt.ToLocalTime();
				sptr = dt.ToString(sbuff, "yyyy-MM-dd HH:mm:ss.fff");
			}
			frame->AddField(frameOfst + 16, 8, CSTR("Reference Timestamp"), CSTRP(sbuff, sptr));
			if (ReadNInt64(&packet[24]) == 0)
				sptr = Text::StrConcatC(sbuff, UTF8STRC("0"));
			else
			{
				Net::NTPServer::ReadTime(&packet[24], &dt);
				dt.ToLocalTime();
				sptr = dt.ToString(sbuff, "yyyy-MM-dd HH:mm:ss.fff");
			}
			frame->AddField(frameOfst + 24, 8, CSTR("Origin Timestamp"), CSTRP(sbuff, sptr));
			if (ReadNInt64(&packet[32]) == 0)
				sptr = Text::StrConcatC(sbuff, UTF8STRC("0"));
			else
			{
				Net::NTPServer::ReadTime(&packet[32], &dt);
				dt.ToLocalTime();
				sptr = dt.ToString(sbuff, "yyyy-MM-dd HH:mm:ss.fff");
			}
			frame->AddField(frameOfst + 32, 8, CSTR("Receive Timestamp"), CSTRP(sbuff, sptr));
			if (ReadNInt64(&packet[40]) == 0)
				sptr = Text::StrConcatC(sbuff, UTF8STRC("0"));
			else
			{
				Net::NTPServer::ReadTime(&packet[40], &dt);
				dt.ToLocalTime();
				sptr = dt.ToString(sbuff, "yyyy-MM-dd HH:mm:ss.fff");
			}
			frame->AddField(frameOfst + 40, 8, CSTR("Transmit Timestamp"), CSTRP(sbuff, sptr));
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
		frame->AddText(frameOfst, CSTR("NetBIOS-NS:"));
		frame->AddHex16(frameOfst + 0, CSTR("NAME_TRN_ID"), ReadMUInt16(&packet[0]));
		frame->AddHex16(frameOfst + 2, CSTR("Flags"), ReadMUInt16(&packet[2]));
		sptr = Text::StrUInt16(sbuff, (UInt16)(packet[2] >> 7));
		frame->AddSubfield(frameOfst + 2, 2, CSTR("Response"), CSTRP(sbuff, sptr));
		sptr = Text::StrUInt16(sbuff, (UInt16)((packet[2] & 0x78) >> 3));
		frame->AddSubfield(frameOfst + 2, 2, CSTR("OPCODE"), CSTRP(sbuff, sptr));
		sptr = Text::StrHexByte(Text::StrConcatC(sbuff, UTF8STRC("0x")), (UInt8)((ReadMUInt16(&packet[2]) & 0x7F0) >> 4));
		frame->AddSubfield(frameOfst + 2, 2, CSTR("NMFLAGS"), CSTRP(sbuff, sptr));
		sptr = Text::StrUInt16(sbuff, packet[3] & 0xf);
		frame->AddSubfield(frameOfst + 2, 2, CSTR("RCODE"), CSTRP(sbuff, sptr));
		frame->AddUInt(frameOfst + 4, 2, CSTR("QDCOUNT"), qdcount);
		frame->AddUInt(frameOfst + 6, 2, CSTR("ANCOUNT"), ancount);
		frame->AddUInt(frameOfst + 8, 2, CSTR("NSCOUNT"), nscount);
		frame->AddUInt(frameOfst + 10, 2, CSTR("ARCOUNT"), arcount);
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
			k = Net::DNSClient::ParseString(sbuff, packet, i, packetSize, &sptr);
			frame->AddNetBIOSName(frameOfst + i, k - i, CSTR("QUESTION_NAME"), CSTRP(sbuff, sptr));
			i = k;
			qType = ReadMUInt16(&packet[i]);
			vName = CSTR_NULL;
			switch (qType)
			{
			case 0x20:
				vName = {UTF8STRC("NB")};
				break;
			case 0x21:
				vName = {UTF8STRC("NBSTAT")};
				break;
			}
			frame->AddUIntName(frameOfst + i, 2, CSTR("QUESTION_TYPE"), qType, vName);

			qClass = ReadMUInt16(&packet[2 + i]);
			vName = CSTR_NULL;
			if (qClass == 1)
			{
				vName = {UTF8STRC("IN")};
			}
			frame->AddUIntName(frameOfst + i + 2, 2, CSTR("QUESTION_CLASS"), qClass, vName);
			i += 4;
			j++;
		}
		j = 0;
		ancount = (UInt16)(ancount + nscount + arcount);
		while (j < ancount)
		{
			k = Net::DNSClient::ParseString(sbuff, packet, i, packetSize, &sptr);
			frame->AddNetBIOSName(frameOfst + i, k - i, CSTR("RR_NAME"), CSTRP(sbuff, sptr));
			i = k;
			rrType = ReadMUInt16(&packet[i]);
			vName = CSTR_NULL;
			switch (rrType)
			{
			case 0x20:
				vName = {UTF8STRC("NB")};
				break;
			case 0x21:
				vName = {UTF8STRC("NBSTAT")};
				break;
			}
			frame->AddUIntName(frameOfst + i, 2, CSTR("RR_TYPE"), rrType, vName);

			rrClass = ReadMUInt16(&packet[2 + i]);
			vName = CSTR_NULL;
			if (rrClass == 1)
			{
				vName = {UTF8STRC("IN")};
			}
			frame->AddUIntName(frameOfst + i + 2, 2, CSTR("RR_CLASS"), rrClass, vName);
			frame->AddUInt(frameOfst + i + 4, 4, CSTR("TTL"), ReadMUInt32(&packet[4 + i]));
			rdLength = ReadMUInt16(&packet[8 + i]);
			frame->AddUInt(frameOfst + i + 8, 2, CSTR("RD_LENGTH"), rdLength);
			i += 10;
			if (rrType == 0x20 && rdLength == 6)
			{
				frame->AddHex16(frameOfst + i, CSTR("NB_FLAGS"), ReadMUInt16(&packet[i]));
				frame->AddIPv4(frameOfst + i + 2, CSTR("NB_ADDRESS"), &packet[i + 2]);
			}
			else if (rrType == 0x21 && rdLength >= 1)
			{
				UOSInt nName = packet[i];
				if (nName * 18 + 43 <= rdLength)
				{
					UOSInt k;
					frame->AddUInt(frameOfst + i, 1, CSTR("Number_of_name"), nName);
					k = 0;
					while (k < nName)
					{
						sptr = Text::StrUOSInt(Text::StrConcatC(sbuff, UTF8STRC("Name")), k);
						MemCopyNO(sbuff2, &packet[i + 1 + k * 18], 15);
						sbuff2[15] = 0;
						sptr2 = Text::StrRTrim(sbuff2);
						frame->AddField(frameOfst + (UInt32)i + 1 + (UInt32)k * 18, 15, CSTRP(sbuff, sptr), CSTRP(sbuff2, sptr2));
						sptr = Text::StrUOSInt(Text::StrConcatC(sbuff, UTF8STRC("Type")), k);
						frame->AddUIntName(frameOfst + i + 1 + k * 18 + 15, 1, CSTRP(sbuff, sptr), packet[i + 1 + k * 18 + 15], Net::NetBIOSUtil::NameTypeGetName(packet[i + 1 + k * 18 + 15]));
						sptr = Text::StrUOSInt(Text::StrConcatC(sbuff, UTF8STRC("Flags")), k);
						frame->AddHex16(frameOfst + i + 1 + k * 18 + 16, CSTRP(sbuff, sptr), ReadMUInt16(&packet[i + 1 + k * 18 + 16]));
						k++;
					}
					frame->AddMACAddr(frameOfst + i + 1 + nName * 18, CSTR("Unit ID"), &packet[i + 1 + nName * 18], true);
					frame->AddHex8(frameOfst + i + 7 + nName * 18, CSTR("Jumpers"), packet[i + 7 + nName * 18]);
					frame->AddHex8(frameOfst + i + 8 + nName * 18, CSTR("Test Result"), packet[i + 8 + nName * 18]);
					frame->AddHex16(frameOfst + i + 9 + nName * 18, CSTR("Version number"), ReadMUInt16(&packet[i + 9 + nName * 18]));
					frame->AddHex16(frameOfst + i + 11 + nName * 18, CSTR("Period of statistics"), ReadMUInt16(&packet[i + 11 + nName * 18]));
					frame->AddUInt(frameOfst + i + 13 + nName * 18, 2, CSTR("Number of CRCs"), ReadMUInt16(&packet[i + 13 + nName * 18]));
					frame->AddUInt(frameOfst + i + 15 + nName * 18, 2, CSTR("Number of alignment errors"), ReadMUInt16(&packet[i + 15 + nName * 18]));
					frame->AddUInt(frameOfst + i + 17 + nName * 18, 2, CSTR("Number of collision"), ReadMUInt16(&packet[i + 17 + nName * 18]));
					frame->AddUInt(frameOfst + i + 19 + nName * 18, 2, CSTR("Number of send aborts"), ReadMUInt16(&packet[i + 19 + nName * 18]));
					frame->AddUInt(frameOfst + i + 21 + nName * 18, 4, CSTR("Number of good sends"), ReadMUInt32(&packet[i + 21 + nName * 18]));
					frame->AddUInt(frameOfst + i + 25 + nName * 18, 4, CSTR("Number of good receives"), ReadMUInt32(&packet[i + 25 + nName * 18]));
					frame->AddUInt(frameOfst + i + 29 + nName * 18, 2, CSTR("Number of retransmits"), ReadMUInt16(&packet[i + 29 + nName * 18]));
					frame->AddUInt(frameOfst + i + 31 + nName * 18, 2, CSTR("Number of no resource conditions"), ReadMUInt16(&packet[i + 31 + nName * 18]));
					frame->AddUInt(frameOfst + i + 33 + nName * 18, 2, CSTR("Number of command blocks"), ReadMUInt16(&packet[i + 33 + nName * 18]));
					frame->AddUInt(frameOfst + i + 35 + nName * 18, 2, CSTR("Number of pending sessions"), ReadMUInt16(&packet[i + 35 + nName * 18]));
					frame->AddUInt(frameOfst + i + 37 + nName * 18, 2, CSTR("Max number of pending sessions"), ReadMUInt16(&packet[i + 37 + nName * 18]));
					frame->AddUInt(frameOfst + i + 39 + nName * 18, 2, CSTR("Max total sessions possible"), ReadMUInt16(&packet[i + 39 + nName * 18]));
					frame->AddUInt(frameOfst + i + 41 + nName * 18, 2, CSTR("Sesison data packet size"), ReadMUInt16(&packet[i + 41 + nName * 18]));
					if (nName * 18 + 43 < rdLength)
					{
						frame->AddHexBuff(frameOfst + i + nName * 18 + 43, rdLength - nName * 18 - 43, CSTR("Unknown"), &packet[i + nName * 18 + 43], false);
					}
				}
				else
				{
					frame->AddHexBuff(frameOfst + i, rdLength, CSTR("RDATA"), &packet[i], false);
				}
			}
			else
			{
				frame->AddHexBuff(frameOfst + i, rdLength, CSTR("RDATA"), &packet[i], false);
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
		frame->AddText(frameOfst, CSTR("NetBIOS-DS:"));
		vName = CSTR_NULL;
		switch (msgType)
		{
		case 0x10:
			vName = {UTF8STRC("Direct Unique Datagram")};
			break;
		case 0x11:
			vName = {UTF8STRC("Direct Group Datagram")};
			break;
		case 0x12:
			vName = {UTF8STRC("Broadcast Datagram")};
			break;
		case 0x13:
			vName = {UTF8STRC("Datagram Error")};
			break;
		case 0x14:
			vName = {UTF8STRC("Datagram Query Request")};
			break;
		case 0x15:
			vName = {UTF8STRC("Datagram Positive Query Response")};
			break;
		case 0x16:
			vName = {UTF8STRC("Datagram Negative Query Response")};
			break;
		}
		frame->AddHex8Name(frameOfst + 0, CSTR("MSG_TYPE"), msgType, vName);
		frame->AddHex8(frameOfst + 1, CSTR("FLAGS"), packet[1]);
		frame->AddUInt(frameOfst + 2, 2, CSTR("DGM_ID"), ReadMUInt16(&packet[2]));
		frame->AddIPv4(frameOfst + 4, CSTR("SOURCE_IP"), &packet[4]);
		frame->AddUInt(frameOfst + 8, 2, CSTR("SOURCE_PORT"), ReadMUInt16(&packet[8]));
		UOSInt i;
		UOSInt j;
		i = 10;
		switch (msgType)
		{
		case 0x10:
		case 0x11:
		case 0x12:
			frame->AddUInt(frameOfst + 10, 2, CSTR("DGM_LENGTH"), ReadMUInt16(&packet[10]));
			frame->AddUInt(frameOfst + 12, 2, CSTR("PACKET_OFFSET"), ReadMUInt16(&packet[12]));
			i = 14;
			j = Net::DNSClient::ParseString(sbuff, packet, i, packetSize, &sptr);
			frame->AddNetBIOSName(frameOfst + i, (UInt32)(j - i), CSTR("SOURCE_NAME"), CSTRP(sbuff, sptr));
			i = j;
			j = Net::DNSClient::ParseString(sbuff, packet, i, packetSize, &sptr);
			frame->AddNetBIOSName(frameOfst + i, (UInt32)(j - i), CSTR("DESTINATION_NAME"), CSTRP(sbuff, sptr));
			i = j;
			break;
		case 0x13:
			if (packetSize >= 19)
			{
				vName = CSTR_NULL;
				switch (packet[10])
				{
				case 0x82:
					vName = {UTF8STRC("Destination Name Not Present")};
					break;
				case 0x83:
					vName = {UTF8STRC("Invalid Source Name Format")};
					break;
				case 0x84:
					vName = {UTF8STRC("Invalid Destination Name Format")};
					break;
				}
				frame->AddHex8Name(frameOfst + 10, CSTR("ERROR_CODE"), packet[10], vName);
				i = 11;
			}
			break;
		case 0x14:
		case 0x15:
		case 0x16:
			j = Net::DNSClient::ParseString(sbuff, packet, i, packetSize, &sptr);
			frame->AddNetBIOSName(frameOfst + i, (UInt32)(j - i), CSTR("DESTINATION_NAME"), CSTRP(sbuff, sptr));
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
			frame->AddText(frameOfst, CSTR("SNMP:"));
			Net::SNMPInfo snmp;
			Text::StringBuilderUTF8 sb;
			UOSInt i = snmp.PDUGetDetail(CSTR("Message"), packet, packetSize, 0, &sb);
			frame->AddField(frameOfst, (UInt32)i, sb.ToCString(), CSTR_NULL);
			if (packetSize > i)
			{
				frame->AddTextHexBuff(frameOfst + i, packetSize - i, &packet[i], true);
			}
		}
	}
	else if (srcPort == 427 || destPort == 427) //RFC 2165/2608
	{
		UOSInt i;
		frame->AddText(frameOfst, CSTR("Service Location Protocol:"));
		frame->AddUInt(frameOfst + 0, 1, CSTR("Version"), packet[0]);
		vName = CSTR_NULL;
		switch (packet[1])
		{
		case 1:
			vName = {UTF8STRC("Service Request")};
			break;
		case 2:
			vName = {UTF8STRC("Service Reply")};
			break;
		case 3:
			vName = {UTF8STRC("Service Registration")};
			break;
		case 4:
			vName = {UTF8STRC("Service Deregister")};
			break;
		case 5:
			vName = {UTF8STRC("Service Acknowledge")};
			break;
		case 6:
			vName = {UTF8STRC("Attribute Request")};
			break;
		case 7:
			vName = {UTF8STRC("Attribute Reply")};
			break;
		case 8:
			vName = {UTF8STRC("DA Advertisement")};
			break;
		case 9:
			vName = {UTF8STRC("Service Type Request")};
			break;
		case 10:
			vName = {UTF8STRC("Service Type Reply")};
			break;
		case 11:
			vName = {UTF8STRC("SA Advertisement")};
			break;
		}
		frame->AddUIntName(frameOfst + 1, 1, CSTR("Function-ID"), packet[1], vName);
		i = 2;
		UInt16 len;
		UInt16 len2;
		if (packet[0] == 1)
		{
			len = ReadMUInt16(&packet[2]);
			frame->AddUInt(frameOfst + 2, 2, CSTR("Length"), len);
			frame->AddHex8(frameOfst + 4, CSTR("Flags"), packet[4]);
			frame->AddUInt(frameOfst + 5, 1, CSTR("Dialect"), packet[5]);
			frame->AddStrC(frameOfst + 6, 2, CSTR("Language Code"), &packet[6]);
			UInt16 enc;
			enc = ReadMUInt16(&packet[8]);
			frame->AddUInt(frameOfst + 8, 2, CSTR("Character Encoding"), enc);
			frame->AddUInt(frameOfst + 10, 2, CSTR("Transaction Identifier"), ReadMUInt16(&packet[10]));
			if (packet[1] == 1)
			{
				i = 12;
				if (i + 2 <= packetSize)
				{
					len2 = ReadMUInt16(&packet[i]);
					i += 2;
					if (len2 == 0)
					{
						frame->AddField(frameOfst + (UInt32)i - 2, 2, CSTR("Previous Responders"), CSTR(""));
					}
					else if (len2 + i <= len)
					{
						frame->AddStrC(frameOfst + i - 2, 2 + (UOSInt)len2, CSTR("Previous Responders"), &packet[i]);
						i += len2;
					}
					else
					{
						frame->AddStrC(frameOfst + i - 2, (UOSInt)len - i + 2, CSTR("Previous Responders"), &packet[i]);
						i = len;
					}
				}
				if (i + 2 <= packetSize)
				{
					len2 = ReadMUInt16(&packet[i]);
					i += 2;
					if (len2 == 0)
					{
						frame->AddField(frameOfst + (UInt32)i - 2, 2, CSTR("Service Request"), CSTR(""));
					}
					else if (len2 + i <= len)
					{
						frame->AddStrC(frameOfst + i - 2, 2 + (UOSInt)len2, CSTR("Service Request"), &packet[i]);
						i += len2;
					}
					else
					{
						frame->AddStrC(frameOfst + i - 2, len - i + 2, CSTR("Service Request"), &packet[i]);
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
			frame->AddText(frameOfst, CSTR("LoRa Gateway:"));
			frame->AddUInt(frameOfst + 0, 1, CSTR("Protocol Version"), packet[0]);
			frame->AddUInt(frameOfst + 1, 2, CSTR("Random Token"), ReadMUInt16(&packet[1]));
			vName = CSTR_NULL;
			switch (packet[3])
			{
			case 0:
				vName = {UTF8STRC("PUSH_DATA")};
				break;
			case 1:
				vName = {UTF8STRC("PUSH_ACK")};
				break;
			case 2:
				vName = {UTF8STRC("PULL_DATA")};
				break;
			case 3:
				vName = {UTF8STRC("PULL_RESP")};
				break;
			case 4:
				vName = {UTF8STRC("PULL_ACK")};
				break;
			case 5:
				vName = {UTF8STRC("TX_ACK")};
				break;
			}
			frame->AddUIntName(frameOfst + 3, 1, CSTR("Protocol Version"), packet[3], vName);
			frame->AddHexBuff(frameOfst + 4, 8, CSTR("Gateway UID"), &packet[4], 0, false);

			if (packetSize > 12)
			{
				if (packet[12] == 0x7B)
				{
					frame->AddText(frameOfst + 12, CSTR("\r\nContent:"));
					Text::StringBuilderUTF8 sb;
					Text::JSText::JSONWellFormat(&packet[12], packetSize - 12, 0, &sb);
					frame->AddField(frameOfst + 12, (UInt32)packetSize - 12, sb.ToCString(), CSTR_NULL);
					Text::JSONBase *json = Text::JSONBase::ParseJSONBytes(&packet[12], packetSize - 12);
					if (json)
					{
						if (json->GetType() == Text::JSONType::Object)
						{
							Text::JSONObject *jobj = (Text::JSONObject*)json;
							Text::JSONBase *jbase = jobj->GetObjectValue(CSTR("rxpk"));
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
										jbase = jobj->GetObjectValue(CSTR("data"));
										if (jbase && jbase->GetType() == Text::JSONType::String)
										{
											Text::JSONString *jstr = (Text::JSONString*)jbase;
											UOSInt dataLen;
											UInt8 *dataBuff;
											Text::String *dataStr = jstr->GetValue();
											sb.ClearStr();
											sb.AppendC(UTF8STRC("\r\n"));
											sb.Append(dataStr);
											sb.AppendC(UTF8STRC(":"));
											dataLen = b64.CalcBinSize(dataStr->v, dataStr->leng);
											dataBuff = MemAlloc(UInt8, dataLen);
											if (b64.DecodeBin(dataStr->v, dataStr->leng, dataBuff) == dataLen)
											{
												PacketLoRaMACGetDetail(dataBuff, dataLen, &sb);
											}
											else
											{
												sb.AppendC(UTF8STRC("\r\nNot base64 encoding"));
											}
											frame->AddText(frameOfst + 12, sb.ToCString());
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
			frame->AddText(frameOfst, CSTR("LoRa Gateway PUSH_ACK:"));
			frame->AddUInt(frameOfst + 0, 1, CSTR("Protocol Version"), packet[0]);
			frame->AddUInt(frameOfst + 1, 2, CSTR("Random Token"), ReadMUInt16(&packet[1]));
			vName = CSTR_NULL;
			switch (packet[3])
			{
			case 0:
				vName = {UTF8STRC("PUSH_DATA")};
				break;
			case 1:
				vName = {UTF8STRC("PUSH_ACK")};
				break;
			case 2:
				vName = {UTF8STRC("PULL_DATA")};
				break;
			case 3:
				vName = {UTF8STRC("PULL_RESP")};
				break;
			case 4:
				vName = {UTF8STRC("PULL_ACK")};
				break;
			case 5:
				vName = {UTF8STRC("TX_ACK")};
				break;
			}
			frame->AddUIntName(frameOfst + 3, 1, CSTR("Protocol Version"), packet[3], vName);
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
		frame->AddText(frameOfst, CSTR("SSDP Request:"));
		Text::String *s = Text::String::New(packet, packetSize);
		frame->AddField(frameOfst, (UInt32)packetSize, s->ToCString(), CSTR_NULL);
		s->Release();
	}
	else if (srcPort == 1900)
	{
		frame->AddText(frameOfst, CSTR("SSDP Reply:"));
		Text::String *s = Text::String::New(packet, packetSize);
		frame->AddField(frameOfst, (UInt32)packetSize, s->ToCString(), CSTR_NULL);
		s->Release();
	}
	else if (destPort == 3702)
	{
		frame->AddText(frameOfst, CSTR("WS-Discovery Request:"));
		Text::StringBuilderUTF8 sb;
		Text::HTMLUtil::XMLWellFormat(packet, packetSize, 0, &sb);
		frame->AddField(frameOfst, (UInt32)packetSize, sb.ToCString(), CSTR_NULL);
	}
	else if (srcPort == 3702)
	{
		frame->AddText(frameOfst, CSTR("WS-Discovery Reply:"));
		Text::StringBuilderUTF8 sb;
		Text::HTMLUtil::XMLWellFormat(packet, packetSize, 0, &sb);
		frame->AddField(frameOfst, (UInt32)packetSize, sb.ToCString(), CSTR_NULL);
	}
	else if (destPort == 5353)
	{
		frame->AddText(frameOfst, CSTR("mDNS:"));
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
		frame->AddText(frameOfst, CSTR("Dropbox LAN Sync Discovery:"));
		Text::StringBuilderUTF8 sb;
		Text::JSText::JSONWellFormat(packet, packetSize, 0, &sb);
		frame->AddField(frameOfst, (UInt32)packetSize, sb.ToCString(), CSTR_NULL);
	}
	else if (srcPort >= 1024 && destPort >= 1024)
	{
		frame->AddHexBuff(frameOfst, packetSize, CSTR("Private Packet"), packet, true);		
	}
	else
	{
		frame->AddText(frameOfst, CSTR("Unknown Data:"));
		frame->AddTextHexBuff(frameOfst, packetSize, packet, true);
	}
}

void Net::PacketAnalyzerEthernet::PacketDNSGetDetail(const UInt8 *packet, UOSInt packetSize, UInt32 frameOfst, IO::FileAnalyse::FrameDetailHandler *frame)
{
	UTF8Char sbuff[128];
	UTF8Char *sptr;
	Text::CString vName;
	frame->AddUInt(frameOfst + 0, 2, CSTR("ID"), ReadMUInt16(&packet[0]));
	if ((packet[2] & 0x80))
	{
		frame->AddField(frameOfst + 2, 1, CSTR("QR"), CSTR("1 (Response)"));
	}
	else
	{
		frame->AddField(frameOfst + 2, 1, CSTR("QR"), CSTR("0 (Request)"));
	}
	UInt8 opcode = (packet[2] & 0x78) >> 3;
	vName = CSTR_NULL;
	switch (opcode)
	{
	case 0:
		vName = {UTF8STRC("QUERY")};
		break;
	case 1:
		vName = {UTF8STRC("IQUERY")};
		break;
	case 2:
		vName = {UTF8STRC("STATUS")};
		break;
	}
	frame->AddUIntName(frameOfst + 2, 1, CSTR("OPCODE"), opcode, vName);
	frame->AddUInt(frameOfst + 2, 1, CSTR("AA"), (packet[2] & 4) >> 2);
	frame->AddUInt(frameOfst + 2, 1, CSTR("TC"), (packet[2] & 2) >> 1);
	frame->AddUInt(frameOfst + 2, 1, CSTR("RD"), (packet[2] & 1));
	frame->AddUInt(frameOfst + 3, 1, CSTR("RA"), (packet[3] & 0x80) >> 7);
	frame->AddUInt(frameOfst + 3, 1, CSTR("Z"), (packet[3] & 0x70) >> 4);
	UInt8 rcode = packet[3] & 0xf;
	vName = CSTR_NULL;
	switch (rcode)
	{
	case 0:
		vName = {UTF8STRC("No error")};
		break;
	case 1:
		vName = {UTF8STRC("Format error")};
		break;
	case 2:
		vName = {UTF8STRC("Server failure")};
		break;
	case 3:
		vName = {UTF8STRC("Name Error")};
		break;
	case 4:
		vName = {UTF8STRC("Not Implemented")};
		break;
	case 5:
		vName = {UTF8STRC("Refused")};
		break;
	}
	frame->AddUIntName(frameOfst + 3, 1, CSTR("RCODE"), rcode, vName);

	UInt16 qdcount = ReadMUInt16(&packet[4]);
	UInt16 ancount = ReadMUInt16(&packet[6]);
	UInt16 nscount = ReadMUInt16(&packet[8]);
	UInt16 arcount = ReadMUInt16(&packet[10]);
	frame->AddUInt(frameOfst + 4, 2, CSTR("QDCOUNT"), qdcount);
	frame->AddUInt(frameOfst + 6, 2, CSTR("ANCOUNT"), ancount);
	frame->AddUInt(frameOfst + 8, 2, CSTR("NSCOUNT"), nscount);
	frame->AddUInt(frameOfst + 10, 2, CSTR("ARCOUNT"), arcount);
	UOSInt i = 12;
	UInt16 j;
	UOSInt k;
	UInt16 t;
	j = 0;
	while (j < qdcount)
	{
		k = Net::DNSClient::ParseString(sbuff, packet, i, packetSize, &sptr);
		frame->AddField(frameOfst + (UInt32)i, (UInt32)(k - i), CSTR("QNAME"), CSTRP(sbuff, sptr));
		i = k;
		t = ReadMUInt16(&packet[i]);
		frame->AddUIntName(frameOfst + i, 2, CSTR("QTYPE"), t, Net::DNSClient::TypeGetID(t));
		frame->AddUInt(frameOfst + i + 2, 2, CSTR("QCLASS"), ReadMUInt16(&packet[i + 2]));

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

		k = Net::DNSClient::ParseString(sbuff, packet, i, packetSize, &sptr);
		frame->AddField(frameOfst + (UInt32)i, (UInt32)(k - i), CSTR("NAME"), CSTRP(sbuff, sptr));
		i = k;
		rrType = ReadMUInt16(&packet[i]);
		rrClass = ReadMUInt16(&packet[i + 2]);
		rdLength = ReadMUInt16(&packet[i + 8]);
		frame->AddUIntName(frameOfst + i, 2, CSTR("TYPE"), rrType, Net::DNSClient::TypeGetID(rrType));
		frame->AddUInt(frameOfst + i + 2, 2, CSTR("CLASS"), rrClass);
		frame->AddUInt(frameOfst + i + 4, 4, CSTR("TTL"), ReadMUInt32(&packet[i + 4]));
		frame->AddUInt(frameOfst + i + 8, 2, CSTR("RDLENGTH"), rdLength);
		i += 10;
		switch (rrType)
		{
		case 1: // A - a host address
			frame->AddIPv4(frameOfst + i, CSTR("RDATA"), &packet[i]);
			break;
		case 2: // NS - an authoritative name server
		case 5: // CNAME - the canonical name for an alias
		case 12: // PTR - a domain name pointer
			k = Net::DNSClient::ParseString(sbuff, packet, i, i + rdLength, &sptr);
			frame->AddField(frameOfst + (UInt32)i, (UInt32)(k - i), CSTR("RDATA"), CSTRP(sbuff, sptr));
			break;
		case 6:
			k = Net::DNSClient::ParseString(sbuff, packet, i, i + rdLength, &sptr);
			frame->AddField(frameOfst + (UInt32)i, (UInt32)(k - i), CSTR("RDATA"), CSTRP(sbuff, sptr));
			l = Net::DNSClient::ParseString(sbuff, packet, k, i + rdLength, &sptr);
			frame->AddField(frameOfst + (UInt32)k, (UInt32)(l - k), CSTR("-MailAddr"), CSTRP(sbuff, sptr));
			k = l;
			if (k + 20 <= i + rdLength)
			{
				frame->AddUInt(frameOfst + k, 4, CSTR("-SN"), ReadMUInt32(&packet[k]));
				frame->AddUInt(frameOfst + k + 4, 4, CSTR("-Refresh"), ReadMUInt32(&packet[k + 4]));
				frame->AddUInt(frameOfst + k + 8, 4, CSTR("-Retry"), ReadMUInt32(&packet[k + 8]));
				frame->AddUInt(frameOfst + k + 12, 4, CSTR("-Expire"), ReadMUInt32(&packet[k + 12]));
				frame->AddUInt(frameOfst + k + 16, 4, CSTR("-DefTTL"), ReadMUInt32(&packet[k + 16]));
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
			frame->AddUInt(frameOfst + i, 2, CSTR("Priority"), ReadMUInt16(&packet[i]));
			k = Net::DNSClient::ParseString(sbuff, packet, i + 2, i + rdLength, &sptr);
			frame->AddField(frameOfst + (UInt32)i + 2, (UInt32)(k - i - 2), CSTR("RDATA"), CSTRP(sbuff, sptr));
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
					frame->AddUInt(frameOfst + i + k, 1, CSTR("StrLen"), packet[i + k]);
					frame->AddStrC(frameOfst + i + k + 1, packet[i + k], CSTR("RDATA"), &packet[i + k + 1]);
					k += (UOSInt)packet[i + k] + 1;
				}
			}
			break;
		case 28: // AAAA
			{
				frame->AddIPv6(frameOfst + i, CSTR("RDATA"), &packet[i]);
			}
			break;
		case 33: // SRV - 
			{
				frame->AddUInt(frameOfst + i, 2, CSTR("Priority"), ReadMUInt16(&packet[i]));
				frame->AddUInt(frameOfst + i + 2, 2, CSTR("Weight"), ReadMUInt16(&packet[i + 2]));
				frame->AddUInt(frameOfst + i + 4, 2, CSTR("Port"), ReadMUInt16(&packet[i + 4]));
				k = Net::DNSClient::ParseString(sbuff, packet, i + 6, i + rdLength, &sptr);
				frame->AddField(frameOfst + (UInt32)i + 6, (UInt32)(k - i - 6), CSTR("Target"), CSTRP(sbuff, sptr));
			}
			break;
		case 41: // OPT - 
			{
				frame->AddUInt(frameOfst + i, 2, CSTR("OPTION-CODE"), ReadMUInt16(&packet[i]));
				frame->AddUInt(frameOfst + i + 2, 2, CSTR("OPTION-LENGTH"), ReadMUInt16(&packet[i + 2]));
				frame->AddHexBuff(frameOfst + i + 4, (UInt32)rdLength - 4, CSTR("OPTION-DATA"), &packet[i + 4], false);
			}
			break;
		case 43: // DS - Delegation signer
			{
				frame->AddUInt(frameOfst + i, 2, CSTR("Key Tag"), ReadMUInt16(&packet[i]));
				frame->AddUInt(frameOfst + i + 2, 1, CSTR("Algorithm"), packet[i + 2]);
				frame->AddUInt(frameOfst + i + 3, 1, CSTR("Digest Type"), packet[i + 3]);
				frame->AddHexBuff(frameOfst + i + 4, (UInt32)rdLength - 4, CSTR("Digest"), &packet[i + 4], false);
			}
			break;
		case 46: // RRSIG - DNSSEC signature
			{
				frame->AddUInt(frameOfst + i, 2, CSTR("Type Covered"), ReadMUInt16(&packet[i]));
				frame->AddUInt(frameOfst + i + 2, 1, CSTR("Algorithm"), packet[i + 2]);
				frame->AddUInt(frameOfst + i + 3, 1, CSTR("Labels"), packet[i + 3]);
				frame->AddUInt(frameOfst + i + 4, 4, CSTR("Original TTL"), ReadMUInt32(&packet[i + 4]));
				frame->AddUInt(frameOfst + i + 8, 4, CSTR("Signature Expiration"), ReadMUInt32(&packet[i + 8]));
				frame->AddUInt(frameOfst + i + 12, 4, CSTR("Signature Inception"), ReadMUInt32(&packet[i + 12]));
				frame->AddUInt(frameOfst + i + 16, 2, CSTR("Key Tag"), ReadMUInt16(&packet[i + 16]));
				UOSInt nameLen = Text::StrCharCnt(&packet[i + 18]);
				frame->AddField(frameOfst + (UInt32)i + 18, (UInt32)nameLen + 1, CSTR("Signer's Name"), {&packet[i + 18], nameLen});
				frame->AddHexBuff(frameOfst + i + 19 + nameLen, (UOSInt)rdLength - 19 - nameLen, CSTR("Signature"), &packet[i + 19 + nameLen], false);
			}
			break;
		case 47: // NSEC - Next Secure record
			{
				UOSInt k = Net::DNSClient::ParseString(sbuff, packet, i, i + rdLength, &sptr);
				frame->AddField(frameOfst + (UInt32)i, (UInt32)(k - i), CSTR("Next Domain Name"), CSTRP(sbuff, sptr));
				if (k < i + rdLength)
				{
					frame->AddHexBuff(frameOfst + k, i + rdLength - k, CSTR("RDATA"), &packet[k], false);
				}
			}
			break;
		case 48: // DNSKEY - DNS Key record
			{
				frame->AddUInt(frameOfst + i, 2, CSTR("Flags"), ReadMUInt16(&packet[i]));
				frame->AddUInt(frameOfst + i + 2, 1, CSTR("Protocol"), packet[i + 2]);
				frame->AddUInt(frameOfst + i + 3, 1, CSTR("Algorithm"), packet[i + 3]);
				frame->AddHexBuff(frameOfst + i + 4, (UOSInt)rdLength - 4, CSTR("Public Key"), &packet[i + 4], false);
			}
			break;
		case 250: // TSIG
			{
				UOSInt k = Net::DNSClient::ParseString(sbuff, packet, i, i + rdLength, &sptr);
				frame->AddField(frameOfst + (UInt32)i, (UInt32)(k - i), CSTR("Algorithm"), CSTRP(sbuff, sptr));
				if (k + 10 < i + rdLength)
				{
					Data::DateTime dt;
					dt.SetUnixTimestamp((Int64)(((UInt64)(ReadMUInt16(&packet[k])) << 32) | ReadMUInt32(&packet[k + 2])));
					dt.ToLocalTime();
					sptr = dt.ToString(sbuff, "yyyy-MM-dd HH:mm:ss zzzz");
					frame->AddField(frameOfst + (UInt32)k, 6, CSTR("Time Signed"), CSTRP(sbuff, sptr));
					frame->AddUInt(frameOfst + k + 6, 2, CSTR("Fudge"), ReadMUInt16(&packet[k + 6]));
					UOSInt macSize = ReadMUInt16(&packet[k + 8]);
					frame->AddUInt(frameOfst + k + 8, 2, CSTR("MAC Size"), macSize);
					k += 10;
					if (macSize > 0 && k + macSize <= i + rdLength)
					{
						frame->AddHexBuff(frameOfst + k, macSize, CSTR("MAC"), &packet[k], false);
						k += macSize;
					}
					if (k + 6 <= i + rdLength)
					{
						frame->AddUInt(frameOfst + k + 0, 2, CSTR("Original Id"), ReadMUInt16(&packet[k + 0]));
						frame->AddUInt(frameOfst + k + 2, 2, CSTR("Error"), ReadMUInt16(&packet[k + 2]));
						frame->AddUInt(frameOfst + k + 4, 2, CSTR("Other Len"), ReadMUInt16(&packet[k + 4]));
						k += 6;
					}
					if (k < i + rdLength)
					{
						frame->AddHexBuff(frameOfst + k, i + rdLength - k, CSTR("Other"), &packet[k], false);
					}
				}
				else
				{
					frame->AddText(frameOfst + (UInt32)k, CSTR("RDDATA:"));
					frame->AddTextHexBuff(frameOfst + k, i + rdLength - k, &packet[k], false);
				}
				
			}
			break;
		default:
			frame->AddText(frameOfst + (UInt32)i, CSTR("RDDATA:"));
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
	Text::CString vName = CSTR_NULL;
	switch (packet[0] >> 5)
	{
	case 0:
		vName = {UTF8STRC("Join-request")};
		break;
	case 1:
		vName = {UTF8STRC("Join-accept")};
		break;
	case 2:
		vName = {UTF8STRC("Unconfirmed Data Up")};
		break;
	case 3:
		vName = {UTF8STRC("Unconfirmed Data Down")};
		break;
	case 4:
		vName = {UTF8STRC("Confirmed Data Up")};
		break;
	case 5:
		vName = {UTF8STRC("Confirmed Data Down")};
		break;
	case 6:
		vName = {UTF8STRC("Rejoin-request")};
		break;
	case 7:
		vName = {UTF8STRC("Proprietary")};
		break;
	}
	frame->AddUIntName(frameOfst + 0, 1, CSTR("Message type (MType)"), (UInt16)(packet[0] >> 5), vName);
	frame->AddUInt(frameOfst + 0, 1, CSTR("RFU"), (packet[0] >> 2) & 7);
	frame->AddUInt(frameOfst + 0, 1, CSTR("Major"), packet[0] & 3);
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
			frame->AddHexBuff(frameOfst + 1, 8, CSTR("JoinEUI"), buff, 0, false);
			buff[0] = packet[16];
			buff[1] = packet[15];
			buff[2] = packet[14];
			buff[3] = packet[13];
			buff[4] = packet[12];
			buff[5] = packet[11];
			buff[6] = packet[10];
			buff[7] = packet[9];
			frame->AddHexBuff(frameOfst + 1, 8, CSTR("DevEUI"), buff, 0, false);
			frame->AddUInt(frameOfst + 17, 2, CSTR("DevNonce"), ReadUInt16(&packet[17]));
		}
		else
		{
			frame->AddHexBuff(frameOfst + 1, packetSize - 5, CSTR("MACPayload"), &packet[1], true);
		}
	}
	else
	{
		frame->AddHexBuff(frameOfst + 1, packetSize - 5, CSTR("MACPayload"), &packet[1], true);
	}
	frame->AddHexBuff(frameOfst + packetSize - 4, 4, CSTR("MIC"), &packet[packetSize - 4], false);
}


UOSInt Net::PacketAnalyzerEthernet::HeaderIPv4GetDetail(const UInt8 *packet, UOSInt packetSize, UInt32 frameOfst, IO::FileAnalyse::FrameDetailHandler *frame)
{
	frame->AddField(frameOfst + 0, 1, CSTR("Version"), CSTR("4"));
	frame->AddUInt(frameOfst + 0, 1, CSTR("Internet Header Length"), (UInt16)packet[0] & 0xf);
	frame->AddUInt(frameOfst + 1, 1, CSTR("DSCP"), (UInt16)(packet[1] >> 2));
	frame->AddUInt(frameOfst + 1, 1, CSTR("ECN"), (UInt16)packet[1] & 0x3);
	frame->AddUInt(frameOfst + 2, 2, CSTR("Total Size"), ReadMUInt16(&packet[2]));
	frame->AddUInt(frameOfst + 4, 2, CSTR("Identification"), ReadMUInt16(&packet[4]));
	frame->AddUInt(frameOfst + 6, 2, CSTR("Flags"), (UInt16)(packet[6] >> 5));
	frame->AddUInt(frameOfst + 6, 2, CSTR("Fragment Offset"), ReadMUInt16(&packet[6]) & 0x1fff);
	frame->AddUInt(frameOfst + 8, 1, CSTR("TTL"), packet[8]);
	frame->AddUInt(frameOfst + 9, 1, CSTR("Protocol"), packet[9]);
	frame->AddHex16(frameOfst + 10, CSTR("Header Checksum"), ReadMUInt16(&packet[10]));
	frame->AddIPv4(frameOfst + 12, CSTR("SrcIP"), &packet[12]);
	frame->AddIPv4(frameOfst + 16, CSTR("DestIP"), &packet[16]);

	if ((packet[0] & 0xf) <= 5)
	{
		return 20;
	}
	else
	{
		frame->AddHexBuff(frameOfst + 20, (UInt32)((packet[0] & 0xf) << 2) - 20, CSTR("Options"), &packet[20], true);
		return (packet[0] & 0xf) << 2;
	}
}

Text::CString Net::PacketAnalyzerEthernet::TCPPortGetName(UInt16 port)
{
	switch (port)
	{
	case 21:
		return {UTF8STRC("FTP")};
	case 22:
		return {UTF8STRC("SSH")};
	case 23:
		return {UTF8STRC("Telnet")};
	case 25:
		return {UTF8STRC("SMTP")};
	case 43:
		return {UTF8STRC("WHOIS")};
	case 53:
		return {UTF8STRC("DNS")};
	case 69:
		return {UTF8STRC("TFTP")};
	case 80:
		return {UTF8STRC("HTTP")};
	case 110:
		return {UTF8STRC("POP3")};
	case 135:
		return {UTF8STRC("Microsoft EPMAP")};
	case 139:
		return {UTF8STRC("NetBIOS-SSN")};
	case 143:
		return {UTF8STRC("IMAP")};
	case 427:
		return {UTF8STRC("SvrLoc")};
	case 443:
		return {UTF8STRC("HTTPS")};
	case 445:
		return {UTF8STRC("Microsoft-DS")};
	case 465:
		return {UTF8STRC("SMTPS")};
	case 538:
		return {UTF8STRC("GDOMAP")};
	case 587:
		return {UTF8STRC("SMTP STARTTLS")};
	case 902:
		return {UTF8STRC("VMWare ESX")};
	case 912:
		return {UTF8STRC("VMWare ESX")};
	case 993:
		return {UTF8STRC("IMAPS")};
	case 995:
		return {UTF8STRC("POP3S")};
	case 1022:
		return {UTF8STRC("RFC3692-style Experiment 2")};
	case 3306:
		return {UTF8STRC("MySQL")};
	case 3389:
		return {UTF8STRC("RDP")};
	}
	return CSTR_NULL;
}

Text::CString Net::PacketAnalyzerEthernet::UDPPortGetName(UInt16 port)
{
	switch (port)
	{
	case 53:
		return {UTF8STRC("DNS")};
	case 67:
		return {UTF8STRC("DHCP Server")};
	case 68:
		return {UTF8STRC("DHCP Client")};
	case 69:
		return {UTF8STRC("TFTP")};
	case 123:
		return {UTF8STRC("NTP")};
	case 137:
		return {UTF8STRC("NetBIOS-NS")};
	case 138:
		return {UTF8STRC("NetBIOS-DS")};
	case 139:
		return {UTF8STRC("NetBIOS-SS")};
	case 161:
		return {UTF8STRC("SNMP")};
	case 162:
		return {UTF8STRC("SNMP-TRAP")};
	case 427:
		return {UTF8STRC("SLP")};
	case 546:
		return {UTF8STRC("DHCPv6 client")};
	case 547:
		return {UTF8STRC("DHCPv6 server")};
	case 1700:
		return {UTF8STRC("LoRa Gateway")};
	case 1900:
		return {UTF8STRC("SSDP")};
	case 3702:
		return {UTF8STRC("WS-Discovery")};
	case 5353:
		return {UTF8STRC("mDNS")};
	case 17500:
		return {UTF8STRC("Dropbox LAN Sync Discovery")};
	case 55208:
		return {UTF8STRC("JavaME Device Detection")};
	case 55209:
		return {UTF8STRC("JavaME Device Detection")};
	}
	return CSTR_NULL;
}

Text::CString Net::PacketAnalyzerEthernet::LSAPGetName(UInt8 lsap)
{
	switch (lsap)
	{
	case 0:
		return {UTF8STRC("Null LSAP")};
	case 2:
		return {UTF8STRC("Individual LLC Sublayer Mgt")};
	case 4:
		return {UTF8STRC("SNA Path Control (individual)")};
	case 6:
		return {UTF8STRC("Reserved for DoD")};
	case 0x0E:
		return {UTF8STRC("ProWay-LAN")};
	case 0x18:
		return {UTF8STRC("Texas Instruments")};
	case 0x42:
		return {UTF8STRC("IEEE 802.1 Bridge Spanning Tree Protocol")};
	case 0x4E:
		return {UTF8STRC("EIA-RS 511")};
	case 0x5E:
		return {UTF8STRC("ISI IP")};
	case 0x7E:
		return {UTF8STRC("ISO 8208 (X.25 over IEEE 802.2 Type LLC)")};
	case 0x80:
		return {UTF8STRC("Xerox Network Systems (XNS)")};
	case 0x82:
		return {UTF8STRC("BACnet/Ethernet")};
	case 0x86:
		return {UTF8STRC("Nestar")};
	case 0x8E:
		return {UTF8STRC("ProWay-LAN (IEC 955)")};
	case 0x98:
		return {UTF8STRC("ARPANET Address Resolution Protocol (ARP)")};
	case 0xA6:
		return {UTF8STRC("RDE (route determination entity)")};
	case 0xAA:
		return {UTF8STRC("SNAP Extension Used")};
	case 0xBC:
		return {UTF8STRC("Banyan Vines")};
	case 0xE0:
		return {UTF8STRC("Novell NetWare")};
	case 0xF0:
		return {UTF8STRC("IBM NetBIOS")};
	case 0xF4:
		return {UTF8STRC("IBM LAN Management (individual)")};
	case 0xF8:
		return {UTF8STRC("IBM Remote Program Load (RPL)")};
	case 0xFA:
		return {UTF8STRC("Ungermann-Bass")};
	case 0xFE:
		return {UTF8STRC("OSI protocols ISO CLNS IS 8473")};
	case 0x03:
		return {UTF8STRC("Group LLC Sublayer Mgt")};
	case 0x05:
		return {UTF8STRC("SNA Path Control (group)")};
	case 0xF5:
		return {UTF8STRC("IBM LAN Management (group)")};
	case 0xFF:
		return {UTF8STRC("Global DSAP (broadcast to all)")};
	}
	return CSTR_NULL;
}

Text::CString Net::PacketAnalyzerEthernet::DHCPOptionGetName(UInt8 t)
{
	switch (t)
	{
	case 1:
		return {UTF8STRC("Subnet Mask")};
	case 2:
		return {UTF8STRC("Time Offset")};
	case 3:
		return {UTF8STRC("Router")};
	case 4:
		return {UTF8STRC("Time Server")};
	case 5:
		return {UTF8STRC("Name Server")};
	case 6:
		return {UTF8STRC("Domain Name Server")};
	case 7:
		return {UTF8STRC("Log Server")};
	case 8:
		return {UTF8STRC("Cookie Server")};
	case 9:
		return {UTF8STRC("LPR Server")};
	case 10:
		return {UTF8STRC("Impress server")};
	case 11:
		return {UTF8STRC("Resource location server")};
	case 12:
		return {UTF8STRC("Host name")};
	case 13:
		return {UTF8STRC("Boot file size")};
	case 14:
		return {UTF8STRC("Merit dump file")};
	case 15:
		return {UTF8STRC("Domain name")};
	case 16:
		return {UTF8STRC("Swap server")};
	case 17:
		return {UTF8STRC("Root path")};
	case 18:
		return {UTF8STRC("Extensions path")};
	case 19:
		return {UTF8STRC("Forward On/Off")};
	case 20:
		return {UTF8STRC("SrcRte On/Off")};
	case 21:
		return {UTF8STRC("Policy Filter")};
	case 22:
		return {UTF8STRC("Max DG Assembly")};
	case 23:
		return {UTF8STRC("Default IP TTL")};
	case 24:
		return {UTF8STRC("MTU Timeout")};
	case 25:
		return {UTF8STRC("MTU Plateau")};
	case 26:
		return {UTF8STRC("MTU Interface")};
	case 27:
		return {UTF8STRC("MTU Subnet")};
	case 28:
		return {UTF8STRC("Broadcast Address")};
	case 29:
		return {UTF8STRC("Mask Discovery")};
	case 30:
		return {UTF8STRC("Mask Supplier")};
	case 31:
		return {UTF8STRC("Router Discovery")};
	case 32:
		return {UTF8STRC("Router Request")};
	case 33:
		return {UTF8STRC("Static Route")};
	case 34:
		return {UTF8STRC("Trailers")};
	case 35:
		return {UTF8STRC("ARP Timeout")};
	case 36:
		return {UTF8STRC("Ethernet")};
	case 37:
		return {UTF8STRC("Default TCP TTL")};
	case 38:
		return {UTF8STRC("Keepalive Time")};
	case 39:
		return {UTF8STRC("Keepalive Data")};
	case 40:
		return {UTF8STRC("NIS Domain")};
	case 41:
		return {UTF8STRC("NIS Servers")};
	case 42:
		return {UTF8STRC("NTP Servers")};
	case 43:
		return {UTF8STRC("Vendor Specific")};
	case 44:
		return {UTF8STRC("NETBIOS Name Srv")};
	case 45:
		return {UTF8STRC("NETBIOS Dist Srv")};
	case 46:
		return {UTF8STRC("NETBIOS Node Type")};
	case 47:
		return {UTF8STRC("NETBIOS Scope")};
	case 48:
		return {UTF8STRC("X Window Font")};
	case 49:
		return {UTF8STRC("X Window Manager")};
	case 50:
		return {UTF8STRC("Requested IP Address")};
	case 51:
		return {UTF8STRC("IP Address Least Time")};
	case 52:
		return {UTF8STRC("Option overload")};
	case 53:
		return {UTF8STRC("DHCP Message Type")};
	case 54:
		return {UTF8STRC("DHCP Server")};
	case 55:
		return {UTF8STRC("Parameter Request List")};
	case 56:
		return {UTF8STRC("Message")};
	case 57:
		return {UTF8STRC("Maximum DHCP message size")};
	case 58:
		return {UTF8STRC("Renew Time Value")};
	case 59:
		return {UTF8STRC("Rebinding Time Value")};
	case 60:
		return {UTF8STRC("Vendor class identifier")};
	case 61:
		return {UTF8STRC("Client ID")};
	case 66:
		return {UTF8STRC("TFTP Server Name")};
	case 67:
		return {UTF8STRC("Bootfile name")};
	case 81:
		return {UTF8STRC("Client FQDN")};
	case 82:
		return {UTF8STRC("Agent Information Option")};
	case 90:
		return {UTF8STRC("Authentication")};
	case 116:
		return {UTF8STRC("Auto-Config")};
	case 120:
		return {UTF8STRC("SIP Servers")};
	case 249:
		return {UTF8STRC("Microsoft Classless Static Route")};
	case 255:
		return {UTF8STRC("End")};
	}
	return CSTR_NULL;
}

Text::CString Net::PacketAnalyzerEthernet::EtherTypeGetName(UInt16 etherType)
{
	switch (etherType)
	{
	case 0x0004:
		return {UTF8STRC("IEEE802.2 LLC")};
	case 0x0006: //ARP
		return {UTF8STRC("ARP")};
	case 0x26: //Legnth = 0x26 (IEEE802.2 LLC)
		return {UTF8STRC("IEEE802.2 LLC")};
	case 0x0800: //IPv4
		return {UTF8STRC("IPv4")};
	case 0x0806: //ARP
		return {UTF8STRC("ARP")};
	case 0x86DD: //IPv6
		return {UTF8STRC("IPv6")};
	case 0x8874: //broadcom
		return {UTF8STRC("Broadcom")};
	}
	return CSTR_NULL;
}
