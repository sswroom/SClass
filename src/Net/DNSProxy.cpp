#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "Manage/HiResClock.h"
#include "Net/DNSProxy.h"
#include "Sync/MutexUsage.h"
#include "Text/MyString.h"
#include "Text/StringBuilder.h"

#define REQTIMEOUT 10000

void __stdcall Net::DNSProxy::ClientPacket(const Net::SocketUtil::AddressInfo *addr, UInt16 port, const UInt8 *buff, UOSInt dataSize, void *userData)
{
	Net::DNSProxy *me = (Net::DNSProxy*)userData;
	CliRequestStatus *req;
	Sync::MutexUsage mutUsage(me->cliReqMut);
	req = me->cliReqMap->Get(ReadMUInt16(buff));
	if (req)
	{
		MemCopyNO(req->respBuff, buff, dataSize);
		req->respSize = dataSize;
		req->finEvt->Set();
	}
	mutUsage.EndUse();
}

void __stdcall Net::DNSProxy::OnDNSRequest(void *userObj, const UTF8Char *reqName, Int32 reqType, Int32 reqClass, const Net::SocketUtil::AddressInfo *reqAddr, UInt16 reqPort, Int32 reqId)
{
	Net::DNSProxy *me = (Net::DNSProxy*)userObj;
	RequestResult *req;
	UTF8Char sbuff[256];
	UInt8 buff[512];
	OSInt buffSize;
	OSInt i;
	Manage::HiResClock clk;
	Data::DateTime currTime;
	currTime.SetCurrTimeUTC();

	if (reqType == 1)
	{
		Sync::MutexUsage reqv4MutUsage(me->reqv4Mut);
		req = me->reqv4Map->Get(reqName);

		if (req)
		{
			reqv4MutUsage.EndUse();
			if (req->status == 2)
			{
				buffSize = BuildEmptyReply(buff, reqId, reqName, reqType, reqClass, false);
				me->svr->ReplyRequest(reqAddr, reqPort, buff, buffSize);
			}
			else if (req->status == 1)
			{
				buffSize = BuildEmptyReply(buff, reqId, reqName, reqType, reqClass, true);
				me->svr->ReplyRequest(reqAddr, reqPort, buff, buffSize);
			}
			else
			{
				Sync::MutexUsage mutUsage(req->mut);
				if (currTime.ToTicks() - req->reqTime > req->ttl * 1000)
				{
					me->RequestDNS(reqName, reqType, reqClass, req);
					req->reqTime = currTime.ToTicks();
					buffSize = req->recSize;
					MemCopyNO(buff, req->recBuff, req->recSize);
					req->ttl = Net::DNSClient::GetResponseTTL(req->recBuff, req->recSize);
					if (req->ttl == 0)
						req->ttl = REQTIMEOUT / 1000;
					mutUsage.EndUse();
					WriteMInt16(buff, reqId);
					me->svr->ReplyRequest(reqAddr, reqPort, buff, buffSize);
				}
				else
				{
					buffSize = req->recSize;
					MemCopyNO(buff, req->recBuff, req->recSize);
					mutUsage.EndUse();
					WriteMInt16(buff, reqId);
					me->svr->ReplyRequest(reqAddr, reqPort, buff, buffSize);
				}
			}
		}
		else
		{
			req = MemAlloc(RequestResult, 1);
			NEW_CLASS(req->mut, Sync::Mutex());
			req->status = 0;
			req->recSize = 0;
			req->reqTime = currTime.ToTicks() - REQTIMEOUT - REQTIMEOUT;
			Sync::MutexUsage mutUsage(req->mut);
			me->reqv4Map->Put(reqName, req);
			me->reqv4Updated = true;
			reqv4MutUsage.EndUse();

			sbuff[0] = '.';
			Sync::MutexUsage blackListMutUsage(me->blackListMut);
			i = me->blackList->GetCount();
			while (i-- > 0)
			{
				Text::StrConcatS(&sbuff[1], me->blackList->GetItem(i), 254);
				if (Text::StrEndsWith(reqName, sbuff))
				{
					req->status = 1;
					break;
				}
			}
			blackListMutUsage.EndUse();

			if (req->status == 1)
			{
				req->reqTime = currTime.ToTicks();
				req->ttl = REQTIMEOUT / 1000;
				mutUsage.EndUse();
				buffSize = BuildEmptyReply(buff, reqId, reqName, reqType, reqClass, true);
			}
			else
			{
				me->RequestDNS(reqName, reqType, reqClass, req);
				req->reqTime = currTime.ToTicks();
				buffSize = req->recSize;
				MemCopyNO(buff, req->recBuff, req->recSize);
				req->ttl = Net::DNSClient::GetResponseTTL(req->recBuff, req->recSize);
				if (req->ttl == 0)
					req->ttl = REQTIMEOUT / 1000;
				mutUsage.EndUse();
				WriteMInt16(buff, reqId);
			}
			me->svr->ReplyRequest(reqAddr, reqPort, buff, buffSize);

			if (req->status == 0 && me->targetMap)
			{
				Data::ArrayList<Net::DNSClient::RequestAnswer*> ansList;
				UInt32 resIP;
				UInt32 sortIP;
				OSInt j;
				TargetInfo *target;
				mutUsage.BeginUse();
				Net::DNSClient::ParseAnswers(req->recBuff, req->recSize, &ansList);
				mutUsage.EndUse();
				Net::DNSClient::RequestAnswer *ans;
				
				i = ansList.GetCount();
				while (i-- > 0)
				{
					ans = ansList.GetItem(i);
					if (ans->recType == 1)
					{
						resIP = ReadNInt32(ans->addr.addr);
						sortIP = ReadMInt32(ans->addr.addr);
						Sync::MutexUsage targetMutUsage(me->targetMut);
						target = me->targetMap->Get(sortIP);
						if (target == 0)
						{
							target = MemAlloc(TargetInfo, 1);
							target->ip = resIP;
							NEW_CLASS(target->mut, Sync::Mutex());
							NEW_CLASS(target->addrList, Data::ArrayListICaseStrUTF8());
							me->targetMap->Put(sortIP, target);
							me->targetUpdated = true;
						}
						Sync::MutexUsage mutUsage(target->mut);
						targetMutUsage.EndUse();
						if (target->addrList->SortedIndexOf(ans->name) < 0)
						{
							target->addrList->SortedInsert(Text::StrCopyNew(ans->name));
						}
						j = i;
						while (j-- > 0)
						{
							ans = ansList.GetItem(j);
							if (ans->recType == 5)
							{
								if (target->addrList->SortedIndexOf(ans->name) < 0)
								{
									target->addrList->SortedInsert(Text::StrCopyNew(ans->name));
								}
							}
						}
						mutUsage.EndUse();
					}
				}
				Net::DNSClient::FreeAnswers(&ansList);
			}
		}
	}
	else if (reqType == 28)
	{
		Sync::MutexUsage reqv6MutUsage(me->reqv6Mut);
		req = me->reqv6Map->Get(reqName);

		if (req)
		{
			reqv6MutUsage.EndUse();
			if (req->status == 2)
			{
				buffSize = BuildEmptyReply(buff, reqId, reqName, reqType, reqClass, false);
				me->svr->ReplyRequest(reqAddr, reqPort, buff, buffSize);
			}
			else if (req->status == 1)
			{
				buffSize = BuildEmptyReply(buff, reqId, reqName, reqType, reqClass, true);
				me->svr->ReplyRequest(reqAddr, reqPort, buff, buffSize);
			}
			else
			{
				Sync::MutexUsage mutUsage(req->mut);
				if (currTime.ToTicks() - req->reqTime > req->ttl * 1000)
				{
					me->RequestDNS(reqName, reqType, reqClass, req);
					req->reqTime = currTime.ToTicks();
					buffSize = req->recSize;
					MemCopyNO(buff, req->recBuff, req->recSize);
					req->ttl = Net::DNSClient::GetResponseTTL(req->recBuff, req->recSize);
					if (req->ttl == 0)
						req->ttl = REQTIMEOUT / 1000;
					mutUsage.EndUse();
					WriteMInt16(buff, reqId);
					me->svr->ReplyRequest(reqAddr, reqPort, buff, buffSize);
				}
				else
				{
					buffSize = req->recSize;
					MemCopyNO(buff, req->recBuff, req->recSize);
					mutUsage.EndUse();
					WriteMInt16(buff, reqId);
					me->svr->ReplyRequest(reqAddr, reqPort, buff, buffSize);
				}
			}
		}
		else
		{
			req = MemAlloc(RequestResult, 1);
			NEW_CLASS(req->mut, Sync::Mutex());
			req->status = 0;
			req->recSize = 0;
			req->reqTime = currTime.ToTicks() - REQTIMEOUT - REQTIMEOUT;
			Sync::MutexUsage mutUsage(req->mut);
			me->reqv6Map->Put(reqName, req);
			me->reqv6Updated = true;
			reqv6MutUsage.EndUse();

			if (me->disableV6)
			{
				req->status = 2;
				req->reqTime = currTime.ToTicks();
				req->ttl = REQTIMEOUT / 1000;
				mutUsage.EndUse();
				buffSize = BuildEmptyReply(buff, reqId, reqName, reqType, reqClass, false);
			}
			else
			{
				me->RequestDNS(reqName, reqType, reqClass, req);
				req->reqTime = currTime.ToTicks();
				buffSize = req->recSize;
				MemCopyNO(buff, req->recBuff, req->recSize);
				req->ttl = Net::DNSClient::GetResponseTTL(req->recBuff, req->recSize);
				if (req->ttl == 0)
					req->ttl = REQTIMEOUT / 1000;
				mutUsage.EndUse();
				WriteMInt16(buff, reqId);
			}
			me->svr->ReplyRequest(reqAddr, reqPort, buff, buffSize);
		}
	}
	else
	{
		Sync::MutexUsage reqothMutUsage(me->reqothMut);
		req = me->reqothMap->Get(reqName);

		if (req)
		{
			reqothMutUsage.EndUse();
			Sync::MutexUsage mutUsage(req->mut);
			if (currTime.ToTicks() - req->reqTime > req->ttl * 1000)
			{
				me->RequestDNS(reqName, reqType, reqClass, req);
				req->reqTime = currTime.ToTicks();
				buffSize = req->recSize;
				MemCopyNO(buff, req->recBuff, req->recSize);
				req->ttl = Net::DNSClient::GetResponseTTL(req->recBuff, req->recSize);
				if (req->ttl == 0)
					req->ttl = REQTIMEOUT / 1000;
				mutUsage.EndUse();
				WriteMInt16(buff, reqId);
				me->svr->ReplyRequest(reqAddr, reqPort, buff, buffSize);
			}
			else
			{
				buffSize = req->recSize;
				MemCopyNO(buff, req->recBuff, req->recSize);
				mutUsage.EndUse();
				WriteMInt16(buff, reqId);
				me->svr->ReplyRequest(reqAddr, reqPort, buff, buffSize);
			}
		}
		else
		{
			req = MemAlloc(RequestResult, 1);
			NEW_CLASS(req->mut, Sync::Mutex());
			req->status = 0;
			req->recSize = 0;
			req->reqTime = currTime.ToTicks() - REQTIMEOUT - REQTIMEOUT;
			Sync::MutexUsage mutUsage(req->mut);
			me->reqothMap->Put(reqName, req);
			me->reqothUpdated = true;
			reqothMutUsage.EndUse();

			me->RequestDNS(reqName, reqType, reqClass, req);
			req->reqTime = currTime.ToTicks();
			buffSize = req->recSize;
			MemCopyNO(buff, req->recBuff, req->recSize);
			req->ttl = Net::DNSClient::GetResponseTTL(req->recBuff, req->recSize);
			if (req->ttl == 0)
				req->ttl = REQTIMEOUT / 1000;
			mutUsage.EndUse();
			WriteMInt16(buff, reqId);
			me->svr->ReplyRequest(reqAddr, reqPort, buff, buffSize);
		}
	}

	Double t = clk.GetTimeDiff();
	Sync::MutexUsage hdlrMutUsage(me->hdlrMut);
	i = me->hdlrList->GetCount();
	while (i-- > 0)
	{
		me->hdlrList->GetItem(i)(me->hdlrObjs->GetItem(i), reqName, reqType, reqClass, reqAddr, reqPort, reqId, t);
	}
	hdlrMutUsage.EndUse();
}

