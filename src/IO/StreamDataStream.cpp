#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/StreamDataStream.h"

IO::StreamDataStream::StreamDataStream(IO::IStreamData *data) : IO::SeekableStream(data->GetFullName())
{
	this->currOfst = 0;
	this->stmDataLeng = data->GetDataSize();
	this->data = data->GetPartialData(0, this->stmDataLeng);
}

IO::StreamDataStream::~StreamDataStream()
{
	DEL_CLASS(data);
}

UOSInt IO::StreamDataStream::Read(UInt8 *buff, UOSInt size)
{
	UOSInt thisSize = size;
	if (this->currOfst + thisSize > this->stmDataLeng)
	{
		thisSize = (UOSInt)(this->stmDataLeng - this->currOfst);
	}
	thisSize = this->data->GetRealData(this->currOfst, thisSize, buff);
	currOfst += thisSize;
	return thisSize;
}

UOSInt IO::StreamDataStream::Write(const UInt8 *buff, UOSInt size)
{
	return 0;
}

void *IO::StreamDataStream::BeginRead(UInt8 *buff, UOSInt size, Sync::Event *evt)
{
	UOSInt readSize = this->Read(buff, size);
	evt->Set();
	return (void*)readSize;
}

UOSInt IO::StreamDataStream::EndRead(void *reqData, Bool toWait)
{
	return (UOSInt)reqData;
}

void IO::StreamDataStream::CancelRead(void *reqData)
{
}

void *IO::StreamDataStream::BeginWrite(const UInt8 *buff, UOSInt size, Sync::Event *evt)
{
	return 0;
}

UOSInt IO::StreamDataStream::EndWrite(void *reqData, Bool toWait)
{
	return 0;
}

void IO::StreamDataStream::CancelWrite(void *reqData)
{
}

Int32 IO::StreamDataStream::Flush()
{
	return 0;
}

void IO::StreamDataStream::Close()
{
}

Bool IO::StreamDataStream::Recover()
{
	//////////////////////////////////////
	return false;
}

UInt64 IO::StreamDataStream::Seek(SeekType origin, Int64 position)
{
	if (origin == IO::SeekableStream::ST_BEGIN)
	{
		this->currOfst = position;
	}
	else if (origin == IO::SeekableStream::ST_CURRENT)
	{
		this->currOfst += position;
	}
	else if (origin == IO::SeekableStream::ST_END)
	{
		this->currOfst = this->stmDataLeng + position;
	}
	if ((Int64)currOfst < 0)
		currOfst = 0;
	else if (currOfst > this->stmDataLeng)
	{
		currOfst = this->stmDataLeng;
	}
	return currOfst;
}

UInt64 IO::StreamDataStream::GetPosition()
{
	return currOfst;
}

UInt64 IO::StreamDataStream::GetLength()
{
	return stmDataLeng;
}
