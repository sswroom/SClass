#include "Stdafx.h"
#include "Crypto/Encrypt/RSACipher.h"
#include "Crypto/Hash/HashCreator.h"
#include "Data/RandomBytesGenerator.h"

//RFC8017 OAEP
UOSInt Crypto::Encrypt::RSACipher::PaddingAppend(UnsafeArray<UInt8> destBuff, UOSInt destSize, UnsafeArray<const UInt8> message, UOSInt msgSize, Padding padding)
{
	if (padding == Padding::PKCS1_OAEP)
	{
		if (destSize > 256 || destSize == 0)
		{
			return 0;
		}
		Crypto::Hash::HashType hashType = Crypto::Hash::HashType::SHA1;
		NN<Crypto::Hash::HashAlgorithm> hash;
		if (!Crypto::Hash::HashCreator::CreateHash(hashType).SetTo(hash))
			return 0;
		UOSInt hLen = hash->GetResultSize();
		if (msgSize + 2 + hLen * 2 > destSize)
		{
			hash.Delete();
			return 0;
		}
		UInt8 tempBuff[256];
		Data::RandomBytesGenerator rnd;
		rnd.NextBytes(&tempBuff[1], hLen);
		hash->Clear();
		hash->GetValue(&tempBuff[1 + hLen]);
		MemCopyNO(&tempBuff[destSize - msgSize], message.Ptr(), msgSize);
		hash.Delete();
		UOSInt i = 1 + hLen * 2;
		UOSInt j = destSize - msgSize - 1;
		tempBuff[j] = 1;
		if (i < j)
		{
			MemClear(&tempBuff[i], j - i);
		}
		MGF1(&destBuff[1 + hLen], &tempBuff[1], hLen, destSize - 1 - hLen, hashType);
		i = 1 + hLen;
		while (i < destSize)
		{
			destBuff[i] = tempBuff[i] ^ destBuff[i];
			i++;
		}
		MGF1(&destBuff[1], &tempBuff[i + hLen], destSize - 1 - hLen, hLen, hashType);
		i = 0;
		while (i < hLen)
		{
			i++;
			destBuff[i] = destBuff[i] ^ tempBuff[i];
		}
		destBuff[0] = 0;
		return destSize;
	}
	else if (padding == Padding::NoPadding)
	{
		MemCopyNO(destBuff.Ptr(), message.Ptr(), msgSize);
		return msgSize;
	}
	else
	{
		return 0;
	}
}

UOSInt Crypto::Encrypt::RSACipher::PaddingRemove(UnsafeArray<UInt8> destBuff, UnsafeArray<const UInt8> blockWithPadding, UOSInt blockSize, Padding padding)
{
	if (padding == Padding::PKCS1_OAEP)
	{
		Crypto::Hash::HashType hashType = Crypto::Hash::HashType::SHA1;
		if (blockSize > 256)
		{
			return 0;
		}
		if (blockWithPadding[0] != 0)
		{
			return 0;
		}
		NN<Crypto::Hash::HashAlgorithm> hash;
		UOSInt i;
		UOSInt hLen;
		UInt8 decBuff[256];
		if (!Crypto::Hash::HashCreator::CreateHash(hashType).SetTo(hash))
		{
			return 0;
		}
		hLen = hash->GetResultSize();
		MGF1(&decBuff[1], &blockWithPadding[1 + hLen], blockSize - 1 - hLen, hLen, hashType);
		i = 0;
		while (i < hLen)
		{
			decBuff[i + 1] = decBuff[i + 1] ^ blockWithPadding[i + 1];
			i++;
		}
		MGF1(&decBuff[1 + hLen], &decBuff[1], hLen, blockSize - hLen - 1, hashType);
		i = 1 + hLen;
		while (i < blockSize)
		{
			decBuff[i] = decBuff[i] ^ blockWithPadding[i];
			i++;
		}
		hash.Delete();
		i = 1 + hLen + hLen;
		while (i < blockSize)
		{
			if (decBuff[i] == 0)
			{
				i++;
			}
			else if (decBuff[i] == 1)
			{
				i++;
				MemCopyNO(destBuff.Ptr(), &decBuff[i], blockSize - i);
				return blockSize - i;
			}
			else
			{
				return 0;
			}
		}
		return 0;
	}
	else
	{
		MemCopyNO(destBuff.Ptr(), blockWithPadding.Ptr(), blockSize);
		return blockSize;
	}
}


Bool Crypto::Encrypt::RSACipher::MGF1(UnsafeArray<UInt8> destBuff, UnsafeArray<const UInt8> seed, UOSInt seedLen, UOSInt len, Crypto::Hash::HashType hashType)
{
	NN<Crypto::Hash::HashAlgorithm> hash;
	if (!Crypto::Hash::HashCreator::CreateHash(hashType).SetTo(hash))
	{
		return false;
	}
	UInt8 tmpBuff[64];
	UOSInt hashSize = hash->GetResultSize();
	UInt32 counter = 0;
	while (len >= hashSize)
	{
		hash->Clear();
		hash->Calc(seed.Ptr(), seedLen);
		WriteMUInt32(tmpBuff, counter);
		hash->Calc(tmpBuff, 4);
		hash->GetValue(destBuff);
		counter++;
		destBuff += hashSize;
		len -= hashSize;
	}
	if (len > 0)
	{
		hash->Clear();
		hash->Calc(seed.Ptr(), seedLen);
		WriteMUInt32(tmpBuff, counter);
		hash->Calc(tmpBuff, 4);
		hash->GetValue(tmpBuff);
		MemCopyNO(destBuff.Ptr(), tmpBuff, len);
	}
	hash.Delete();
	return true;
}

Text::CStringNN Crypto::Encrypt::RSACipher::PaddingGetName(Padding rsaPadding)
{
	switch (rsaPadding)
	{
	case Padding::PKCS1:
		return CSTR("PKCS1");
	case Padding::NoPadding:
		return CSTR("NoPadding");
	case Padding::PKCS1_OAEP:
		return CSTR("PKCS1_OAEP");
	case Padding::X931:
		return CSTR("X931");
	case Padding::PKCS1_PSS:
		return CSTR("PKCS1_PSS");
	case Padding::PKCS1_WithTLS:
		return CSTR("PKCS1_WithTLS");
	default:
		return CSTR("Unknown");
	}
}
