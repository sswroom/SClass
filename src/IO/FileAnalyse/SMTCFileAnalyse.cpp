#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "IO/FileAnalyse/SMTCFileAnalyse.h"
#include "Net/SocketFactory.h"
#include "Sync/MutexUsage.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/StringTool.h"

void __stdcall IO::FileAnalyse::SMTCFileAnalyse::ParseThread(NN<Sync::Thread> thread)
{
	NN<IO::FileAnalyse::SMTCFileAnalyse> me = thread->GetUserObj().GetNN<IO::FileAnalyse::SMTCFileAnalyse>();
	NN<DataInfo> data;
	UInt64 dataSize;
	UInt64 ofst;
	UInt8 packetHdr[23];
	ofst = 4;
	dataSize = me->fd->GetDataSize();
	while (ofst <= dataSize - 21 && !thread->IsStopping())
	{
		if (me->fd->GetRealData(ofst, 23, BYTEARR(packetHdr)) < 21)
			break;
		
		data = MemAllocNN(DataInfo);
		data->ofst = ofst;
		data->type = packetHdr[20];
		if (data->type == 0 || data->type == 1)
		{
			data->size = 21;
		}
		else if (data->type == 2 || data->type == 3)
		{
			data->size = 23 + (UOSInt)ReadUInt16(&packetHdr[21]);
		}
		else
		{
			MemFreeNN(data);
			break;
		}
		Sync::MutexUsage mutUsage(me->dataMut);
		me->dataList.Add(data);
		mutUsage.EndUse();
		ofst += data->size;
	}
}

IO::FileAnalyse::SMTCFileAnalyse::SMTCFileAnalyse(NN<IO::StreamData> fd) : packetBuff(65535 + 23), thread(ParseThread, this, CSTR("SMTCFileAnalyse"))
{
	UInt8 buff[25];
	this->fd = 0;
	this->pauseParsing = false;
	if (fd->GetRealData(0, 25, BYTEARR(buff)) != 25)
	{
		return;
	}
	if (ReadNInt32(buff) == *(Int32*)"SmTC" && buff[24] == 0)
	{
		this->fd = fd->GetPartialData(0, fd->GetDataSize()).Ptr();
	}
	else
	{
		return;
	}
	this->thread.Start();
}

IO::FileAnalyse::SMTCFileAnalyse::~SMTCFileAnalyse()
{
	this->thread.Stop();
	SDEL_CLASS(this->fd);
}

Text::CStringNN IO::FileAnalyse::SMTCFileAnalyse::GetFormatName()
{
	return CSTR("SMTC");
}

UOSInt IO::FileAnalyse::SMTCFileAnalyse::GetFrameCount()
{
	return 1 + this->dataList.GetCount();
}

Bool IO::FileAnalyse::SMTCFileAnalyse::GetFrameName(UOSInt index, NN<Text::StringBuilderUTF8> sb)
{
	if (index == 0)
	{
		sb->AppendC(UTF8STRC("SMTC Header"));
		return true;
	}
	NN<DataInfo> data;
	if (index > this->dataList.GetCount())
	{
		return false;
	}
	UTF8Char sbuff[64];
	UnsafeArray<UTF8Char> sptr;
	UInt8 hdr[21];
	Sync::MutexUsage mutUsage(this->dataMut);
	data = this->dataList.GetItemNoCheck(index - 1);
	mutUsage.EndUse();
	this->fd->GetRealData(data->ofst, 21, BYTEARR(hdr));
	Data::Timestamp ts = Data::Timestamp(Data::TimeInstant(ReadInt64(&hdr[0]), ReadUInt32(&hdr[8])), Data::DateTimeUtil::GetLocalTzQhr());
	UInt32 ip;
	UInt16 port;
	Net::SocketFactory::FromSocketId(ReadUInt64(&hdr[12]), ip, port);
	sb->AppendTSNoZone(ts);
	sb->AppendC(UTF8STRC(", "));
	sptr = Net::SocketUtil::GetIPv4Name(sbuff, ip, port);
	sb->AppendP(sbuff, sptr);
	sb->AppendC(UTF8STRC(", "));
	switch (data->type)
	{
	case 0:
		sb->AppendC(UTF8STRC("Connected"));
		break;
	case 1:
		sb->AppendC(UTF8STRC("Disconnected"));
		break;
	case 2:
		sb->AppendC(UTF8STRC("Recv "));
		sb->AppendUOSInt(data->size - 23);
		sb->AppendC(UTF8STRC(" bytes"));
		break;
	case 3:
		sb->AppendC(UTF8STRC("Sent "));
		sb->AppendUOSInt(data->size - 23);
		sb->AppendC(UTF8STRC(" bytes"));
		break;
	}
	return true;
}

