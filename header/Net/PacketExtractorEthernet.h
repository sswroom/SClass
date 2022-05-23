#ifndef _SM_NET_PACKETEXTRACTORETHERNET
#define _SM_NET_PACKETEXTRACTORETHERNET

namespace Net
{
	class PacketExtractorEthernet
	{
	public:
		struct EthernetHeader
		{
			UInt8 destAddr[6];
			UInt8 srcAddr[6];
			UInt16 etherType;
		};

		struct IPv4Header
		{
			UInt8 verLen;
			UInt8 dsvc;
			UInt16 length;
			UInt16 id;
			UInt16 flags;
			UInt8 ttl;
			UInt8 protocol;
			UInt16 chksum;
			UInt8 srcAddr[4];
			UInt8 destAddr[4];
		};

		struct UDPHeader
		{
			UInt16 srcPort;
			UInt16 destPort;
			UInt16 leng;
			UInt16 chksum;
		};
	public:
		static const UInt8 *EthernetExtractUDP(const UInt8 *packet, UOSInt packetSize, UOSInt *dataSize, EthernetHeader *etherHdr, IPv4Header *ipv4Hdr, UDPHeader *udpHdr);
		static const UInt8 *EthernetExtract(const UInt8 *packet, UOSInt packetSize, UOSInt *dataSize, EthernetHeader *etherHdr);
		static const UInt8 *IPv4Extract(const UInt8 *packet, UOSInt packetSize, UOSInt *dataSize, IPv4Header *ipv4Hdr);
		static const UInt8 *UDPExtract(const UInt8 *packet, UOSInt packetSize, UOSInt *dataSize, UDPHeader *udpHdr);
	};
}
#endif
