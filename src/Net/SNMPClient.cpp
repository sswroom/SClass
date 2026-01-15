#include "Stdafx.h"
#include "MyMemory.h"
#include "Net/ASN1PDUBuilder.h"
#include "Net/ASN1Util.h"
#include "Net/SNMPClient.h"
#include "Net/SNMPInfo.h"
#include "Sync/MutexUsage.h"
#include "Sync/ThreadUtil.h"
#include "Text/StringBuilderUTF8.h"

void __stdcall Net::SNMPClient::OnSNMPPacket(NN<const Net::SocketUtil::AddressInfo> addr, UInt16 port, Data::ByteArrayR data, AnyType userData)
{
	NN<Net::SNMPClient> me = userData.GetNN<Net::SNMPClient>();
	Data::ArrayListNN<Net::SNMPUtil::BindingItem> itemList;
	Int32 reqId;
	Net::SNMPUtil::ErrorStatus err;
	NN<Data::ArrayListNN<Net::SocketUtil::AddressInfo>> scanList;
	NN<Data::ArrayListNN<Net::SNMPUtil::BindingItem>> respList;
	Sync::MutexUsage mutUsage(me->scanMut);
	if (me->scanList.SetTo(scanList))
	{
		if (port == 161)
		{
			NN<Net::SocketUtil::AddressInfo> remoteAddr = MemAllocNN(Net::SocketUtil::AddressInfo);
			remoteAddr.CopyFrom(addr);
			scanList->Add(remoteAddr);
		}
		mutUsage.EndUse();
		return;
	}
	mutUsage.EndUse();
	err = Net::SNMPUtil::PDUParseMessage(data, reqId, itemList);
	if (!me->hasResp && (err != Net::SNMPUtil::ES_NOERROR || reqId == me->reqId))
	{
		if (me->respList.SetTo(respList))
		{
			respList->AddAll(itemList);
		}
		else
		{
			itemList.FreeAll(Net::SNMPUtil::FreeBindingItem);
		}
		me->respStatus = err;
		me->hasResp = true;
		me->respEvt.Set();
	}
	else
	{
		itemList.FreeAll(Net::SNMPUtil::FreeBindingItem);
	}
}

Net::SNMPClient::SNMPClient(NN<Net::SocketFactory> sockf, NN<IO::LogTool> log)
{
	this->scanList = nullptr;
	NEW_CLASS(this->svr, Net::UDPServer(sockf, nullptr, 0, nullptr, OnSNMPPacket, this, log, nullptr, 1, false));
	this->reqId = 1;
	this->hasResp = true;
	this->respStatus = Net::SNMPUtil::ES_NOERROR;
	this->respList = nullptr;
}

Net::SNMPClient::~SNMPClient()
{
	DEL_CLASS(this->svr);
}

Bool Net::SNMPClient::IsError()
{
	return this->svr->IsError();
}

Net::SNMPUtil::ErrorStatus Net::SNMPClient::V1GetRequest(NN<const Net::SocketUtil::AddressInfo> agentAddr, NN<Text::String> community, Text::CStringNN oidText, NN<Data::ArrayListNN<Net::SNMPUtil::BindingItem>> itemList)
{
	UInt8 pduBuff[64];
	UOSInt oidLen;
	oidLen = Net::ASN1Util::OIDText2PDU(oidText, pduBuff);
	return V1GetRequestPDU(agentAddr, community, pduBuff, oidLen, itemList);
}

Net::SNMPUtil::ErrorStatus Net::SNMPClient::V1GetRequestPDU(NN<const Net::SocketUtil::AddressInfo> agentAddr, NN<Text::String> community, const UInt8 *oid, UOSInt oidLen, NN<Data::ArrayListNN<Net::SNMPUtil::BindingItem>> itemList)
{
	UOSInt buffSize;
	UnsafeArray<const UInt8> buff;
	Net::SNMPUtil::ErrorStatus ret;
	Sync::MutexUsage mutUsage(this->mut);
	Net::ASN1PDUBuilder pdu;
	pdu.BeginSequence();
	pdu.AppendInt32(0);
	pdu.AppendOctetString(community);
	pdu.BeginOther(0xA0);
	pdu.AppendInt32(this->reqId);
	pdu.AppendInt32(0);
	pdu.AppendInt32(0);
	pdu.BeginSequence();
	pdu.BeginSequence();
	pdu.AppendOID(oid, oidLen);
	pdu.AppendNull();
	pdu.EndAll();
	buff = pdu.GetBuff(buffSize);
	this->respList = itemList;
	this->respStatus = Net::SNMPUtil::ES_NORESP;
	this->hasResp = false;
	this->respEvt.Clear();
	this->svr->SendTo(agentAddr, 161, buff, buffSize);
	this->respEvt.Wait(5000);
	this->respList = nullptr;
	ret = this->respStatus;
	this->hasResp = true;

	this->reqId++;
	mutUsage.EndUse();
	return ret;
}

Net::SNMPUtil::ErrorStatus Net::SNMPClient::V1GetNextRequest(NN<const Net::SocketUtil::AddressInfo> agentAddr, NN<Text::String> community, Text::CStringNN oidText, NN<Data::ArrayListNN<Net::SNMPUtil::BindingItem>> itemList)
{
	UInt8 pduBuff[64];
	UOSInt oidLen;
	oidLen = Net::ASN1Util::OIDText2PDU(oidText, pduBuff);
	return V1GetNextRequestPDU(agentAddr, community, pduBuff, oidLen, itemList);
}

