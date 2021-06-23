#include "Stdafx.h"
#include "MyMemory.h"
#include "Net/ASN1PDUBuilder.h"
#include "Net/SNMPClient.h"
#include "Net/SNMPInfo.h"
#include "Sync/MutexUsage.h"
#include "Sync/Thread.h"
#include "Text/StringBuilderUTF8.h"

void __stdcall Net::SNMPClient::OnSNMPPacket(const Net::SocketUtil::AddressInfo *addr, UInt16 port, const UInt8 *buff, UOSInt dataSize, void *userData)
{
	Net::SNMPClient *me = (Net::SNMPClient*)userData;
	Data::ArrayList<Net::SNMPUtil::BindingItem*> itemList;
	Int32 reqId;
	Net::SNMPUtil::ErrorStatus err;
	Sync::MutexUsage mutUsage(me->scanMut);
	if (me->scanList)
	{
		if (port == 161)
		{
			Net::SocketUtil::AddressInfo *remoteAddr = MemAlloc(Net::SocketUtil::AddressInfo, 1);
			MemCopyNO(remoteAddr, addr, sizeof(Net::SocketUtil::AddressInfo));
			me->scanList->Add(remoteAddr);
		}
		mutUsage.EndUse();
		return;
	}
	mutUsage.EndUse();
	err = Net::SNMPUtil::PDUParseMessage(buff, dataSize, &reqId, &itemList);
	if (!me->hasResp && (err != Net::SNMPUtil::ES_NOERROR || reqId == me->reqId))
	{
		if (me->respList)
		{
			me->respList->AddRange(&itemList);
		}
		else
		{
			UOSInt i = itemList.GetCount();
			while (i-- >0)
			{
				Net::SNMPUtil::FreeBindingItem(itemList.GetItem(i));
			}
		}
		me->respStatus = err;
		me->hasResp = true;
		me->respEvt->Set();
	}
	else
	{
		UOSInt i = itemList.GetCount();
		while (i-- >0)
		{
			Net::SNMPUtil::FreeBindingItem(itemList.GetItem(i));
		}
	}
}

Net::SNMPClient::SNMPClient(Net::SocketFactory *sockf)
{
	NEW_CLASS(this->scanMut, Sync::Mutex());
	this->scanList = 0;
	NEW_CLASS(this->svr, Net::UDPServer(sockf, 0, 0, 0, OnSNMPPacket, this, 0, 0, 1, false));
	this->reqId = 1;
	NEW_CLASS(this->mut, Sync::Mutex());
	NEW_CLASS(this->respEvt, Sync::Event(true, (const UTF8Char*)"Net.SNMPClient.respEvt"));
	this->hasResp = true;
	this->respStatus = Net::SNMPUtil::ES_NOERROR;
	this->respList = 0;
}

Net::SNMPClient::~SNMPClient()
{
	DEL_CLASS(this->svr);
	DEL_CLASS(this->mut);
	DEL_CLASS(this->respEvt);
	DEL_CLASS(this->scanMut);
}

Bool Net::SNMPClient::IsError()
{
	return this->svr->IsError();
}

Net::SNMPUtil::ErrorStatus Net::SNMPClient::V1GetRequest(const Net::SocketUtil::AddressInfo *agentAddr, const UTF8Char *community, const UTF8Char *oid, Data::ArrayList<Net::SNMPUtil::BindingItem *> *itemList)
{
	UInt8 pduBuff[64];
	UOSInt oidLen;
	oidLen = Net::SNMPUtil::OIDText2PDU(oid, pduBuff);
	return V1GetRequestPDU(agentAddr, community, pduBuff, oidLen, itemList);
}

