#include "Stdafx.h"
#include "MemTool.h"
#include "MyMemory.h"
#include "Crypto/Encrypt/BlockCipher.h"

Crypto::Encrypt::BlockCipher::BlockCipher(UIntOS blockSize)
{
	this->blockSize = blockSize;
	this->cm = ChainMode::ECB;
	this->iv = MemAllocArr(UInt8, blockSize);
	this->pad = PaddingMode::None;
	MemClear(this->iv.Ptr(), this->blockSize);
}

Crypto::Encrypt::BlockCipher::~BlockCipher()
{
	MemFreeArr(this->iv);
}

UIntOS Crypto::Encrypt::BlockCipher::Encrypt(UnsafeArray<const UInt8> inBuff, UIntOS inSize, UnsafeArray<UInt8> outBuff)
{
	UnsafeArray<UInt8> blk;
	UIntOS outSize;
	UIntOS blkCnt = 0;
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
		if (pad == PaddingMode::None)
		{
			if (inSize > 0)
			{
				blk = MemAllocArr(UInt8, this->blockSize);
				MemClear(blk.Ptr(), this->blockSize);
				MemCopyNO(blk.Ptr(), inBuff.Ptr(), inSize);
				EncryptBlock(blk, outBuff);
				blkCnt++;
				MemFreeArr(blk);
			}
		}
		else
		{
			blk = MemAlloc(UInt8, this->blockSize);
			UIntOS i = this->blockSize;
			UIntOS j = this->blockSize - inSize;
			if (inSize > 0)
			{
				MemCopyNO(blk.Ptr(), inBuff.Ptr(), inSize);
				while (i-- > inSize)
				{
					blk[i] = (UInt8)j;
				}
				EncryptBlock(blk, outBuff);
			}
			else
			{
				while (i-- > 0)
				{
					blk[i] = (UInt8)this->blockSize;
				}
				EncryptBlock(blk, outBuff);
			}
			blkCnt++;
			MemFreeArr(blk);
		}
		return blkCnt * this->blockSize;
	case ChainMode::CBC:
		blk = MemAllocArr(UInt8, this->blockSize);
		MemCopyNO(blk.Ptr(), this->iv.Ptr(), this->blockSize);
		while (inSize >= blockSize)
		{
			MemXOR(blk.Ptr(), inBuff.Ptr(), blk.Ptr(), this->blockSize);
			EncryptBlock(blk, outBuff);
			MemCopyNO(blk.Ptr(), outBuff.Ptr(), this->blockSize);
			blkCnt++;
			inBuff += this->blockSize;
			outBuff += this->blockSize;
			inSize = inSize - this->blockSize;
		}
		if (pad == PaddingMode::None)
		{
			if (inSize > 0)
			{
				MemXOR(blk.Ptr(), inBuff.Ptr(), blk.Ptr(), inSize);
				EncryptBlock(blk, outBuff);
				blkCnt++;
			}
		}
		else
		{
			if (inSize > 0)
			{
				MemXOR(blk.Ptr(), inBuff.Ptr(), blk.Ptr(), inSize);
			}
			UIntOS i = this->blockSize;
			UIntOS j = this->blockSize - inSize;
			while (i-- > inSize)
			{
				blk[i] ^= (UInt8)j;
			}
			EncryptBlock(blk, outBuff);
			blkCnt++;
		}
		MemFreeArr(blk);
		return blkCnt * this->blockSize;
	case ChainMode::PCBC:
		blk = MemAllocArr(UInt8, this->blockSize);
		MemCopyNO(blk.Ptr(), this->iv.Ptr(), this->blockSize);
		while (inSize >= blockSize)
		{
			MemXOR(blk.Ptr(), inBuff.Ptr(), blk.Ptr(), this->blockSize);
			EncryptBlock(inBuff, outBuff);
			MemXOR(inBuff.Ptr(), outBuff.Ptr(), blk.Ptr(), this->blockSize);
			blkCnt++;
			inBuff += this->blockSize;
			outBuff += this->blockSize;
			inSize = inSize - this->blockSize;
		}
		if (pad == PaddingMode::None)
		{
			if (inSize > 0)
			{
				MemXOR(blk.Ptr(), inBuff.Ptr(), blk.Ptr(), inSize);
				EncryptBlock(inBuff, outBuff);
				blkCnt++;
			}
		}
		else
		{
			if (inSize > 0)
			{
				MemXOR(blk.Ptr(), inBuff.Ptr(), blk.Ptr(), inSize);
			}
			UIntOS i = this->blockSize;
			UIntOS j = this->blockSize - inSize;
			while (i-- > inSize)
			{
				blk[i] ^= (UInt8)j;
			}
			EncryptBlock(inBuff, outBuff);
			blkCnt++;
		}
		MemFreeArr(blk);
		return blkCnt * this->blockSize;
	case ChainMode::CFB:
		blk = MemAllocArr(UInt8, this->blockSize);
		outSize = 0;
		MemCopyNO(blk.Ptr(), this->iv.Ptr(), this->blockSize);
		while (inSize > 0)
		{
			EncryptBlock(blk, outBuff);
			blkCnt++;
			if (inSize >= this->blockSize)
			{
				MemXOR(outBuff.Ptr(), inBuff.Ptr(), outBuff.Ptr(), this->blockSize);
				MemCopyNO(blk.Ptr(), outBuff.Ptr(), this->blockSize);
				inBuff += this->blockSize;
				outBuff += this->blockSize;
				inSize = inSize - this->blockSize;
				outSize += this->blockSize;
			}
			else
			{
				MemXOR(outBuff.Ptr(), inBuff.Ptr(), outBuff.Ptr(), inSize);
				outSize += inSize;
			}
		}
		MemFreeArr(blk);
		return outSize;
	case ChainMode::OFB:
		blk = MemAllocArr(UInt8, this->blockSize);
		MemCopyNO(blk.Ptr(), this->iv.Ptr(), this->blockSize);
		outSize = 0;
		while (inSize > 0)
		{
			EncryptBlock(blk, outBuff);
			MemCopyNO(blk.Ptr(), outBuff.Ptr(), this->blockSize);
			if (inSize >= blockSize)
			{
				MemXOR(outBuff.Ptr(), inBuff.Ptr(), outBuff.Ptr(), this->blockSize);
				inBuff += this->blockSize;
				outBuff += this->blockSize;
				inSize = inSize - this->blockSize;
				outSize += this->blockSize;
			}
			else
			{
				MemXOR(outBuff.Ptr(), inBuff.Ptr(), outBuff.Ptr(), inSize);
				outSize += inSize;
				break;
			}
		}
		MemFreeArr(blk);
		return outSize;
	case ChainMode::CTR:
		blk = MemAllocArr(UInt8, this->blockSize);
		outSize = 0;
		MemCopyNO(blk.Ptr(), this->iv.Ptr(), this->blockSize);
		while (inSize > 0)
		{
			EncryptBlock(blk, outBuff);
			blkCnt++;
			if (inSize >= this->blockSize)
			{
				UIntOS i = this->blockSize;
				while (i-- > 0)
				{
					if (++(blk[i]) != 0)
						break;
				}
				MemXOR(outBuff.Ptr(), inBuff.Ptr(), outBuff.Ptr(), this->blockSize);
				inBuff += this->blockSize;
				outBuff += this->blockSize;
				inSize = inSize - this->blockSize;
				outSize += this->blockSize;
			}
			else
			{
				MemXOR(outBuff.Ptr(), inBuff.Ptr(), outBuff.Ptr(), inSize);
				outSize += inSize;
				break;
			}
		}
		MemFreeArr(blk);
		return outSize;
	case ChainMode::GCM:
		blk = MemAllocArr(UInt8, this->blockSize);
		outSize = 0;
		MemCopyNO(blk.Ptr(), this->iv.Ptr(), this->blockSize);
		while (inSize > 0)
		{
			UIntOS i = this->blockSize;
			while (i-- > 0)
			{
				if (++(blk[i]) != 0)
					break;
			}
			EncryptBlock(blk, outBuff);
			blkCnt++;
			if (inSize >= this->blockSize)
			{
				MemXOR(outBuff.Ptr(), inBuff.Ptr(), outBuff.Ptr(), this->blockSize);
				inBuff += this->blockSize;
				outBuff += this->blockSize;
				inSize = inSize - this->blockSize;
				outSize += this->blockSize;
			}
			else
			{
				MemXOR(outBuff.Ptr(), inBuff.Ptr(), outBuff.Ptr(), inSize);
				outSize += inSize;
				break;
			}
		}
		MemFreeArr(blk);
		return outSize;
	default:
		return 0;
	}
}