Net::SNMPUtil::ErrorStatus Net::SNMPClient::V1GetNextRequestPDU(NN<const Net::SocketUtil::AddressInfo> agentAddr, NN<Text::String> community, const UInt8 *oid, UOSInt oidLen, NN<Data::ArrayListNN<Net::SNMPUtil::BindingItem>> itemList)
{
	UOSInt buffSize;
	UnsafeArray<const UInt8> buff;
	Net::SNMPUtil::ErrorStatus ret;
	Sync::MutexUsage mutUsage(this->mut);
	Net::ASN1PDUBuilder pdu;
	pdu.BeginSequence();
	pdu.AppendInt32(0);
	pdu.AppendOctetString(community);
	pdu.BeginOther(0xA1);
	pdu.AppendInt32(this->reqId);
	pdu.AppendInt32(0);
	pdu.AppendInt32(0);
	pdu.BeginSequence();
	pdu.BeginSequence();
	pdu.AppendOID(oid, oidLen);
	pdu.AppendNull();
	pdu.EndAll();
	buff = pdu.GetBuff(buffSize);
	this->respList = itemList;
	this->respStatus = Net::SNMPUtil::ES_NORESP;
	this->hasResp = false;
	this->respEvt.Clear();
	this->svr->SendTo(agentAddr, 161, buff, buffSize);
	this->respEvt.Wait(5000);
	this->respList = nullptr;
	ret = this->respStatus;
	this->hasResp = true;

	this->reqId++;
	mutUsage.EndUse();
	return ret;
}

Net::SNMPUtil::ErrorStatus Net::SNMPClient::V1Walk(NN<const Net::SocketUtil::AddressInfo> agentAddr, NN<Text::String> community, Text::CStringNN oidText, NN<Data::ArrayListNN<Net::SNMPUtil::BindingItem>> itemList)
{
	Net::SNMPUtil::ErrorStatus ret;
	Data::ArrayListNN<Net::SNMPUtil::BindingItem> thisList;
	NN<Net::SNMPUtil::BindingItem> item;
	Optional<Net::SNMPUtil::BindingItem> lastItem = nullptr;
	NN<Net::SNMPUtil::BindingItem> nnlastItem;
	ret = this->V1GetNextRequest(agentAddr, community, oidText, thisList);
	if (ret != Net::SNMPUtil::ES_NOERROR)
	{
		itemList->AddAll(thisList);
		return ret;
	}
	while (thisList.GetCount() == 1)
	{
		item = thisList.GetItemNoCheck(0);
		if (lastItem.SetTo(nnlastItem) && nnlastItem->oidLen == item->oidLen && Net::ASN1Util::OIDCompare(Data::ByteArrayR(nnlastItem->oid, nnlastItem->oidLen), Data::ByteArrayR(item->oid, item->oidLen)) == 0)
		{
			break;
		}
		thisList.Clear();
		itemList->Add(item);
		lastItem = item;
		ret = this->V1GetNextRequestPDU(agentAddr, community, item->oid, item->oidLen, thisList);
		if (ret == Net::SNMPUtil::ES_NORESP)
		{
			thisList.FreeAll(Net::SNMPUtil::FreeBindingItem);
			ret = this->V1GetNextRequestPDU(agentAddr, community, item->oid, item->oidLen, thisList);
		}
		if (ret == Net::SNMPUtil::ES_NOSUCHNAME)
		{
			thisList.FreeAll(Net::SNMPUtil::FreeBindingItem);
			break;
		}
		else if (ret != Net::SNMPUtil::ES_NOERROR)
		{
			break;
		}
	}
	itemList->AddAll(thisList);
	return Net::SNMPUtil::ES_NOERROR;
}

UOSInt Net::SNMPClient::V1ScanGetRequest(NN<const Net::SocketUtil::AddressInfo> broadcastAddr, NN<Text::String> community, Text::CStringNN oidText, NN<Data::ArrayListNN<Net::SocketUtil::AddressInfo>> addrList, Data::Duration timeout, Bool scanIP)
{
	UInt8 pduBuff[64];
	UOSInt oidLen;
	UOSInt buffSize;
	UnsafeArray<const UInt8> buff;
	UOSInt initCnt = addrList->GetCount();
	Sync::MutexUsage mutUsage(this->mut);
	Net::ASN1PDUBuilder pdu;
	pdu.BeginSequence();
	pdu.AppendInt32(0);
	pdu.AppendOctetString(community);
	pdu.BeginOther(0xA0);
	pdu.AppendInt32(this->reqId);
	pdu.AppendInt32(0);
	pdu.AppendInt32(0);
	pdu.BeginSequence();
	pdu.BeginSequence();
	oidLen = Net::ASN1Util::OIDText2PDU(oidText, pduBuff);
	pdu.AppendOID(pduBuff, oidLen);
	pdu.AppendNull();
	pdu.EndAll();
	buff = pdu.GetBuff(buffSize);
	Sync::MutexUsage scanMutUsage(this->scanMut);
	this->scanList = addrList;
	scanMutUsage.EndUse();
	if (scanIP && broadcastAddr->addrType == Net::AddrType::IPv4)
	{
		Net::SocketUtil::AddressInfo scanAddr;
		MemCopyNO(&scanAddr, broadcastAddr.Ptr(), sizeof(scanAddr));
		scanAddr.addr[3] = 1;
		while (scanAddr.addr[3] < 255)
		{
			this->svr->SendTo(scanAddr, 161, buff, buffSize);
			scanAddr.addr[3]++;
		}
	}
	else
	{
		this->svr->SetBroadcast(true);
		this->svr->SendTo(broadcastAddr, 161, buff, buffSize);
	}
	Sync::ThreadUtil::SleepDur(timeout);
	scanMutUsage.BeginUse();
	this->scanList = nullptr;
	scanMutUsage.EndUse();
	mutUsage.EndUse();
	return addrList->GetCount() - initCnt;

}
