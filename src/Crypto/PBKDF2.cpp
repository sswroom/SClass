#include "Stdafx.h"
#include "MyMemory.h"
#include "Crypto/PBKDF2.h"
#include "Data/ByteTool.h"

UOSInt Crypto::PBKDF2::F(const UInt8 *salt, UOSInt saltLen, UOSInt iterationCount, UInt32 i, Crypto::Hash::IHash *hashFunc, UInt8 *outBuff)
{
	UOSInt resSize = hashFunc->GetResultSize();
	UInt8 iBuff[4];
	WriteMUInt32(iBuff, i);
	UInt8 *tmpBuff = MemAlloc(UInt8, resSize);
	hashFunc->Clear();
	hashFunc->Calc(salt, saltLen);
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
	MemFree(tmpBuff);
	return resSize;
}

UOSInt Crypto::PBKDF2::Calc(const UInt8 *salt, UOSInt saltLen, UOSInt iterationCount, UOSInt dkLen, Crypto::Hash::IHash *hashFunc, UInt8 *outBuff)
{
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
			UInt8 *blockBuff = MemAlloc(UInt8, resSize);
			F(salt, saltLen, iterationCount, i, hashFunc, blockBuff);
			MemCopyNO(outBuff, blockBuff, dkLen);
			dkLen = 0;
			MemFree(blockBuff);
			break;
		}
		i++;
	}
	return retLen;
}