UIntOS Crypto::Encrypt::BlockCipher::Decrypt(UnsafeArray<const UInt8> inBuff, UIntOS inSize, UnsafeArray<UInt8> outBuff)
{
	UInt8 *blk;
	UInt8 *blk2;
	UInt8 *blkTmp;
	UIntOS blkCnt = 0;
	UIntOS outSize;
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
		if (pad == PaddingMode::None)
		{
			return blkCnt * this->blockSize;
		}
		else
		{
			return blkCnt * this->blockSize - outBuff[-1];
		}
	case ChainMode::CBC:
		if (inBuff != outBuff)
		{
			blk = MemAlloc(UInt8, this->blockSize);
			MemCopyNO(blk, this->iv.Ptr(), this->blockSize);
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
			MemCopyNO(blk, this->iv.Ptr(), this->blockSize);
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
		if (pad == PaddingMode::None)
		{
			return blkCnt * this->blockSize;
		}
		else
		{
			return blkCnt * this->blockSize - outBuff[-1];
		}
	case ChainMode::PCBC:
		if (inBuff != outBuff)
		{
			blk = MemAlloc(UInt8, this->blockSize);
			MemCopyNO(blk, this->iv.Ptr(), this->blockSize);
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
			MemCopyNO(blk, this->iv.Ptr(), this->blockSize);
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
		if (pad == PaddingMode::None)
		{
			return blkCnt * this->blockSize;
		}
		else
		{
			return blkCnt * this->blockSize - outBuff[-1];
		}
	case ChainMode::CFB:
		if (inBuff != outBuff)
		{
			blk = MemAlloc(UInt8, this->blockSize);
			outSize = 0;
			MemCopyNO(blk, this->iv.Ptr(), this->blockSize);
			while (inSize > 0)
			{
				EncryptBlock(blk, outBuff);
				blkCnt++;
				if (inSize >= this->blockSize)
				{
					MemCopyNO(blk, inBuff.Ptr(), this->blockSize);
					MemXOR(outBuff.Ptr(), inBuff.Ptr(), outBuff.Ptr(), this->blockSize);
					inBuff += this->blockSize;
					outBuff += this->blockSize;
					inSize = inSize - this->blockSize;
					outSize += this->blockSize;
				}
				else
				{
					MemXOR(outBuff.Ptr(), inBuff.Ptr(), outBuff.Ptr(), inSize);
					outSize += inSize;
					break;
				}
			}
			MemFree(blk);
		}
		else
		{
			blk = MemAlloc(UInt8, this->blockSize);
			blk2 = MemAlloc(UInt8, this->blockSize);
			outSize = 0;
			MemCopyNO(blk, this->iv.Ptr(), this->blockSize);
			while (inSize > 0)
			{
				MemCopyNO(blk2, inBuff.Ptr(), this->blockSize);
				EncryptBlock(blk, outBuff);
				blkCnt++;
				if (inSize >= this->blockSize)
				{
					MemXOR(outBuff.Ptr(), blk2, outBuff.Ptr(), this->blockSize);
					blkTmp = blk;
					blk = blk2;
					blk2 = blkTmp;
					inBuff += this->blockSize;
					outBuff += this->blockSize;
					inSize = inSize - this->blockSize;
					outSize += this->blockSize;
				}
				else
				{
					MemXOR(outBuff.Ptr(), blk2, outBuff.Ptr(), inSize);
					outSize += inSize;
					break;
				}
			}
			MemFree(blk);
			MemFree(blk2);
		}
		return outSize;
	case ChainMode::OFB:
		if (inBuff != outBuff)
		{
			blk = MemAlloc(UInt8, this->blockSize);
			outSize = 0;
			MemCopyNO(blk, this->iv.Ptr(), this->blockSize);
			while (inSize > 0)
			{
				EncryptBlock(blk, outBuff);
				blkCnt++;
				MemCopyNO(blk, outBuff.Ptr(), this->blockSize);
				if (inSize >= this->blockSize)
				{
					MemXOR(outBuff.Ptr(), inBuff.Ptr(), outBuff.Ptr(), this->blockSize);
					inBuff += this->blockSize;
					outBuff += this->blockSize;
					inSize = inSize - this->blockSize;
					outSize += this->blockSize;
				}
				else
				{
					MemXOR(outBuff.Ptr(), inBuff.Ptr(), outBuff.Ptr(), inSize);
					outSize += inSize;
					break;
				}
			}
			MemFree(blk);
			return outSize;
		}
		else
		{
			blk = MemAlloc(UInt8, this->blockSize);
			blk2 = MemAlloc(UInt8, this->blockSize);
			outSize = 0;
			MemCopyNO(blk, this->iv.Ptr(), this->blockSize);
			while (inSize > 0)
			{
				MemCopyNO(blk2, inBuff.Ptr(), this->blockSize);
				EncryptBlock(blk, outBuff);
				blkCnt++;
				MemCopyNO(blk, outBuff.Ptr(), this->blockSize);
				if (inSize >= this->blockSize)
				{
					MemXOR(outBuff.Ptr(), blk2, outBuff.Ptr(), this->blockSize);
					inBuff += this->blockSize;
					outBuff += this->blockSize;
					inSize = inSize - this->blockSize;
					outSize += this->blockSize;
				}
				else
				{
					MemXOR(outBuff.Ptr(), blk2, outBuff.Ptr(), inSize);
					outSize += inSize;
					break;
				}
			}
			MemFree(blk);
			MemFree(blk2);
		}
		return outSize;
	case ChainMode::CTR:
		if (inBuff != outBuff)
		{
			blk = MemAlloc(UInt8, this->blockSize);
			outSize = 0;
			MemCopyNO(blk, this->iv.Ptr(), this->blockSize);
			while (inSize > 0)
			{
				EncryptBlock(blk, outBuff);
				blkCnt++;
				if (inSize >= this->blockSize)
				{
					UIntOS i = this->blockSize;
					while (i-- > 0)
					{
						if (++(blk[i]) != 0)
							break;
					}
					MemXOR(outBuff.Ptr(), inBuff.Ptr(), outBuff.Ptr(), this->blockSize);
					inBuff += this->blockSize;
					outBuff += this->blockSize;
					inSize = inSize - this->blockSize;
					outSize += this->blockSize;
				}
				else
				{
					MemXOR(outBuff.Ptr(), inBuff.Ptr(), outBuff.Ptr(), inSize);
					outSize += inSize;
					break;
				}
			}
			MemFree(blk);
		}
		else
		{
			blk = MemAlloc(UInt8, this->blockSize);
			blk2 = MemAlloc(UInt8, this->blockSize);
			outSize = 0;
			MemCopyNO(blk, this->iv.Ptr(), this->blockSize);
			while (inSize > 0)
			{
				MemCopyNO(blk2, inBuff.Ptr(), this->blockSize);
				EncryptBlock(blk, outBuff);
				blkCnt++;
				if (inSize >= this->blockSize)
				{
					UIntOS i = this->blockSize;
					while (i-- > 0)
					{
						if (++(blk[i]) != 0)
							break;
					}
					MemXOR(outBuff.Ptr(), blk2, outBuff.Ptr(), this->blockSize);
					inBuff += this->blockSize;
					outBuff += this->blockSize;
					inSize = inSize - this->blockSize;
					outSize += this->blockSize;
				}
				else
				{
					MemXOR(outBuff.Ptr(), blk2, outBuff.Ptr(), inSize);
					outSize += inSize;
					break;
				}
			}
			MemFree(blk);
			MemFree(blk2);
		}
		return outSize;
	case ChainMode::GCM:
		inSize -= this->blockSize;
		if (inBuff != outBuff)
		{
			blk = MemAlloc(UInt8, this->blockSize);
			outSize = 0;
			MemCopyNO(blk, this->iv.Ptr(), this->blockSize);
			while (inSize > 0)
			{
				UIntOS i = this->blockSize;
				while (i-- > 12)
				{
					if (++(blk[i]) != 0)
						break;
				}
				EncryptBlock(blk, outBuff);
				blkCnt++;
				if (inSize >= this->blockSize)
				{
					MemXOR(outBuff.Ptr(), inBuff.Ptr(), outBuff.Ptr(), this->blockSize);
					inBuff += this->blockSize;
					outBuff += this->blockSize;
					inSize = inSize - this->blockSize;
					outSize += this->blockSize;
				}
				else
				{
					MemXOR(outBuff.Ptr(), inBuff.Ptr(), outBuff.Ptr(), inSize);
					outSize += inSize;
					break;
				}
			}
			MemFree(blk);
		}
		else
		{
			blk = MemAlloc(UInt8, this->blockSize);
			blk2 = MemAlloc(UInt8, this->blockSize);
			outSize = 0;
			MemCopyNO(blk, this->iv.Ptr(), this->blockSize);
			while (inSize > 0)
			{
				MemCopyNO(blk2, inBuff.Ptr(), this->blockSize);
				UIntOS i = this->blockSize;
				while (i-- > 12)
				{
					if (++(blk[i]) != 0)
						break;
				}
				EncryptBlock(blk, outBuff);
				blkCnt++;
				if (inSize >= this->blockSize)
				{
					MemXOR(outBuff.Ptr(), blk2, outBuff.Ptr(), this->blockSize);
					inBuff += this->blockSize;
					outBuff += this->blockSize;
					inSize = inSize - this->blockSize;
					outSize += this->blockSize;
				}
				else
				{
					MemXOR(outBuff.Ptr(), blk2, outBuff.Ptr(), inSize);
					outSize += inSize;
					break;
				}
			}
			MemFree(blk);
			MemFree(blk2);
		}
		return outSize;
	default:
		return 0;
	}
}

UIntOS Crypto::Encrypt::BlockCipher::GetEncBlockSize() const
{
	return this->blockSize;
}

UIntOS Crypto::Encrypt::BlockCipher::GetDecBlockSize() const
{
	return this->blockSize;
}

void Crypto::Encrypt::BlockCipher::SetChainMode(ChainMode cm)
{
	this->cm = cm;
}

void Crypto::Encrypt::BlockCipher::SetIV(UnsafeArray<const UInt8> iv)
{
	MemCopyNO(this->iv.Ptr(), iv.Ptr(), this->blockSize);
}

void Crypto::Encrypt::BlockCipher::SetPaddingMode(PaddingMode pad)
{
	this->pad = pad;
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
	case ChainMode::CTR:
		return CSTR("CTR");
	case ChainMode::GCM:
		return CSTR("GCM");
	default:
		return CSTR("Unknown");
	}
}

Text::CStringNN Crypto::Encrypt::PaddingModeGetName(PaddingMode pad)
{
	switch (pad)
	{
	case PaddingMode::None:
	default:
		return CSTR("None");
	case PaddingMode::PKCS7:
		return CSTR("PKCS7");
	}
}