void Net::DNSProxy::RequestDNS(const UTF8Char *reqName, Int32 reqType, Int32 reqClass, RequestResult *req)
{
	UInt8 buff[512];
	UInt8 *ptr1;
	UInt8 *ptr2;
	const UTF8Char *cptr1;
	const UTF8Char *cptr2;
	UTF8Char c;
	Int32 currId = this->NextId();

	WriteMInt16(&buff[0], currId);
	WriteMInt16(&buff[2], 0x100); //flags
	WriteMInt16(&buff[4], 1); //reqCount
	WriteMInt16(&buff[6], 0); //respCount;
	WriteMInt16(&buff[8], 0); //authorCount
	WriteMInt16(&buff[10], 0); //extraCount
	ptr1 = &buff[12];
	cptr1 = reqName;
	while (true)
	{
		cptr2 = cptr1;
		while ((c = *cptr2) != 0)
		{
			if (c == '.')
				break;
			cptr2++;
		}
		*ptr1++ = (UInt8)(cptr2 - cptr1);
		while (cptr1 < cptr2)
		{
			*ptr1++ = (UInt8)*cptr1++;
		}
		if (*cptr1 == 0)
		{
			*ptr1++ = 0;
			break;
		}
		else
		{
			cptr1++;
		}
	}
	WriteMInt16(&ptr1[0], reqType);
	WriteMInt16(&ptr1[2], reqClass);
	ptr1 += 4;
	ptr2 = buff;
	
	CliRequestStatus *cliReq = this->NewCliReq(currId);
	Net::SocketUtil::AddressInfo addr;
	Net::SocketUtil::SetAddrInfoV4(&addr, this->currServerIP);
	cliReq->finEvt->Clear();
	this->cli->SendTo(&addr, 53, buff, ptr1 - ptr2);
	cliReq->finEvt->Wait(2000);
	if (cliReq->respSize > 12)
	{
		MemCopyNO(req->recBuff, cliReq->respBuff, cliReq->respSize);
		req->recSize = cliReq->respSize;
	}
	else
	{
		if (req->recSize == 0)
		{
			MemCopyNO(req->recBuff, buff, ptr1 - ptr2);
			req->recSize = ptr1 - ptr2;
			WriteMInt16(&req->recBuff[2], 0x8581);
		}
		Data::DateTime currTime;
		currTime.SetCurrTimeUTC();
		Sync::MutexUsage mutUsage(this->dnsMut);
		if (currTime.DiffMS(this->currIPTime) > REQTIMEOUT)
		{
			this->currServerIndex = (this->currServerIndex + 1) % this->dnsList->GetCount();
			this->currServerIP = this->dnsList->GetItem(this->currServerIndex);
			this->currIPTime->SetValue(&currTime);
			req->ttl = 1;
		}
		mutUsage.EndUse();
	}
	this->DelCliReq(currId);
}

