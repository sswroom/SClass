#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/StreamDataStream.h"

IO::StreamDataStream::StreamDataStream(NN<IO::StreamData> data) : IO::SeekableStream(data->GetFullName())
{
	this->currOfst = 0;
	this->stmDataLeng = data->GetDataSize();
	this->data = data->GetPartialData(0, this->stmDataLeng);
}

IO::StreamDataStream::StreamDataStream(NN<IO::StreamData> data, UInt64 ofst, UInt64 length) : IO::SeekableStream(data->GetFullName())
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

UOSInt IO::StreamDataStream::Write(Data::ByteArrayR buff)
{
	return 0;
}

Optional<IO::StreamReadReq> IO::StreamDataStream::BeginRead(const Data::ByteArray &buff, NN<Sync::Event> evt)
{
	UOSInt readSize = this->Read(buff);
	if (readSize)
		evt->Set();
	return (IO::StreamReadReq*)readSize;
}

UOSInt IO::StreamDataStream::EndRead(NN<IO::StreamReadReq> reqData, Bool toWait, OutParam<Bool> incomplete)
{
	incomplete.Set(false);
	return (UOSInt)reqData.Ptr();
}

void IO::StreamDataStream::CancelRead(NN<IO::StreamReadReq> reqData)
{
}

Optional<IO::StreamWriteReq> IO::StreamDataStream::BeginWrite(Data::ByteArrayR buff, NN<Sync::Event> evt)
{
	return nullptr;
}

UOSInt IO::StreamDataStream::EndWrite(NN<IO::StreamWriteReq> reqData, Bool toWait)
{
	return 0;
}

void IO::StreamDataStream::CancelWrite(NN<IO::StreamWriteReq> reqData)
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
