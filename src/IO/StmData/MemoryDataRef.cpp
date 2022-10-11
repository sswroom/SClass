#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/StmData/MemoryDataRef.h"

IO::StmData::MemoryDataRef::MemoryDataRef(const UInt8 *data, UOSInt dataLength)
{
	this->data = data;
	this->dataLength = dataLength;
}

IO::StmData::MemoryDataRef::~MemoryDataRef()
{
}

UOSInt IO::StmData::MemoryDataRef::GetRealData(UInt64 offset, UOSInt length, UInt8 *buffer)
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

Text::String *IO::StmData::MemoryDataRef::GetFullName()
{
	return Text::String::NewEmpty();
}

Text::CString IO::StmData::MemoryDataRef::GetShortName()
{
	return CSTR("Memory");
}

UInt64 IO::StmData::MemoryDataRef::GetDataSize()
{
	return this->dataLength;
}

const UInt8 *IO::StmData::MemoryDataRef::GetPointer()
{
	return this->data;
}

IO::IStreamData *IO::StmData::MemoryDataRef::GetPartialData(UInt64 offset, UInt64 length)
{
	IO::StmData::MemoryDataRef *data;
	if (offset >= this->dataLength)
	{
		NEW_CLASS(data, IO::StmData::MemoryDataRef(this->data, 0));
		return data;
	}
	if (offset + length > this->dataLength)
	{
		length = this->dataLength - offset;
	}
	NEW_CLASS(data, IO::StmData::MemoryDataRef(&this->data[offset], (UOSInt)length));
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
