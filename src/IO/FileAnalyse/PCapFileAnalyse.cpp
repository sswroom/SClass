#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "IO/RAWMonitor.h"
#include "IO/FileAnalyse/PCapFileAnalyse.h"
#include "Net/PacketAnalyzer.h"
#include "Sync/MutexUsage.h"
#include "Sync/Thread.h"
#include "Text/StringBuilderUTF8.h"

UInt32 __stdcall IO::FileAnalyse::PCapFileAnalyse::ParseThread(void *userObj)
{
	IO::FileAnalyse::PCapFileAnalyse *me = (IO::FileAnalyse::PCapFileAnalyse*)userObj;
	UInt64 dataSize;
	UInt64 ofst;
	UInt32 thisSize;
	UInt8 packetHdr[16];
	me->threadRunning = true;
	me->threadStarted = true;
	ofst = 24;
	dataSize = me->fd->GetDataSize();
	while (ofst < dataSize - 16 && !me->threadToStop)
	{
		if (me->fd->GetRealData(ofst, 16, packetHdr) != 16)
			break;
		
		if (me->isBE)
		{
			thisSize = ReadMUInt32(&packetHdr[8]);
		}
		else
		{
			thisSize = ReadUInt32(&packetHdr[8]);
		}
		if (thisSize + ofst > dataSize)
		{
			break;
		}
		Sync::MutexUsage mutUsage(me->dataMut);
		me->ofstList->Add(ofst);
		me->sizeList->Add(thisSize + 16);
		mutUsage.EndUse();
		ofst += thisSize + 16;
	}
	me->threadRunning = false;
	return 0;
}

IO::FileAnalyse::PCapFileAnalyse::PCapFileAnalyse(IO::IStreamData *fd)
{
	UInt8 buff[24];
	this->fd = 0;
	this->threadRunning = false;
	this->pauseParsing = false;
	this->threadToStop = false;
	this->threadStarted = false;
	this->isBE = false;
	NEW_CLASS(this->dataMut, Sync::Mutex());
	NEW_CLASS(this->ofstList, Data::ArrayList<UInt64>());
	NEW_CLASS(this->sizeList, Data::ArrayList<UInt64>());
	this->packetBuff = MemAlloc(UInt8, 65536);
	if (fd->GetRealData(0, 24, buff) != 24)
	{
		return;
	}
	if (ReadUInt32(buff) == 0xa1b2c3d4)
	{
		this->fd = fd->GetPartialData(0, fd->GetDataSize());
		this->isBE = false;
		this->linkType = ReadUInt32(&buff[20]);
	}
	else if (ReadMUInt32(buff) == 0xa1b2c3d4)
	{
		this->fd = fd->GetPartialData(0, fd->GetDataSize());
		this->isBE = true;
		this->linkType = ReadMUInt32(&buff[20]);
	}
	else
	{
		return;
	}
	
	Sync::Thread::Create(ParseThread, this);
	while (!this->threadStarted)
	{
		Sync::Thread::Sleep(10);
	}
}

IO::FileAnalyse::PCapFileAnalyse::~PCapFileAnalyse()
{
	if (this->threadRunning)
	{
		this->threadToStop = true;
		while (this->threadRunning)
		{
			Sync::Thread::Sleep(10);
		}
	}

	SDEL_CLASS(this->fd);
	DEL_CLASS(this->ofstList);
	DEL_CLASS(this->sizeList);
	DEL_CLASS(this->dataMut);
	MemFree(this->packetBuff);
}

const UTF8Char *IO::FileAnalyse::PCapFileAnalyse::GetFormatName()
{
	return (const UTF8Char*)"pcap";
}

UOSInt IO::FileAnalyse::PCapFileAnalyse::GetFrameCount()
{
	return 1 + this->ofstList->GetCount();
}

Bool IO::FileAnalyse::PCapFileAnalyse::GetFrameName(UOSInt index, Text::StringBuilderUTF *sb)
{
	if (index == 0)
	{
		sb->Append((const UTF8Char*)"PCAP Header");
		return true;
	}
	UInt64 ofst;
	UInt64 size;
	UInt32 psize;
	if (index > this->ofstList->GetCount())
	{
		return false;
	}
	Sync::MutexUsage mutUsage(this->dataMut);
	ofst = this->ofstList->GetItem(index - 1);
	size = this->sizeList->GetItem(index - 1);
	mutUsage.EndUse();
	fd->GetRealData(ofst, (UOSInt)size, this->packetBuff);
	sb->AppendU64(ofst);
	sb->Append((const UTF8Char*)", psize=");
	if (this->isBE)
	{
		psize = ReadMUInt32(&this->packetBuff[12]);
	}
	else
	{
		psize = ReadUInt32(&this->packetBuff[12]);
	}
	sb->AppendU32(psize);
	sb->Append((const UTF8Char*)", ");
	if (!Net::PacketAnalyzer::PacketDataGetName(this->linkType, &this->packetBuff[16], psize, sb))
	{
		sb->Append((const UTF8Char*)"Unknown");
	}
	return true;
}

