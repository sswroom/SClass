#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "Manage/HiResClock.h"
#include "Net/DNSProxy.h"
#include "Sync/MutexUsage.h"
#include "Text/MyString.h"
#include "Text/StringBuilder.h"

#define REQTIMEOUT 10000

void __stdcall Net::DNSProxy::ClientPacket(NN<const Net::SocketUtil::AddressInfo> addr, UInt16 port, const UInt8 *buff, UOSInt dataSize, AnyType userData)
{
	NN<Net::DNSProxy> me = userData.GetNN<Net::DNSProxy>();
	NN<CliRequestStatus> req;
	Sync::MutexUsage mutUsage(me->cliReqMut);
	if (me->cliReqMap.Get(ReadMUInt16(buff)).SetTo(req))
	{
		MemCopyNO(req->respBuff, buff, dataSize);
		req->respSize = dataSize;
		req->finEvt.Set();
	}
}

void __stdcall Net::DNSProxy::OnDNSRequest(AnyType userObj, Text::CStringNN reqName, Int32 reqType, Int32 reqClass, NN<const Net::SocketUtil::AddressInfo> reqAddr, UInt16 reqPort, UInt32 reqId)
{
	NN<Net::DNSProxy> me = userObj.GetNN<Net::DNSProxy>();
	NN<RequestResult> req;
	UInt8 buff[512];
	UOSInt buffSize;
	UOSInt i;
	Manage::HiResClock clk;
	Data::Timestamp currTime = Data::Timestamp::UtcNow();

	if (reqType == 1)
	{
		Sync::MutexUsage reqv4MutUsage(me->reqv4Mut);
		if (me->reqv4Map.Get(reqName).SetTo(req))
		{
			reqv4MutUsage.EndUse();
			if (req->status == NS_CUSTOM)
			{
				buffSize = BuildAddressReply(buff, reqId, reqName.v, reqClass, &req->customAddr);
				me->svr->ReplyRequest(reqAddr, reqPort, buff, buffSize);
			}
			else if (req->status == NS_BLOCKED)
			{
				buffSize = BuildEmptyReply(buff, reqId, reqName.v, reqType, reqClass, me->disableV6);
				me->svr->ReplyRequest(reqAddr, reqPort, buff, buffSize);
			}
			else
			{
				Sync::MutexUsage mutUsage(req->mut);
				if (currTime.DiffMS(req->reqTime) > req->ttl * 1000)
				{
					me->RequestDNS(reqName.v, reqType, reqClass, req);
					req->reqTime = currTime;
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
			NEW_CLASSNN(req, RequestResult());
			req->status = NS_NORMAL;
			req->recSize = 0;
			req->reqTime = currTime.AddMS(-REQTIMEOUT - REQTIMEOUT);
			Sync::MutexUsage mutUsage(req->mut);
			me->reqv4Map.Put(reqName, req);
			me->reqv4Updated = true;
			reqv4MutUsage.EndUse();

			Sync::MutexUsage blackListMutUsage(me->blackListMut);
			i = me->blackList.GetCount();
			while (i-- > 0)
			{
				NN<Text::String> blName;
				if (me->blackList.GetItem(i).SetTo(blName))
				{
					if (reqName.Equals(blName))
					{
						req->status = NS_BLOCKED;
						break;
					}
					else if (reqName.EndsWith(blName->v, blName->leng) && reqName.v[reqName.leng - blName->leng] == '.')
					{
						req->status = NS_BLOCKED;
						break;
					}
				}
			}
			blackListMutUsage.EndUse();

			if (req->status == NS_BLOCKED)
			{
				req->reqTime = currTime;
				req->ttl = REQTIMEOUT / 1000;
				mutUsage.EndUse();
				buffSize = BuildEmptyReply(buff, reqId, reqName.v, reqType, reqClass, me->disableV6);
			}
			else
			{
				me->RequestDNS(reqName.v, reqType, reqClass, req);
				req->reqTime = currTime;
				buffSize = req->recSize;
				MemCopyNO(buff, req->recBuff, req->recSize);
				req->ttl = Net::DNSClient::GetResponseTTL(req->recBuff, req->recSize);
				if (req->ttl == 0)
					req->ttl = REQTIMEOUT / 1000;
				mutUsage.EndUse();
				WriteMInt16(buff, reqId);
			}
			me->svr->ReplyRequest(reqAddr, reqPort, buff, buffSize);

			NN<Sync::Mutex> targetMut;
			if (req->status == NS_NORMAL && me->targetMap && targetMut.Set(me->targetMut))
			{
				Data::ArrayListNN<Net::DNSClient::RequestAnswer> ansList;
				UInt32 resIP;
				UInt32 sortIP;
				UOSInt j;
				NN<TargetInfo> target;
				mutUsage.BeginUse();
				Net::DNSClient::ParseAnswers(req->recBuff, req->recSize, ansList);
				mutUsage.EndUse();
				NN<Net::DNSClient::RequestAnswer> ans;
				
				i = ansList.GetCount();
				while (i-- > 0)
				{
					ans = ansList.GetItemNoCheck(i);
					if (ans->recType == 1)
					{
						resIP = ReadNUInt32(ans->addr.addr);
						sortIP = ReadMUInt32(ans->addr.addr);
						Sync::MutexUsage targetMutUsage(targetMut);
						if (!me->targetMap->Get(sortIP).SetTo(target))
						{
							NEW_CLASSNN(target, TargetInfo());
							target->ip = resIP;
							me->targetMap->Put(sortIP, target);
							me->targetUpdated = true;
						}
						Sync::MutexUsage mutUsage(target->mut);
						targetMutUsage.EndUse();
						if (target->addrList.SortedIndexOf(ans->name.Ptr()) < 0)
						{
							target->addrList.SortedInsert(ans->name->Clone().Ptr());
						}
						j = i;
						while (j-- > 0)
						{
							ans = ansList.GetItemNoCheck(j);
							if (ans->recType == 5)
							{
								if (target->addrList.SortedIndexOf(ans->name.Ptr()) < 0)
								{
									target->addrList.SortedInsert(ans->name->Clone().Ptr());
								}
							}
						}
						mutUsage.EndUse();
					}
				}
				Net::DNSClient::FreeAnswers(ansList);
			}
		}
	}
	else if (reqType == 28)
	{
		Sync::MutexUsage reqv6MutUsage(me->reqv6Mut);
		if (me->reqv6Map.Get(reqName).SetTo(req))
		{
			reqv6MutUsage.EndUse();
			if (req->status == NS_CUSTOM)
			{
				buffSize = BuildAddressReply(buff, reqId, reqName.v, reqClass, &req->customAddr);
				me->svr->ReplyRequest(reqAddr, reqPort, buff, buffSize);
			}
			else if (req->status == NS_BLOCKED)
			{
				buffSize = BuildEmptyReply(buff, reqId, reqName.v, reqType, reqClass, me->disableV6);
				me->svr->ReplyRequest(reqAddr, reqPort, buff, buffSize);
			}
			else
			{
				Sync::MutexUsage mutUsage(req->mut);
				if (currTime.DiffMS(req->reqTime) > req->ttl * 1000)
				{
					me->RequestDNS(reqName.v, reqType, reqClass, req);
					req->reqTime = currTime;
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
			NEW_CLASSNN(req, RequestResult());
			req->status = NS_NORMAL;
			req->recSize = 0;
			req->reqTime = currTime.AddMS(-REQTIMEOUT - REQTIMEOUT);
			Sync::MutexUsage mutUsage(req->mut);
			me->reqv6Map.Put(reqName, req);
			me->reqv6Updated = true;
			reqv6MutUsage.EndUse();

			if (me->disableV6)
			{
				req->status = NS_BLOCKED;
				req->reqTime = currTime;
				req->ttl = REQTIMEOUT / 1000;
				mutUsage.EndUse();
				buffSize = BuildEmptyReply(buff, reqId, reqName.v, reqType, reqClass, me->disableV6);
			}
			else
			{
				me->RequestDNS(reqName.v, reqType, reqClass, req);
				req->reqTime = currTime;
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
		if (me->reqothMap.Get(reqName).SetTo(req))
		{
			reqothMutUsage.EndUse();
			Sync::MutexUsage mutUsage(req->mut);
			if (currTime.DiffMS(req->reqTime) > req->ttl * 1000)
			{
				me->RequestDNS(reqName.v, reqType, reqClass, req);
				req->reqTime = currTime;
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
			NEW_CLASSNN(req, RequestResult());
			req->status = NS_NORMAL;
			req->recSize = 0;
			req->reqTime = currTime.AddMS(-REQTIMEOUT - REQTIMEOUT);
			Sync::MutexUsage mutUsage(req->mut);
			me->reqothMap.Put(reqName, req);
			me->reqothUpdated = true;
			reqothMutUsage.EndUse();

			me->RequestDNS(reqName.v, reqType, reqClass, req);
			req->reqTime = currTime;
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
	i = me->hdlrList.GetCount();
	while (i-- > 0)
	{
		Data::CallbackStorage<DNSProxyRequest> cb = me->hdlrList.GetItem(i);
		cb.func(cb.userObj, reqName, reqType, reqClass, reqAddr, reqPort, reqId, t);
	}
	hdlrMutUsage.EndUse();
}

void Net::DNSProxy::RequestDNS(const UTF8Char *reqName, Int32 reqType, Int32 reqClass, NN<RequestResult> req)
{
	UInt8 buff[512];
	UInt8 *ptr1;
	UInt8 *ptr2;
	const UTF8Char *cptr1;
	const UTF8Char *cptr2;
	UTF8Char c;
	UInt32 currId = this->NextId();

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
	
	NN<CliRequestStatus> cliReq = this->NewCliReq(currId);
	Net::SocketUtil::AddressInfo addr;
	Net::SocketUtil::SetAddrInfoV4(addr, this->currServerIP);
	cliReq->finEvt.Clear();
	this->cli->SendTo(addr, 53, buff, (UOSInt)(ptr1 - ptr2));
	cliReq->finEvt.Wait(2000);
	if (cliReq->respSize > 12)
	{
		MemCopyNO(req->recBuff, cliReq->respBuff, cliReq->respSize);
		req->recSize = cliReq->respSize;
	}
	else
	{
		if (req->recSize == 0)
		{
			MemCopyNO(req->recBuff, buff, (UOSInt)(ptr1 - ptr2));
			req->recSize = (UOSInt)(ptr1 - ptr2);
			WriteMInt16(&req->recBuff[2], 0x8581);
		}
		Data::DateTime currTime;
		currTime.SetCurrTimeUTC();
		Sync::MutexUsage mutUsage(this->dnsMut);
		if (currTime.DiffMS(this->currIPTime) > REQTIMEOUT)
		{
			this->currServerIndex = (this->currServerIndex + 1) % this->dnsList.GetCount();
			this->currServerIP = this->dnsList.GetItem(this->currServerIndex);
			this->currIPTime.SetValue(currTime);
			req->ttl = 1;
		}
		mutUsage.EndUse();
	}
	this->DelCliReq(currId);
}

UInt32 Net::DNSProxy::NextId()
{
	UInt32 id;
	Sync::MutexUsage mutUsage(this->lastIdMut);
	this->lastId++;
	if (this->lastId >= 65536)
		this->lastId = 1;
	id = this->lastId;
	mutUsage.EndUse();
	return id;
}

NN<Net::DNSProxy::CliRequestStatus> Net::DNSProxy::NewCliReq(UInt32 id)
{
	NN<CliRequestStatus> req;
	NEW_CLASSNN(req, CliRequestStatus());
	req->respSize = 0;
	Sync::MutexUsage mutUsage(this->cliReqMut);
	this->cliReqMap.Put(id, req);
	mutUsage.EndUse();
	return req;
}

void Net::DNSProxy::DelCliReq(UInt32 id)
{
	Optional<CliRequestStatus> req;
	Sync::MutexUsage mutUsage(this->cliReqMut);
	req = this->cliReqMap.Remove(id);
	mutUsage.EndUse();
	req.Delete();
}

UOSInt Net::DNSProxy::BuildEmptyReply(UInt8 *buff, UInt32 id, const UTF8Char *reqName, Int32 reqType, Int32 reqClass, Bool disableV6)
{
	Bool localhostResp;
	if (reqType == 28 && disableV6)
	{
		localhostResp = false;
	}
	else
	{
		localhostResp = true;
	}
	// RFC1035
	UOSInt i;
	UOSInt j;
	UTF8Char c;
	WriteMInt16(&buff[0], id); // Header
	buff[2] = 0x81; //Question
	buff[3] = 0x80;
	WriteMInt16(&buff[4], 1); //QDCOUNT
	WriteMInt16(&buff[6], localhostResp?1:0); //ANCOUNT
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

	if (localhostResp)
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

	return i;
}

UOSInt Net::DNSProxy::BuildAddressReply(UInt8 *buff, UInt32 id, const UTF8Char *reqName, Int32 reqClass, const Net::SocketUtil::AddressInfo *addr)
{
	Int16 reqType;
	if (addr->addrType == Net::AddrType::IPv4)
	{
		reqType = 1;
	}
	else if (addr->addrType == Net::AddrType::IPv6)
	{
		reqType = 28;
	}
	else
	{
		return 0;
	}
	UOSInt i;
	UOSInt j;
	UTF8Char c;
	WriteMInt16(&buff[0], id); // Header
	buff[2] = 0x81; //Question
	buff[3] = 0x80;
	WriteMInt16(&buff[4], 1); //QDCOUNT
	WriteMInt16(&buff[6], 1); //ANCOUNT
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

	//ANSWER
	if (reqType == 28)
	{
		buff[i] = 0xc0;
		buff[i + 1] = 0x0c;
		WriteMInt16(&buff[i + 2], 28); //TYPE=A
		WriteMInt16(&buff[i + 4], 1); //CLASS=IN
		WriteMInt32(&buff[i + 6], 10000); //TTL=10000
		WriteMInt16(&buff[i + 10], 16); //RDLENGTH=16
		MemCopyNO(&buff[i + 12], addr->addr, 16);
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
		MemCopyNO(&buff[i + 12], addr->addr, 4);
		i += 16;
	}

	return i;
}

Net::DNSProxy::DNSProxy(NN<Net::SocketFactory> sockf, Bool analyzeTarget, NN<IO::LogTool> log)
{
	this->sockf = sockf;
	this->disableV6 = false;
	this->reqv4Updated = true;
	this->reqv6Updated = true;
	this->reqothUpdated = true;
	if (analyzeTarget)
	{
		NEW_CLASS(this->targetMut, Sync::Mutex());
		NEW_CLASS(this->targetMap, Data::UInt32FastMapNN<TargetInfo>());
		this->targetUpdated = true;
	}
	else
	{
		this->targetMap = 0;
		this->targetMut = 0;
		this->targetUpdated = false;
	}
	this->lastId = 0;
	sockf->GetDNSList(&this->dnsList);
	this->currServerIP = this->dnsList.GetItem(0);
	this->currServerIndex = 0;
	this->currIPTime.SetCurrTimeUTC();

	NEW_CLASS(this->cli, Net::UDPServer(this->sockf, 0, 0, CSTR_NULL, ClientPacket, this, log, CSTR_NULL, 2, false));
	NEW_CLASS(this->svr, Net::DNSServer(this->sockf, log));
	this->svr->HandleRequest(OnDNSRequest, this);
}

Net::DNSProxy::~DNSProxy()
{
	DEL_CLASS(this->svr);
	DEL_CLASS(this->cli);

	NN<const Data::ArrayListNN<RequestResult>> reqList;
	NN<RequestResult> req;
	UOSInt i;
	reqList = this->reqv4Map.GetValues();
	i = reqList->GetCount();
	while (i-- > 0)
	{
		req = reqList->GetItemNoCheck(i);
		req.Delete();
	}

	reqList = this->reqv6Map.GetValues();
	i = reqList->GetCount();
	while (i-- > 0)
	{
		req = reqList->GetItemNoCheck(i);
		req.Delete();
	}

	reqList = this->reqothMap.GetValues();
	i = reqList->GetCount();
	while (i-- > 0)
	{
		req = reqList->GetItemNoCheck(i);
		req.Delete();
	}
	this->blackList.FreeAll();
	
	if (this->targetMap)
	{
		NN<TargetInfo> target;
		i = this->targetMap->GetCount();
		while (i-- > 0)
		{
			target = this->targetMap->GetItemNoCheck(i);
			LIST_FREE_STRING(&target->addrList);
			target.Delete();
		}
		DEL_CLASS(this->targetMut);
		DEL_CLASS(this->targetMap);
	}
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

UOSInt Net::DNSProxy::GetReqv4List(NN<Data::ArrayListNN<Text::String>> reqList)
{
	Sync::MutexUsage mutUsage(this->reqv4Mut);
	return reqList->AddAllOpt(this->reqv4Map.GetKeys());
}

UOSInt Net::DNSProxy::GetReqv6List(NN<Data::ArrayListNN<Text::String>> reqList)
{
	Sync::MutexUsage mutUsage(this->reqv6Mut);
	return reqList->AddAllOpt(this->reqv6Map.GetKeys());
}

UOSInt Net::DNSProxy::GetReqOthList(NN<Data::ArrayListNN<Text::String>> reqList)
{
	Sync::MutexUsage mutUsage(this->reqothMut);
	return reqList->AddAllOpt(this->reqothMap.GetKeys());
}

UOSInt Net::DNSProxy::GetTargetList(NN<Data::ArrayListNN<TargetInfo>> targetList)
{
	NN<const Data::ReadingListNN<TargetInfo>> thisList;
	NN<Sync::Mutex> targetMut;
	if (!thisList.Set(this->targetMap) || !targetMut.Set(this->targetMut))
		return 0;
	Sync::MutexUsage mutUsage(targetMut);
	return targetList->AddAll(thisList);
}

UOSInt Net::DNSProxy::SearchIPv4(NN<Data::ArrayListNN<Text::String>> reqList, UInt32 ip, UInt32 mask)
{
	NN<Data::ArrayList<Text::String*>> keys;
	NN<Text::String> key;
	NN<const Data::ArrayListNN<RequestResult>> results;
	Data::ArrayListNN<Net::DNSClient::RequestAnswer> ansList;
	NN<Net::DNSClient::RequestAnswer> ans;
	NN<RequestResult> result;
	Bool valid;
	UOSInt i;
	UOSInt j;
	UOSInt k;
	UOSInt retCnt = 0;
	Sync::MutexUsage mutUsage(this->reqv4Mut);
	keys = this->reqv4Map.GetKeys();
	results = this->reqv4Map.GetValues();
	i = 0;
	j = results->GetCount();
	while (i < j)
	{
		result = results->GetItemNoCheck(i);
		valid = false;
		Sync::MutexUsage mutUsage(result->mut);
		ansList.Clear();
		Net::DNSClient::ParseAnswers(result->recBuff, result->recSize, ansList);
		mutUsage.EndUse();
		k = ansList.GetCount();
		while (k-- > 0)
		{
			ans = ansList.GetItemNoCheck(k);
			if (ans->recType == 1)
			{
				if ((ip & mask) == (ReadNUInt32(ans->addr.addr) & mask))
				{
					valid = true;
					break;
				}
			}
		}
		Net::DNSClient::FreeAnswers(ansList);

		if (valid && key.Set(keys->GetItem(i)))
		{
			reqList->Add(key);
			retCnt++;
		}
		i++;
	}
	mutUsage.EndUse();
	return retCnt;
}

Bool Net::DNSProxy::GetRequestInfov4(Text::CStringNN req, NN<Data::ArrayListNN<Net::DNSClient::RequestAnswer>> ansList, NN<Data::DateTime> reqTime, OutParam<UInt32> ttl)
{
	NN<RequestResult> result;
	Sync::MutexUsage mutUsage(this->reqv4Mut);
	if (this->reqv4Map.Get(req).SetTo(result))
	{
		mutUsage.EndUse();
		Sync::MutexUsage mutUsage(result->mut);
		Net::DNSClient::ParseAnswers(result->recBuff, result->recSize, ansList);
		reqTime->SetInstant(result->reqTime.inst);
		ttl.Set(result->ttl);
		return true;
	}
	else
	{
		return false;
	}
}

Bool Net::DNSProxy::GetRequestInfov6(Text::CStringNN req, NN<Data::ArrayListNN<Net::DNSClient::RequestAnswer>> ansList, NN<Data::DateTime> reqTime, OutParam<UInt32> ttl)
{
	NN<RequestResult> result;
	Sync::MutexUsage mutUsage(this->reqv6Mut);
	if (this->reqv6Map.Get(req).SetTo(result))
	{
		mutUsage.EndUse();
		Sync::MutexUsage mutUsage(result->mut);
		Net::DNSClient::ParseAnswers(result->recBuff, result->recSize, ansList);
		reqTime->SetInstant(result->reqTime.inst);
		ttl.Set(result->ttl);
		return true;
	}
	else
	{
		return false;
	}
}

Bool Net::DNSProxy::GetRequestInfoOth(Text::CStringNN req, NN<Data::ArrayListNN<Net::DNSClient::RequestAnswer>> ansList, NN<Data::DateTime> reqTime, OutParam<UInt32> ttl)
{
	NN<RequestResult> result;
	Sync::MutexUsage mutUsage(this->reqothMut);
	if (this->reqothMap.Get(req).SetTo(result))
	{
		mutUsage.EndUse();
		Sync::MutexUsage mutUsage(result->mut);
		Net::DNSClient::ParseAnswers(result->recBuff, result->recSize, ansList);
		reqTime->SetInstant(result->reqTime.inst);
		ttl.Set(result->ttl);
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
	this->dnsList.Clear();
	this->dnsList.Add(serverIP);
	this->currServerIP = serverIP;
	this->currIPTime.SetCurrTimeUTC();
	this->currServerIndex = 0;
}

void Net::DNSProxy::GetDNSList(NN<Data::ArrayList<UInt32>> dnsList)
{
	Sync::MutexUsage mutUsage(this->dnsMut);
	dnsList->AddAll(this->dnsList);
}

void Net::DNSProxy::AddDNSIP(UInt32 serverIP)
{
	Sync::MutexUsage mutUsage(this->dnsMut);
	this->dnsList.Add(serverIP);
}

void Net::DNSProxy::SwitchDNS()
{
	Sync::MutexUsage mutUsage(this->dnsMut);
	this->currServerIndex = (this->currServerIndex + 1) % this->dnsList.GetCount();
	this->currServerIP = this->dnsList.GetItem(this->currServerIndex);
	this->currIPTime.SetCurrTimeUTC();
}

Bool Net::DNSProxy::IsDisableV6()
{
	return this->disableV6;
}

void Net::DNSProxy::SetDisableV6(Bool disableV6)
{
	this->disableV6 = disableV6;
}

UOSInt Net::DNSProxy::GetBlackList(NN<Data::ArrayListNN<Text::String>> blackList)
{
	Sync::MutexUsage mutUsage(this->blackListMut);
	return blackList->AddAll(this->blackList);
}

Bool Net::DNSProxy::AddBlackList(NN<Text::String> blackList)
{
	return this->AddBlackList(blackList->ToCString());
}

Bool Net::DNSProxy::AddBlackList(Text::CStringNN blackList)
{
	Sync::MutexUsage blackListMutUsage(this->blackListMut);
	this->blackList.Add(Text::String::New(blackList));
	blackListMutUsage.EndUse();

	UTF8Char sbuff[256];
	sbuff[0] = '.';
	blackList.ConcatTo(&sbuff[1]);

	UOSInt i;
	NN<RequestResult> req;
	Text::String *reqName;
	NN<const Data::ArrayListNN<RequestResult>> reqList;
	NN<Data::ArrayList<Text::String *>> reqNames;
	Sync::MutexUsage reqv4MutUsage(this->reqv4Mut);
	reqList = this->reqv4Map.GetValues();
	reqNames = this->reqv4Map.GetKeys();
	i = reqList->GetCount();
	while (i-- > 0)
	{
		reqName = reqNames->GetItem(i);
		if (reqName->Equals(blackList.v, blackList.leng) || (reqName->EndsWithICase(blackList.v, blackList.leng) && reqName->v[reqName->leng - blackList.leng] == '.'))
		{
			req = reqList->GetItemNoCheck(i);
			req->status = NS_BLOCKED;
		}
	}
	reqv4MutUsage.EndUse();

	return true;
}

void Net::DNSProxy::HandleDNSRequest(DNSProxyRequest hdlr, AnyType userObj)
{
	Sync::MutexUsage mutUsage(this->hdlrMut);
	this->hdlrList.Add({hdlr, userObj});
}

void Net::DNSProxy::SetCustomAnswer(Text::CStringNN name, NN<const Net::SocketUtil::AddressInfo> addr)
{
	Data::Timestamp currTime = Data::Timestamp::UtcNow();
	if (addr->addrType == Net::AddrType::IPv4)
	{
		NN<RequestResult> req;
		Sync::MutexUsage reqv4MutUsage(this->reqv4Mut);
		Sync::MutexUsage mutUsage;
		if (!this->reqv4Map.Get(name).SetTo(req))
		{
			NEW_CLASSNN(req, RequestResult());
			req->status = NS_CUSTOM;
			req->recSize = 0;
			req->reqTime = 0;
			mutUsage.ReplaceMutex(req->mut);
			this->reqv4Map.Put(name, req);
			this->reqv4Updated = true;
			reqv4MutUsage.EndUse();
		}
		else
		{
			mutUsage.ReplaceMutex(req->mut);
			reqv4MutUsage.EndUse();
		}
		req->status = NS_CUSTOM;
		req->reqTime = currTime;
		req->ttl = REQTIMEOUT / 1000;
		req->customAddr = addr.Ptr()[0];
		mutUsage.EndUse();
	}
	else if (addr->addrType == Net::AddrType::IPv6)
	{
		NN<RequestResult> req;
		Sync::MutexUsage reqv6MutUsage(this->reqv6Mut);
		Sync::MutexUsage mutUsage;
		if (!this->reqv6Map.Get(name).SetTo(req))
		{
			NEW_CLASSNN(req, RequestResult());
			req->status = NS_CUSTOM;
			req->recSize = 0;
			req->reqTime = 0;
			mutUsage.ReplaceMutex(req->mut);
			this->reqv6Map.Put(name, req);
			this->reqv6Updated = true;
			reqv6MutUsage.EndUse();
		}
		else
		{
			mutUsage.ReplaceMutex(req->mut);
			reqv6MutUsage.EndUse();
		}
		req->status = NS_CUSTOM;
		req->reqTime = currTime;
		req->ttl = REQTIMEOUT / 1000;
		req->customAddr = addr.Ptr()[0];
		mutUsage.EndUse();
	}
}

void Net::DNSProxy::SetWebProxyAutoDiscovery(NN<const Net::SocketUtil::AddressInfo> addr)
{
	this->SetCustomAnswer(CSTR("wpad"), addr);
}
