#ifndef _SM_NET_PACKETANALYZERETHERNET
#define _SM_NET_PACKETANALYZERETHERNET
#include "Text/StringBuilderUTF.h"

namespace Net
{
	class PacketAnalyzerEthernet
	{
	public:
		static Bool PacketNullGetName(const UInt8 *packet, UOSInt packetSize, Text::StringBuilderUTF *sb);
		static Bool PacketEthernetGetName(const UInt8 *packet, UOSInt packetSize, Text::StringBuilderUTF *sb);
		static Bool PacketLinuxGetName(const UInt8 *packet, UOSInt packetSize, Text::StringBuilderUTF *sb);
		static Bool PacketEthernetDataGetName(UInt16 etherType, const UInt8 *packet, UOSInt packetSize, Text::StringBuilderUTF *sb);
		static Bool PacketIPv4GetName(const UInt8 *packet, UOSInt packetSize, Text::StringBuilderUTF *sb);
		static Bool PacketIPv6GetName(const UInt8 *packet, UOSInt packetSize, Text::StringBuilderUTF *sb);
		static Bool PacketIPDataGetName(UInt8 protocol, const UInt8 *packet, UOSInt packetSize, Text::StringBuilderUTF *sb);

		static void PacketNullGetDetail(const UInt8 *packet, UOSInt packetSize, Text::StringBuilderUTF *sb);
		static void PacketEthernetGetDetail(const UInt8 *packet, UOSInt packetSize, Text::StringBuilderUTF *sb);
		static void PacketLinuxGetDetail(const UInt8 *packet, UOSInt packetSize, Text::StringBuilderUTF *sb);
		static void PacketEthernetDataGetDetail(UInt16 etherType, const UInt8 *packet, UOSInt packetSize, Text::StringBuilderUTF *sb);
		static void PacketIEEE802_2LLCGetDetail(const UInt8 *packet, UOSInt packetSize, Text::StringBuilderUTF *sb);
		static void PacketIPv4GetDetail(const UInt8 *packet, UOSInt packetSize, Text::StringBuilderUTF *sb);
		static void PacketIPv6GetDetail(const UInt8 *packet, UOSInt packetSize, Text::StringBuilderUTF *sb);
		static void PacketIPDataGetDetail(UInt8 protocol, const UInt8 *packet, UOSInt packetSize, Text::StringBuilderUTF *sb);
		static void PacketUDPGetDetail(UInt16 srcPort, UInt16 destPort, const UInt8 *packet, UOSInt packetSize, Text::StringBuilderUTF *sb);
		static void PacketDNSGetDetail(const UInt8 *packet, UOSInt packetSize, Text::StringBuilderUTF *sb);
		static void PacketLoRaMACGetDetail(const UInt8 *packet, UOSInt packetSize, Text::StringBuilderUTF *sb);

		static UOSInt HeaderIPv4GetDetail(const UInt8 *packet, UOSInt packetSize, Text::StringBuilderUTF *sb);

		static UTF8Char *NetBIOSGetName(UTF8Char *sbuff, const UTF8Char *nbName);
		static const UTF8Char *TCPPortGetName(UInt16 port); //null = unknwon
		static const UTF8Char *UDPPortGetName(UInt16 port); //null = unknwon
		static const UTF8Char *LSAPGetName(UInt8 lsap);
		static const UTF8Char *DHCPOptionGetName(UInt8 t);
	};
}
#endif
