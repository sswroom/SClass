#include "Stdafx.h"
#include "MemTool.h"
#include "MyMemory.h"
#include "SIMD.h"
#include "Crypto/Encrypt/BlockCipher.h"
#include "Text/StringBuilderUTF8.h"

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
	if (this->blockSize != 16)
		return 0;
	else
	{
/*		UInt8 encBlk[16];
		UInt8 gcmBuff[64];
		blk = gcmBuff + 48;
		outSize = 0;
		MemClear(gcmBuff, 64);
		EncryptBlock(blk, gcmBuff + 16);
		MemCopyNO(blk.Ptr(), this->iv.Ptr(), this->ivSize);
		blk[15] = 1;
		EncryptBlock(blk, gcmBuff + 32);
		UnsafeArray<UInt8> aad;
		if (this->aad.SetTo(aad))
		{
			GhashUpdate(gcmBuff, aad, this->aadSize);
		}
		while (inSize > 0)
		{
			UIntOS i = 16;
			while (i-- > 0)
			{
				if (++(blk[i]) != 0)
					break;
			}
			EncryptBlock(blk, encBlk);

			blkCnt++;
			if (inSize >= 16)
			{
				PStoreUInt8x16(&outBuff[0], PXORUB16(PLoadUInt8x16(&encBlk[0]), PLoadUInt8x16(&inBuff[0])));
//				WriteNUInt64(&outBuff[0], ReadNUInt64(&encBlk[0]) ^ ReadNUInt64(&inBuff[0]));
//				WriteNUInt64(&outBuff[8], ReadNUInt64(&encBlk[8]) ^ ReadNUInt64(&inBuff[8]));
				GhashUpdateBlock(gcmBuff, outBuff);
				//MemXOR(outBuff.Ptr(), inBuff.Ptr(), outBuff.Ptr(), 16);
				inBuff += 16;
				outBuff += 16;
				inSize = inSize - 16;
				outSize += 16;
			}
			else
			{
				MemXOR(encBlk, inBuff.Ptr(), outBuff.Ptr(), inSize);
				GhashUpdateBlockPadding(gcmBuff, outBuff, inSize);
				outSize += inSize;
				outBuff += inSize;
				break;
			}
		}
		WriteMUInt64(&blk[0], (UInt64)this->aadSize * 8);
		WriteMUInt64(&blk[8], (UInt64)outSize * 8);
		GhashUpdateBlock(gcmBuff, blk);

		WriteNUInt64(&outBuff[0], ReadNUInt64(&gcmBuff[0]) ^ ReadNUInt64(&gcmBuff[32]));
		WriteNUInt64(&outBuff[8], ReadNUInt64(&gcmBuff[8]) ^ ReadNUInt64(&gcmBuff[40]));
		return outSize + 16;
*/
		UInt8 encBlk[16];
		UInt8x16 hash = PUInt8x16Clear();
		UInt8 hKeyBuff[16];
		UInt8x16 hKey;
		UInt8 gcmBuff3[32];
		blk = gcmBuff3 + 16;
		outSize = 0;
		MemClear(gcmBuff3, 32);
		EncryptBlock(blk, hKeyBuff);
		hKey = PUInt64x2Set(ReadMUInt64(&hKeyBuff[8]), ReadMUInt64(&hKeyBuff[0]));
		MemCopyNO(blk.Ptr(), this->iv.Ptr(), this->ivSize);
		blk[15] = 1;
		EncryptBlock(blk, gcmBuff3);
		UnsafeArray<UInt8> aad;
		if (this->aad.SetTo(aad))
		{
			hash = GhashUpdateSIMD(hash, hKey, aad, this->aadSize);
		}
		while (inSize > 0)
		{
			UIntOS i = 16;
			while (i-- > 0)
			{
				if (++(blk[i]) != 0)
					break;
			}
			EncryptBlock(blk, encBlk);

			blkCnt++;
			if (inSize >= 16)
			{
				PStoreUInt8x16(&outBuff[0], PXORUB16(PLoadUInt8x16(&encBlk[0]), PLoadUInt8x16(&inBuff[0])));
				hash = GhashUpdateSIMD(hash, hKey, outBuff, 16);
				inBuff += 16;
				outBuff += 16;
				inSize = inSize - 16;
				outSize += 16;
			}
			else
			{
				MemXOR(encBlk, inBuff.Ptr(), outBuff.Ptr(), inSize);
				hash = GhashUpdateBlockPaddingSIMD(hash, hKey, outBuff, inSize);
				outSize += inSize;
				outBuff += inSize;
				break;
			}
		}
		WriteMUInt64(&blk[0], (UInt64)this->aadSize * 8);
		WriteMUInt64(&blk[8], (UInt64)outSize * 8);
		hash = GhashUpdateSIMD(hash, hKey, blk, 16);
		PStoreUInt8x16(&outBuff[0], PXORUB16(hash, PLoadUInt8x16(&gcmBuff3[0])));
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
		if (this->blockSize != 16)
			return 0;
		else
		{
			inSize -= 16;
			UInt8 blk[16];
			UInt8 encBlk[16];
			outSize = 0;
			MemCopyNO(blk, this->iv.Ptr(), this->ivSize);
			MemClear(blk + this->ivSize, 16 - this->ivSize);
			blk[15] = 1;
			while (inSize > 0)
			{
				UIntOS i = 16;
				while (i-- > 12)
				{
					if (++(blk[i]) != 0)
						break;
				}
				EncryptBlock(blk, encBlk);
				blkCnt++;
				if (inSize >= 16)
				{
					WriteNUInt64(&outBuff[0], ReadNUInt64(&encBlk[0]) ^ ReadNUInt64(&inBuff[0]));
					WriteNUInt64(&outBuff[8], ReadNUInt64(&encBlk[8]) ^ ReadNUInt64(&inBuff[8]));
					inBuff += 16;
					outBuff += 16;
					inSize = inSize - 16;
					outSize += 16;
				}
				else
				{
					MemXOR(inBuff.Ptr(), encBlk, outBuff.Ptr(), inSize);
					outSize += inSize;
					break;
				}
			}
			return outSize;
		}
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
#if _OSINT_SIZE == 64
    UInt64 v1 = ReadMUInt64(&b[0]);
	UInt64 v2 = ReadMUInt64(&b[8]);
	UInt8x16 cTmp = PUInt8x16Clear();
	UIntOS i = 0;
	UIntOS j = 16 * 8;
    while (i < j)
	{
        if (a[i / 8] & (0x80 >> (i % 8))) 
		{
			cTmp = PXORUB16(cTmp, PUInt64x2Set(v1, v2));
		}
        UInt8 carry = v2 & 0x01;
		v2 = (v2 >> 1) | (v1 << 63);
		v1 = (v1 >> 1);
		if (carry) v1 ^= 0xE100000000000000LL; // GCM polynomial reduction
		i++;
    }
	PStoreUInt8x16(c.Ptr(), cTmp);
	WriteMUInt64(&c[0], ReadNUInt64(&c[0]));
	WriteMUInt64(&c[8], ReadNUInt64(&c[8]));
#else
    UInt8 v[16];
    MemClear(c.Ptr(), 16);
    MemCopyNO(v, b.Ptr(), 16);
	UIntOS i = 0;
	UIntOS j = 16 * 8;
	UIntOS k;
    while (i < j)
	{
        if (a[i / 8] & (0x80 >> (i % 8))) 
		{
			WriteNUInt64(&c[0], ReadNUInt64(&c[0]) ^ ReadNUInt64(&v[0]));
			WriteNUInt64(&c[8], ReadNUInt64(&c[8]) ^ ReadNUInt64(&v[8]));
        }
        UInt8 carry = v[k = 15] & 0x01;
		UInt32 v1 = ReadMUInt32(&v[12]);
		UInt32 v2 = ReadMUInt32(&v[8]);
		WriteMUInt32(&v[12], (v1 >> 1) | (v2 << 31));
		v1 = ReadMUInt32(&v[4]);
		WriteMUInt32(&v[8], (v2 >> 1) | (v1 << 31));
		v2 = ReadMUInt32(&v[0]);
		WriteMUInt32(&v[4], (v1 >> 1) | (v2 << 31));
		WriteMUInt32(&v[0], (v2 >> 1));
        if (carry) v[0] ^= 0xE1; // GCM polynomial reduction
		i++;
    }
#endif
}

