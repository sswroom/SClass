#include "Stdafx.h"
#include "Net/LoRaGWMonitor.h"
#include "Net/PacketExtractorEthernet.h"

#include <stdio.h>

void __stdcall Net::LoRaGWMonitor::OnRAWPacket(AnyType userData, const UInt8 *packetData, UOSInt packetSize)
{
	NN<Net::LoRaGWMonitor> me = userData.GetNN<Net::LoRaGWMonitor>();
	Net::PacketExtractorEthernet::IPv4Header ipv4Hdr;
	Net::PacketExtractorEthernet::UDPHeader udpHdr;
	Net::PacketExtractorEthernet::EthernetHeader etherHdr;
	UOSInt udpSize;
	const UInt8 *udpData = Net::PacketExtractorEthernet::EthernetExtractUDP(packetData, packetSize, &udpSize, &etherHdr, &ipv4Hdr, &udpHdr);
	if (udpData && udpSize >= ((UOSInt)udpHdr.leng - 8) && udpHdr.leng >= 12)
	{
		if (udpHdr.srcPort == me->port)
		{
			me->msgHdlr(me->msgHdlrObj, false, udpData[0], ReadMUInt16(&udpData[1]), udpData[3], udpData + 4, (UOSInt)udpHdr.leng - 4 - 8);
		}
		else if (udpHdr.destPort == me->port)
		{
			me->msgHdlr(me->msgHdlrObj, true, udpData[0], ReadMUInt16(&udpData[1]), udpData[3], udpData + 4, (UOSInt)udpHdr.leng - 4 - 8);
		}
	}
}

Net::LoRaGWMonitor::LoRaGWMonitor(NN<Net::SocketFactory> sockf, UInt16 port, GWMPMessage msgHdlr, AnyType msgHdlrObj)
{
	this->sockf = sockf;
	this->port = port;
	this->msgHdlr = msgHdlr;
	this->msgHdlrObj = msgHdlrObj;
	this->s = this->sockf->CreateRAWSocket();
	if (this->s)
	{
		NEW_CLASS(this->socMon, Net::SocketMonitor(this->sockf, this->s, OnRAWPacket, this, 4));
	}
	else
	{
		this->socMon = 0;
	}
}

Net::LoRaGWMonitor::~LoRaGWMonitor()
{
	SDEL_CLASS(this->socMon);
}

Bool Net::LoRaGWMonitor::IsError()
{
	return this->socMon == 0;
};