Int32 Net::DNSProxy::NextId()
{
	Int32 id;
	Sync::MutexUsage mutUsage(this->lastIdMut);
	this->lastId++;
	if (this->lastId >= 65536)
		this->lastId = 1;
	id = this->lastId;
	mutUsage.EndUse();
	return id;
}

Net::DNSProxy::CliRequestStatus *Net::DNSProxy::NewCliReq(Int32 id)
{
	CliRequestStatus *req = MemAlloc(CliRequestStatus, 1);
	req->respSize = 0;
	NEW_CLASS(req->finEvt, Sync::Event(true, (const UTF8Char*)"MainForm.CliRequestStatus.finEvt"));
	Sync::MutexUsage mutUsage(this->cliReqMut);
	this->cliReqMap->Put(id, req);
	mutUsage.EndUse();
	return req;
}

void Net::DNSProxy::DelCliReq(Int32 id)
{
	CliRequestStatus *req;
	Sync::MutexUsage mutUsage(this->cliReqMut);
	req = this->cliReqMap->Remove(id);
	mutUsage.EndUse();
	if (req)
	{
		DEL_CLASS(req->finEvt);
		MemFree(req);
	}
}

OSInt Net::DNSProxy::BuildEmptyReply(UInt8 *buff, Int32 id, const UTF8Char *reqName, Int32 reqType, Int32 reqClass, Bool blockResult)
{
	// RFC1035
	OSInt i;
	OSInt j;
	UTF8Char c;
	WriteMInt16(&buff[0], id); // Header
	buff[2] = 0x81; //Question
	buff[3] = 0x80;
	WriteMInt16(&buff[4], 1); //QDCOUNT
	WriteMInt16(&buff[6], blockResult?1:0); //ANCOUNT
	WriteMInt16(&buff[8], 0); //NSCOUNT
	WriteMInt16(&buff[10], 0); //ARCOUNT
	i = 12;
	j = 0;
	while (true)
	{
		c = *reqName++;
		if (c == 0 || c == '.')
		{
			buff[i] = (UInt8)j;
			i += j + 1;
			if (c == 0)
			{
				buff[i] = 0;
				i++;
				break;
			}
			j = 0;
		}
		else
		{
			j++;
			buff[i + j] = (UInt8)c;
		}
	}
	WriteMInt16(&buff[i], reqType);
	WriteMInt16(&buff[i + 2], reqClass);
	i += 4;

	if (blockResult)
	{
		//ANSWER
		if (reqType == 28)
		{
			buff[i] = 0xc0;
			buff[i + 1] = 0x0c;
			WriteMInt16(&buff[i + 2], 28); //TYPE=A
			WriteMInt16(&buff[i + 4], 1); //CLASS=IN
			WriteMInt32(&buff[i + 6], 10000); //TTL=10000
			WriteMInt16(&buff[i + 10], 16); //RDLENGTH=16
			buff[i + 12] = 0;
			buff[i + 13] = 0;
			buff[i + 14] = 0;
			buff[i + 15] = 0;
			buff[i + 16] = 0;
			buff[i + 17] = 0;
			buff[i + 18] = 0;
			buff[i + 19] = 0;
			buff[i + 20] = 0;
			buff[i + 21] = 0;
			buff[i + 22] = 0;
			buff[i + 23] = 0;
			buff[i + 24] = 0;
			buff[i + 25] = 0;
			buff[i + 26] = 0;
			buff[i + 27] = 1;
			i += 28;
		}
		else
		{
			buff[i] = 0xc0;
			buff[i + 1] = 0x0c;
			WriteMInt16(&buff[i + 2], 1); //TYPE=A
			WriteMInt16(&buff[i + 4], 1); //CLASS=IN
			WriteMInt32(&buff[i + 6], 10000); //TTL=10000
			WriteMInt16(&buff[i + 10], 4); //RDLENGTH=4
			buff[i + 12] = 127;
			buff[i + 13] = 0;
			buff[i + 14] = 0;
			buff[i + 15] = 1;
			i += 16;
		}
	}

/*	buff[i] = 0xc0;
	buff[i + 1] = 0x0c;
	WriteMInt16(&buff[i + 2], 6);
	WriteMInt16(&buff[i + 4], 1);
	WriteMInt32(&buff[i + 6], 10000);
	WriteMInt16(&buff[i + 10], 9);
	buff[i + 12] = 7;
	Text::StrConcat((Char*)&buff[i + 13], "sswroom");
	i += 21;*/

	return i;
}

