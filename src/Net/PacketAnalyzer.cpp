#include "Stdafx.h"
#include "IO/FileAnalyse/SBFrameDetail.h"
#include "Net/PacketAnalyzer.h"
#include "Net/PacketAnalyzerBluetooth.h"
#include "Net/PacketAnalyzerEthernet.h"
#include "Text/StringBuilderUTF8.h"

Bool Net::PacketAnalyzer::PacketDataGetName(UInt32 linkType, const UInt8 *packet, UOSInt packetSize, Text::StringBuilderUTF8 *sb)
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

void Net::PacketAnalyzer::PacketDataGetDetail(UInt32 linkType, const UInt8 *packet, UOSInt packetSize, Text::StringBuilderUTF8 *sb)
{
	IO::FileAnalyse::SBFrameDetail frame(sb);
	PacketDataGetDetail(linkType, packet, packetSize, 0, &frame);
}

void Net::PacketAnalyzer::PacketDataGetDetail(UInt32 linkType, const UInt8 *packet, UOSInt packetSize, UInt32 frameOfst, IO::FileAnalyse::FrameDetailHandler *frame)
{
	switch (linkType)
	{
	case 0:
		Net::PacketAnalyzerEthernet::PacketNullGetDetail(packet, packetSize, frameOfst, frame);
		break;
	case 1:
		Net::PacketAnalyzerEthernet::PacketEthernetGetDetail(packet, packetSize, frameOfst, frame);
		break;
	case 101:
		Net::PacketAnalyzerEthernet::PacketIPv4GetDetail(packet, packetSize, frameOfst, frame);
		break;
	case 113:
		Net::PacketAnalyzerEthernet::PacketLinuxGetDetail(packet, packetSize, frameOfst, frame);
		break;
	case 201:
		Net::PacketAnalyzerBluetooth::PacketGetDetail(packet, packetSize, frameOfst, frame);
		break;
	default:
		{
			Text::StringBuilderUTF8 sb;	
			sb.AppendHexBuff(packet, packetSize, ' ', Text::LineBreakType::CRLF);
			frame->AddField(frameOfst, (UInt32)packetSize, (const UTF8Char*)"Unknown", sb.ToString());
		}
		break;
	}
}