Net::SNMPUtil::ErrorStatus Net::SNMPClient::V1GetRequestPDU(const Net::SocketUtil::AddressInfo *agentAddr, const UTF8Char *community, const UInt8 *oid, UOSInt oidLen, Data::ArrayList<Net::SNMPUtil::BindingItem *> *itemList)
{
	UOSInt buffSize;
	const UInt8 *buff;
	Net::SNMPUtil::ErrorStatus ret;
	Sync::MutexUsage mutUsage(this->mut);
	Net::ASN1PDUBuilder pdu;
	pdu.SequenceBegin(0x30);
	pdu.AppendInt32(0);
	pdu.AppendString(community);
	pdu.SequenceBegin(0xA0);
	pdu.AppendInt32(this->reqId);
	pdu.AppendInt32(0);
	pdu.AppendInt32(0);
	pdu.SequenceBegin(0x30);
	pdu.SequenceBegin(0x30);
	pdu.AppendOID(oid, oidLen);
	pdu.AppendNull();
	pdu.SequenceEnd();
	pdu.SequenceEnd();
	pdu.SequenceEnd();
	pdu.SequenceEnd();
	buff = pdu.GetBuff(&buffSize);
	this->respList = itemList;
	this->respStatus = Net::SNMPUtil::ES_NORESP;
	this->hasResp = false;
	this->respEvt->Clear();
	this->svr->SendTo(agentAddr, 161, buff, buffSize);
	this->respEvt->Wait(5000);
	this->respList = 0;
	ret = this->respStatus;
	this->hasResp = true;

	this->reqId++;
	mutUsage.EndUse();
	return ret;
}

Net::SNMPUtil::ErrorStatus Net::SNMPClient::V1GetNextRequest(const Net::SocketUtil::AddressInfo *agentAddr, const UTF8Char *community, const UTF8Char *oid, Data::ArrayList<Net::SNMPUtil::BindingItem *> *itemList)
{
	UInt8 pduBuff[64];
	UOSInt oidLen;
	oidLen = Net::SNMPUtil::OIDText2PDU(oid, pduBuff);
	return V1GetNextRequestPDU(agentAddr, community, pduBuff, oidLen, itemList);
}

Net::SNMPUtil::ErrorStatus Net::SNMPClient::V1GetNextRequestPDU(const Net::SocketUtil::AddressInfo *agentAddr, const UTF8Char *community, const UInt8 *oid, UOSInt oidLen, Data::ArrayList<Net::SNMPUtil::BindingItem *> *itemList)
{
	UOSInt buffSize;
	const UInt8 *buff;
	Net::SNMPUtil::ErrorStatus ret;
	Sync::MutexUsage mutUsage(this->mut);
	Net::ASN1PDUBuilder pdu;
	pdu.SequenceBegin(0x30);
	pdu.AppendInt32(0);
	pdu.AppendString(community);
	pdu.SequenceBegin(0xA1);
	pdu.AppendInt32(this->reqId);
	pdu.AppendInt32(0);
	pdu.AppendInt32(0);
	pdu.SequenceBegin(0x30);
	pdu.SequenceBegin(0x30);
	pdu.AppendOID(oid, oidLen);
	pdu.AppendNull();
	pdu.SequenceEnd();
	pdu.SequenceEnd();
	pdu.SequenceEnd();
	pdu.SequenceEnd();
	buff = pdu.GetBuff(&buffSize);
	this->respList = itemList;
	this->respStatus = Net::SNMPUtil::ES_NORESP;
	this->hasResp = false;
	this->respEvt->Clear();
	this->svr->SendTo(agentAddr, 161, buff, buffSize);
	this->respEvt->Wait(5000);
	this->respList = 0;
	ret = this->respStatus;
	this->hasResp = true;

	this->reqId++;
	mutUsage.EndUse();
	return ret;
}