Net::DNSProxy::DNSProxy(Net::SocketFactory *sockf, Bool analyzeTarget)
{
	this->sockf = sockf;
	this->disableV6 = false;
	NEW_CLASS(this->reqv4Mut, Sync::Mutex());
	NEW_CLASS(this->reqv4Map, Data::ICaseStringUTF8Map<RequestResult*>());
	this->reqv4Updated = true;
	NEW_CLASS(this->reqv6Mut, Sync::Mutex());
	NEW_CLASS(this->reqv6Map, Data::ICaseStringUTF8Map<RequestResult*>());
	this->reqv6Updated = true;
	NEW_CLASS(this->reqothMut, Sync::Mutex());
	NEW_CLASS(this->reqothMap, Data::ICaseStringUTF8Map<RequestResult*>());
	this->reqothUpdated = true;
	if (analyzeTarget)
	{
		NEW_CLASS(this->targetMut, Sync::Mutex());
		NEW_CLASS(this->targetMap, Data::Integer32Map<TargetInfo*>());
		this->targetUpdated = true;
	}
	else
	{
		this->targetMap = 0;
		this->targetMut = 0;
		this->targetUpdated = false;
	}
	NEW_CLASS(this->lastIdMut, Sync::Mutex());
	NEW_CLASS(this->dnsList, Data::ArrayList<UInt32>());
	NEW_CLASS(this->dnsMut, Sync::Mutex());
	NEW_CLASS(this->currIPTime, Data::DateTime());
	this->lastId = 0;
	NEW_CLASS(this->cliReqMut, Sync::Mutex());
	NEW_CLASS(this->cliReqMap, Data::Integer32Map<CliRequestStatus*>());
	NEW_CLASS(this->blackList, Data::ArrayList<const UTF8Char *>());
	NEW_CLASS(this->blackListMut, Sync::Mutex());
	sockf->GetDNSList(this->dnsList);
	this->currServerIP = this->dnsList->GetItem(0);
	this->currServerIndex = 0;
	this->currIPTime->SetCurrTimeUTC();
	NEW_CLASS(this->hdlrMut, Sync::Mutex());
	NEW_CLASS(this->hdlrList, Data::ArrayList<DNSProxyRequest>());
	NEW_CLASS(this->hdlrObjs, Data::ArrayList<void*>());

	NEW_CLASS(this->cli, Net::UDPServer(this->sockf, 0, 0, 0, ClientPacket, this, 0, 0, 2, false));
	NEW_CLASS(this->svr, Net::DNSServer(this->sockf));
	this->svr->HandleRequest(OnDNSRequest, this);
}