void Crypto::Encrypt::BlockCipher::GhashUpdateBlock(UnsafeArray<UInt8> gcmBlk, UnsafeArray<const UInt8> inBuff)
{
	WriteNUInt64(&gcmBlk[0], ReadNUInt64(&gcmBlk[0]) ^ ReadNUInt64(&inBuff[0]));
	WriteNUInt64(&gcmBlk[8], ReadNUInt64(&gcmBlk[8]) ^ ReadNUInt64(&inBuff[8]));
	UInt8 nextHash[16];
	GFMult(&gcmBlk[0], &gcmBlk[16], nextHash);
	MemCopyNO(&gcmBlk[0], nextHash, 16);
}

void Crypto::Encrypt::BlockCipher::GhashUpdateBlockPadding(UnsafeArray<UInt8> gcmBlk, UnsafeArray<const UInt8> inBuff, UIntOS inSize)
{
	UInt8 tmpBuff[16];
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

UInt8x16 Crypto::Encrypt::BlockCipher::GFMultSIMD(UInt8x16 a, UInt8x16 b)
{
	UInt8x16 cTmp = PUInt8x16Clear();
	UInt8x16 carryXOR = PUInt64x2Set(0, 0xE100000000000000LL);
	UIntOS i = 0;
	UIntOS j = 16;
	UIntOS k;
	UInt8 aByte;
    while (i < j)
	{
		aByte = PEXTUB16(a, 0);
		a = PSHRBytes(a, 1);
		k = 0;
		while (k < 8)
		{
			if (aByte & (0x80 >> k)) 
			{
				cTmp = PXORUB16(cTmp, b);
			}
			if (PEXTUB16(b, 0) & 0x01)
			{
				b = PXORUB16(PSHRDQ(b, 1), carryXOR);
			}
			else
			{
				b = PSHRDQ(b, 1);
			}
			k++;
		}
		i++;
    }
	return BSWAP128(cTmp);
}

UInt8x16 Crypto::Encrypt::BlockCipher::GhashUpdateBlockSIMD(UInt8x16 hash, UInt8x16 hKey, UnsafeArray<const UInt8> inBuff)
{
	hash = PXORUB16(hash, PLoadUInt8x16(&inBuff[0]));
	return GFMultSIMD(hash, hKey);
}

UInt8x16 Crypto::Encrypt::BlockCipher::GhashUpdateBlockPaddingSIMD(UInt8x16 hash, UInt8x16 hKey, UnsafeArray<const UInt8> inBuff, UIntOS inSize)
{
	UInt8 tmpBuff[16];
	MemCopyNO(tmpBuff, inBuff.Ptr(), inSize);
	MemClear(&tmpBuff[inSize], 16 - inSize);
	return GhashUpdateBlockSIMD(hash, hKey, tmpBuff);
}

UInt8x16 Crypto::Encrypt::BlockCipher::GhashUpdateSIMD(UInt8x16 hash, UInt8x16 hKey, UnsafeArray<const UInt8> inBuff, UIntOS inSize)
{
	UIntOS i = 0;
	while (i < inSize)
	{
		if (inSize - i >= 16)
		{
			hash = GhashUpdateBlockSIMD(hash, hKey, &inBuff[i]);
			i += 16;
		}
		else
		{
			hash = GhashUpdateBlockPaddingSIMD(hash, hKey, &inBuff[i], inSize - i);
			break;
		}
	}
	return hash;
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
