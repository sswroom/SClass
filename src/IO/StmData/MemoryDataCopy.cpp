#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/StmData/MemoryDataCopy.h"
#include "Sync/Interlocked.h"

IO::StmData::MemoryDataCopy::MemoryDataCopy(MemoryStats *stat, const UInt8 *data, UOSInt dataLength)
{
	this->stat = stat;
	Sync::Interlocked::Increment(&stat->useCnt);
	this->data = data;
	this->dataLength = dataLength;
}

IO::StmData::MemoryDataCopy::MemoryDataCopy(const UInt8 *data, UOSInt dataLength)
{
	this->stat = MemAlloc(MemoryStats, 1);
	this->stat->data = MemAlloc(UInt8, dataLength);
	this->stat->dataLength = dataLength;
	MemCopyNO(this->stat->data, data, dataLength);
	this->stat->useCnt = 1;
	this->data = this->stat->data;
	this->dataLength = dataLength;
}

IO::StmData::MemoryDataCopy::~MemoryDataCopy()
{
	if (Sync::Interlocked::Decrement(&this->stat->useCnt) == 0)
	{
		MemFree(this->stat->data);
		MemFree(this->stat);
	}
}

UOSInt IO::StmData::MemoryDataCopy::GetRealData(UInt64 offset, UOSInt length, UInt8 *buffer)
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

Text::String *IO::StmData::MemoryDataCopy::GetFullName()
{
	return Text::String::NewEmpty();
}

Text::CString IO::StmData::MemoryDataCopy::GetShortName()
{
	return CSTR("Memory2");
}

UInt64 IO::StmData::MemoryDataCopy::GetDataSize()
{
	return this->dataLength;
}

const UInt8 *IO::StmData::MemoryDataCopy::GetPointer()
{
	return this->data;
}

IO::IStreamData *IO::StmData::MemoryDataCopy::GetPartialData(UInt64 offset, UInt64 length)
{
	IO::StmData::MemoryDataCopy *data;
	if (offset >= this->dataLength)
	{
		NEW_CLASS(data, IO::StmData::MemoryDataCopy(this->stat, this->data, 0));
		return data;
	}
	if (offset + length > this->dataLength)
	{
		length = this->dataLength - offset;
	}
	NEW_CLASS(data, IO::StmData::MemoryDataCopy(this->stat, &this->data[offset], (UOSInt)length));
	return data;
}

Bool IO::StmData::MemoryDataCopy::IsFullFile()
{
	return false;
}

Bool IO::StmData::MemoryDataCopy::IsLoading()
{
	return false;
}

UOSInt IO::StmData::MemoryDataCopy::GetSeekCount()
{
	return 0;
}