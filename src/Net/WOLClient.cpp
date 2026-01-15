#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/ByteTool_C.h"
#include "Net/WOLClient.h"

void __stdcall Net::WOLClient::PacketHdlr(NN<const Net::SocketUtil::AddressInfo> addr, UInt16 port, Data::ByteArrayR data, AnyType userData)
{
}

Net::WOLClient::WOLClient(NN<Net::SocketFactory> sockf, UInt32 adapterIP, NN<IO::LogTool> log)
{
	this->sockf = sockf;
	this->adapterIP = Net::SocketUtil::IPv4ToBroadcast(adapterIP);
	NEW_CLASSNN(this->svr, Net::UDPServer(sockf, nullptr, 0, nullptr, PacketHdlr, this, log, nullptr, 1, false));
	this->svr->SetBroadcast(true);
}

Net::WOLClient::~WOLClient()
{
	this->svr.Delete();
}

Bool Net::WOLClient::IsError()
{
	return this->svr->IsError();
}

Bool Net::WOLClient::WakeDevice(UnsafeArray<const UInt8> macAddr)
{
	UInt8 packet[102];
	WriteNUInt32(&packet[0], 0xffffffff);
	WriteNInt16(&packet[4], (Int16)0xffff);
	UOSInt i = 0;
	while (i < 16)
	{
		MemCopyNO(&packet[i * 6 + 6], macAddr.Ptr(), 6);
		i++;
	}
	Net::SocketUtil::AddressInfo addr;
	Net::SocketUtil::SetAddrInfoV4(addr, this->adapterIP);
	return this->svr->SendTo(addr, 9, packet, 102);
}
