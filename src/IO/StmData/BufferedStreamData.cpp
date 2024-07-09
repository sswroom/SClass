#include "Stdafx.h"
#include "IO/StmData/BufferedStreamData.h"

#define BUFFSIZE 65536

IO::StmData::BufferedStreamData::BufferedStreamData(NN<IO::StreamData> stmData) : buff(BUFFSIZE)
{
	this->stmData = stmData;
	this->buffLength = 0;
	this->buffOfst = 0;
	this->dataLength = this->stmData->GetDataSize();
}

IO::StmData::BufferedStreamData::~BufferedStreamData()
{
	this->stmData.Delete();
}

UOSInt IO::StmData::BufferedStreamData::GetRealData(UInt64 offset, UOSInt length, Data::ByteArray buffer)
{
	if (offset >= this->dataLength)
	{
		return 0;
	}
	UInt64 endOfst = offset + length;
	UInt64 buffEndOfst = this->buffOfst + this->buffLength;
	if (endOfst > this->dataLength)
	{
		length = (UOSInt)(this->dataLength - offset);
		endOfst = this->dataLength;
	}
	if (this->buffOfst <= offset && buffEndOfst >= endOfst)
	{
		buffer.CopyFrom(Data::ByteArrayR(&this->buff[(UOSInt)(offset - this->buffOfst)], length));
		return length;
	}
	else if (length >= (BUFFSIZE / 2))
	{
		return this->stmData->GetRealData(offset, length, buffer);
	}
	
	if (this->buffOfst > offset || endOfst >= this->buffOfst)
	{
		this->buffOfst = offset;
		this->buffLength = this->stmData->GetRealData(offset, BUFFSIZE, this->buff);
		if (length > this->buffLength)
		{
			length = this->buffLength;
		}
		buffer.CopyFrom(this->buff.WithSize(length));
		return length;
	}
	UOSInt ret = (UOSInt)(buffEndOfst - offset);
	buffer.CopyFrom(this->buff.SubArray((UOSInt)(offset - this->buffOfst), ret));
	buffer += ret;
	length -= ret;
	this->buffOfst = buffEndOfst;
	this->buffLength = this->stmData->GetRealData(this->buffOfst, BUFFSIZE, this->buff);
	if (length > this->buffLength)
	{
		length = this->buffLength;
	}
	buffer.CopyFrom(this->buff.WithSize(length));
	return length + ret;
}

NN<Text::String> IO::StmData::BufferedStreamData::GetFullName()
{
	return this->stmData->GetFullName();
}

Text::CString IO::StmData::BufferedStreamData::GetShortName()
{
	return this->stmData->GetShortName();
}

void IO::StmData::BufferedStreamData::SetFullName(Text::CStringNN fullName)
{
	this->stmData->SetFullName(fullName);
}

UInt64 IO::StmData::BufferedStreamData::GetDataSize()
{
	return this->stmData->GetDataSize();
}

UnsafeArrayOpt<const UInt8> IO::StmData::BufferedStreamData::GetPointer()
{
	return this->stmData->GetPointer();
}

NN<IO::StreamData> IO::StmData::BufferedStreamData::GetPartialData(UInt64 offset, UInt64 length)
{
	return this->stmData->GetPartialData(offset, length);
}

Bool IO::StmData::BufferedStreamData::IsFullFile()
{
	return this->stmData->IsFullFile();
}

NN<Text::String> IO::StmData::BufferedStreamData::GetFullFileName()
{
	return this->stmData->GetFullFileName();
}

Bool IO::StmData::BufferedStreamData::IsLoading()
{
	return this->stmData->IsLoading();
}

UOSInt IO::StmData::BufferedStreamData::GetSeekCount()
{
	return this->stmData->GetSeekCount();
}
