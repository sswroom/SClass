#include "Stdafx.h"
#include "IO/StmData/BufferedStreamData.h"

#define BUFFSIZE 65536

IO::StmData::BufferedStreamData::BufferedStreamData(IO::IStreamData *stmData)
{
	this->stmData = stmData;
	this->buff = MemAlloc(UInt8, BUFFSIZE);
	this->buffLength = 0;
	this->buffOfst = 0;
	this->dataLength = this->stmData->GetDataSize();
}

IO::StmData::BufferedStreamData::~BufferedStreamData()
{
	MemFree(this->buff);
	DEL_CLASS(this->stmData);
}

UOSInt IO::StmData::BufferedStreamData::GetRealData(UInt64 offset, UOSInt length, UInt8 *buffer)
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
		MemCopyNO(buffer, &this->buff[(UOSInt)(offset - this->buffOfst)], length);
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
		MemCopyNO(buffer, this->buff, length);
		return length;
	}
	UOSInt ret = (UOSInt)(buffEndOfst - offset);
	MemCopyNO(buffer, &this->buff[(UOSInt)(offset - this->buffOfst)], ret);
	buffer += ret;
	length -= ret;
	this->buffOfst = buffEndOfst;
	this->buffLength = this->stmData->GetRealData(this->buffOfst, BUFFSIZE, this->buff);
	if (length > this->buffLength)
	{
		length = this->buffLength;
	}
	MemCopyNO(buffer, this->buff, length);
	return length + ret;
}

Text::String *IO::StmData::BufferedStreamData::GetFullName()
{
	return this->stmData->GetFullName();
}

Text::CString IO::StmData::BufferedStreamData::GetShortName()
{
	return this->stmData->GetShortName();
}

void IO::StmData::BufferedStreamData::SetFullName(Text::CString fullName)
{
	this->stmData->SetFullName(fullName);
}

UInt64 IO::StmData::BufferedStreamData::GetDataSize()
{
	return this->stmData->GetDataSize();
}

const UInt8 *IO::StmData::BufferedStreamData::GetPointer()
{
	return this->stmData->GetPointer();
}

IO::IStreamData *IO::StmData::BufferedStreamData::GetPartialData(UInt64 offset, UInt64 length)
{
	return this->stmData->GetPartialData(offset, length);
}

Bool IO::StmData::BufferedStreamData::IsFullFile()
{
	return this->stmData->IsFullFile();
}

Text::String *IO::StmData::BufferedStreamData::GetFullFileName()
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
