#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/BufferedInputStream.h"

IO::BufferedInputStream::BufferedInputStream(NotNullPtr<IO::SeekableStream> stm, UOSInt buffSize) : IO::SeekableStream(stm->GetSourceNameObj()), buff(buffSize)
{
	this->stm = stm;
	this->buffOfst = 0;
	this->stmPos = this->stm->GetPosition();
	this->stmBuffSize = 0;
}

IO::BufferedInputStream::~BufferedInputStream()
{
}

Bool IO::BufferedInputStream::IsDown() const
{
	return this->stm->IsDown();
}

UOSInt IO::BufferedInputStream::Read(const Data::ByteArray &buff)
{
	UOSInt copySize = 0;
	Data::ByteArray myBuff = buff;
	while (myBuff.GetSize() > 0)
	{
		if (this->buffOfst < this->stmBuffSize)
		{
			if (myBuff.GetSize() <= this->stmBuffSize - this->buffOfst)
			{
				myBuff.CopyFrom(0, this->buff.SubArray(this->buffOfst, myBuff.GetSize()));
				this->buffOfst += myBuff.GetSize();
				copySize += myBuff.GetSize();
				return copySize;
			}
			else
			{
				copySize = this->stmBuffSize - this->buffOfst;
				myBuff.CopyFrom(0, this->buff.SubArray(buffOfst, copySize));
				myBuff = myBuff.SubArray(copySize);
				this->stmPos += this->stmBuffSize;
				this->stmBuffSize = 0;
			}
		}


		this->stmBuffSize = this->stm->Read(this->buff);
		this->buffOfst = 0;
		if (this->stmBuffSize <= 0)
			return copySize;
	}
	return copySize;
}

UOSInt IO::BufferedInputStream::Write(const UInt8 *buff, UOSInt size)
{
	return 0;
}

void *IO::BufferedInputStream::BeginRead(const Data::ByteArray &buff, Sync::Event *evt)
{
	UOSInt sz = Read(buff);
	evt->Set();
	return (void*)sz;
}

UOSInt IO::BufferedInputStream::EndRead(void *reqData, Bool toWait, Bool *incomplete)
{
	if (incomplete)
	{
		*incomplete = false;
	}
	return (UOSInt)reqData;
}

void IO::BufferedInputStream::CancelRead(void *reqData)
{
}

void *IO::BufferedInputStream::BeginWrite(const UInt8 *buff, UOSInt size, Sync::Event *evt)
{
	evt->Set();
	return (void*)-1;
}

UOSInt IO::BufferedInputStream::EndWrite(void *reqData, Bool toWait)
{
	return 0;
}

void IO::BufferedInputStream::CancelWrite(void *reqData)
{
}

Int32 IO::BufferedInputStream::Flush()
{
	return 0;
}

void IO::BufferedInputStream::Close()
{
	this->stm->Close();
}

Bool IO::BufferedInputStream::Recover()
{
	return this->stm->Recover();
}

IO::StreamType IO::BufferedInputStream::GetStreamType() const
{
	return IO::StreamType::BufferedInput;
}

UInt64 IO::BufferedInputStream::SeekFromBeginning(UInt64 position)
{
	if (position >= this->stmPos && position <= this->stmPos + this->stmBuffSize)
	{
		this->buffOfst = (UOSInt)(position - this->stmPos);
	}
	else
	{
		position = this->stm->SeekFromBeginning(position);
		this->buffOfst = 0;
		this->stmPos = position;
		this->stmBuffSize = 0;
	}
	return position;
}

UInt64 IO::BufferedInputStream::SeekFromCurrent(Int64 position)
{
	Int64 targetPos = (Int64)(this->stmPos + this->buffOfst) + position;
	if (targetPos < 0)
	{
		targetPos = 0;
	}
	return SeekFromBeginning((UInt64)targetPos);
}


UInt64 IO::BufferedInputStream::SeekFromEnd(Int64 position)
{
	Int64 targetPos = (Int64)this->stm->GetLength() + position;
	if (targetPos < 0)
	{
		targetPos = 0;
	}
	return SeekFromBeginning((UInt64)targetPos);
}

UInt64 IO::BufferedInputStream::GetPosition()
{
	return this->stmPos + this->buffOfst;
}

UInt64 IO::BufferedInputStream::GetLength()
{
	return this->stm->GetLength();
}
