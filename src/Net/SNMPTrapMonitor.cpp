#include "Stdafx.h"
#include "MyMemory.h"
#include "Net/SNMPTrapMonitor.h"

void __stdcall Net::SNMPTrapMonitor::OnSNMPPacket(NN<const Net::SocketUtil::AddressInfo> addr, UInt16 port, Data::ByteArrayR data, AnyType userData)
{
	NN<Net::SNMPTrapMonitor> me = userData.GetNN<Net::SNMPTrapMonitor>();
	Net::SNMPUtil::TrapInfo trap;
	Data::ArrayListNN<Net::SNMPUtil::BindingItem> itemList;
	Net::SNMPUtil::ErrorStatus err;
	err = Net::SNMPUtil::PDUParseTrapMessage(data, trap, itemList);
	if (err == Net::SNMPUtil::ES_NOERROR)
	{
		if (!me->hdlr(me->hdlrObj, addr, port, trap, itemList))
		{
			itemList.FreeAll(Net::SNMPUtil::FreeBindingItem);
		}
	}
	else
	{
		itemList.FreeAll(Net::SNMPUtil::FreeBindingItem);
	}
}

Net::SNMPTrapMonitor::SNMPTrapMonitor(NN<Net::SocketFactory> sockf, SNMPTrapHandler hdlr, AnyType userObj, NN<IO::LogTool> log)
{
	this->hdlr = hdlr;
	this->hdlrObj = userObj;
	NEW_CLASS(this->svr, Net::UDPServer(sockf, 0, 162, nullptr, OnSNMPPacket, this, log, nullptr, 2, false));
}

Net::SNMPTrapMonitor::~SNMPTrapMonitor()
{
	DEL_CLASS(this->svr);
}

Bool Net::SNMPTrapMonitor::IsError()
{
	return this->svr->IsError();
}
