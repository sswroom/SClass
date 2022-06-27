#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "Net/NetBIOSScanner.h"
#include "Net/NetBIOSUtil.h"
#include "Sync/MutexUsage.h"

void __stdcall Net::NetBIOSScanner::OnUDPPacket(const Net::SocketUtil::AddressInfo *addr, UInt16 port, const UInt8 *buff, UOSInt dataSize, void *userData)
{
	Net::NetBIOSScanner *me = (Net::NetBIOSScanner*)userData;
	UInt32 sortableIP = ReadMUInt32(addr->addr);
	if (dataSize < 56)
	{
		return;
	}
	UInt16 nQuestions = ReadMUInt16(&buff[4]);
	UInt16 nAnswer = ReadMUInt16(&buff[6]);
	UInt16 dataLen = ReadMUInt16(&buff[54]);
	if (nQuestions == 0 && nAnswer == 1 && dataSize >= 56 + (UOSInt)dataLen)
	{
		UInt8 nName = buff[56];
		if (dataLen >= 7 + nName * 18)
		{
			NameAnswer *ans;
			Sync::MutexUsage mutUsage(&me->ansMut);
			ans = me->answers.Get(sortableIP);
			if (ans == 0)
			{
				ans = MemAlloc(NameAnswer, 1);
				ans->sortableIP = sortableIP;
				ans->names = 0;
				ans->namesCnt = 0;
				ans->ttl = ReadMUInt32(&buff[50]);
				MemCopyNO(ans->unitId, &buff[57 + nName * 18], 6);
				me->answers.Put(sortableIP, ans);
			}
			const UInt8 *namePtr = &buff[57];
			NameEntry *ent;
			if (ans->names)
			{
				MemFree(ans->names);
			}
			ans->namesCnt = nName;
			ans->names = MemAlloc(NameEntry, ans->namesCnt);
			ent = ans->names;
			while (nName-- > 0)
			{
				Text::StrConcatC(ent->nameBuff, namePtr, 15);
				ent->nameType = namePtr[15];
				ent->flags = ReadMUInt16(&namePtr[16]);
				namePtr += 18;
				ent++;
			}
			if (me->hdlr)
			{
				me->hdlr(me->hdlrObj, sortableIP);
			}
		}
	}
}

void Net::NetBIOSScanner::FreeAnswer(NameAnswer *ans)
{
	if (ans->names)
	{
		MemFree(ans->names);
	}
	MemFree(ans);
}

Net::NetBIOSScanner::NetBIOSScanner(Net::SocketFactory *sockf)
{
	NEW_CLASS(this->svr, Net::UDPServer(sockf, 0, 0, CSTR_NULL, OnUDPPacket, this, 0, CSTR_NULL, 2, false));
	this->hdlr = 0;
	this->hdlrObj = 0;
	if (!this->svr->IsError())
	{
		this->svr->SetBroadcast(true);
	}
}

Net::NetBIOSScanner::~NetBIOSScanner()
{
	DEL_CLASS(this->svr);
	const Data::ArrayList<NameAnswer*> *ansList = this->answers.GetValues();
	LIST_CALL_FUNC(ansList, FreeAnswer);
}

Bool Net::NetBIOSScanner::IsError() const
{
	return this->svr->IsError();
}

void Net::NetBIOSScanner::SendRequest(UInt32 ip)
{
	Data::DateTime dt;
	dt.SetCurrTimeUTC();
	UInt8 buff[50];
	WriteMUInt16(&buff[0], (UInt16)(dt.GetSecond() * 1000 + dt.GetMS())); //transaction ID
	WriteMUInt16(&buff[2], 0x10); //Flags = Broadcast
	WriteMUInt16(&buff[4], 1); //Questions
	WriteMUInt16(&buff[6], 0); //Answer RRs
	WriteMUInt16(&buff[8], 0); //Authority RRs
	WriteMUInt16(&buff[10], 0); //Additional RRs
	buff[12] = 32;
	Net::NetBIOSUtil::SetName(&buff[13], (const UTF8Char*)"*");
	WriteMUInt16(&buff[46], 0x21); //Question Type = NBSTAT
	WriteMUInt16(&buff[48], 0x01); //Question Class = IN
	Net::SocketUtil::AddressInfo addr;
	Net::SocketUtil::SetAddrInfoV4(&addr, ip);
	this->svr->SendTo(&addr, 137, buff, 50);
}

void Net::NetBIOSScanner::SetAnswerHandler(AnswerUpdated hdlr, void *userObj)
{
	this->hdlrObj = userObj;
	this->hdlr = hdlr;
}

const Data::ArrayList<Net::NetBIOSScanner::NameAnswer*> *Net::NetBIOSScanner::GetAnswers(Sync::MutexUsage *mutUsage) const
{
	mutUsage->ReplaceMutex(&this->ansMut);
	return this->answers.GetValues();
}
