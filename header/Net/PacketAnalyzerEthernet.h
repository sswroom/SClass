#ifndef _SM_NET_PACKETANALYZERETHERNET
#define _SM_NET_PACKETANALYZERETHERNET
#include "IO/FileAnalyse/FrameDetailHandler.h"
#include "Text/StringBuilderUTF8.h"

namespace Net
{
	class PacketAnalyzerEthernet
	{
	public:
		static Bool PacketNullGetName(UnsafeArray<const UInt8> packet, UIntOS packetSize, NN<Text::StringBuilderUTF8> sb);
		static Bool PacketEthernetGetName(UnsafeArray<const UInt8> packet, UIntOS packetSize, NN<Text::StringBuilderUTF8> sb);
		static Bool PacketLinuxGetName(UnsafeArray<const UInt8> packet, UIntOS packetSize, NN<Text::StringBuilderUTF8> sb);
		static Bool PacketEthernetDataGetName(UInt16 etherType, UnsafeArray<const UInt8> packet, UIntOS packetSize, NN<Text::StringBuilderUTF8> sb);
		static Bool PacketIPv4GetName(UnsafeArray<const UInt8> packet, UIntOS packetSize, NN<Text::StringBuilderUTF8> sb);
		static Bool PacketIPv6GetName(UnsafeArray<const UInt8> packet, UIntOS packetSize, NN<Text::StringBuilderUTF8> sb);
		static Bool PacketIPDataGetName(UInt8 protocol, UnsafeArray<const UInt8> packet, UIntOS packetSize, NN<Text::StringBuilderUTF8> sb);

		static void PacketNullGetDetail(UnsafeArray<const UInt8> packet, UIntOS packetSize, NN<Text::StringBuilderUTF8> sb);
		static void PacketEthernetGetDetail(UnsafeArray<const UInt8> packet, UIntOS packetSize, NN<Text::StringBuilderUTF8> sb);
		static void PacketLinuxGetDetail(UnsafeArray<const UInt8> packet, UIntOS packetSize, NN<Text::StringBuilderUTF8> sb);
		static void PacketEthernetDataGetDetail(UInt16 etherType, UnsafeArray<const UInt8> packet, UIntOS packetSize, NN<Text::StringBuilderUTF8> sb);
		static void PacketIEEE802_2LLCGetDetail(UnsafeArray<const UInt8> packet, UIntOS packetSize, NN<Text::StringBuilderUTF8> sb);
		static void PacketIPv4GetDetail(UnsafeArray<const UInt8> packet, UIntOS packetSize, NN<Text::StringBuilderUTF8> sb);
		static void PacketIPv6GetDetail(UnsafeArray<const UInt8> packet, UIntOS packetSize, NN<Text::StringBuilderUTF8> sb);
		static void PacketIPDataGetDetail(UInt8 protocol, UnsafeArray<const UInt8> packet, UIntOS packetSize, NN<Text::StringBuilderUTF8> sb);
		static void PacketUDPGetDetail(UInt16 srcPort, UInt16 destPort, UnsafeArray<const UInt8> packet, UIntOS packetSize, NN<Text::StringBuilderUTF8> sb);
		static void PacketDNSGetDetail(UnsafeArray<const UInt8> packet, UIntOS packetSize, NN<Text::StringBuilderUTF8> sb);
		static void PacketLoRaMACGetDetail(UnsafeArray<const UInt8> packet, UIntOS packetSize, NN<Text::StringBuilderUTF8> sb);

		static UIntOS HeaderIPv4GetDetail(UnsafeArray<const UInt8> packet, UIntOS packetSize, NN<Text::StringBuilderUTF8> sb);

		static void PacketNullGetDetail(UnsafeArray<const UInt8> packet, UIntOS packetSize, UInt32 frameOfst, NN<IO::FileAnalyse::FrameDetailHandler> frame);
		static void PacketEthernetGetDetail(UnsafeArray<const UInt8> packet, UIntOS packetSize, UInt32 frameOfst, NN<IO::FileAnalyse::FrameDetailHandler> frame);
		static void PacketLinuxGetDetail(UnsafeArray<const UInt8> packet, UIntOS packetSize, UInt32 frameOfst, NN<IO::FileAnalyse::FrameDetailHandler> frame);
		static void PacketEthernetDataGetDetail(UInt16 etherType, UnsafeArray<const UInt8> packet, UIntOS packetSize, UInt32 frameOfst, NN<IO::FileAnalyse::FrameDetailHandler> frame);
		static void PacketIEEE802_2LLCGetDetail(UnsafeArray<const UInt8> packet, UIntOS packetSize, UInt32 frameOfst, NN<IO::FileAnalyse::FrameDetailHandler> frame);
		static void PacketARPGetDetail(UnsafeArray<const UInt8> packet, UIntOS packetSize, UInt32 frameOfst, NN<IO::FileAnalyse::FrameDetailHandler> frame);
		static void PacketIPv4GetDetail(UnsafeArray<const UInt8> packet, UIntOS packetSize, UInt32 frameOfst, NN<IO::FileAnalyse::FrameDetailHandler> frame);
		static void PacketIPv6GetDetail(UnsafeArray<const UInt8> packet, UIntOS packetSize, UInt32 frameOfst, NN<IO::FileAnalyse::FrameDetailHandler> frame);
		static void PacketIPDataGetDetail(UInt8 protocol, UnsafeArray<const UInt8> packet, UIntOS packetSize, UInt32 frameOfst, NN<IO::FileAnalyse::FrameDetailHandler> frame);
		static void PacketUDPGetDetail(UInt16 srcPort, UInt16 destPort, UnsafeArray<const UInt8> packet, UIntOS packetSize, UInt32 frameOfst, NN<IO::FileAnalyse::FrameDetailHandler> frame);
		static void PacketDNSGetDetail(UnsafeArray<const UInt8> packet, UIntOS packetSize, UInt32 frameOfst, NN<IO::FileAnalyse::FrameDetailHandler> frame);
		static void PacketLoRaMACGetDetail(UnsafeArray<const UInt8> packet, UIntOS packetSize, UInt32 frameOfst, NN<IO::FileAnalyse::FrameDetailHandler> frame);

		static UIntOS HeaderIPv4GetDetail(UnsafeArray<const UInt8> packet, UIntOS packetSize, UInt32 frameOfst, NN<IO::FileAnalyse::FrameDetailHandler> frame);

		static Text::CString TCPPortGetName(UInt16 port); //null = unknwon
		static Text::CString UDPPortGetName(UInt16 port); //null = unknwon
		static Text::CString LSAPGetName(UInt8 lsap);
		static Text::CString DHCPOptionGetName(UInt8 t);
		static Text::CString EtherTypeGetName(UInt16 etherType);
	};
}
#endif
