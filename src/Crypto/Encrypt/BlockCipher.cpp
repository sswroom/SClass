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

UOSInt Crypto::Encrypt::BlockCipher::Encrypt(const UInt8 *inBuff, UOSInt inSize, UInt8 *outBuff)
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
			MemCopyNO(blk, inBuff, inSize);
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
			MemXOR(blk, inBuff, blk, this->blockSize);
			EncryptBlock(blk, outBuff);
			MemCopyNO(blk, outBuff, this->blockSize);
			blkCnt++;
			inBuff += this->blockSize;
			outBuff += this->blockSize;
			inSize = inSize - this->blockSize;
		}
		if (inSize > 0)
		{
			MemXOR(blk, inBuff, blk, inSize);
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
			MemXOR(blk, inBuff, blk, this->blockSize);
			EncryptBlock(inBuff, outBuff);
			MemXOR(inBuff, outBuff, blk, this->blockSize);
			blkCnt++;
			inBuff += this->blockSize;
			outBuff += this->blockSize;
			inSize = inSize - this->blockSize;
		}
		if (inSize > 0)
		{
			MemXOR(blk, inBuff, blk, inSize);
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
			MemXOR(outBuff, inBuff, outBuff, this->blockSize);
			MemCopyNO(blk, outBuff, this->blockSize);
			blkCnt++;
			inBuff += this->blockSize;
			outBuff += this->blockSize;
			inSize = inSize - this->blockSize;
		}
		if (inSize > 0)
		{
			EncryptBlock(blk, outBuff);
			MemXOR(outBuff, inBuff, outBuff, inSize);
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
			MemCopyNO(blk, outBuff, this->blockSize);
			MemXOR(outBuff, inBuff, outBuff, this->blockSize);
			blkCnt++;
			inBuff += this->blockSize;
			outBuff += this->blockSize;
			inSize = inSize - this->blockSize;
		}
		if (inSize > 0)
		{
			EncryptBlock(blk, outBuff);
			MemXOR(outBuff, inBuff, outBuff, inSize);
			blkCnt++;
		}
		MemFree(blk);
		return blkCnt * this->blockSize;
	default:
		return 0;
	}
}

UOSInt Crypto::Encrypt::BlockCipher::Decrypt(const UInt8 *inBuff, UOSInt inSize, UInt8 *outBuff)
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
				MemXOR(outBuff, blk, outBuff, this->blockSize);
				MemCopyNO(blk, inBuff, this->blockSize);
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
				MemCopyNO(blk2, inBuff, this->blockSize);
				DecryptBlock(inBuff, outBuff);
				blkCnt++;
				MemXOR(outBuff, blk, outBuff, this->blockSize);
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
				MemXOR(outBuff, blk, outBuff, this->blockSize);
				MemXOR(inBuff, outBuff, blk, this->blockSize);
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
				MemCopyNO(blk2, inBuff, this->blockSize);
				DecryptBlock(inBuff, outBuff);
				blkCnt++;
				MemXOR(outBuff, blk, outBuff, this->blockSize);
				MemXOR(blk2, outBuff, blk, this->blockSize);
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
				MemXOR(outBuff, inBuff, outBuff, this->blockSize);
				MemCopyNO(blk, inBuff, this->blockSize);
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
				MemCopyNO(blk2, inBuff, this->blockSize);
				DecryptBlock(blk, outBuff);
				blkCnt++;
				MemXOR(outBuff, blk2, outBuff, this->blockSize);
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
				MemCopyNO(blk, outBuff, this->blockSize);
				MemXOR(outBuff, inBuff, outBuff, this->blockSize);
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
				MemCopyNO(blk2, inBuff, this->blockSize);
				DecryptBlock(blk, outBuff);
				blkCnt++;
				MemCopyNO(blk, outBuff, this->blockSize);
				MemXOR(outBuff, blk2, outBuff, this->blockSize);
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

void Crypto::Encrypt::BlockCipher::SetIV(const UInt8 *iv)
{
	MemCopyNO(this->iv, iv, this->blockSize);
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
