#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/StmData/MemoryDataRef.h"

IO::StmData::MemoryDataRef::MemoryDataRef(const UInt8 *data, UOSInt dataLength) : data(data, dataLength)
{
}

IO::StmData::MemoryDataRef::MemoryDataRef(const Data::ByteArrayR &data) : data(data)
{
}

IO::StmData::MemoryDataRef::~MemoryDataRef()
{
}

UOSInt IO::StmData::MemoryDataRef::GetRealData(UInt64 offset, UOSInt length, Data::ByteArray buffer)
{
	if (offset >= this->data.GetSize())
	{
		return 0;
	}
	if (offset + length > this->data.GetSize())
	{
		length = (UOSInt)(this->data.GetSize() - offset);
	}
	if (length > 0)
	{
		buffer.CopyFrom(this->data.SubArray(offset, length));
	}
	return length;
}

NotNullPtr<Text::String> IO::StmData::MemoryDataRef::GetFullName()
{
	return Text::String::NewEmpty();
}

Text::CString IO::StmData::MemoryDataRef::GetShortName()
{
	return CSTR("Memory");
}

UInt64 IO::StmData::MemoryDataRef::GetDataSize()
{
	return this->data.GetSize();
}

const UInt8 *IO::StmData::MemoryDataRef::GetPointer()
{
	return this->data.Ptr();
}

IO::StreamData *IO::StmData::MemoryDataRef::GetPartialData(UInt64 offset, UInt64 length)
{
	IO::StmData::MemoryDataRef *data;
	if (offset >= this->data.GetSize())
	{
		NEW_CLASS(data, IO::StmData::MemoryDataRef(this->data.Ptr(), 0));
		return data;
	}
	if (offset + length > this->data.GetSize())
	{
		length = this->data.GetSize() - offset;
	}
	NEW_CLASS(data, IO::StmData::MemoryDataRef(this->data.SubArray(offset, (UOSInt)length)));
	return data;
}

Bool IO::StmData::MemoryDataRef::IsFullFile()
{
	return false;
}

Bool IO::StmData::MemoryDataRef::IsLoading()
{
	return false;
}

UOSInt IO::StmData::MemoryDataRef::GetSeekCount()
{
	return 0;
}
