#include "Stdafx.h"
#include "MyMemory.h"
#include "Crypto/PBKDF2.h"
#include "Core/ByteTool_C.h"

UIntOS Crypto::PBKDF2::F(UnsafeArray<const UInt8> salt, UIntOS saltLen, UIntOS iterationCount, UInt32 i, NN<Crypto::Hash::HashAlgorithm> hashFunc, UnsafeArray<UInt8> outBuff)
{
	UInt8 tmpBuff[64];
	UIntOS resSize = hashFunc->GetResultSize();
	UInt8 iBuff[4];
	WriteMUInt32(iBuff, i);
	hashFunc->Clear();
	hashFunc->Calc(salt.Ptr(), saltLen);
	hashFunc->Calc(iBuff, 4);
	hashFunc->GetValue(outBuff);
	MemCopyNO(tmpBuff, outBuff.Ptr(), resSize);
	i = 1;
	while (i < iterationCount)
	{
		i++;
		hashFunc->Clear();
		hashFunc->Calc(tmpBuff, resSize);
		hashFunc->GetValue(tmpBuff);
		MemXOR(outBuff.Ptr(), tmpBuff, outBuff.Ptr(), resSize);
	}
	return resSize;
}

UIntOS Crypto::PBKDF2::Calc(UnsafeArray<const UInt8> salt, UIntOS saltLen, UIntOS iterationCount, UIntOS dkLen, NN<Crypto::Hash::HashAlgorithm> hashFunc, UnsafeArray<UInt8> outBuff)
{
	UInt8 blockBuff[64];
	UIntOS retLen = dkLen;
	UIntOS resSize = hashFunc->GetResultSize();
	UInt32 i = 1;
	while (dkLen > 0)
	{
		if (dkLen >= resSize)
		{
			F(salt, saltLen, iterationCount, i, hashFunc, outBuff);
			outBuff += resSize;
			dkLen -= resSize;
		}
		else
		{
			F(salt, saltLen, iterationCount, i, hashFunc, blockBuff);
			MemCopyNO(outBuff.Ptr(), blockBuff, dkLen);
			dkLen = 0;
			break;
		}
		i++;
	}
	return retLen;
}
