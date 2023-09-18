#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "Net/WOLClient.h"

void __stdcall Net::WOLClient::PacketHdlr(NotNullPtr<const Net::SocketUtil::AddressInfo> addr, UInt16 port, const UInt8 *buff, UOSInt dataSize, void *userData)
{
}

Net::WOLClient::WOLClient(NotNullPtr<Net::SocketFactory> sockf, UInt32 adapterIP, NotNullPtr<IO::LogTool> log)
{
	this->sockf = sockf;
	this->adapterIP = Net::SocketUtil::IPv4ToBroadcast(adapterIP);
	NEW_CLASS(this->svr, Net::UDPServer(sockf, 0, 0, CSTR_NULL, PacketHdlr, this, log, CSTR_NULL, 1, false));
	this->svr->SetBroadcast(true);
}

Net::WOLClient::~WOLClient()
{
	DEL_CLASS(this->svr);
}

Bool Net::WOLClient::IsError()
{
	return this->svr->IsError();
}

Bool Net::WOLClient::WakeDevice(const UInt8 *macAddr)
{
	UInt8 packet[102];
	WriteNUInt32(&packet[0], 0xffffffff);
	WriteNInt16(&packet[4], (Int16)0xffff);
	UOSInt i = 0;
	while (i < 16)
	{
		MemCopyNO(&packet[i * 6 + 6], macAddr, 6);
		i++;
	}
	Net::SocketUtil::AddressInfo addr;
	Net::SocketUtil::SetAddrInfoV4(addr, this->adapterIP);
	return this->svr->SendTo(addr, 9, packet, 102);
}
