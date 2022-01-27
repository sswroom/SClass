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

Text::CString IO::FileAnalyse::PCapFileAnalyse::GetFormatName()
{
	return {UTF8STRC("pcap")};
}

UOSInt IO::FileAnalyse::PCapFileAnalyse::GetFrameCount()
{
	return 1 + this->ofstList->GetCount();
}

Bool IO::FileAnalyse::PCapFileAnalyse::GetFrameName(UOSInt index, Text::StringBuilderUTF8 *sb)
{
	if (index == 0)
	{
		sb->AppendC(UTF8STRC("PCAP Header"));
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
	sb->AppendC(UTF8STRC(", psize="));
	if (this->isBE)
	{
		psize = ReadMUInt32(&this->packetBuff[12]);
	}
	else
	{
		psize = ReadUInt32(&this->packetBuff[12]);
	}
	sb->AppendU32(psize);
	sb->AppendC(UTF8STRC(", "));
	if (!Net::PacketAnalyzer::PacketDataGetName(this->linkType, &this->packetBuff[16], psize, sb))
	{
		sb->AppendC(UTF8STRC("Unknown"));
	}
	return true;
}

Bool IO::FileAnalyse::PCapFileAnalyse::GetFrameDetail(UOSInt index, Text::StringBuilderUTF8 *sb)
{
	if (index == 0)
	{
		UInt16 version_major;
		UInt16 version_minor;
		Int32  thiszone;
		UInt32 sigfigs;
		UInt32 snaplen;
		UInt32 network;
		Text::CString cstr;
		sb->AppendC(UTF8STRC("PCAP Header"));
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
		sb->AppendC(UTF8STRC("\r\nVersionMajor="));
		sb->AppendU16(version_major);
		sb->AppendC(UTF8STRC("\r\nVersionMinor="));
		sb->AppendU16(version_minor);
		sb->AppendC(UTF8STRC("\r\nThisZone="));
		sb->AppendI32(thiszone);
		sb->AppendC(UTF8STRC("\r\nSigfigs="));
		sb->AppendU32(sigfigs);
		sb->AppendC(UTF8STRC("\r\nSnapLen="));
		sb->AppendU32(snaplen);
		sb->AppendC(UTF8STRC("\r\nNetwork="));
		sb->AppendU32(network);
		cstr = IO::RAWMonitor::LinkTypeGetName(network);
		if (cstr.v)
		{
			sb->AppendC(UTF8STRC(" ("));
			sb->Append(cstr);
			sb->AppendC(UTF8STRC(")"));
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
	sb->AppendC(UTF8STRC("Offset="));
	sb->AppendU64(ofst);
	sb->AppendC(UTF8STRC("\r\nTotalSize="));
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
	UTF8Char *sptr;
	dt.ToLocalTime();
	sptr = dt.ToString(sbuff, "yyyy-MM-dd HH:mm:ss.fff");
	sb->AppendC(UTF8STRC("\r\nTime="));
	sb->AppendC(sbuff, (UOSInt)(sptr - sbuff));
	sb->AppendC(UTF8STRC("\r\nPacketSize="));
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
	UTF8Char *sptr;
	if (index == 0)
	{
		UInt16 version_major;
		UInt16 version_minor;
		Int32  thiszone;
		UInt32 sigfigs;
		UInt32 snaplen;
		UInt32 network;
		NEW_CLASS(frame, IO::FileAnalyse::FrameDetail(0, 24));
		frame->AddHeader(CSTR("PCAP Header"));
		fd->GetRealData(0, 24, this->packetBuff);
		if (this->isBE)
		{
			frame->AddField(0, 4, CSTR("Endian"), CSTR("Big Endian"));
			version_major = ReadMUInt16(&this->packetBuff[4]);
			version_minor = ReadMUInt16(&this->packetBuff[6]);
			thiszone = ReadMInt32(&this->packetBuff[8]);
			sigfigs = ReadMUInt32(&this->packetBuff[12]);
			snaplen = ReadMUInt32(&this->packetBuff[16]);
			network = ReadMUInt32(&this->packetBuff[20]);
		}
		else
		{
			frame->AddField(0, 4, CSTR("Endian"), CSTR("Little Endian"));
			version_major = ReadUInt16(&this->packetBuff[4]);
			version_minor = ReadUInt16(&this->packetBuff[6]);
			thiszone = ReadInt32(&this->packetBuff[8]);
			sigfigs = ReadUInt32(&this->packetBuff[12]);
			snaplen = ReadUInt32(&this->packetBuff[16]);
			network = ReadUInt32(&this->packetBuff[20]);
		}
		frame->AddUInt(4, 2, CSTR("VersionMajor"), version_major);
		frame->AddUInt(6, 2, CSTR("VersionMinor"), version_minor);
		frame->AddInt(8, 4, CSTR("ThisZone"), thiszone);
		frame->AddUInt(12, 4, CSTR("Sigfigs"), sigfigs);
		frame->AddUInt(16, 4, CSTR("SnapLen"), snaplen);
		frame->AddUIntName(20, 4, CSTR("Network"), network, IO::RAWMonitor::LinkTypeGetName(network));
		return frame;
	}
	UInt64 ofst;
	UInt64 size;
	UInt32 storeSize;
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
	fd->GetRealData(ofst, (UOSInt)size, this->packetBuff);
	sptr = Text::StrUInt64(Text::StrConcatC(sbuff, UTF8STRC("TotalSize=")), size);
	frame->AddHeader({sbuff, (UOSInt)(sptr - sbuff)});
	Data::DateTime dt;
	if (this->isBE)
	{
		dt.SetUnixTimestamp(ReadMUInt32(&this->packetBuff[0]));
		dt.SetMS(ReadMUInt32(&this->packetBuff[4]) / 1000);
		storeSize = ReadMUInt32(&this->packetBuff[8]);
		psize = ReadMUInt32(&this->packetBuff[12]);
	}
	else
	{
		dt.SetUnixTimestamp(ReadUInt32(&this->packetBuff[0]));
		dt.SetMS(ReadUInt32(&this->packetBuff[4]) / 1000);
		storeSize = ReadUInt32(&this->packetBuff[8]);
		psize = ReadUInt32(&this->packetBuff[12]);
	}
	dt.ToLocalTime();
	sptr = dt.ToString(sbuff, "yyyy-MM-dd HH:mm:ss.fff");
	frame->AddField(0, 8, CSTR("Time"), {sbuff, (UOSInt)(sptr - sbuff)});
	frame->AddUInt(8, 4, CSTR("StorageSize"), storeSize);
	frame->AddUInt(12, 4, CSTR("PacketSize"), psize);
	Net::PacketAnalyzer::PacketDataGetDetail(linkType, &this->packetBuff[16], psize, 16, frame);
	return frame;
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
