#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/CryptoStream.h"

IO::CryptoStream::CryptoStream(NN<IO::Stream> srcStream, NN<Crypto::Encrypt::Encryption> crypto) : IO::Stream(srcStream->GetSourceNameObj())
{
	this->stm = srcStream;
	this->crypto = crypto;
	this->encBuff = MemAllocArr(UInt8, this->crypto->GetEncBlockSize());
	this->decBuff = MemAllocArr(UInt8, this->crypto->GetEncBlockSize());
	this->encBuffSize = 0;
	this->decBuffSize = 0;
	this->tmpBuff = 0;
	this->tmpBuffSize = 0;
}

IO::CryptoStream::~CryptoStream()
{
	Close();
	MemFreeArr(this->encBuff);
	MemFreeArr(this->decBuff);
	UnsafeArray<UInt8> tmpBuff;
	if (this->tmpBuff.SetTo(tmpBuff))
		MemFreeArr(tmpBuff);
}

UIntOS IO::CryptoStream::Read(const Data::ByteArray &buff)
{
	NN<IO::Stream> nnstm;
	if (!this->stm.SetTo(nnstm))
		return 0;
	UnsafeArray<UInt8> tmpBuff;
	UIntOS size = buff.GetSize();
	UnsafeArray<UInt8> buffPtr = buff.Arr();
	UIntOS retSize = 0;
	UIntOS eblkSize = this->crypto->GetEncBlockSize();
	UIntOS dblkSize = this->crypto->GetDecBlockSize();
	UIntOS i;
	if (this->decBuffSize > 0)
	{
		if (this->decBuffSize > size)
		{
			MemCopyNO(buffPtr.Ptr(), &this->decBuff[0], size);
			MemCopyO(&this->decBuff[0], &this->decBuff[size], this->decBuffSize - size);
			this->decBuffSize -= size;
			retSize = size;
			size = 0;
		}
		else
		{
			MemCopyNO(buffPtr.Ptr(), &this->decBuff[0], this->decBuffSize);
			size -= this->decBuffSize;
			retSize = this->decBuffSize;
			buffPtr += this->decBuffSize;
			this->decBuffSize = 0;
		}
	}
	if (size > 0)
	{
		IntOS blkCnt = size / eblkSize;
		if (!this->tmpBuff.SetTo(tmpBuff) || this->tmpBuffSize < blkCnt * dblkSize)
		{
			if (this->tmpBuff.SetTo(tmpBuff))
				MemFreeArr(tmpBuff);
			this->tmpBuffSize = blkCnt * dblkSize;
			this->tmpBuff = tmpBuff = MemAllocArr(UInt8, this->tmpBuffSize);
		}
		i = nnstm->Read(Data::ByteArray(tmpBuff, blkCnt * dblkSize));
		i = this->crypto->Decrypt(tmpBuff, i, buffPtr);
		retSize += i;
		size -= i;
		buffPtr += i;
	}
	if (size > 0 && size < eblkSize)
	{
		if (!this->tmpBuff.SetTo(tmpBuff) || this->tmpBuffSize < dblkSize)
		{
			if (this->tmpBuff.SetTo(tmpBuff))
				MemFreeArr(tmpBuff);
			this->tmpBuffSize = dblkSize;
			this->tmpBuff = tmpBuff = MemAllocArr(UInt8, this->tmpBuffSize);
		}
		i = nnstm->Read(Data::ByteArray(tmpBuff, dblkSize));
		this->decBuffSize = this->crypto->Decrypt(tmpBuff, i, this->decBuff);
		if (this->decBuffSize > size)
		{
			MemCopyNO(buffPtr.Ptr(), &this->decBuff[0], size);
			MemCopyO(&this->decBuff[0], &this->decBuff[size], this->decBuffSize - size);
			this->decBuffSize -= size;
			retSize += size;
			size = 0;
		}
		else if (this->decBuffSize > 0)
		{
			MemCopyNO(buffPtr.Ptr(), &this->decBuff[0], this->decBuffSize);
			retSize += this->decBuffSize;
			size -= this->decBuffSize;
			buffPtr += this->decBuffSize;
			this->decBuffSize = 0;
		}
	}
	return retSize;
}

UIntOS IO::CryptoStream::Write(Data::ByteArrayR buff)
{
	NN<IO::Stream> nnstm;
	if (!this->stm.SetTo(nnstm))
		return 0;
	UnsafeArray<UInt8> tmpBuff;
	UIntOS size = buff.GetSize();
	UnsafeArray<const UInt8> buffPtr = buff.Arr();
	UIntOS retSize = size;
	UIntOS eblkSize = this->crypto->GetEncBlockSize();
	UIntOS dblkSize = this->crypto->GetDecBlockSize();
	UIntOS i;
	if (this->encBuffSize + size < eblkSize)
	{
		MemCopyNO(&this->encBuff[this->encBuffSize], buffPtr.Ptr(), size);
		this->encBuffSize += size;
	}
	else
	{
		if (this->encBuffSize > 0)
		{
			if (!this->tmpBuff.SetTo(tmpBuff) || this->tmpBuffSize < dblkSize)
			{
				if (this->tmpBuff.SetTo(tmpBuff))
					MemFreeArr(tmpBuff);
				this->tmpBuffSize = dblkSize;
				this->tmpBuff = tmpBuff = MemAllocArr(UInt8, this->tmpBuffSize);
			}
			MemCopyNO(&this->encBuff[this->encBuffSize], buffPtr.Ptr(), eblkSize - this->encBuffSize);
			i = this->crypto->Encrypt(this->encBuff, eblkSize, tmpBuff);
			nnstm->Write(Data::ByteArrayR(tmpBuff, i));
			buffPtr += eblkSize - this->encBuffSize;
			size -= eblkSize - this->encBuffSize;
			this->encBuffSize = 0;
		}
		IntOS blkCnt = size / eblkSize;
		if (blkCnt > 0)
		{
			if (!this->tmpBuff.SetTo(tmpBuff) || this->tmpBuffSize < blkCnt * dblkSize)
			{
				if (this->tmpBuff.SetTo(tmpBuff))
					MemFreeArr(tmpBuff);
				this->tmpBuffSize = blkCnt * dblkSize;
				this->tmpBuff = tmpBuff = MemAlloc(UInt8, this->tmpBuffSize);
			}
			i = this->crypto->Encrypt(buffPtr, blkCnt * eblkSize, tmpBuff);
			nnstm->Write(Data::ByteArrayR(tmpBuff, i));
			buffPtr += blkCnt * eblkSize;
			size -= blkCnt * eblkSize;
		}
		if (size > 0)
		{
			MemCopyNO(&this->encBuff[0], buffPtr.Ptr(), size);
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
	NN<IO::Stream> nnstm;
	if (this->stm.SetTo(nnstm))
	{
		if (this->encBuffSize > 0)
		{
			UnsafeArray<UInt8> tmpBuff;
			UIntOS i;
			UIntOS dblkSize = this->crypto->GetDecBlockSize();
			if (!this->tmpBuff.SetTo(tmpBuff) || this->tmpBuffSize < dblkSize)
			{
				if (this->tmpBuff.SetTo(tmpBuff))
					MemFreeArr(tmpBuff);
				this->tmpBuffSize = dblkSize;
				this->tmpBuff = tmpBuff = MemAllocArr(UInt8, this->tmpBuffSize);
			}
			i = this->crypto->Encrypt(this->encBuff, this->encBuffSize, tmpBuff);
			nnstm->Write(Data::ByteArrayR(tmpBuff, i));
		}
		nnstm->Close();
		this->stm = 0;
	}
}
