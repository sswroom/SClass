#include "Stdafx.h"
#include "Core/ByteTool_C.h"
#include "Net/NetBIOSScanner.h"
#include "Net/NetBIOSUtil.h"
#include "Sync/MutexUsage.h"

void __stdcall Net::NetBIOSScanner::OnUDPPacket(NN<const Net::SocketUtil::AddressInfo> addr, UInt16 port, Data::ByteArrayR data, AnyType userData)
{
	NN<Net::NetBIOSScanner> me = userData.GetNN<Net::NetBIOSScanner>();
	UInt32 sortableIP = ReadMUInt32(addr->addr);
	if (data.GetSize() < 56)
	{
		return;
	}
	UInt16 nQuestions = ReadMUInt16(&data[4]);
	UInt16 nAnswer = ReadMUInt16(&data[6]);
	UInt16 dataLen = ReadMUInt16(&data[54]);
	if (nQuestions == 0 && nAnswer == 1 && data.GetSize() >= 56 + (UIntOS)dataLen)
	{
		UInt8 nName = data[56];
		if (dataLen >= 7 + nName * 18)
		{
			NN<NameAnswer> ans;
			Sync::MutexUsage mutUsage(me->ansMut);
			if (!me->answers.Get(sortableIP).SetTo(ans))
			{
				ans = MemAllocNN(NameAnswer);
				ans->sortableIP = sortableIP;
				ans->names = 0;
				ans->namesCnt = 0;
				ans->ttl = ReadMUInt32(&data[50]);
				MemCopyNO(ans->unitId, &data[57 + nName * 18], 6);
				me->answers.Put(sortableIP, ans);
			}
			const UInt8 *namePtr = &data[57];
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
			if (me->hdlr.func)
			{
				me->hdlr.func(me->hdlr.userObj, sortableIP);
			}
		}
	}
}

void __stdcall Net::NetBIOSScanner::FreeAnswer(NN<NameAnswer> ans)
{
	if (ans->names)
	{
		MemFree(ans->names);
	}
	MemFreeNN(ans);
}

Net::NetBIOSScanner::NetBIOSScanner(NN<Net::SocketFactory> sockf, NN<IO::LogTool> log)
{
	NEW_CLASS(this->svr, Net::UDPServer(sockf, nullptr, 0, nullptr, OnUDPPacket, this, log, nullptr, 2, false));
	this->hdlr = 0;
	if (!this->svr->IsError())
	{
		this->svr->SetBroadcast(true);
	}
}

Net::NetBIOSScanner::~NetBIOSScanner()
{
	DEL_CLASS(this->svr);
	this->answers.FreeAll(FreeAnswer);
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
	Net::SocketUtil::SetAddrInfoV4(addr, ip);
	this->svr->SendTo(addr, 137, buff, 50);
}

void Net::NetBIOSScanner::SetAnswerHandler(AnswerUpdated hdlr, AnyType userObj)
{
	this->hdlr = {hdlr, userObj};
}

NN<const Data::ReadingListNN<Net::NetBIOSScanner::NameAnswer>> Net::NetBIOSScanner::GetAnswers(NN<Sync::MutexUsage> mutUsage) const
{
	mutUsage->ReplaceMutex(this->ansMut);
	return this->answers;
}