Net::SNMPUtil::ErrorStatus Net::SNMPClient::V1Walk(const Net::SocketUtil::AddressInfo *agentAddr, const UTF8Char *community, const UTF8Char *oid, Data::ArrayList<Net::SNMPUtil::BindingItem *> *itemList)
{
	Net::SNMPUtil::ErrorStatus ret;
	Data::ArrayList<Net::SNMPUtil::BindingItem *> thisList;
	Net::SNMPUtil::BindingItem *item;
	Net::SNMPUtil::BindingItem *lastItem = 0;
	ret = this->V1GetNextRequest(agentAddr, community, oid, &thisList);
	if (ret != Net::SNMPUtil::ES_NOERROR)
	{
		itemList->AddRange(&thisList);
		return ret;
	}
	while (thisList.GetCount() == 1)
	{
		item = thisList.GetItem(0);
		if (lastItem && lastItem->oidLen == item->oidLen && Net::SNMPUtil::OIDCompare(lastItem->oid, lastItem->oidLen, item->oid, item->oidLen) == 0)
		{
			break;
		}
		thisList.Clear();
		itemList->Add(item);
		lastItem = item;
		ret = this->V1GetNextRequestPDU(agentAddr, community, item->oid, item->oidLen, &thisList);
		if (ret == Net::SNMPUtil::ES_NORESP)
		{
			UOSInt i = thisList.GetCount();
			while (i-- > 0)
			{
				Net::SNMPUtil::FreeBindingItem(thisList.GetItem(i));
			}
			thisList.Clear();
			ret = this->V1GetNextRequestPDU(agentAddr, community, item->oid, item->oidLen, &thisList);
		}
		if (ret == Net::SNMPUtil::ES_NOSUCHNAME)
		{
			UOSInt i = thisList.GetCount();
			while (i-- > 0)
			{
				Net::SNMPUtil::FreeBindingItem(thisList.GetItem(i));
			}
			thisList.Clear();
			break;
		}
		else if (ret != Net::SNMPUtil::ES_NOERROR)
		{
			break;
		}
	}
	itemList->AddRange(&thisList);
	return Net::SNMPUtil::ES_NOERROR;
}

UOSInt Net::SNMPClient::V1ScanGetRequest(const Net::SocketUtil::AddressInfo *broadcastAddr, const UTF8Char *community, const UTF8Char *oid, Data::ArrayList<Net::SocketUtil::AddressInfo *> *addrList, UOSInt timeoutMS, Bool scanIP)
{
	UInt8 pduBuff[64];
	UOSInt oidLen;
	UOSInt buffSize;
	const UInt8 *buff;
	UOSInt initCnt = addrList->GetCount();
	Sync::MutexUsage mutUsage(this->mut);
	Net::ASN1PDUBuilder pdu;
	pdu.SequenceBegin(0x30);
	pdu.AppendInt32(0);
	pdu.AppendString(community);
	pdu.SequenceBegin(0xA0);
	pdu.AppendInt32(this->reqId);
	pdu.AppendInt32(0);
	pdu.AppendInt32(0);
	pdu.SequenceBegin(0x30);
	pdu.SequenceBegin(0x30);
	oidLen = Net::SNMPUtil::OIDText2PDU(oid, pduBuff);
	pdu.AppendOID(pduBuff, oidLen);
	pdu.AppendNull();
	pdu.SequenceEnd();
	pdu.SequenceEnd();
	pdu.SequenceEnd();
	pdu.SequenceEnd();
	buff = pdu.GetBuff(&buffSize);
	Sync::MutexUsage scanMutUsage(this->scanMut);
	this->scanList = addrList;
	scanMutUsage.EndUse();
	if (scanIP && broadcastAddr->addrType == Net::SocketUtil::AT_IPV4)
	{
		Net::SocketUtil::AddressInfo scanAddr;
		MemCopyNO(&scanAddr, broadcastAddr, sizeof(scanAddr));
		scanAddr.addr[3] = 1;
		while (scanAddr.addr[3] < 255)
		{
			this->svr->SendTo(&scanAddr, 161, buff, buffSize);
			scanAddr.addr[3]++;
		}
	}
	else
	{
		this->svr->SetBroadcast(true);
		this->svr->SendTo(broadcastAddr, 161, buff, buffSize);
	}
	Sync::Thread::Sleep(timeoutMS);
	scanMutUsage.BeginUse();
	this->scanList = 0;
	scanMutUsage.EndUse();
	mutUsage.EndUse();
	return addrList->GetCount() - initCnt;

}
