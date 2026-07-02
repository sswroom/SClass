#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/WriteCacheStream.h"

IO::WriteCacheStream::WriteCacheStream(NN<IO::Stream> outStm) : IO::Stream(outStm->GetSourceNameObj())
{
	this->outStm = outStm;
	this->cacheBuff = nullptr;
	this->cacheBuffSize = 0;
	this->cacheSize = 0;
}

IO::WriteCacheStream::~WriteCacheStream()
{
	UnsafeArray<UInt8> cacheBuff;
	this->Flush();
	if (this->cacheBuff.SetTo(cacheBuff))
	{
		MemFreeArr(cacheBuff);
		this->cacheBuff = nullptr;
	}
}

Bool IO::WriteCacheStream::IsDown() const
{
	return this->outStm->IsDown();
}

UIntOS IO::WriteCacheStream::Read(const Data::ByteArray &buff)
{
	return this->outStm->Read(buff);
}

UIntOS IO::WriteCacheStream::Write(Data::ByteArrayR buff)
{
	UIntOS ret;
	UnsafeArray<UInt8> newBuff;
	UnsafeArray<UInt8> cacheBuff;
	if (!this->cacheBuff.SetTo(cacheBuff))
	{
		ret = this->outStm->Write(buff);
		if (ret == buff.GetSize())
			return ret;

		this->cacheBuffSize = 1024;
		while ((buff.GetSize() - ret) > this->cacheBuffSize)
		{
			this->cacheBuffSize = this->cacheBuffSize << 1;
		}
		this->cacheBuff = cacheBuff = MemAllocArr(UInt8, this->cacheBuffSize);
		MemCopyNO(&cacheBuff[0], &buff[ret], buff.GetSize() - ret);
		this->cacheSize = buff.GetSize() - ret;
		return buff.GetSize();
	}
	else if (this->cacheSize == 0)
	{
		ret = this->outStm->Write(buff);
		if (ret == buff.GetSize())
			return ret;

		if ((buff.GetSize() - ret) > this->cacheBuffSize)
		{
			while ((buff.GetSize() - ret) > this->cacheBuffSize)
			{
				this->cacheBuffSize = this->cacheBuffSize << 1;
			}
			MemFreeArr(cacheBuff);
			this->cacheBuff = cacheBuff = MemAllocArr(UInt8, this->cacheBuffSize);
			MemCopyNO(&cacheBuff[0], &buff[ret], buff.GetSize() - ret);
			this->cacheSize = buff.GetSize() - ret;
			return buff.GetSize();
		}
		else
		{
			MemCopyNO(&cacheBuff[0], &buff[ret], buff.GetSize() - ret);
			this->cacheSize = buff.GetSize() - ret;
			return buff.GetSize();
		}
	}

	if ((buff.GetSize() + this->cacheSize) <= this->cacheBuffSize)
	{
		MemCopyNO(&cacheBuff[this->cacheSize], buff.Ptr(), buff.GetSize());
		this->cacheSize += buff.GetSize();
		ret = this->outStm->Write(Data::ByteArrayR(cacheBuff, this->cacheSize));
		if (ret == this->cacheSize)
		{
			this->cacheSize = 0;
			return buff.GetSize();
		}
		else if (ret == 0)
		{
			return buff.GetSize();
		}
		else
		{
			MemCopyO(&cacheBuff[0], &cacheBuff[ret], this->cacheSize - ret);
			this->cacheSize -= ret;
			return buff.GetSize();
		}
	}

	while ((buff.GetSize() + this->cacheSize) > this->cacheBuffSize)
	{
		this->cacheBuffSize = this->cacheBuffSize << 1;
	}
	newBuff = MemAllocArr(UInt8, this->cacheBuffSize);
	MemCopyNO(&newBuff[0], &cacheBuff[0], this->cacheSize);
	MemCopyNO(&newBuff[this->cacheSize], buff.Ptr(), buff.GetSize());
	this->cacheSize += buff.GetSize();
	MemFreeArr(cacheBuff);
	this->cacheBuff = newBuff;
	ret = this->outStm->Write(Data::ByteArrayR(newBuff, this->cacheSize));
	if (ret == this->cacheSize)
	{
		this->cacheSize = 0;
		return buff.GetSize();
	}
	else if (ret == 0)
	{
		return buff.GetSize();
	}
	else
	{
		MemCopyO(&newBuff[0], &newBuff[ret], this->cacheSize - ret);
		this->cacheSize -= ret;
		return buff.GetSize();
	}
}

Int32 IO::WriteCacheStream::Flush()
{
	UIntOS writeSize;
	UnsafeArray<UInt8> cacheBuff;
	if (this->cacheBuff.SetTo(cacheBuff))
	{
		while (this->cacheSize > 0)
		{
			writeSize = this->outStm->Write(Data::ByteArrayR(cacheBuff, this->cacheSize));
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
				MemCopyO(&cacheBuff[0], &cacheBuff[writeSize], this->cacheSize - writeSize);
				this->cacheSize -= writeSize;
			}
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
