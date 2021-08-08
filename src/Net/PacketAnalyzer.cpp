#include "Stdafx.h"
#include "Net/PacketAnalyzer.h"
#include "Net/PacketAnalyzerBluetooth.h"
#include "Net/PacketAnalyzerEthernet.h"

Bool Net::PacketAnalyzer::PacketDataGetName(UInt32 linkType, const UInt8 *packet, UOSInt packetSize, Text::StringBuilderUTF *sb)
{
	switch (linkType)
	{
	case 0:
		return Net::PacketAnalyzerEthernet::PacketNullGetName(packet, packetSize, sb);
	case 1:
		return Net::PacketAnalyzerEthernet::PacketEthernetGetName(packet, packetSize, sb);
	case 101:
		return Net::PacketAnalyzerEthernet::PacketIPv4GetName(packet, packetSize, sb);
	case 113:
		return Net::PacketAnalyzerEthernet::PacketLinuxGetName(packet, packetSize, sb);
	case 201:
		return Net::PacketAnalyzerBluetooth::PacketGetName(packet, packetSize, sb);
	}
	return false;
}

void Net::PacketAnalyzer::PacketDataGetDetail(UInt32 linkType, const UInt8 *packet, UOSInt packetSize, Text::StringBuilderUTF *sb)
{
	switch (linkType)
	{
	case 0:
		Net::PacketAnalyzerEthernet::PacketNullGetDetail(packet, packetSize, sb);
		break;
	case 1:
		Net::PacketAnalyzerEthernet::PacketEthernetGetDetail(packet, packetSize, sb);
		break;
	case 101:
		Net::PacketAnalyzerEthernet::PacketIPv4GetDetail(packet, packetSize, sb);
		break;
	case 113:
		Net::PacketAnalyzerEthernet::PacketLinuxGetDetail(packet, packetSize, sb);
		break;
	case 201:
		Net::PacketAnalyzerBluetooth::PacketGetDetail(packet, packetSize, sb);
		break;
	default:
		sb->Append((const UTF8Char*)"\r\n");
		sb->AppendHexBuff(packet, packetSize, ' ', Text::LBT_CRLF);
	}
}