Net::DNSProxy::~DNSProxy()
{
	DEL_CLASS(this->svr);
	DEL_CLASS(this->cli);
	DEL_CLASS(this->reqv4Mut);
	DEL_CLASS(this->reqv6Mut);
	DEL_CLASS(this->reqothMut);

	Data::ArrayList<RequestResult*> *reqList;
	RequestResult *req;
	OSInt i;
	reqList = this->reqv4Map->GetValues();
	i = reqList->GetCount();
	while (i-- > 0)
	{
		req = reqList->GetItem(i);
		DEL_CLASS(req->mut);
		MemFree(req);
	}

	reqList = this->reqv6Map->GetValues();
	i = reqList->GetCount();
	while (i-- > 0)
	{
		req = reqList->GetItem(i);
		DEL_CLASS(req->mut);
		MemFree(req);
	}

	reqList = this->reqothMap->GetValues();
	i = reqList->GetCount();
	while (i-- > 0)
	{
		req = reqList->GetItem(i);
		DEL_CLASS(req->mut);
		MemFree(req);
	}

	i = this->blackList->GetCount();
	while (i-- > 0)
	{
		Text::StrDelNew(this->blackList->GetItem(i));
	}
	
	if (this->targetMap)
	{
		TargetInfo *target;
		OSInt j;
		Data::ArrayList<TargetInfo*> *targetList = this->targetMap->GetValues();
		i = targetList->GetCount();
		while (i-- > 0)
		{
			target = targetList->GetItem(i);
			DEL_CLASS(target->mut);
			j = target->addrList->GetCount();
			while (j-- > 0)
			{
				Text::StrDelNew(target->addrList->GetItem(j));
			}
			MemFree(target);
		}
		DEL_CLASS(this->targetMut);
		DEL_CLASS(this->targetMap);
	}
	DEL_CLASS(this->hdlrList);
	DEL_CLASS(this->hdlrMut);
	DEL_CLASS(this->hdlrObjs);
	DEL_CLASS(this->reqv4Map);
	DEL_CLASS(this->reqv6Map);
	DEL_CLASS(this->reqothMap);
	DEL_CLASS(this->lastIdMut);
	DEL_CLASS(this->cliReqMap);
	DEL_CLASS(this->cliReqMut);
	DEL_CLASS(this->blackList);
	DEL_CLASS(this->blackListMut);
	DEL_CLASS(this->currIPTime);
	DEL_CLASS(this->dnsList);
	DEL_CLASS(this->dnsMut);
}