UOSInt IO::FileAnalyse::SMTCFileAnalyse::GetFrameIndex(UInt64 ofst)
{
	if (ofst < 4)
	{
		return 0;
	}
	OSInt i = 0;
	OSInt j = (OSInt)this->dataList.GetCount() - 1;
	OSInt k;
	NN<DataInfo> data;
	while (i <= j)
	{
		k = (i + j) >> 1;
		data = this->dataList.GetItemNoCheck((UOSInt)k);
		if (ofst < data->ofst)
		{
			j = k - 1;
		}
		else if (ofst >= data->ofst + data->size)
		{
			i = k + 1;
		}
		else
		{
			return (UOSInt)k + 1;
		}
	}
	return INVALID_INDEX;
}

Optional<IO::FileAnalyse::FrameDetail> IO::FileAnalyse::SMTCFileAnalyse::GetFrameDetail(UOSInt index)
{
	Text::StringBuilderUTF8 sb;
	NN<IO::FileAnalyse::FrameDetail> frame;
	UTF8Char sbuff[128];
	UnsafeArray<UTF8Char> sptr;
	if (index == 0)
	{
		NEW_CLASSNN(frame, IO::FileAnalyse::FrameDetail(0, 4));
		fd->GetRealData(0, 4, this->packetBuff);
		frame->AddHexBuff(0, CSTR("File Header"), this->packetBuff.WithSize(4), false);
		return frame;
	}
	NN<DataInfo> data;
	if (index > this->dataList.GetCount())
	{
		return 0;
	}
	Sync::MutexUsage mutUsage(this->dataMut);
	data = this->dataList.GetItemNoCheck(index - 1);
	mutUsage.EndUse();
	NEW_CLASSNN(frame, IO::FileAnalyse::FrameDetail(data->ofst, data->size));
	fd->GetRealData(data->ofst, (UOSInt)data->size, this->packetBuff);
	Data::Timestamp ts = Data::Timestamp(Data::TimeInstant(this->packetBuff.ReadI64(0), this->packetBuff.ReadU32(8)), Data::DateTimeUtil::GetLocalTzQhr());
	sptr = ts.ToStringNoZone(sbuff);
	frame->AddField(0, 12, CSTR("Time"), CSTRP(sbuff, sptr));
	UInt32 ip;
	UInt16 port;
	Net::SocketFactory::FromSocketId(this->packetBuff.ReadU64(12), ip, port);
	sptr = Net::SocketUtil::GetIPv4Name(sbuff, ip, port);
	frame->AddField(12, 8, CSTR("Client IP"), CSTRP(sbuff, sptr));
	Bool hasData = false;
	switch (data->type)
	{
	case 0:
		frame->AddUIntName(20, 1, CSTR("Type"), data->type, CSTR("Connected"));
		break;
	case 1:
		frame->AddUIntName(20, 1, CSTR("Type"), data->type, CSTR("Disconnected"));
		break;
	case 2:
		frame->AddUIntName(20, 1, CSTR("Type"), data->type, CSTR("Recv"));
		hasData = true;
		break;
	case 3:
		frame->AddUIntName(20, 1, CSTR("Type"), data->type, CSTR("Sent"));
		hasData = true;
		break;
	}
	if (hasData)
	{
		frame->AddUInt(21, 2, CSTR("Packet Size"), this->packetBuff.ReadU16(21));
		if (Text::StringTool::IsASCIIText(this->packetBuff.WithSize(data->size).SubArray(23)))
		{
			frame->AddStrC(23, data->size - 23, CSTR("Packet Data"), &this->packetBuff[23]);
		}
		else
		{
			frame->AddHexBuff(23, CSTR("Packet Data"), this->packetBuff.WithSize(data->size).SubArray(23), true);
		}
	}
	return frame;
}

Bool IO::FileAnalyse::SMTCFileAnalyse::IsError()
{
	return this->fd == 0;
}

Bool IO::FileAnalyse::SMTCFileAnalyse::IsParsing()
{
	return this->thread.IsRunning();
}

Bool IO::FileAnalyse::SMTCFileAnalyse::TrimPadding(Text::CStringNN outputFile)
{
	return false;
}