Bool IO::FileAnalyse::PCapFileAnalyse::GetFrameDetail(UOSInt index, Text::StringBuilderUTF *sb)
{
	if (index == 0)
	{
		UInt16 version_major;
		UInt16 version_minor;
		Int32  thiszone;
		UInt32 sigfigs;
		UInt32 snaplen;
		UInt32 network;
		const UTF8Char *csptr;
		sb->Append((const UTF8Char*)"PCAP Header");
		fd->GetRealData(0, 24, this->packetBuff);
		if (this->isBE)
		{
			version_major = ReadMUInt16(&this->packetBuff[4]);
			version_minor = ReadMUInt16(&this->packetBuff[6]);
			thiszone = ReadMInt32(&this->packetBuff[8]);
			sigfigs = ReadMUInt32(&this->packetBuff[12]);
			snaplen = ReadMUInt32(&this->packetBuff[16]);
			network = ReadMUInt32(&this->packetBuff[20]);
		}
		else
		{
			version_major = ReadUInt16(&this->packetBuff[4]);
			version_minor = ReadUInt16(&this->packetBuff[6]);
			thiszone = ReadInt32(&this->packetBuff[8]);
			sigfigs = ReadUInt32(&this->packetBuff[12]);
			snaplen = ReadUInt32(&this->packetBuff[16]);
			network = ReadUInt32(&this->packetBuff[20]);
		}
		sb->Append((const UTF8Char*)"\r\nVersionMajor=");
		sb->AppendU16(version_major);
		sb->Append((const UTF8Char*)"\r\nVersionMinor=");
		sb->AppendU16(version_minor);
		sb->Append((const UTF8Char*)"\r\nThisZone=");
		sb->AppendI32(thiszone);
		sb->Append((const UTF8Char*)"\r\nSigfigs=");
		sb->AppendU32(sigfigs);
		sb->Append((const UTF8Char*)"\r\nSnapLen=");
		sb->AppendU32(snaplen);
		sb->Append((const UTF8Char*)"\r\nNetwork=");
		sb->AppendU32(network);
		csptr = IO::RAWMonitor::LinkTypeGetName(network);
		if (csptr)
		{
			sb->Append((const UTF8Char*)" (");
			sb->Append(csptr);
			sb->Append((const UTF8Char*)")");
		}
		return true;
	}
	UInt64 ofst;
	UInt64 size;
	UInt32 psize;
	if (index > this->ofstList->GetCount())
	{
		return false;
	}
	Sync::MutexUsage mutUsage(this->dataMut);
	ofst = this->ofstList->GetItem(index - 1);
	size = this->sizeList->GetItem(index - 1);
	mutUsage.EndUse();
	fd->GetRealData(ofst, (UOSInt)size, this->packetBuff);
	sb->Append((const UTF8Char*)"Offset=");
	sb->AppendU64(ofst);
	sb->Append((const UTF8Char*)"\r\nTotalSize=");
	sb->AppendU64(size);
	Data::DateTime dt;
	if (this->isBE)
	{
		dt.SetUnixTimestamp(ReadMUInt32(&this->packetBuff[0]));
		dt.SetMS(ReadMUInt32(&this->packetBuff[4]) / 1000);
		psize = ReadMUInt32(&this->packetBuff[12]);
	}
	else
	{
		dt.SetUnixTimestamp(ReadUInt32(&this->packetBuff[0]));
		dt.SetMS(ReadUInt32(&this->packetBuff[4]) / 1000);
		psize = ReadUInt32(&this->packetBuff[12]);
	}
	UTF8Char sbuff[64];
	dt.ToLocalTime();
	dt.ToString(sbuff, "yyyy-MM-dd HH:mm:ss.fff");
	sb->Append((const UTF8Char*)"\r\nTime=");
	sb->Append(sbuff);
	sb->Append((const UTF8Char*)"\r\nPacketSize=");
	sb->AppendU32(psize);
	Net::PacketAnalyzer::PacketDataGetDetail(linkType, &this->packetBuff[16], psize, sb);
	return true;
}

