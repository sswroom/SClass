#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/StmData/MemoryData.h"

IO::StmData::MemoryData::MemoryData(const UInt8 *data, UOSInt dataLength)
{
	this->data = data;
	this->dataLength = dataLength;
}

IO::StmData::MemoryData::~MemoryData()
{
}

UOSInt IO::StmData::MemoryData::GetRealData(UInt64 offset, UOSInt length, UInt8 *buffer)
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

Text::String *IO::StmData::MemoryData::GetFullName()
{
	return Text::String::NewEmpty();
}

Text::CString IO::StmData::MemoryData::GetShortName()
{
	return CSTR("Memory");
}

UInt64 IO::StmData::MemoryData::GetDataSize()
{
	return this->dataLength;
}

const UInt8 *IO::StmData::MemoryData::GetPointer()
{
	return this->data;
}

IO::IStreamData *IO::StmData::MemoryData::GetPartialData(UInt64 offset, UInt64 length)
{
	IO::StmData::MemoryData *data;
	if (offset >= this->dataLength)
	{
		NEW_CLASS(data, IO::StmData::MemoryData(this->data, 0));
		return data;
	}
	if (offset + length > this->dataLength)
	{
		length = this->dataLength - offset;
	}
	NEW_CLASS(data, IO::StmData::MemoryData(&this->data[offset], (UOSInt)length));
	return data;
}

Bool IO::StmData::MemoryData::IsFullFile()
{
	return false;
}

Bool IO::StmData::MemoryData::IsLoading()
{
	return false;
}

UOSInt IO::StmData::MemoryData::GetSeekCount()
{
	return 0;
}
