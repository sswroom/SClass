#include "Stdafx.h"
#include "MemTool.h"
#include "MyMemory.h"
#include "Crypto/Encrypt/BlockCipher.h"

Crypto::Encrypt::BlockCipher::BlockCipher(UOSInt blockSize)
{
	this->blockSize = blockSize;
	this->cm = ChainMode::ECB;
	this->iv = MemAlloc(UInt8, blockSize);
	MemClear(this->iv, this->blockSize);
}

Crypto::Encrypt::BlockCipher::~BlockCipher()
{
	MemFree(this->iv);
}

UOSInt Crypto::Encrypt::BlockCipher::Encrypt(UnsafeArray<const UInt8> inBuff, UOSInt inSize, UnsafeArray<UInt8> outBuff)
{
	UInt8 *blk;
	UOSInt blkCnt = 0;
	switch (this->cm)
	{
	case ChainMode::ECB:
		while (inSize >= this->blockSize)
		{
			EncryptBlock(inBuff, outBuff);
			blkCnt++;
			inBuff += this->blockSize;
			outBuff += this->blockSize;
			inSize = inSize - this->blockSize;
		}
		if (inSize > 0)
		{
			blk = MemAlloc(UInt8, this->blockSize);
			MemClear(blk, this->blockSize);
			MemCopyNO(blk, inBuff.Ptr(), inSize);
			EncryptBlock(blk, outBuff);
			blkCnt++;
			MemFree(blk);
		}
		return blkCnt * this->blockSize;
	case ChainMode::CBC:
		blk = MemAlloc(UInt8, this->blockSize);
		MemCopyNO(blk, this->iv, this->blockSize);
		while (inSize >= blockSize)
		{
			MemXOR(blk, inBuff.Ptr(), blk, this->blockSize);
			EncryptBlock(blk, outBuff);
			MemCopyNO(blk, outBuff.Ptr(), this->blockSize);
			blkCnt++;
			inBuff += this->blockSize;
			outBuff += this->blockSize;
			inSize = inSize - this->blockSize;
		}
		if (inSize > 0)
		{
			MemXOR(blk, inBuff.Ptr(), blk, inSize);
			EncryptBlock(blk, outBuff);
			blkCnt++;
		}
		MemFree(blk);
		return blkCnt * this->blockSize;
	case ChainMode::PCBC:
		blk = MemAlloc(UInt8, this->blockSize);
		MemCopyNO(blk, this->iv, this->blockSize);
		while (inSize >= blockSize)
		{
			MemXOR(blk, inBuff.Ptr(), blk, this->blockSize);
			EncryptBlock(inBuff, outBuff);
			MemXOR(inBuff.Ptr(), outBuff.Ptr(), blk, this->blockSize);
			blkCnt++;
			inBuff += this->blockSize;
			outBuff += this->blockSize;
			inSize = inSize - this->blockSize;
		}
		if (inSize > 0)
		{
			MemXOR(blk, inBuff.Ptr(), blk, inSize);
			EncryptBlock(inBuff, outBuff);
			blkCnt++;
		}
		MemFree(blk);
		return blkCnt * this->blockSize;
	case ChainMode::CFB:
		blk = MemAlloc(UInt8, this->blockSize);
		MemCopyNO(blk, this->iv, this->blockSize);
		while (inSize >= blockSize)
		{
			EncryptBlock(blk, outBuff);
			MemXOR(outBuff.Ptr(), inBuff.Ptr(), outBuff.Ptr(), this->blockSize);
			MemCopyNO(blk, outBuff.Ptr(), this->blockSize);
			blkCnt++;
			inBuff += this->blockSize;
			outBuff += this->blockSize;
			inSize = inSize - this->blockSize;
		}
		if (inSize > 0)
		{
			EncryptBlock(blk, outBuff);
			MemXOR(outBuff.Ptr(), inBuff.Ptr(), outBuff.Ptr(), inSize);
			blkCnt++;
		}
		MemFree(blk);
		return blkCnt * this->blockSize;
	case ChainMode::OFB:
		blk = MemAlloc(UInt8, this->blockSize);
		MemCopyNO(blk, this->iv, this->blockSize);
		while (inSize >= blockSize)
		{
			EncryptBlock(blk, outBuff);
			MemCopyNO(blk, outBuff.Ptr(), this->blockSize);
			MemXOR(outBuff.Ptr(), inBuff.Ptr(), outBuff.Ptr(), this->blockSize);
			blkCnt++;
			inBuff += this->blockSize;
			outBuff += this->blockSize;
			inSize = inSize - this->blockSize;
		}
		if (inSize > 0)
		{
			EncryptBlock(blk, outBuff);
			MemXOR(outBuff.Ptr(), inBuff.Ptr(), outBuff.Ptr(), inSize);
			blkCnt++;
		}
		MemFree(blk);
		return blkCnt * this->blockSize;
	default:
		return 0;
	}
}

