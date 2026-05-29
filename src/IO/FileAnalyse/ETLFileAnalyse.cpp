#include "Stdafx.h"
#include "Core/ByteTool_C.h"
#include "Data/ArrayListNative.hpp"
#include "IO/RAWMonitor.h"
#include "IO/FileAnalyse/ETLFileAnalyse.h"
#include "Net/PacketAnalyzer.h"
#include "Text/StringBuilderUTF8.h"

#define ETL_FILE_SIGNATURE 0x6C662074
#define ETL_BUFFER_HEADER_SIZE 64

/**
 * 0x0 Header size
LogFileName=C:\Progs\PktMon.etl
LoggerName=(null)
CurrentTime=0
BuffersRead=0
LogFileMode=67174401

 * 0x68 LogfileHeader.BufferSize
 * 0x6C LogfileHeader.Version
 * 0x70 LogfileHeader.ProviderVersion
 * 0x74 LogfileHeader.NumberOfProcessors
 * 0x78 LogfileHeader.EndTime
 * 0x80 LogfileHeader.TimerResolution
 * 0x84 LogfileHeader.MaximumFileSize
 * 0x88 LogfileHeader.LogFileMode
 * 0x8C LogfileHeader.BuffersWritten
 * 0x90 LogfileHeader.StartBuffers
 * 0x94 LogfileHeader.PointerSize
 * 0x98 LogfileHeader.EventsLost
 * 0x9C LogfileHeader.CpuSpeedInMHz
LoggerName=(null)
LogFileName=(null)
 * 0xB0 LogfileHeader.TimeZone.Bias
 * 0xB4 LogfileHeader.TimeZone.StandardName (WCHAR[32])
 * 0xF4 LogfileHeader.TimeZone.StandardDate (SYSTEMTIME)
 * 0x104 LogfileHeader.TimeZone.StandardBias
 * 0x108 LogfileHeader.TimeZone.DaylightName (WCHAR[32])
 * 0x148 LogfileHeader.TimeZone.DaylightDate (SYSTEMTIME)
 * 0x158 LogfileHeader.TimeZone.DaylightBias
 * 0x15C ?
 * 0x160 LogfileHeader.BootTime
 * 0x168 LogfileHeader.PerfFreq
 * 0x170 LogfileHeader.StartTime
 * 0x178 LogfileHeader.ReservedFlags
 * 0x17C LogfileHeader.BuffersLost
 * 
 */

void __stdcall IO::FileAnalyse::ETLFileAnalyse::ParseThread(NN<Sync::Thread> thread)
{
	NN<IO::FileAnalyse::ETLFileAnalyse> me = thread->GetUserObj().GetNN<IO::FileAnalyse::ETLFileAnalyse>();
	NN<IO::StreamData> fd;
	UInt64 ofst;
	UInt64 dataSize;
	UInt32 blockSize;

	if (!me->fd.SetTo(fd))
	{
		return;
	}

	dataSize = fd->GetDataSize();
	ofst = 0;

	while (ofst < dataSize - 4 && !thread->IsStopping())
	{
		UInt8 header[32];
		if (fd->GetRealData(ofst, 32, BYTEARR(header)) != 32)
			break;

		blockSize = ReadUInt32(&header[0]);
		
		if (blockSize < 32 || ofst + blockSize > dataSize)
			break;

		UInt16 recordId = ReadUInt16(&header[4]);
		UInt16 level = header[8];
		UInt32 processorId = ReadUInt32(&header[12]);
		UInt64 timestamp = ReadUInt64(&header[16]);

		NN<IO::FileAnalyse::ETLFileAnalyse::RecordInfo> record = MemAllocNN(IO::FileAnalyse::ETLFileAnalyse::RecordInfo);
		record->ofst = ofst;
		record->blockSize = blockSize;
		record->recordId = recordId;
		record->level = level;
		record->processorId = processorId;
		record->timestamp = timestamp;
		me->recordList.Add(record);
		
		ofst += blockSize;
	}
}

IO::FileAnalyse::ETLFileAnalyse::ETLFileAnalyse(NN<IO::StreamData> fd) : packetBuff(65536), thread(ParseThread, this, CSTR("ETLFileAnaly"))
{
	UInt8 buff[16];
	this->fd = nullptr;

	if (fd->GetRealData(0, 16, BYTEARR(buff)) != 16)
	{
		return;
	}

	UInt32 signature = ReadUInt32(buff);
	
	if (signature == ETL_FILE_SIGNATURE)
	{
		this->fd = fd->GetPartialData(0, fd->GetDataSize()).Ptr();
	}
	else
	{
		return;
	}

	this->thread.Start();
}

IO::FileAnalyse::ETLFileAnalyse::~ETLFileAnalyse()
{
	this->thread.Stop();
	this->fd.Delete();
	this->recordList.MemFreeAll();
}

Text::CStringNN IO::FileAnalyse::ETLFileAnalyse::GetFormatName()
{
	return CSTR("etl");
}

UIntOS IO::FileAnalyse::ETLFileAnalyse::GetFrameCount()
{
	return this->recordList.GetCount();
}

