#include "stdafx.h"
#include "MyMemory.h"
#include "IO/CryptoStream.h"

IO::CryptoStream::CryptoStream(IO::Stream *srcStream, Crypto::Encrypt::Encryption *crypto, void *encParam) : IO::Stream(srcStream->GetSourceNameObj())
{
	this->stm = srcStream;
	this->crypto = crypto;
	this->encParam = encParam;
	this->encBuff = MemAlloc(UInt8, this->crypto->GetEncBlockSize());
	this->decBuff = MemAlloc(UInt8, this->crypto->GetEncBlockSize());
	this->encBuffSize = 0;
	this->decBuffSize = 0;
	this->tmpBuff = 0;
	this->tmpBuffSize = 0;
}

IO::CryptoStream::~CryptoStream()
{
	Close();
	MemFree(this->encBuff);
	MemFree(this->decBuff);
	if (this->tmpBuff)
		MemFree(this->tmpBuff);
}

OSInt IO::CryptoStream::Read(UInt8 *buff, OSInt size)
{
	if (this->stm == 0)
		return 0;
	OSInt retSize = 0;
	OSInt eblkSize = this->crypto->GetEncBlockSize();
	OSInt dblkSize = this->crypto->GetDecBlockSize();
	OSInt i;
	if (this->decBuffSize > 0)
	{
		if (this->decBuffSize > size)
		{
			MemCopy(buff, this->decBuff, size);
			MemCopy(this->decBuff, &this->decBuff[size], this->decBuffSize - size);
			this->decBuffSize -= size;
			retSize = size;
			size = 0;
		}
		else
		{
			MemCopy(buff, this->decBuff, this->decBuffSize);
			size -= this->decBuffSize;
			retSize = this->decBuffSize;
			buff += this->decBuffSize;
			this->decBuffSize = 0;
		}
	}
	if (size > 0)
	{
		OSInt blkCnt = size / eblkSize;
		if (this->tmpBuffSize < blkCnt * dblkSize)
		{
			if (this->tmpBuff)
				MemFree(this->tmpBuff);
			this->tmpBuffSize = blkCnt * dblkSize;
			this->tmpBuff = MemAlloc(UInt8, this->tmpBuffSize);
		}
		i = this->stm->Read(this->tmpBuff, blkCnt * dblkSize);
		i = this->crypto->Decrypt(this->tmpBuff, i, buff, this->encParam);
		retSize += i;
		size -= i;
		buff += i;
	}
	if (size > 0 && size < eblkSize)
	{
		if (this->tmpBuffSize < dblkSize)
		{
			if (this->tmpBuff)
				MemFree(this->tmpBuff);
			this->tmpBuffSize = dblkSize;
			this->tmpBuff = MemAlloc(UInt8, this->tmpBuffSize);
		}
		i = this->stm->Read(this->tmpBuff, dblkSize);
		this->decBuffSize = this->crypto->Decrypt(this->tmpBuff, i, this->decBuff, this->encParam);
		if (this->decBuffSize > size)
		{
			MemCopy(buff, this->decBuff, size);
			MemCopy(this->decBuff, &this->decBuff[size], this->decBuffSize - size);
			this->decBuffSize -= size;
			retSize += size;
			size = 0;
		}
		else if (this->decBuffSize > 0)
		{
			MemCopy(buff, this->decBuff, this->decBuffSize);
			retSize += this->decBuffSize;
			size -= this->decBuffSize;
			buff += this->decBuffSize;
			this->decBuffSize = 0;
		}
	}
	return retSize;
}

OSInt IO::CryptoStream::Write(const UInt8 *buff, OSInt size)
{
	if (this->stm == 0)
		return 0;
	OSInt retSize = size;
	OSInt eblkSize = this->crypto->GetEncBlockSize();
	OSInt dblkSize = this->crypto->GetDecBlockSize();
	OSInt i;
	if (this->encBuffSize + size < eblkSize)
	{
		MemCopy(&this->encBuff[this->encBuffSize], buff, size);
		this->encBuffSize += size;
	}
	else
	{
		if (this->encBuffSize > 0)
		{
			if (this->tmpBuffSize < dblkSize)
			{
				if (this->tmpBuff)
					MemFree(this->tmpBuff);
				this->tmpBuffSize = dblkSize;
				this->tmpBuff = MemAlloc(UInt8, this->tmpBuffSize);
			}
			MemCopy(&this->encBuff[this->encBuffSize], buff, eblkSize - this->encBuffSize);
			i = this->crypto->Encrypt(this->encBuff, eblkSize, this->tmpBuff, this->encParam);
			this->stm->Write(this->tmpBuff, i);
			buff += eblkSize - this->encBuffSize;
			size -= eblkSize - this->encBuffSize;
			this->encBuffSize = 0;
		}
		OSInt blkCnt = size / eblkSize;
		if (blkCnt > 0)
		{
			if (this->tmpBuffSize < blkCnt * dblkSize)
			{
				if (this->tmpBuff)
					MemFree(this->tmpBuff);
				this->tmpBuffSize = blkCnt * dblkSize;
				this->tmpBuff = MemAlloc(UInt8, this->tmpBuffSize);
			}
			i = this->crypto->Encrypt(buff, blkCnt * eblkSize, this->tmpBuff, this->encParam);
			this->stm->Write(this->tmpBuff, i);
			buff += blkCnt * eblkSize;
			size -= blkCnt * eblkSize;
		}
		if (size > 0)
		{
			MemCopy(this->encBuff, buff, size);
			this->encBuffSize = size;
		}
	}

	return retSize;
}

Int32 IO::CryptoStream::Flush()
{
	return 0;
}

void IO::CryptoStream::Close()
{
	if (this->stm)
	{
		if (this->encBuffSize > 0)
		{
			OSInt i;
			OSInt dblkSize = this->crypto->GetDecBlockSize();
			if (this->tmpBuffSize < dblkSize)
			{
				if (this->tmpBuff)
					MemFree(this->tmpBuff);
				this->tmpBuffSize = dblkSize;
				this->tmpBuff = MemAlloc(UInt8, this->tmpBuffSize);
			}
			i = this->crypto->Encrypt(this->encBuff, this->encBuffSize, this->tmpBuff, this->encParam);
			this->stm->Write(this->tmpBuff, i);
		}
		this->stm->Close();
		this->stm = 0;
	}
}
