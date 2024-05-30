#include "Stdafx.h"
#include "MyMemory.h"
#include "Crypto/PBKDF2.h"
#include "Data/ByteTool.h"

UOSInt Crypto::PBKDF2::F(UnsafeArray<const UInt8> salt, UOSInt saltLen, UOSInt iterationCount, UInt32 i, NN<Crypto::Hash::IHash> hashFunc, UInt8 *outBuff)
{
	UInt8 tmpBuff[64];
	UOSInt resSize = hashFunc->GetResultSize();
	UInt8 iBuff[4];
	WriteMUInt32(iBuff, i);
	hashFunc->Clear();
	hashFunc->Calc(salt.Ptr(), saltLen);
	hashFunc->Calc(iBuff, 4);
	hashFunc->GetValue(outBuff);
	MemCopyNO(tmpBuff, outBuff, resSize);
	i = 1;
	while (i < iterationCount)
	{
		i++;
		hashFunc->Clear();
		hashFunc->Calc(tmpBuff, resSize);
		hashFunc->GetValue(tmpBuff);
		MemXOR(outBuff, tmpBuff, outBuff, resSize);
	}
	return resSize;
}

UOSInt Crypto::PBKDF2::Calc(UnsafeArray<const UInt8> salt, UOSInt saltLen, UOSInt iterationCount, UOSInt dkLen, NN<Crypto::Hash::IHash> hashFunc, UInt8 *outBuff)
{
	UInt8 blockBuff[64];
	UOSInt retLen = dkLen;
	UOSInt resSize = hashFunc->GetResultSize();
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
			MemCopyNO(outBuff, blockBuff, dkLen);
			dkLen = 0;
			break;
		}
		i++;
	}
	return retLen;
}
