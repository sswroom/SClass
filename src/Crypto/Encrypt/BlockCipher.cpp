#include "Stdafx.h"
#include "MemTool.h"
#include "MyMemory.h"
#include "Crypto/Encrypt/BlockCipher.h"

Crypto::Encrypt::BlockCipher::BlockCipher(UIntOS blockSize) : BlockCipher(blockSize, blockSize)
{
}

Crypto::Encrypt::BlockCipher::BlockCipher(UIntOS blockSize, UIntOS ivSize)
{
	this->blockSize = blockSize;
	this->ivSize = ivSize;
	this->cm = ChainMode::ECB;
	this->iv = MemAllocArr(UInt8, ivSize);
	this->pad = PaddingMode::None;
	MemClear(this->iv.Ptr(), this->ivSize);
	this->aad = nullptr;
	this->aadSize = 0;
}

Crypto::Encrypt::BlockCipher::~BlockCipher()
{
	MemFreeArr(this->iv);
	UnsafeArray<UInt8> blk;
	if (this->aad.SetTo(blk))
	{
		MemFreeArr(blk);
	}
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
			blk = MemAllocArr(UInt8, this->blockSize);
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
		MemCopyNO(blk.Ptr(), this->iv.Ptr(), this->ivSize);
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
		MemCopyNO(blk.Ptr(), this->iv.Ptr(), this->ivSize);
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
		MemCopyNO(blk.Ptr(), this->iv.Ptr(), this->ivSize);
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
		MemCopyNO(blk.Ptr(), this->iv.Ptr(), this->ivSize);
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
		MemCopyNO(blk.Ptr(), this->iv.Ptr(), this->ivSize);
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
	{
		blk = MemAllocArr(UInt8, this->blockSize * 4);
		outSize = 0;
		MemCopyNO(blk.Ptr(), this->iv.Ptr(), this->ivSize);
		MemClear(blk.Ptr() + this->ivSize, this->blockSize + 32 - this->ivSize);
		blk[this->blockSize - 1] = 1;
		EncryptBlock(blk + this->blockSize, blk + this->blockSize * 2);
		EncryptBlock(blk, &outBuff[inSize]);
		UnsafeArray<UInt8> aad;
		if (this->aad.SetTo(aad))
		{
			GhashUpdate(blk + this->blockSize, aad, this->aadSize);
		}
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
				GhashUpdate(blk + this->blockSize, outBuff, this->blockSize);
				inBuff += this->blockSize;
				outBuff += this->blockSize;
				inSize = inSize - this->blockSize;
				outSize += this->blockSize;
			}
			else
			{
				MemXOR(outBuff.Ptr(), inBuff.Ptr(), outBuff.Ptr(), inSize);
				GhashUpdate(blk + this->blockSize, outBuff, inSize);
				outSize += inSize;
				outBuff += inSize;
				break;
			}
		}
		WriteMUInt64(&blk[0], (UInt64)this->aadSize * 8);
		WriteMUInt64(&blk[8], (UInt64)outSize * 8);
		GhashUpdateBlock(blk + this->blockSize, blk);
		MemXOR(outBuff.Ptr(), &blk[this->blockSize], outBuff.Ptr(), 16);
		MemFreeArr(blk);
		return outSize + 16;
	}
	default:
		return 0;
	}
}

