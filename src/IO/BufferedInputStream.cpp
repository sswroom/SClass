#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/BufferedInputStream.h"

IO::BufferedInputStream::BufferedInputStream(IO::SeekableStream *stm, UOSInt buffSize) : IO::SeekableStream(stm->GetSourceNameObj())
{
	this->stm = stm;
	this->buffSize = buffSize;
	this->buffOfst = 0;
	this->buff = MemAlloc(UInt8, this->buffSize);
	this->stmPos = this->stm->GetPosition();
	this->stmBuffSize = 0;
}

IO::BufferedInputStream::~BufferedInputStream()
{
	MemFree(buff);
}

UOSInt IO::BufferedInputStream::Read(UInt8 *buff, UOSInt size)
{
	UOSInt copySize = 0;
	while (size > 0)
	{
		if (this->buffOfst < this->stmBuffSize)
		{
			if (size <= this->stmBuffSize - this->buffOfst)
			{
				MemCopyNO(buff, &this->buff[this->buffOfst], size);
				this->buffOfst += size;
				copySize += size;
				return copySize;
			}
			else
			{
				copySize = this->stmBuffSize - this->buffOfst;
				MemCopyNO(buff, &this->buff[buffOfst], copySize);
				size -= copySize;
				buff += copySize;
				this->stmPos += this->stmBuffSize;
				this->stmBuffSize = 0;
			}
		}


		this->stmBuffSize = this->stm->Read(this->buff, this->buffSize);
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

void *IO::BufferedInputStream::BeginRead(UInt8 *buff, UOSInt size, Sync::Event *evt)
{
	UOSInt sz = Read(buff, size);
	evt->Set();
	return (void*)sz;
}

UOSInt IO::BufferedInputStream::EndRead(void *reqData, Bool toWait)
{
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

UInt64 IO::BufferedInputStream::Seek(SeekType origin, Int64 position)
{
	UInt64 targetPos;
	if (origin == IO::SeekableStream::ST_END)
	{
		targetPos = this->stm->GetLength() + position;
	}
	else if (origin == IO::SeekableStream::ST_BEGIN)
	{
		targetPos = position;
	}
	else if (origin == IO::SeekableStream::ST_CURRENT)
	{
		targetPos = this->stmPos + this->buffOfst + position;
	}
	else
	{
		targetPos = position;
	}
	
	if (targetPos >= this->stmPos && targetPos <= this->stmPos + this->stmBuffSize)
	{
		this->buffOfst = (OSInt)(targetPos - this->stmPos);
	}
	else
	{
		targetPos = this->stm->Seek(IO::SeekableStream::ST_BEGIN, targetPos);
		this->buffOfst = 0;
		this->stmPos = targetPos;
		this->stmBuffSize = 0;
	}
	return targetPos;
}

UInt64 IO::BufferedInputStream::GetPosition()
{
	return this->stmPos + this->buffOfst;
}

UInt64 IO::BufferedInputStream::GetLength()
{
	return this->stm->GetLength();
}
