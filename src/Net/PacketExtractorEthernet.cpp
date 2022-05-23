#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "Net/PacketExtractorEthernet.h"

const UInt8 *Net::PacketExtractorEthernet::EthernetExtractUDP(const UInt8 *packet, UOSInt packetSize, UOSInt *udpSize, EthernetHeader *etherHdr, IPv4Header *ipv4Hdr, UDPHeader *udpHdr)
{
	EthernetHeader myEtherHdr;
	if (etherHdr == 0)
	{
		etherHdr = &myEtherHdr;
	}
	UOSInt etherDataSize;
	const UInt8 *etherData = EthernetExtract(packet, packetSize, &etherDataSize, etherHdr);
	if (etherData == 0)
	{
		return 0;
	}
	if (etherHdr->etherType == 0x0800)
	{
		IPv4Header myIPv4Hdr;
		if (ipv4Hdr == 0)
		{
			ipv4Hdr = &myIPv4Hdr;
		}
		UOSInt ipv4Size;
		const UInt8 *ipv4Data = IPv4Extract(etherData, etherDataSize, &ipv4Size, ipv4Hdr);
		if (ipv4Data == 0)
		{
			return 0;
		}

		if (ipv4Hdr->protocol != 17) //UDP
		{
			return 0;
		}
		return UDPExtract(ipv4Data, ipv4Size, udpSize, udpHdr);
	}
	else
	{
		return 0;
	}
}

const UInt8 *Net::PacketExtractorEthernet::EthernetExtract(const UInt8 *packet, UOSInt packetSize, UOSInt *dataSize, EthernetHeader *etherHdr)
{
	if (packetSize < 14)
	{
		return 0;
	}
	MemCopyNO(etherHdr->destAddr, &packet[0], 6);
	MemCopyNO(etherHdr->srcAddr, &packet[6], 6);
	etherHdr->etherType = ReadMUInt16(&packet[12]);
	*dataSize = packetSize - 14;
	return packet + 14;
}

const UInt8 *Net::PacketExtractorEthernet::IPv4Extract(const UInt8 *packet, UOSInt packetSize, UOSInt *dataSize, IPv4Header *ipv4Hdr)
{
	if (packetSize < 20)
	{
		return 0;
	}
	if (packet[0] != 0x45)
	{
		return 0;
	}
	ipv4Hdr->verLen = packet[0];
	ipv4Hdr->dsvc = packet[1];
	ipv4Hdr->length = ReadMUInt16(&packet[2]);
	ipv4Hdr->id = ReadMUInt16(&packet[4]);
	ipv4Hdr->flags = ReadMUInt16(&packet[6]);
	ipv4Hdr->ttl = packet[8];
	ipv4Hdr->protocol = packet[9];
	ipv4Hdr->chksum = ReadMUInt16(&packet[10]);
	MemCopyNO(ipv4Hdr->srcAddr, &packet[12], 4);
	MemCopyNO(ipv4Hdr->destAddr, &packet[16], 4);
	*dataSize = packetSize - 20;
	return packet + 20;
}

const UInt8 *Net::PacketExtractorEthernet::UDPExtract(const UInt8 *packet, UOSInt packetSize, UOSInt *dataSize, UDPHeader *udpHdr)
{
	if (packetSize < 8)
	{
		return 0;
	}
	udpHdr->srcPort = ReadMUInt16(&packet[0]);
	udpHdr->destPort = ReadMUInt16(&packet[2]);
	udpHdr->leng = ReadMUInt16(&packet[4]);
	udpHdr->chksum = ReadMUInt16(&packet[6]);
	*dataSize = packetSize - 8;
	return packet + 8;
}
