#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/StmData/MemoryDataCopy.h"
#include "Sync/Interlocked.h"

IO::StmData::MemoryDataCopy::MemoryDataCopy(NN<MemoryStats> stat, UnsafeArray<const UInt8> data, UOSInt dataLength)
{
	this->stat = stat;
	Sync::Interlocked::IncrementI32(stat->useCnt);
	this->data = data;
	this->dataLength = dataLength;
}

IO::StmData::MemoryDataCopy::MemoryDataCopy(UnsafeArray<const UInt8> data, UOSInt dataLength)
{
	this->stat = MemAllocNN(MemoryStats);
	this->stat->data = MemAllocArr(UInt8, dataLength);
	this->stat->dataLength = dataLength;
	MemCopyNO(this->stat->data.Ptr(), data.Ptr(), dataLength);
	this->stat->useCnt = 1;
	this->stat->fullName = 0;
	this->data = this->stat->data;
	this->dataLength = dataLength;
}

IO::StmData::MemoryDataCopy::MemoryDataCopy(const Data::ByteArrayR &data)
{
	this->stat = MemAllocNN(MemoryStats);
	this->stat->data = MemAllocArr(UInt8, data.GetSize());
	this->stat->dataLength = data.GetSize();
	MemCopyNO(this->stat->data.Ptr(), data.Arr().Ptr(), data.GetSize());
	this->stat->useCnt = 1;
	this->stat->fullName = 0;
	this->data = this->stat->data;
	this->dataLength = data.GetSize();
}


IO::StmData::MemoryDataCopy::~MemoryDataCopy()
{
	if (Sync::Interlocked::DecrementI32(this->stat->useCnt) == 0)
	{
		MemFreeArr(this->stat->data);
		OPTSTR_DEL(this->stat->fullName);
		MemFreeNN(this->stat);
	}
}

UOSInt IO::StmData::MemoryDataCopy::GetRealData(UInt64 offset, UOSInt length, Data::ByteArray buffer)
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
		buffer.CopyFrom(Data::ByteArrayR(&this->data[offset], length));
	}
	return length;
}

NN<Text::String> IO::StmData::MemoryDataCopy::GetFullName() const
{
	NN<Text::String> retStr;
	if (this->stat->fullName.SetTo(retStr))
		return retStr;
	return Text::String::NewEmpty();
}

Text::CString IO::StmData::MemoryDataCopy::GetShortName() const
{
	NN<Text::String> fullName;
	if (this->stat->fullName.SetTo(fullName))
	{
		UOSInt i = fullName->LastIndexOf('/');
		if (i != INVALID_INDEX)
		{
			return fullName->ToCString().Substring(i + 1);
		}
		return fullName->ToCString();
	}
	return CSTR("MemoryCopy");
}

void IO::StmData::MemoryDataCopy::SetFullName(Text::CStringNN fullName)
{
	OPTSTR_DEL(this->stat->fullName);
	this->stat->fullName = Text::String::New(fullName);
}

UInt64 IO::StmData::MemoryDataCopy::GetDataSize() const
{
	return this->dataLength;
}

UnsafeArrayOpt<const UInt8> IO::StmData::MemoryDataCopy::GetPointer() const
{
	return this->data;
}

NN<IO::StreamData> IO::StmData::MemoryDataCopy::GetPartialData(UInt64 offset, UInt64 length)
{
	NN<IO::StmData::MemoryDataCopy> data;
	if (offset >= this->dataLength)
	{
		NEW_CLASSNN(data, IO::StmData::MemoryDataCopy(this->stat, this->data, 0));
		return data;
	}
	if (offset + length > this->dataLength)
	{
		length = this->dataLength - offset;
	}
	NEW_CLASSNN(data, IO::StmData::MemoryDataCopy(this->stat, &this->data[offset], (UOSInt)length));
	return data;
}

Bool IO::StmData::MemoryDataCopy::IsFullFile() const
{
	return false;
}

Bool IO::StmData::MemoryDataCopy::IsLoading() const
{
	return false;
}

UOSInt IO::StmData::MemoryDataCopy::GetSeekCount() const
{
	return 0;
}
