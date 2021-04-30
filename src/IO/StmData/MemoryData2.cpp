#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/StmData/MemoryData2.h"
#include "Sync/Interlocked.h"

IO::StmData::MemoryData2::MemoryData2(MemoryStats *stat, const UInt8 *data, UOSInt dataLength)
{
	this->stat = stat;
	Sync::Interlocked::Increment(&stat->useCnt);
	this->data = data;
	this->dataLength = dataLength;
}

IO::StmData::MemoryData2::MemoryData2(const UInt8 *data, UOSInt dataLength)
{
	this->stat = MemAlloc(MemoryStats, 1);
	this->stat->data = MemAlloc(UInt8, dataLength);
	this->stat->dataLength = dataLength;
	MemCopyNO(this->stat->data, data, dataLength);
	this->stat->useCnt = 1;
	this->data = this->stat->data;
	this->dataLength = dataLength;
}

IO::StmData::MemoryData2::~MemoryData2()
{
	if (Sync::Interlocked::Decrement(&this->stat->useCnt) == 0)
	{
		MemFree(this->stat->data);
		MemFree(this->stat);
	}
}

UOSInt IO::StmData::MemoryData2::GetRealData(UInt64 offset, UOSInt length, UInt8 *buffer)
{
	if (offset >= this->dataLength)
	{
		return 0;
	}
	if (offset + length > this->dataLength)
	{
		length = (UOSInt)(this->dataLength - offset);
	}
	if (length > 0)
	{
		MemCopyNO(buffer, &this->data[offset], length);
	}
	return length;
}

const UTF8Char *IO::StmData::MemoryData2::GetFullName()
{
	return (const UTF8Char*)"Memory2";
}

const UTF8Char *IO::StmData::MemoryData2::GetShortName()
{
	return (const UTF8Char*)"Memory2";
}

UInt64 IO::StmData::MemoryData2::GetDataSize()
{
	return this->dataLength;
}

const UInt8 *IO::StmData::MemoryData2::GetPointer()
{
	return this->data;
}

IO::IStreamData *IO::StmData::MemoryData2::GetPartialData(UInt64 offset, UInt64 length)
{
	IO::StmData::MemoryData2 *data;
	if (offset >= this->dataLength)
	{
		NEW_CLASS(data, IO::StmData::MemoryData2(this->stat, this->data, 0));
		return data;
	}
	if (offset + length > this->dataLength)
	{
		length = this->dataLength - offset;
	}
	NEW_CLASS(data, IO::StmData::MemoryData2(this->stat, &this->data[offset], (UOSInt)length));
	return data;
}

Bool IO::StmData::MemoryData2::IsFullFile()
{
	return false;
}

Bool IO::StmData::MemoryData2::IsLoading()
{
	return false;
}

UOSInt IO::StmData::MemoryData2::GetSeekCount()
{
	return 0;
}