Bool IO::FileAnalyse::ETLFileAnalyse::GetFrameName(UIntOS index, NN<Text::StringBuilderUTF8> sb)
{
	NN<IO::FileAnalyse::ETLFileAnalyse::RecordInfo> record;

	if (index >= this->recordList.GetCount())
	{
		return false;
	}

	record = this->recordList.GetItemNoCheck(index);
	UTF8Char sbuff[64];
	UnsafeArray<UTF8Char> sptr = Data::Timestamp::FromSYSTEMTIME(&record->timestamp, Data::DateTimeUtil::GetLocalTzQhr()).ToString(sbuff, "yyyy-MM-dd HH:mm:ss.fff");
	
	sb->AppendC(sbuff, (UIntOS)(sptr - sbuff));
	sb->AppendC(UTF8STRC(" [CPU:"));
	sb->AppendU32(record->processorId);
	sb->AppendC(UTF8STRC(", Level:"));
	sb->AppendU16(record->level);
	sb->AppendC(UTF8STRC(", ID:"));
	sb->AppendU16(record->recordId);
	sb->AppendC(UTF8STRC("]"));

	return true;
}

Bool IO::FileAnalyse::ETLFileAnalyse::GetFrameDetail(UIntOS index, NN<Text::StringBuilderUTF8> sb)
{
	NN<IO::FileAnalyse::ETLFileAnalyse::RecordInfo> record;

	if (index >= this->recordList.GetCount())
	{
		return false;
	}

	record = this->recordList.GetItemNoCheck(index);
	
	sb->AppendC(UTF8STRC("Offset="));
	sb->AppendU64(record->ofst);
	sb->AppendC(UTF8STRC("\r\nBlockSize="));
	sb->AppendU32(record->blockSize);
	sb->AppendC(UTF8STRC("\r\nRecordID="));
	sb->AppendU16(record->recordId);
	sb->AppendC(UTF8STRC("\r\nLevel="));
	sb->AppendU16(record->level);
	sb->AppendC(UTF8STRC("\r\nProcessorID="));
	sb->AppendU32(record->processorId);
	sb->AppendC(UTF8STRC("\r\nTimestamp="));
	sb->AppendU64(record->timestamp);

	return true;
}

UIntOS IO::FileAnalyse::ETLFileAnalyse::GetFrameIndex(UInt64 ofst)
{
	IntOS i = 0;
	IntOS j = (IntOS)this->recordList.GetCount() - 1;
	IntOS k;
	NN<RecordInfo> record;
	
	while (i <= j)
	{
		k = (i + j) >> 1;
		record = this->recordList.GetItemNoCheck((UIntOS)k);
		
		if (ofst < record->ofst)
		{
			j = k - 1;
		}
		else if (ofst >= record->ofst + record->blockSize)
		{
			i = k + 1;
		}
		else
		{
			return (UIntOS)k;
		}
	}

	return INVALID_INDEX;
}

Optional<IO::FileAnalyse::FrameDetail> IO::FileAnalyse::ETLFileAnalyse::GetFrameDetail(UIntOS index)
{
	NN<IO::FileAnalyse::FrameDetail> frame;

	if (index >= this->recordList.GetCount())
	{
		return nullptr;
	}

	NN<IO::FileAnalyse::ETLFileAnalyse::RecordInfo> record = this->recordList.GetItemNoCheck(index);
	
	NEW_CLASSNN(frame, IO::FileAnalyse::FrameDetail(record->ofst, record->blockSize));
	
	Data::ByteBuffer buffer(record->blockSize);
	NN<IO::StreamData> fd;
	UTF8Char sbuff[64];
	UnsafeArray<UTF8Char> sptr;
	if (!this->fd.SetTo(fd))
	{
		return nullptr;
	}
	
	fd->GetRealData(record->ofst, record->blockSize, buffer);

	frame->AddUInt(0, 4, CSTR("BlockSize"), ReadUInt32(&buffer[0]));
	frame->AddUInt(4, 2, CSTR("RecordID"), ReadUInt16(&buffer[4]));
	frame->AddUInt(8, 1, CSTR("Level"), buffer[8]);
	frame->AddUInt(12, 4, CSTR("ProcessorID"), ReadUInt32(&buffer[12]));
	sptr = Data::Timestamp::FromSYSTEMTIME(&buffer[16], Data::DateTimeUtil::GetLocalTzQhr()).ToString(sbuff, "yyyy-MM-dd HH:mm:ss.fff");
	frame->AddUInt64Name(16, 8, CSTR("Timestamp"), ReadUInt64(&buffer[16]), CSTRP(sbuff, sptr));

	return frame;
}

Bool IO::FileAnalyse::ETLFileAnalyse::IsError()
{
	return this->fd.IsNull();
}

Bool IO::FileAnalyse::ETLFileAnalyse::IsParsing()
{
	return this->thread.IsRunning();
}

Bool IO::FileAnalyse::ETLFileAnalyse::TrimPadding(Text::CStringNN outputFile)
{
	return false;
}
