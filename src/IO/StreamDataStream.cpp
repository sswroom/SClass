#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/StreamDataStream.h"

IO::StreamDataStream::StreamDataStream(NotNullPtr<IO::StreamData> data) : IO::SeekableStream(data->GetFullName())
{
	this->currOfst = 0;
	this->stmDataLeng = data->GetDataSize();
	this->data = data->GetPartialData(0, this->stmDataLeng);
}

IO::StreamDataStream::StreamDataStream(NotNullPtr<IO::StreamData> data, UInt64 ofst, UInt64 length) : IO::SeekableStream(data->GetFullName())
{
	this->currOfst = 0;
	this->data = data->GetPartialData(ofst, length);
	this->stmDataLeng = this->data->GetDataSize();
}

IO::StreamDataStream::~StreamDataStream()
{
	this->data.Delete();
}

Bool IO::StreamDataStream::IsDown() const
{
	return false;
}

UOSInt IO::StreamDataStream::Read(const Data::ByteArray &buff)
{
	UOSInt thisSize = buff.GetSize();
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

void *IO::StreamDataStream::BeginRead(const Data::ByteArray &buff, Sync::Event *evt)
{
	UOSInt readSize = this->Read(buff);
	evt->Set();
	return (void*)readSize;
}

UOSInt IO::StreamDataStream::EndRead(void *reqData, Bool toWait, Bool *incomplete)
{
	if (incomplete) *incomplete = false;
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

IO::StreamType IO::StreamDataStream::GetStreamType() const
{
	return IO::StreamType::StreamData;
}

UInt64 IO::StreamDataStream::SeekFromBeginning(UInt64 position)
{
	this->currOfst = position;
	if (this->currOfst > this->stmDataLeng)
	{
		this->currOfst = this->stmDataLeng;
	}
	return this->currOfst;
}

UInt64 IO::StreamDataStream::SeekFromCurrent(Int64 position)
{
	Int64 targetPos = (Int64)this->currOfst + position;
	if (targetPos < 0)
	{
		return this->SeekFromBeginning(0);
	}
	else
	{
		return this->SeekFromBeginning((UInt64)targetPos);
	}
}

UInt64 IO::StreamDataStream::SeekFromEnd(Int64 position)
{
	Int64 targetPos = (Int64)this->stmDataLeng + position;
	if (targetPos < 0)
	{
		return this->SeekFromBeginning(0);
	}
	else
	{
		return this->SeekFromBeginning((UInt64)targetPos);
	}
}

UInt64 IO::StreamDataStream::GetPosition()
{
	return currOfst;
}

UInt64 IO::StreamDataStream::GetLength()
{
	return stmDataLeng;
}