UOSInt Crypto::Encrypt::BlockCipher::Decrypt(UnsafeArray<const UInt8> inBuff, UOSInt inSize, UnsafeArray<UInt8> outBuff)
{
	UInt8 *blk;
	UInt8 *blk2;
	UInt8 *blkTmp;
	UOSInt blkCnt = 0;
	switch (this->cm)
	{
	case ChainMode::ECB:
		while (inSize >= this->blockSize)
		{
			DecryptBlock(inBuff, outBuff);
			blkCnt++;
			inBuff += this->blockSize;
			outBuff += this->blockSize;
			inSize = inSize - this->blockSize;
		}
		return blkCnt * this->blockSize;
	case ChainMode::CBC:
		if (inBuff != outBuff)
		{
			blk = MemAlloc(UInt8, this->blockSize);
			MemCopyNO(blk, this->iv, this->blockSize);
			while (inSize >= this->blockSize)
			{
				DecryptBlock(inBuff, outBuff);
				blkCnt++;
				MemXOR(outBuff.Ptr(), blk, outBuff.Ptr(), this->blockSize);
				MemCopyNO(blk, inBuff.Ptr(), this->blockSize);
				inBuff += this->blockSize;
				outBuff += this->blockSize;
				inSize = inSize - this->blockSize;
			}
			MemFree(blk);
		}
		else
		{
			blk = MemAlloc(UInt8, this->blockSize);
			blk2 = MemAlloc(UInt8, this->blockSize);
			MemCopyNO(blk, this->iv, this->blockSize);
			while (inSize >= this->blockSize)
			{
				MemCopyNO(blk2, inBuff.Ptr(), this->blockSize);
				DecryptBlock(inBuff, outBuff);
				blkCnt++;
				MemXOR(outBuff.Ptr(), blk, outBuff.Ptr(), this->blockSize);
				blkTmp = blk;
				blk = blk2;
				blk2 = blkTmp;
				inBuff += this->blockSize;
				outBuff += this->blockSize;
				inSize = inSize - this->blockSize;
			}
			MemFree(blk);
			MemFree(blk2);
		}
		return blkCnt * this->blockSize;
	case ChainMode::PCBC:
		if (inBuff != outBuff)
		{
			blk = MemAlloc(UInt8, this->blockSize);
			MemCopyNO(blk, this->iv, this->blockSize);
			while (inSize >= this->blockSize)
			{
				DecryptBlock(inBuff, outBuff);
				blkCnt++;
				MemXOR(outBuff.Ptr(), blk, outBuff.Ptr(), this->blockSize);
				MemXOR(inBuff.Ptr(), outBuff.Ptr(), blk, this->blockSize);
				inBuff += this->blockSize;
				outBuff += this->blockSize;
				inSize = inSize - this->blockSize;
			}
			MemFree(blk);
		}
		else
		{
			blk = MemAlloc(UInt8, this->blockSize);
			blk2 = MemAlloc(UInt8, this->blockSize);
			MemCopyNO(blk, this->iv, this->blockSize);
			while (inSize >= this->blockSize)
			{
				MemCopyNO(blk2, inBuff.Ptr(), this->blockSize);
				DecryptBlock(inBuff, outBuff);
				blkCnt++;
				MemXOR(outBuff.Ptr(), blk, outBuff.Ptr(), this->blockSize);
				MemXOR(blk2, outBuff.Ptr(), blk, this->blockSize);
				inBuff += this->blockSize;
				outBuff += this->blockSize;
				inSize = inSize - this->blockSize;
			}
			MemFree(blk);
			MemFree(blk2);
		}
		return blkCnt * this->blockSize;
	case ChainMode::CFB:
		if (inBuff != outBuff)
		{
			blk = MemAlloc(UInt8, this->blockSize);
			MemCopyNO(blk, this->iv, this->blockSize);
			while (inSize >= this->blockSize)
			{
				DecryptBlock(blk, outBuff);
				blkCnt++;
				MemXOR(outBuff.Ptr(), inBuff.Ptr(), outBuff.Ptr(), this->blockSize);
				MemCopyNO(blk, inBuff.Ptr(), this->blockSize);
				inBuff += this->blockSize;
				outBuff += this->blockSize;
				inSize = inSize - this->blockSize;
			}
			MemFree(blk);
		}
		else
		{
			blk = MemAlloc(UInt8, this->blockSize);
			blk2 = MemAlloc(UInt8, this->blockSize);
			MemCopyNO(blk, this->iv, this->blockSize);
			while (inSize >= this->blockSize)
			{
				MemCopyNO(blk2, inBuff.Ptr(), this->blockSize);
				DecryptBlock(blk, outBuff);
				blkCnt++;
				MemXOR(outBuff.Ptr(), blk2, outBuff.Ptr(), this->blockSize);
				blkTmp = blk;
				blk = blk2;
				blk2 = blkTmp;
				inBuff += this->blockSize;
				outBuff += this->blockSize;
				inSize = inSize - this->blockSize;
			}
			MemFree(blk);
			MemFree(blk2);
		}
		return blkCnt * this->blockSize;
	case ChainMode::OFB:
		if (inBuff != outBuff)
		{
			blk = MemAlloc(UInt8, this->blockSize);
			MemCopyNO(blk, this->iv, this->blockSize);
			while (inSize >= this->blockSize)
			{
				DecryptBlock(blk, outBuff);
				blkCnt++;
				MemCopyNO(blk, outBuff.Ptr(), this->blockSize);
				MemXOR(outBuff.Ptr(), inBuff.Ptr(), outBuff.Ptr(), this->blockSize);
				inBuff += this->blockSize;
				outBuff += this->blockSize;
				inSize = inSize - this->blockSize;
			}
			MemFree(blk);
		}
		else
		{
			blk = MemAlloc(UInt8, this->blockSize);
			blk2 = MemAlloc(UInt8, this->blockSize);
			MemCopyNO(blk, this->iv, this->blockSize);
			while (inSize >= this->blockSize)
			{
				MemCopyNO(blk2, inBuff.Ptr(), this->blockSize);
				DecryptBlock(blk, outBuff);
				blkCnt++;
				MemCopyNO(blk, outBuff.Ptr(), this->blockSize);
				MemXOR(outBuff.Ptr(), blk2, outBuff.Ptr(), this->blockSize);
				inBuff += this->blockSize;
				outBuff += this->blockSize;
				inSize = inSize - this->blockSize;
			}
			MemFree(blk);
			MemFree(blk2);
		}
		return blkCnt * this->blockSize;
	default:
		return 0;
	}
}

UOSInt Crypto::Encrypt::BlockCipher::GetEncBlockSize() const
{
	return this->blockSize;
}

UOSInt Crypto::Encrypt::BlockCipher::GetDecBlockSize() const
{
	return this->blockSize;
}

void Crypto::Encrypt::BlockCipher::SetChainMode(ChainMode cm)
{
	this->cm = cm;
}

void Crypto::Encrypt::BlockCipher::SetIV(UnsafeArray<const UInt8> iv)
{
	MemCopyNO(this->iv, iv.Ptr(), this->blockSize);
}

Text::CStringNN Crypto::Encrypt::ChainModeGetName(ChainMode cm)
{
	switch (cm)
	{
	case ChainMode::ECB:
		return CSTR("ECB");
	case ChainMode::CBC:
		return CSTR("CBC");
	case ChainMode::PCBC:
		return CSTR("PCBC");
	case ChainMode::CFB:
		return CSTR("CFB");
	case ChainMode::OFB:
		return CSTR("OFB");
	default:
		return CSTR("Unknown");
	}
}
