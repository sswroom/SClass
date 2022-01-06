#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "Crypto/Hash/SHA256.h"
#include "Text/MyString.h"

extern "C"
{
	void SHA256_CalcBlock(UInt32 *intermediateHash, const UInt8 *messageBlock);
}

Crypto::Hash::SHA256::SHA256()
{
	this->intermediateHash = MemAlloc(UInt32, 8);
	this->messageBlock = MemAlloc(UInt8, 64);
	Clear();
}

Crypto::Hash::SHA256::~SHA256()
{
	MemFree(this->intermediateHash);
	MemFree(this->messageBlock);
}

UTF8Char *Crypto::Hash::SHA256::GetName(UTF8Char *sbuff)
{
	return Text::StrConcatC(sbuff, UTF8STRC("SHA-256"));
}

Crypto::Hash::IHash *Crypto::Hash::SHA256::Clone()
{
	Crypto::Hash::SHA256 *sha256;
	NEW_CLASS(sha256, Crypto::Hash::SHA256());
	sha256->messageLength = this->messageLength;
	sha256->messageBlockIndex = this->messageBlockIndex;
    
	sha256->intermediateHash[0] = this->intermediateHash[0];
	sha256->intermediateHash[1] = this->intermediateHash[1];
	sha256->intermediateHash[2] = this->intermediateHash[2];
	sha256->intermediateHash[3] = this->intermediateHash[3];
	sha256->intermediateHash[4] = this->intermediateHash[4];
	sha256->intermediateHash[5] = this->intermediateHash[5];
	sha256->intermediateHash[6] = this->intermediateHash[6];
	sha256->intermediateHash[7] = this->intermediateHash[7];
	return sha256;
}
void Crypto::Hash::SHA256::Clear()
{
	this->messageLength        = 0;
	this->messageBlockIndex    = 0;
    
	this->intermediateHash[0]   = 0x6a09e667;
	this->intermediateHash[1]   = 0xbb67ae85;
	this->intermediateHash[2]   = 0x3c6ef372;
	this->intermediateHash[3]   = 0xa54ff53a;
	this->intermediateHash[4]   = 0x510e527f;
	this->intermediateHash[5]   = 0x9b05688c;
	this->intermediateHash[6]   = 0x1f83d9ab;
	this->intermediateHash[7]   = 0x5be0cd19;
}

void Crypto::Hash::SHA256::Calc(const UInt8 *buff, UOSInt buffSize)
{
	this->messageLength += (buffSize << 3);
	if ((buffSize + this->messageBlockIndex) < 64)
	{
		MemCopyNO(&this->messageBlock[this->messageBlockIndex], buff, buffSize);
		this->messageBlockIndex += buffSize;
		return;
	}
    
	if (this->messageBlockIndex > 0)
	{
		MemCopyNO(&this->messageBlock[this->messageBlockIndex], buff, 64 - this->messageBlockIndex);
		SHA256_CalcBlock(this->intermediateHash, this->messageBlock);
		buff += 64 - this->messageBlockIndex;
		buffSize -= 64 - this->messageBlockIndex;
		this->messageBlockIndex = 0;
	}

	while (buffSize >= 64)
	{
		SHA256_CalcBlock(this->intermediateHash, buff);
		buff += 64;
		buffSize -= 64;
	}
	if (buffSize > 0)
	{
		MemCopyNO(this->messageBlock, buff, this->messageBlockIndex = buffSize);
	}
}

void Crypto::Hash::SHA256::GetValue(UInt8 *buff)
{
	UInt8 calBuff[64];
	UInt32 intHash[8];
	MemCopyNO(intHash, this->intermediateHash, 32);

	UOSInt i;
	if (this->messageBlockIndex < 55)
	{
		MemCopyNO(calBuff, this->messageBlock, messageBlockIndex);
		i = messageBlockIndex;
		calBuff[i++] = 0x80;
		while (i < 56)
		{
			calBuff[i++] = 0;
		}

	}
	else
	{
		MemCopyNO(calBuff, this->messageBlock, messageBlockIndex);
		i = messageBlockIndex;
		calBuff[i++] = 0x80;
		while (i < 64)
		{
			calBuff[i++] = 0;
		}
		SHA256_CalcBlock(this->intermediateHash, calBuff);

		MemClear(calBuff, 56);
	}

	UInt64 msgLeng = this->messageLength;
	WriteMUInt64(&calBuff[56], msgLeng);
	SHA256_CalcBlock(this->intermediateHash, calBuff);
	UInt8 *res = (UInt8*)this->intermediateHash;
	i = 32;
	while (i > 0)
	{
		i -= 4;
		buff[i + 0] = res[i + 3];
		buff[i + 1] = res[i + 2];
		buff[i + 2] = res[i + 1];
		buff[i + 3] = res[i + 0];
	}
	MemCopyNO(this->intermediateHash, intHash, 32);
}

UOSInt Crypto::Hash::SHA256::GetBlockSize()
{
	return 64;
}

UOSInt Crypto::Hash::SHA256::GetResultSize()
{
	return 32;
}
