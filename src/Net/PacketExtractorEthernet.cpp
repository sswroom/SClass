#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "Net/PacketExtractorEthernet.h"

UnsafeArrayOpt<const UInt8> Net::PacketExtractorEthernet::EthernetExtractUDP(UnsafeArray<const UInt8> packet, UOSInt packetSize, OutParam<UOSInt> udpSize, Optional<EthernetHeader> etherHdr, Optional<IPv4Header> ipv4Hdr, NN<UDPHeader> udpHdr)
{
	EthernetHeader myEtherHdr;
	NN<EthernetHeader> nnetherHdr;
	if (!etherHdr.SetTo(nnetherHdr))
	{
		nnetherHdr = myEtherHdr;
	}
	UOSInt etherDataSize;
	UnsafeArray<const UInt8> etherData;
	if (!EthernetExtract(packet, packetSize, etherDataSize, nnetherHdr).SetTo(etherData))
	{
		return 0;
	}
	if (nnetherHdr->etherType == 0x0800)
	{
		IPv4Header myIPv4Hdr;
		NN<IPv4Header> nnipv4Hdr;
		if (!ipv4Hdr.SetTo(nnipv4Hdr))
		{
			nnipv4Hdr = myIPv4Hdr;
		}
		UOSInt ipv4Size;
		UnsafeArray<const UInt8> ipv4Data;
		if (!IPv4Extract(etherData, etherDataSize, ipv4Size, nnipv4Hdr).SetTo(ipv4Data))
		{
			return 0;
		}

		if (nnipv4Hdr->protocol != 17) //UDP
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

UnsafeArrayOpt<const UInt8> Net::PacketExtractorEthernet::EthernetExtract(UnsafeArray<const UInt8> packet, UOSInt packetSize, OutParam<UOSInt> dataSize, NN<EthernetHeader> etherHdr)
{
	if (packetSize < 14)
	{
		return 0;
	}
	MemCopyNO(etherHdr->destAddr, &packet[0], 6);
	MemCopyNO(etherHdr->srcAddr, &packet[6], 6);
	etherHdr->etherType = ReadMUInt16(&packet[12]);
	dataSize.Set(packetSize - 14);
	return packet + 14;
}

UnsafeArrayOpt<const UInt8> Net::PacketExtractorEthernet::IPv4Extract(UnsafeArray<const UInt8> packet, UOSInt packetSize, OutParam<UOSInt> dataSize, NN<IPv4Header> ipv4Hdr)
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
	dataSize.Set(packetSize - 20);
	return packet + 20;
}

UnsafeArrayOpt<const UInt8> Net::PacketExtractorEthernet::UDPExtract(UnsafeArray<const UInt8> packet, UOSInt packetSize, OutParam<UOSInt> dataSize, NN<UDPHeader> udpHdr)
{
	if (packetSize < 8)
	{
		return 0;
	}
	udpHdr->srcPort = ReadMUInt16(&packet[0]);
	udpHdr->destPort = ReadMUInt16(&packet[2]);
	udpHdr->leng = ReadMUInt16(&packet[4]);
	udpHdr->chksum = ReadMUInt16(&packet[6]);
	dataSize.Set(packetSize - 8);
	return packet + 8;
}
