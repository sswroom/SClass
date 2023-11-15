#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/StmData/MemoryDataRef.h"

IO::StmData::MemoryDataRef::MemoryDataRef(const UInt8 *data, UOSInt dataLength) : data(data, dataLength)
{
	this->name = 0;
}

IO::StmData::MemoryDataRef::MemoryDataRef(const Data::ByteArrayR &data) : data(data)
{
	this->name = 0;
}

IO::StmData::MemoryDataRef::~MemoryDataRef()
{
	SDEL_STRING(this->name);
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
	return Text::String::OrEmpty(this->name);
}

Text::CString IO::StmData::MemoryDataRef::GetShortName()
{
	return CSTR("Memory");
}

void IO::StmData::MemoryDataRef::SetFullName(Text::CString fullName)
{
	SDEL_STRING(this->name);
	this->name = Text::String::New(fullName).Ptr();
}

UInt64 IO::StmData::MemoryDataRef::GetDataSize()
{
	return this->data.GetSize();
}

const UInt8 *IO::StmData::MemoryDataRef::GetPointer()
{
	return this->data.Ptr();
}

NotNullPtr<IO::StreamData> IO::StmData::MemoryDataRef::GetPartialData(UInt64 offset, UInt64 length)
{
	NotNullPtr<Text::String> s;
	NotNullPtr<IO::StmData::MemoryDataRef> data;
	if (offset >= this->data.GetSize())
	{
		NEW_CLASSNN(data, IO::StmData::MemoryDataRef(this->data.Ptr(), 0));
		if (s.Set(this->name))
			data->SetName(s);
		return data;
	}
	if (offset + length > this->data.GetSize())
	{
		length = this->data.GetSize() - offset;
	}
	NEW_CLASSNN(data, IO::StmData::MemoryDataRef(this->data.SubArray(offset, (UOSInt)length)));
	if (s.Set(this->name))
		data->SetName(s);
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

void IO::StmData::MemoryDataRef::SetName(Text::CStringNN name)
{
	SDEL_STRING(this->name);
	this->name = Text::String::New(name).Ptr();
}

void IO::StmData::MemoryDataRef::SetName(NotNullPtr<Text::String> name)
{
	SDEL_STRING(this->name);
	this->name = name->Clone().Ptr();
}
