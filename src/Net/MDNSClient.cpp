#include "Stdafx.h"
#include "Net/MDNSClient.h"

void __stdcall Net::MDNSClient::OnUDPPacket(NN<const Net::SocketUtil::AddressInfo> addr, UInt16 port, Data::ByteArrayR data, AnyType userData)
{
	UTF8Char sbuff[128];
	sbuff[0] = 0;
	Net::SocketUtil::GetAddrName(sbuff, addr);
	printf("Packet received from %s\r\n", sbuff);
}

Net::MDNSClient::MDNSClient(NN<Net::SocketFactory> sockf)
{
	this->sockf = sockf;
	NEW_CLASSNN(this->udp, Net::UDPServer(sockf, 0, 5353, 0, OnUDPPacket, this, log, 0, 4, true));
	this->udp->AddMulticastIP(Net::SocketUtil::GetIPAddr(CSTR("224.0.0.251")));
}

Net::MDNSClient::~MDNSClient()
{
	this->udp.Delete();
}

Bool Net::MDNSClient::IsError()
{
	return this->udp->IsError();
}