UOSInt IO::FileAnalyse::PCapFileAnalyse::GetFrameIndex(UInt64 ofst)
{
	if (ofst < 24)
	{
		return 0;
	}
	OSInt i = 0;
	OSInt j = (OSInt)this->ofstList->GetCount() - 1;
	OSInt k;
	UInt64 packOfst;
	while (i <= j)
	{
		k = (i + j) >> 1;
		packOfst = this->ofstList->GetItem((UOSInt)k);
		if (ofst < packOfst)
		{
			j = k - 1;
		}
		else if (ofst >= packOfst + this->sizeList->GetItem((UOSInt)k))
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

IO::FileAnalyse::FrameDetail *IO::FileAnalyse::PCapFileAnalyse::GetFrameDetail(UOSInt index)
{
	Text::StringBuilderUTF8 sb;
	IO::FileAnalyse::FrameDetail *frame;
	UTF8Char sbuff[128];
	if (index == 0)
	{
		UInt16 version_major;
		UInt16 version_minor;
		Int32  thiszone;
		UInt32 sigfigs;
		UInt32 snaplen;
		UInt32 network;
		const UTF8Char *csptr;
		NEW_CLASS(frame, IO::FileAnalyse::FrameDetail(0, 24));
		frame->AddHeader((const UTF8Char*)"PCAP Header");
		fd->GetRealData(0, 24, this->packetBuff);
		if (this->isBE)
		{
			version_major = ReadMUInt16(&this->packetBuff[4]);
			version_minor = ReadMUInt16(&this->packetBuff[6]);
			thiszone = ReadMInt32(&this->packetBuff[8]);
			sigfigs = ReadMUInt32(&this->packetBuff[12]);
			snaplen = ReadMUInt32(&this->packetBuff[16]);
			network = ReadMUInt32(&this->packetBuff[20]);
		}
		else
		{
			version_major = ReadUInt16(&this->packetBuff[4]);
			version_minor = ReadUInt16(&this->packetBuff[6]);
			thiszone = ReadInt32(&this->packetBuff[8]);
			sigfigs = ReadUInt32(&this->packetBuff[12]);
			snaplen = ReadUInt32(&this->packetBuff[16]);
			network = ReadUInt32(&this->packetBuff[20]);
		}
		Text::StrUInt16(sbuff, version_major);
		frame->AddField(4, 2, (const UTF8Char*)"VersionMajor", sbuff);
		Text::StrUInt16(sbuff, version_minor);
		frame->AddField(6, 2, (const UTF8Char*)"VersionMinor", sbuff);
		Text::StrInt32(sbuff, thiszone);
		frame->AddField(8, 4, (const UTF8Char*)"ThisZone", sbuff);
		Text::StrUInt32(sbuff, sigfigs);
		frame->AddField(12, 4, (const UTF8Char*)"Sigfigs", sbuff);
		Text::StrUInt32(sbuff, snaplen);
		frame->AddField(16, 4, (const UTF8Char*)"SnapLen", sbuff);
		sb.ClearStr();
		sb.AppendU32(network);
		csptr = IO::RAWMonitor::LinkTypeGetName(network);
		if (csptr)
		{
			sb.Append((const UTF8Char*)" (");
			sb.Append(csptr);
			sb.Append((const UTF8Char*)")");
		}
		frame->AddField(20, 4, (const UTF8Char*)"Network", sb.ToString());
		return frame;
	}
	UInt64 ofst;
	UInt64 size;
	UInt32 psize;
	if (index > this->ofstList->GetCount())
	{
		return 0;
	}
	Sync::MutexUsage mutUsage(this->dataMut);
	ofst = this->ofstList->GetItem(index - 1);
	size = this->sizeList->GetItem(index - 1);
	mutUsage.EndUse();
	NEW_CLASS(frame, IO::FileAnalyse::FrameDetail(ofst, (UInt32)size));
	return frame;
/*	
	fd->GetRealData(ofst, (UOSInt)size, this->packetBuff);
	sb->Append((const UTF8Char*)"Offset=");
	sb->AppendU64(ofst);
	sb->Append((const UTF8Char*)"\r\nTotalSize=");
	sb->AppendU64(size);
	Data::DateTime dt;
	if (this->isBE)
	{
		dt.SetUnixTimestamp(ReadMUInt32(&this->packetBuff[0]));
		dt.SetMS(ReadMUInt32(&this->packetBuff[4]) / 1000);
		psize = ReadMUInt32(&this->packetBuff[12]);
	}
	else
	{
		dt.SetUnixTimestamp(ReadUInt32(&this->packetBuff[0]));
		dt.SetMS(ReadUInt32(&this->packetBuff[4]) / 1000);
		psize = ReadUInt32(&this->packetBuff[12]);
	}
	UTF8Char sbuff[64];
	dt.ToLocalTime();
	dt.ToString(sbuff, "yyyy-MM-dd HH:mm:ss.fff");
	sb->Append((const UTF8Char*)"\r\nTime=");
	sb->Append(sbuff);
	sb->Append((const UTF8Char*)"\r\nPacketSize=");
	sb->AppendU32(psize);
	Net::PacketAnalyzer::PacketDataGetDetail(linkType, &this->packetBuff[16], psize, sb);
	return true;*/
}

Bool IO::FileAnalyse::PCapFileAnalyse::IsError()
{
	return this->fd == 0;
}

Bool IO::FileAnalyse::PCapFileAnalyse::IsParsing()
{
	return this->threadRunning;
}

Bool IO::FileAnalyse::PCapFileAnalyse::TrimPadding(const UTF8Char *outputFile)
{
	return false;
}