Bool Net::DNSProxy::IsError()
{
	return this->svr->IsError() || this->cli->IsError() || (this->currServerIP == 0);
}

Bool Net::DNSProxy::IsReqListv4Chg()
{
	if (this->reqv4Updated)
	{
		this->reqv4Updated = false;
		return true;
	}
	return false;
}

Bool Net::DNSProxy::IsReqListv6Chg()
{
	if (this->reqv6Updated)
	{
		this->reqv6Updated = false;
		return true;
	}
	return false;
}

Bool Net::DNSProxy::IsReqListOthChg()
{
	if (this->reqothUpdated)
	{
		this->reqothUpdated = false;
		return true;
	}
	return false;
}

Bool Net::DNSProxy::IsTargetChg()
{
	if (this->targetUpdated)
	{
		this->targetUpdated = false;
		return true;
	}
	return false;
}

OSInt Net::DNSProxy::GetReqv4List(Data::ArrayList<const UTF8Char *> *reqList)
{
	Sync::MutexUsage mutUsage(this->reqv4Mut);
	reqList->AddRange(this->reqv4Map->GetKeys());
	mutUsage.EndUse();
	return reqList->GetCount();
}

OSInt Net::DNSProxy::GetReqv6List(Data::ArrayList<const UTF8Char *> *reqList)
{
	Sync::MutexUsage mutUsage(this->reqv6Mut);
	reqList->AddRange(this->reqv6Map->GetKeys());
	mutUsage.EndUse();
	return reqList->GetCount();
}

