#include "Stdafx.h"
#include "Crypto/Encrypt/RSACipher.h"
#include "Crypto/Hash/HashCreator.h"

//RFC8017 OAEP
UOSInt Crypto::Encrypt::RSACipher::PaddingRemove(UInt8* destBuff, const UInt8* blockWithPadding, UOSInt blockSize, Padding padding)
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
		Crypto::Hash::IHash *hash = Crypto::Hash::HashCreator::CreateHash(hashType);
		UOSInt i;
		UOSInt hLen;
		UInt8 decBuff[256];
		if (hash == 0)
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
		DEL_CLASS(hash);
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
				MemCopyNO(destBuff, &decBuff[i], blockSize - i);
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
		MemCopyNO(destBuff, blockWithPadding, blockSize);
		return blockSize;
	}
}


Bool Crypto::Encrypt::RSACipher::MGF1(UInt8 *destBuff, const UInt8 *seed, UOSInt seedLen, UOSInt len, Crypto::Hash::HashType hashType)
{
	Crypto::Hash::IHash *hash = Crypto::Hash::HashCreator::CreateHash(hashType);
	if (hash == 0)
	{
		return false;
	}
	UInt8 tmpBuff[64];
	UOSInt hashSize = hash->GetResultSize();
	UInt32 counter = 0;
	while (len >= hashSize)
	{
		hash->Clear();
		hash->Calc(seed, seedLen);
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
		hash->Calc(seed, seedLen);
		WriteMUInt32(tmpBuff, counter);
		hash->Calc(tmpBuff, 4);
		hash->GetValue(tmpBuff);
		MemCopyNO(destBuff, tmpBuff, len);
	}
	DEL_CLASS(hash);
	return true;
}

Text::CString Crypto::Encrypt::RSACipher::PaddingGetName(Padding rsaPadding)
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
