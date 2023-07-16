#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/WriteCacheStream.h"

IO::WriteCacheStream::WriteCacheStream(IO::Stream *outStm) : IO::Stream(outStm->GetSourceNameObj())
{
	this->outStm = outStm;
	this->cacheBuff = 0;
	this->cacheBuffSize = 0;
	this->cacheSize = 0;
}

IO::WriteCacheStream::~WriteCacheStream()
{
	this->Flush();
	if (this->cacheBuff)
	{
		MemFree(this->cacheBuff);
		this->cacheBuff = 0;
	}
}

Bool IO::WriteCacheStream::IsDown() const
{
	return this->outStm->IsDown();
}

UOSInt IO::WriteCacheStream::Read(Data::ByteArray buff)
{
	return this->outStm->Read(buff);
}

UOSInt IO::WriteCacheStream::Write(const UInt8 *buff, UOSInt size)
{
	UOSInt ret;
	UInt8 *newBuff;
	if (this->cacheBuff == 0)
	{
		ret = this->outStm->Write(buff, size);
		if (ret == size)
			return ret;

		this->cacheBuffSize = 1024;
		while ((size - ret) > this->cacheBuffSize)
		{
			this->cacheBuffSize = this->cacheBuffSize << 1;
		}
		this->cacheBuff = MemAlloc(UInt8, this->cacheBuffSize);
		MemCopyNO(this->cacheBuff, &buff[ret], size - ret);
		this->cacheSize = size - ret;
		return size;
	}
	else if (this->cacheSize == 0)
	{
		ret = this->outStm->Write(buff, size);
		if (ret == size)
			return ret;

		if ((size - ret) > this->cacheBuffSize)
		{
			while ((size - ret) > this->cacheBuffSize)
			{
				this->cacheBuffSize = this->cacheBuffSize << 1;
			}
			MemFree(this->cacheBuff);
			this->cacheBuff = MemAlloc(UInt8, this->cacheBuffSize);
			MemCopyNO(this->cacheBuff, &buff[ret], size - ret);
			this->cacheSize = size - ret;
			return size;
		}
		else
		{
			MemCopyNO(this->cacheBuff, &buff[ret], size - ret);
			this->cacheSize = size - ret;
			return size;
		}
	}

	if ((size + this->cacheSize) <= this->cacheBuffSize)
	{
		MemCopyNO(&this->cacheBuff[this->cacheSize], buff, size);
		this->cacheSize += size;
		ret = this->outStm->Write(this->cacheBuff, this->cacheSize);
		if (ret == this->cacheSize)
		{
			this->cacheSize = 0;
			return size;
		}
		else if (ret == 0)
		{
			return size;
		}
		else
		{
			MemCopyO(this->cacheBuff, &this->cacheBuff[ret], this->cacheSize - ret);
			this->cacheSize -= ret;
			return size;
		}
	}

	while ((size + this->cacheSize) > this->cacheBuffSize)
	{
		this->cacheBuffSize = this->cacheBuffSize << 1;
	}
	newBuff = MemAlloc(UInt8, this->cacheBuffSize);
	MemCopyNO(newBuff, this->cacheBuff, this->cacheSize);
	MemCopyNO(&newBuff[this->cacheSize], buff, size);
	this->cacheSize += size;
	MemFree(this->cacheBuff);
	this->cacheBuff = newBuff;
	ret = this->outStm->Write(this->cacheBuff, this->cacheSize);
	if (ret == this->cacheSize)
	{
		this->cacheSize = 0;
		return size;
	}
	else if (ret == 0)
	{
		return size;
	}
	else
	{
		MemCopyO(this->cacheBuff, &this->cacheBuff[ret], this->cacheSize - ret);
		this->cacheSize -= ret;
		return size;
	}
}

Int32 IO::WriteCacheStream::Flush()
{
	UOSInt writeSize;
	while (this->cacheSize > 0)
	{
		writeSize = this->outStm->Write(this->cacheBuff, this->cacheSize);
		if (writeSize == this->cacheSize)
		{
			this->cacheSize = 0;
			break;
		}
		else if (writeSize == 0)
		{
			break;
		}
		else
		{
			MemCopyO(this->cacheBuff, &this->cacheBuff[writeSize], this->cacheSize - writeSize);
			this->cacheSize -= writeSize;
		}
	}
	return this->outStm->Flush();
}

void IO::WriteCacheStream::Close()
{
	this->outStm->Close();
}

Bool IO::WriteCacheStream::Recover()
{
	return this->outStm->Recover();
}

IO::StreamType IO::WriteCacheStream::GetStreamType() const
{
	return IO::StreamType::WriteCache;
}