UIntOS Crypto::Encrypt::BlockCipher::Decrypt(UnsafeArray<const UInt8> inBuff, UIntOS inSize, UnsafeArray<UInt8> outBuff)
{
	UnsafeArray<UInt8> blk;
	UnsafeArray<UInt8> blk2;
	UnsafeArray<UInt8> blkTmp;
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
			blk = MemAllocArr(UInt8, this->blockSize);
			MemCopyNO(blk.Ptr(), this->iv.Ptr(), this->ivSize);
			while (inSize >= this->blockSize)
			{
				DecryptBlock(inBuff, outBuff);
				blkCnt++;
				MemXOR(outBuff.Ptr(), blk.Ptr(), outBuff.Ptr(), this->blockSize);
				MemCopyNO(blk.Ptr(), inBuff.Ptr(), this->blockSize);
				inBuff += this->blockSize;
				outBuff += this->blockSize;
				inSize = inSize - this->blockSize;
			}
			MemFreeArr(blk);
		}
		else
		{
			blk = MemAllocArr(UInt8, this->blockSize);
			blk2 = MemAllocArr(UInt8, this->blockSize);
			MemCopyNO(blk.Ptr(), this->iv.Ptr(), this->ivSize);
			while (inSize >= this->blockSize)
			{
				MemCopyNO(blk2.Ptr(), inBuff.Ptr(), this->blockSize);
				DecryptBlock(inBuff, outBuff);
				blkCnt++;
				MemXOR(outBuff.Ptr(), blk.Ptr(), outBuff.Ptr(), this->blockSize);
				blkTmp = blk;
				blk = blk2;
				blk2 = blkTmp;
				inBuff += this->blockSize;
				outBuff += this->blockSize;
				inSize = inSize - this->blockSize;
			}
			MemFreeArr(blk);
			MemFreeArr(blk2);
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
			blk = MemAllocArr(UInt8, this->blockSize);
			MemCopyNO(blk.Ptr(), this->iv.Ptr(), this->ivSize);
			while (inSize >= this->blockSize)
			{
				DecryptBlock(inBuff, outBuff);
				blkCnt++;
				MemXOR(outBuff.Ptr(), blk.Ptr(), outBuff.Ptr(), this->blockSize);
				MemXOR(inBuff.Ptr(), outBuff.Ptr(), blk.Ptr(), this->blockSize);
				inBuff += this->blockSize;
				outBuff += this->blockSize;
				inSize = inSize - this->blockSize;
			}
			MemFreeArr(blk);
		}
		else
		{
			blk = MemAllocArr(UInt8, this->blockSize);
			blk2 = MemAllocArr(UInt8, this->blockSize);
			MemCopyNO(blk.Ptr(), this->iv.Ptr(), this->ivSize);
			while (inSize >= this->blockSize)
			{
				MemCopyNO(blk2.Ptr(), inBuff.Ptr(), this->blockSize);
				DecryptBlock(inBuff, outBuff);
				blkCnt++;
				MemXOR(outBuff.Ptr(), blk.Ptr(), outBuff.Ptr(), this->blockSize);
				MemXOR(blk2.Ptr(), outBuff.Ptr(), blk.Ptr(), this->blockSize);
				inBuff += this->blockSize;
				outBuff += this->blockSize;
				inSize = inSize - this->blockSize;
			}
			MemFreeArr(blk);
			MemFreeArr(blk2);
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
			blk = MemAllocArr(UInt8, this->blockSize);
			outSize = 0;
			MemCopyNO(blk.Ptr(), this->iv.Ptr(), this->ivSize);
			while (inSize > 0)
			{
				EncryptBlock(blk, outBuff);
				blkCnt++;
				if (inSize >= this->blockSize)
				{
					MemCopyNO(blk.Ptr(), inBuff.Ptr(), this->blockSize);
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
		}
		else
		{
			blk = MemAllocArr(UInt8, this->blockSize);
			blk2 = MemAllocArr(UInt8, this->blockSize);
			outSize = 0;
			MemCopyNO(blk.Ptr(), this->iv.Ptr(), this->ivSize);
			while (inSize > 0)
			{
				MemCopyNO(blk2.Ptr(), inBuff.Ptr(), this->blockSize);
				EncryptBlock(blk, outBuff);
				blkCnt++;
				if (inSize >= this->blockSize)
				{
					MemXOR(outBuff.Ptr(), blk2.Ptr(), outBuff.Ptr(), this->blockSize);
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
					MemXOR(outBuff.Ptr(), blk2.Ptr(), outBuff.Ptr(), inSize);
					outSize += inSize;
					break;
				}
			}
			MemFreeArr(blk);
			MemFreeArr(blk2);
		}
		return outSize;
	case ChainMode::OFB:
		if (inBuff != outBuff)
		{
			blk = MemAllocArr(UInt8, this->blockSize);
			outSize = 0;
			MemCopyNO(blk.Ptr(), this->iv.Ptr(), this->ivSize);
			while (inSize > 0)
			{
				EncryptBlock(blk, outBuff);
				blkCnt++;
				MemCopyNO(blk.Ptr(), outBuff.Ptr(), this->blockSize);
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
		}
		else
		{
			blk = MemAllocArr(UInt8, this->blockSize);
			blk2 = MemAllocArr(UInt8, this->blockSize);
			outSize = 0;
			MemCopyNO(blk.Ptr(), this->iv.Ptr(), this->ivSize);
			while (inSize > 0)
			{
				MemCopyNO(blk2.Ptr(), inBuff.Ptr(), this->blockSize);
				EncryptBlock(blk, outBuff);
				blkCnt++;
				MemCopyNO(blk.Ptr(), outBuff.Ptr(), this->blockSize);
				if (inSize >= this->blockSize)
				{
					MemXOR(outBuff.Ptr(), blk2.Ptr(), outBuff.Ptr(), this->blockSize);
					inBuff += this->blockSize;
					outBuff += this->blockSize;
					inSize = inSize - this->blockSize;
					outSize += this->blockSize;
				}
				else
				{
					MemXOR(outBuff.Ptr(), blk2.Ptr(), outBuff.Ptr(), inSize);
					outSize += inSize;
					break;
				}
			}
			MemFreeArr(blk);
			MemFreeArr(blk2);
		}
		return outSize;
	case ChainMode::CTR:
		if (inBuff != outBuff)
		{
			blk = MemAllocArr(UInt8, this->blockSize);
			outSize = 0;
			MemCopyNO(blk.Ptr(), this->iv.Ptr(), this->ivSize);
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
		}
		else
		{
			blk = MemAllocArr(UInt8, this->blockSize);
			blk2 = MemAllocArr(UInt8, this->blockSize);
			outSize = 0;
			MemCopyNO(blk.Ptr(), this->iv.Ptr(), this->ivSize);
			while (inSize > 0)
			{
				MemCopyNO(blk2.Ptr(), inBuff.Ptr(), this->blockSize);
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
					MemXOR(outBuff.Ptr(), blk2.Ptr(), outBuff.Ptr(), this->blockSize);
					inBuff += this->blockSize;
					outBuff += this->blockSize;
					inSize = inSize - this->blockSize;
					outSize += this->blockSize;
				}
				else
				{
					MemXOR(outBuff.Ptr(), blk2.Ptr(), outBuff.Ptr(), inSize);
					outSize += inSize;
					break;
				}
			}
			MemFreeArr(blk);
			MemFreeArr(blk2);
		}
		return outSize;
	case ChainMode::GCM:
		inSize -= this->blockSize;
		if (inBuff != outBuff)
		{
			blk = MemAllocArr(UInt8, this->blockSize);
			outSize = 0;
			MemCopyNO(blk.Ptr(), this->iv.Ptr(), this->ivSize);
			MemClear(blk.Ptr() + this->ivSize, this->blockSize - this->ivSize);
			blk[this->blockSize - 1] = 1;
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
			MemFreeArr(blk);
		}
		else
		{
			blk = MemAllocArr(UInt8, this->blockSize);
			blk2 = MemAllocArr(UInt8, this->blockSize);
			outSize = 0;
			MemCopyNO(blk.Ptr(), this->iv.Ptr(), this->ivSize);
			MemClear(blk.Ptr() + this->ivSize, this->blockSize - this->ivSize);
			blk[this->blockSize - 1] = 1;
			while (inSize > 0)
			{
				MemCopyNO(blk2.Ptr(), inBuff.Ptr(), this->blockSize);
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
					MemXOR(outBuff.Ptr(), blk2.Ptr(), outBuff.Ptr(), this->blockSize);
					inBuff += this->blockSize;
					outBuff += this->blockSize;
					inSize = inSize - this->blockSize;
					outSize += this->blockSize;
				}
				else
				{
					MemXOR(outBuff.Ptr(), blk2.Ptr(), outBuff.Ptr(), inSize);
					outSize += inSize;
					break;
				}
			}
			MemFreeArr(blk);
			MemFreeArr(blk2);
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

UIntOS Crypto::Encrypt::BlockCipher::GetIVSize() const
{
	return this->ivSize;
}

void Crypto::Encrypt::BlockCipher::SetChainMode(ChainMode cm)
{
	this->cm = cm;
	if (this->cm == ChainMode::GCM)
	{
		this->ivSize = 12;
	}
}

void Crypto::Encrypt::BlockCipher::SetIV(UnsafeArray<const UInt8> iv)
{
	MemCopyNO(this->iv.Ptr(), iv.Ptr(), this->ivSize);
}

void Crypto::Encrypt::BlockCipher::SetPaddingMode(PaddingMode pad)
{
	this->pad = pad;
}

void Crypto::Encrypt::BlockCipher::SetAAD(UnsafeArray<const UInt8> aad, UIntOS aadSize)
{
	UnsafeArray<UInt8> blk;
	if (this->aad.SetTo(blk))
	{
		MemFreeArr(blk);
	}
	blk = MemAllocArr(UInt8, aadSize);
	MemCopyNO(blk.Ptr(), aad.Ptr(), aadSize);
	this->aad = blk;
	this->aadSize = aadSize;
}

void Crypto::Encrypt::BlockCipher::GFMult(UnsafeArray<const UInt8> a, UnsafeArray<const UInt8> b, UnsafeArray<UInt8> c)
{
    UInt8 v[64];
    MemClear(c.Ptr(), 16);
    MemCopyNO(v, b.Ptr(), 16);
	UIntOS i = 0;
	UIntOS j = 16 * 8;
	UIntOS k;
    while (i < j)
	{
        if (a[i / 8] & (0x80 >> (i % 8))) 
		{
			k = 0;
            while (k < 16)
			{
				c[k] ^= v[k];
				k++;
			}
        }
        UInt8 carry = v[k = 15] & 0x01;
		while (k > 0)
		{
            v[k] = (v[k] >> 1) | (v[k - 1] << 7);
			k--;
		}
        v[0] >>= 1;
        if (carry) v[0] ^= 0xE1; // GCM polynomial reduction
		i++;
    }
}

void Crypto::Encrypt::BlockCipher::GhashUpdateBlock(UnsafeArray<UInt8> gcmBlk, UnsafeArray<const UInt8> inBuff)
{
	UIntOS i = 0;
	while (i < 16)
	{
		gcmBlk[i] ^= inBuff[i];
		i++;
	}
	UInt8 nextHash[16];
	GFMult(&gcmBlk[0], &gcmBlk[16], nextHash);
	MemCopyNO(&gcmBlk[0], nextHash, 16);
}

void Crypto::Encrypt::BlockCipher::GhashUpdateBlockPadding(UnsafeArray<UInt8> gcmBlk, UnsafeArray<const UInt8> inBuff, UIntOS inSize)
{
	UInt8 tmpBuff[64];
	MemCopyNO(tmpBuff, inBuff.Ptr(), inSize);
	MemClear(&tmpBuff[inSize], 16 - inSize);
	GhashUpdateBlock(gcmBlk, tmpBuff);
}

void Crypto::Encrypt::BlockCipher::GhashUpdate(UnsafeArray<UInt8> gcmBlk, UnsafeArray<const UInt8> inBuff, UIntOS inSize)
{
	UIntOS i = 0;
	while (i < inSize)
	{
		if (inSize - i >= 16)
		{
			GhashUpdateBlock(gcmBlk, &inBuff[i]);
			i += 16;
		}
		else
		{
			GhashUpdateBlockPadding(gcmBlk, &inBuff[i], inSize - i);
			break;
		}
	}
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
