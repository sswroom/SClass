#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/Path.h"
#include "IO/StmData/MemoryDataRef.h"

IO::StmData::MemoryDataRef::MemoryDataRef(UnsafeArray<const UInt8> data, UOSInt dataLength) : data(data, dataLength)
{
	this->name = 0;
}

IO::StmData::MemoryDataRef::MemoryDataRef(const Data::ByteArrayR &data) : data(data)
{
	this->name = 0;
}

IO::StmData::MemoryDataRef::~MemoryDataRef()
{
	OPTSTR_DEL(this->name);
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

NN<Text::String> IO::StmData::MemoryDataRef::GetFullName()
{
	return Text::String::OrEmpty(this->name);
}

Text::CString IO::StmData::MemoryDataRef::GetShortName()
{
	NN<Text::String> name;
	if (this->name.SetTo(name))
	{
		UOSInt i = name->LastIndexOf(IO::Path::PATH_SEPERATOR);
		return name->ToCString().Substring(i + 1);
	}
	return CSTR("Memory");
}

void IO::StmData::MemoryDataRef::SetFullName(Text::CStringNN fullName)
{
	OPTSTR_DEL(this->name);
	this->name = Text::String::New(fullName);
}

UInt64 IO::StmData::MemoryDataRef::GetDataSize()
{
	return this->data.GetSize();
}

UnsafeArrayOpt<const UInt8> IO::StmData::MemoryDataRef::GetPointer()
{
	return this->data.Arr();
}

NN<IO::StreamData> IO::StmData::MemoryDataRef::GetPartialData(UInt64 offset, UInt64 length)
{
	NN<Text::String> s;
	NN<IO::StmData::MemoryDataRef> data;
	if (offset >= this->data.GetSize())
	{
		NEW_CLASSNN(data, IO::StmData::MemoryDataRef(this->data.Arr(), 0));
		if (this->name.SetTo(s))
			data->SetName(s);
		return data;
	}
	if (offset + length > this->data.GetSize())
	{
		length = this->data.GetSize() - offset;
	}
	NEW_CLASSNN(data, IO::StmData::MemoryDataRef(this->data.SubArray(offset, (UOSInt)length)));
	if (this->name.SetTo(s))
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
	OPTSTR_DEL(this->name);
	this->name = Text::String::New(name);
}

void IO::StmData::MemoryDataRef::SetName(NN<Text::String> name)
{
	OPTSTR_DEL(this->name);
	this->name = name->Clone();
}
