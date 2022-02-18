#include "Stdafx.h"
#include "MyMemory.h"
#include "Net/SNMPTrapMonitor.h"

void __stdcall Net::SNMPTrapMonitor::OnSNMPPacket(const Net::SocketUtil::AddressInfo *addr, UInt16 port, const UInt8 *buff, UOSInt dataSize, void *userData)
{
	Net::SNMPTrapMonitor *me = (Net::SNMPTrapMonitor*)userData;
	Net::SNMPUtil::TrapInfo trap;
	Data::ArrayList<Net::SNMPUtil::BindingItem*> itemList;
	Net::SNMPUtil::ErrorStatus err;
	err = Net::SNMPUtil::PDUParseTrapMessage(buff, dataSize, &trap, &itemList);
	if (err == Net::SNMPUtil::ES_NOERROR)
	{
		if (!me->hdlr(me->hdlrObj, addr, port, &trap, &itemList))
		{
			UOSInt i = itemList.GetCount();
			while (i-- > 0)
			{
				Net::SNMPUtil::FreeBindingItem(itemList.GetItem(i));
			}
		}
	}
	else
	{
		UOSInt i = itemList.GetCount();
		while (i-- > 0)
		{
			Net::SNMPUtil::FreeBindingItem(itemList.GetItem(i));
		}
	}
}

Net::SNMPTrapMonitor::SNMPTrapMonitor(Net::SocketFactory *sockf, SNMPTrapHandler hdlr, void *userObj)
{
	this->hdlr = hdlr;
	this->hdlrObj = userObj;
	NEW_CLASS(this->svr, Net::UDPServer(sockf, 0, 162, CSTR_NULL, OnSNMPPacket, this, 0, CSTR_NULL, 2, false));
}

Net::SNMPTrapMonitor::~SNMPTrapMonitor()
{
	DEL_CLASS(this->svr);
}

Bool Net::SNMPTrapMonitor::IsError()
{
	return this->svr->IsError();
}