OSInt Net::DNSProxy::GetReqOthList(Data::ArrayList<const UTF8Char *> *reqList)
{
	Sync::MutexUsage mutUsage(this->reqothMut);
	reqList->AddRange(this->reqothMap->GetKeys());
	mutUsage.EndUse();
	return reqList->GetCount();
}

OSInt Net::DNSProxy::GetTargetList(Data::ArrayList<TargetInfo*> *targetList)
{
	if (this->targetMap == 0)
		return 0;
	Sync::MutexUsage mutUsage(this->targetMut);
	targetList->AddRange(this->targetMap->GetValues());
	mutUsage.EndUse();
	return targetList->GetCount();
}

OSInt Net::DNSProxy::SearchIPv4(Data::ArrayList<const UTF8Char *> *reqList, Int32 ip, Int32 mask)
{
	Data::ArrayList<const UTF8Char *> *keys;
	Data::ArrayList<RequestResult*> *results;
	Data::ArrayList<Net::DNSClient::RequestAnswer*> ansList;
	Net::DNSClient::RequestAnswer *ans;
	RequestResult *result;
	Bool valid;
	OSInt i;
	OSInt j;
	OSInt k;
	OSInt retCnt = 0;
	Sync::MutexUsage mutUsage(this->reqv4Mut);
	keys = this->reqv4Map->GetKeys();
	results = this->reqv4Map->GetValues();
	i = 0;
	j = results->GetCount();
	while (i < j)
	{
		result = results->GetItem(i);
		valid = false;
		Sync::MutexUsage mutUsage(result->mut);
		ansList.Clear();
		Net::DNSClient::ParseAnswers(result->recBuff, result->recSize, &ansList);
		mutUsage.EndUse();
		k = ansList.GetCount();
		while (k-- > 0)
		{
			ans = ansList.GetItem(k);
			if (ans->recType == 1)
			{
				if ((ip & mask) == ((*(Int32*)ans->addr.addr) & mask))
				{
					valid = true;
					break;
				}
			}
		}
		Net::DNSClient::FreeAnswers(&ansList);

		if (valid)
		{
			reqList->Add(keys->GetItem(i));
			retCnt++;
		}
		i++;
	}
	mutUsage.EndUse();
	return retCnt;
}

Bool Net::DNSProxy::GetRequestInfov4(const UTF8Char *req, Data::ArrayList<Net::DNSClient::RequestAnswer*> *ansList, Data::DateTime *reqTime, Int32 *ttl)
{
	RequestResult *result;
	Sync::MutexUsage mutUsage(this->reqv4Mut);
	result = this->reqv4Map->Get(req);
	mutUsage.EndUse();
	if (result)
	{
		Sync::MutexUsage mutUsage(result->mut);
		Net::DNSClient::ParseAnswers(result->recBuff, result->recSize, ansList);
		reqTime->SetTicks(result->reqTime);
		*ttl = result->ttl;
		mutUsage.EndUse();
		return true;
	}
	else
	{
		return false;
	}
}

Bool Net::DNSProxy::GetRequestInfov6(const UTF8Char *req, Data::ArrayList<Net::DNSClient::RequestAnswer*> *ansList, Data::DateTime *reqTime, Int32 *ttl)
{
	RequestResult *result;
	Sync::MutexUsage mutUsage(this->reqv6Mut);
	result = this->reqv6Map->Get(req);
	mutUsage.EndUse();
	if (result)
	{
		Sync::MutexUsage mutUsage(result->mut);
		Net::DNSClient::ParseAnswers(result->recBuff, result->recSize, ansList);
		reqTime->SetTicks(result->reqTime);
		*ttl = result->ttl;
		mutUsage.EndUse();
		return true;
	}
	else
	{
		return false;
	}
}

