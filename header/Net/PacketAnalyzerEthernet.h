#ifndef _SM_NET_PACKETANALYZERETHERNET
#define _SM_NET_PACKETANALYZERETHERNET
#include "IO/FileAnalyse/FrameDetailHandler.h"
#include "Text/StringBuilderUTF8.h"

namespace Net
{
	class PacketAnalyzerEthernet
	{
	public:
		static Bool PacketNullGetName(const UInt8 *packet, UOSInt packetSize, Text::StringBuilderUTF8 *sb);
		static Bool PacketEthernetGetName(const UInt8 *packet, UOSInt packetSize, Text::StringBuilderUTF8 *sb);
		static Bool PacketLinuxGetName(const UInt8 *packet, UOSInt packetSize, Text::StringBuilderUTF8 *sb);
		static Bool PacketEthernetDataGetName(UInt16 etherType, const UInt8 *packet, UOSInt packetSize, Text::StringBuilderUTF8 *sb);
		static Bool PacketIPv4GetName(const UInt8 *packet, UOSInt packetSize, Text::StringBuilderUTF8 *sb);
		static Bool PacketIPv6GetName(const UInt8 *packet, UOSInt packetSize, Text::StringBuilderUTF8 *sb);
		static Bool PacketIPDataGetName(UInt8 protocol, const UInt8 *packet, UOSInt packetSize, Text::StringBuilderUTF8 *sb);

		static void PacketNullGetDetail(const UInt8 *packet, UOSInt packetSize, Text::StringBuilderUTF8 *sb);
		static void PacketEthernetGetDetail(const UInt8 *packet, UOSInt packetSize, Text::StringBuilderUTF8 *sb);
		static void PacketLinuxGetDetail(const UInt8 *packet, UOSInt packetSize, Text::StringBuilderUTF8 *sb);
		static void PacketEthernetDataGetDetail(UInt16 etherType, const UInt8 *packet, UOSInt packetSize, Text::StringBuilderUTF8 *sb);
		static void PacketIEEE802_2LLCGetDetail(const UInt8 *packet, UOSInt packetSize, Text::StringBuilderUTF8 *sb);
		static void PacketIPv4GetDetail(const UInt8 *packet, UOSInt packetSize, Text::StringBuilderUTF8 *sb);
		static void PacketIPv6GetDetail(const UInt8 *packet, UOSInt packetSize, Text::StringBuilderUTF8 *sb);
		static void PacketIPDataGetDetail(UInt8 protocol, const UInt8 *packet, UOSInt packetSize, Text::StringBuilderUTF8 *sb);
		static void PacketUDPGetDetail(UInt16 srcPort, UInt16 destPort, const UInt8 *packet, UOSInt packetSize, Text::StringBuilderUTF8 *sb);
		static void PacketDNSGetDetail(const UInt8 *packet, UOSInt packetSize, Text::StringBuilderUTF8 *sb);
		static void PacketLoRaMACGetDetail(const UInt8 *packet, UOSInt packetSize, Text::StringBuilderUTF8 *sb);

		static UOSInt HeaderIPv4GetDetail(const UInt8 *packet, UOSInt packetSize, Text::StringBuilderUTF8 *sb);

		static void PacketNullGetDetail(const UInt8 *packet, UOSInt packetSize, UInt32 frameOfst, IO::FileAnalyse::FrameDetailHandler *frame);
		static void PacketEthernetGetDetail(const UInt8 *packet, UOSInt packetSize, UInt32 frameOfst, IO::FileAnalyse::FrameDetailHandler *frame);
		static void PacketLinuxGetDetail(const UInt8 *packet, UOSInt packetSize, UInt32 frameOfst, IO::FileAnalyse::FrameDetailHandler *frame);
		static void PacketEthernetDataGetDetail(UInt16 etherType, const UInt8 *packet, UOSInt packetSize, UInt32 frameOfst, IO::FileAnalyse::FrameDetailHandler *frame);
		static void PacketIEEE802_2LLCGetDetail(const UInt8 *packet, UOSInt packetSize, UInt32 frameOfst, IO::FileAnalyse::FrameDetailHandler *frame);
		static void PacketARPGetDetail(const UInt8 *packet, UOSInt packetSize, UInt32 frameOfst, IO::FileAnalyse::FrameDetailHandler *frame);
		static void PacketIPv4GetDetail(const UInt8 *packet, UOSInt packetSize, UInt32 frameOfst, IO::FileAnalyse::FrameDetailHandler *frame);
		static void PacketIPv6GetDetail(const UInt8 *packet, UOSInt packetSize, UInt32 frameOfst, IO::FileAnalyse::FrameDetailHandler *frame);
		static void PacketIPDataGetDetail(UInt8 protocol, const UInt8 *packet, UOSInt packetSize, UInt32 frameOfst, IO::FileAnalyse::FrameDetailHandler *frame);
		static void PacketUDPGetDetail(UInt16 srcPort, UInt16 destPort, const UInt8 *packet, UOSInt packetSize, UInt32 frameOfst, IO::FileAnalyse::FrameDetailHandler *frame);
		static void PacketDNSGetDetail(const UInt8 *packet, UOSInt packetSize, UInt32 frameOfst, IO::FileAnalyse::FrameDetailHandler *frame);
		static void PacketLoRaMACGetDetail(const UInt8 *packet, UOSInt packetSize, UInt32 frameOfst, IO::FileAnalyse::FrameDetailHandler *frame);

		static UOSInt HeaderIPv4GetDetail(const UInt8 *packet, UOSInt packetSize, UInt32 frameOfst, IO::FileAnalyse::FrameDetailHandler *frame);

		static Text::CString TCPPortGetName(UInt16 port); //null = unknwon
		static Text::CString UDPPortGetName(UInt16 port); //null = unknwon
		static Text::CString LSAPGetName(UInt8 lsap);
		static Text::CString DHCPOptionGetName(UInt8 t);
		static Text::CString EtherTypeGetName(UInt16 etherType);
	};
}
#endif