Bool Net::DNSProxy::GetRequestInfoOth(const UTF8Char *req, Data::ArrayList<Net::DNSClient::RequestAnswer*> *ansList, Data::DateTime *reqTime, Int32 *ttl)
{
	RequestResult *result;
	Sync::MutexUsage mutUsage(this->reqothMut);
	result = this->reqothMap->Get(req);
	mutUsage.EndUse();
	if (result)
	{
		Sync::MutexUsage mutUsage(result->mut);
		Net::DNSClient::ParseAnswers(result->recBuff, result->recSize, ansList);
		reqTime->SetTicks(result->reqTime);
		*ttl = result->ttl;
		mutUsage.EndUse();
		return true;
	}
	else
	{
		return false;
	}
}

UInt32 Net::DNSProxy::GetServerIP()
{
	return this->currServerIP;
}

void Net::DNSProxy::SetServerIP(UInt32 serverIP)
{
	Sync::MutexUsage mutUsage(this->dnsMut);
	this->dnsList->Clear();
	this->dnsList->Add(serverIP);
	this->currServerIP = serverIP;
	this->currIPTime->SetCurrTimeUTC();
	this->currServerIndex = 0;
	mutUsage.EndUse();
}

void Net::DNSProxy::GetDNSList(Data::ArrayList<UInt32> *dnsList)
{
	Sync::MutexUsage mutUsage(this->dnsMut);
	dnsList->AddRange(this->dnsList);
	mutUsage.EndUse();
}

void Net::DNSProxy::AddDNSIP(UInt32 serverIP)
{
	Sync::MutexUsage mutUsage(this->dnsMut);
	this->dnsList->Add(serverIP);
	mutUsage.EndUse();
}

void Net::DNSProxy::SwitchDNS()
{
	Sync::MutexUsage mutUsage(this->dnsMut);
	this->currServerIndex = (this->currServerIndex + 1) % this->dnsList->GetCount();
	this->currServerIP = this->dnsList->GetItem(this->currServerIndex);
	this->currIPTime->SetCurrTimeUTC();
	mutUsage.EndUse();
}

Bool Net::DNSProxy::IsDisableV6()
{
	return this->disableV6;
}

void Net::DNSProxy::SetDisableV6(Bool disableV6)
{
	this->disableV6 = disableV6;
}

OSInt Net::DNSProxy::GetBlackList(Data::ArrayList<const UTF8Char*> *blackList)
{
	OSInt ret;
	Sync::MutexUsage mutUsage(this->blackListMut);
	ret = this->blackList->GetCount();
	blackList->AddRange(this->blackList);
	mutUsage.EndUse();
	return ret;
}

Bool Net::DNSProxy::AddBlackList(const UTF8Char *blackList)
{
	Sync::MutexUsage blackListMutUsage(this->blackListMut);
	this->blackList->Add(Text::StrCopyNew(blackList));
	blackListMutUsage.EndUse();

	UTF8Char sbuff[256];
	sbuff[0] = '.';
	Text::StrConcat(&sbuff[1], blackList);

	OSInt i;
	RequestResult *req;
	const UTF8Char *reqName;
	Data::ArrayList<RequestResult*> *reqList;
	Data::ArrayList<const UTF8Char *> *reqNames;
	Sync::MutexUsage reqv4MutUsage(this->reqv4Mut);
	reqList = this->reqv4Map->GetValues();
	reqNames = this->reqv4Map->GetKeys();
	i = reqList->GetCount();
	while (i-- > 0)
	{
		reqName = reqNames->GetItem(i);
		if (Text::StrEndsWithICase(reqName, sbuff))
		{
			req = reqList->GetItem(i);
			req->status = 1;
		}
	}
	reqv4MutUsage.EndUse();

	return true;
}

void Net::DNSProxy::HandleDNSRequest(DNSProxyRequest hdlr, void *userObj)
{
	Sync::MutexUsage mutUsage(this->hdlrMut);
	this->hdlrList->Add(hdlr);
	this->hdlrObjs->Add(userObj);
	mutUsage.EndUse();
}
