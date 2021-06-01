#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "Crypto/Hash/SHA512.h"
#include "Text/MyString.h"

extern "C"
{
	void SHA512_CalcBlock(UInt64 *intermediateHash, const UInt8 *messageBlock);
}

Crypto::Hash::SHA512::SHA512()
{
	this->intermediateHash = MemAlloc(UInt64, 8);
	this->messageBlock = MemAlloc(UInt8, 128);
	Clear();
}

Crypto::Hash::SHA512::~SHA512()
{
	MemFree(this->intermediateHash);
	MemFree(this->messageBlock);
}

UTF8Char *Crypto::Hash::SHA512::GetName(UTF8Char *sbuff)
{
	return Text::StrConcat(sbuff, (const UTF8Char*)"SHA-512");
}

Crypto::Hash::IHash *Crypto::Hash::SHA512::Clone()
{
	Crypto::Hash::SHA512 *sha512;
	NEW_CLASS(sha512, Crypto::Hash::SHA512());
	sha512->messageLength = this->messageLength;
	sha512->messageBlockIndex = this->messageBlockIndex;
    
	sha512->intermediateHash[0] = this->intermediateHash[0];
	sha512->intermediateHash[1] = this->intermediateHash[1];
	sha512->intermediateHash[2] = this->intermediateHash[2];
	sha512->intermediateHash[3] = this->intermediateHash[3];
	sha512->intermediateHash[4] = this->intermediateHash[4];
	sha512->intermediateHash[5] = this->intermediateHash[5];
	sha512->intermediateHash[6] = this->intermediateHash[6];
	sha512->intermediateHash[7] = this->intermediateHash[7];
	return sha512;
}
void Crypto::Hash::SHA512::Clear()
{
	this->messageLength        = 0;
	this->messageBlockIndex    = 0;
    
	this->intermediateHash[0]   = 0x6a09e667f3bcc908;
	this->intermediateHash[1]   = 0xbb67ae8584caa73b;
	this->intermediateHash[2]   = 0x3c6ef372fe94f82b;
	this->intermediateHash[3]   = 0xa54ff53a5f1d36f1;
	this->intermediateHash[4]   = 0x510e527fade682d1;
	this->intermediateHash[5]   = 0x9b05688c2b3e6c1f;
	this->intermediateHash[6]   = 0x1f83d9abfb41bd6b;
	this->intermediateHash[7]   = 0x5be0cd19137e2179;
}

void Crypto::Hash::SHA512::Calc(const UInt8 *buff, UOSInt buffSize)
{
	this->messageLength += (buffSize << 3);
	if ((buffSize + this->messageBlockIndex) < 128)
	{
		MemCopyNO(&this->messageBlock[this->messageBlockIndex], buff, buffSize);
		this->messageBlockIndex += buffSize;
		return;
	}
    
	if (this->messageBlockIndex > 0)
	{
		MemCopyNO(&this->messageBlock[this->messageBlockIndex], buff, 128 - this->messageBlockIndex);
		SHA512_CalcBlock(this->intermediateHash, this->messageBlock);
		buff += 128 - this->messageBlockIndex;
		buffSize -= 128 - this->messageBlockIndex;
		this->messageBlockIndex = 0;
	}

	while (buffSize >= 128)
	{
		SHA512_CalcBlock(this->intermediateHash, buff);
		buff += 128;
		buffSize -= 128;
	}
	if (buffSize > 0)
	{
		MemCopyNO(this->messageBlock, buff, this->messageBlockIndex = buffSize);
	}
}

void Crypto::Hash::SHA512::GetValue(UInt8 *buff)
{
	UInt8 calBuff[128];
	UInt64 intHash[8];
	MemCopyNO(intHash, this->intermediateHash, 64);

	UOSInt i;
	if (this->messageBlockIndex < 111)
	{
		MemCopyNO(calBuff, this->messageBlock, messageBlockIndex);
		i = messageBlockIndex;
		calBuff[i++] = 0x80;
		while (i < 120)
		{
			calBuff[i++] = 0;
		}

	}
	else
	{
		MemCopyNO(calBuff, this->messageBlock, messageBlockIndex);
		i = messageBlockIndex;
		calBuff[i++] = 0x80;
		while (i < 128)
		{
			calBuff[i++] = 0;
		}
		SHA512_CalcBlock(this->intermediateHash, calBuff);

		MemClear(calBuff, 120);
	}

	UInt64 msgLeng = this->messageLength;
	WriteMUInt64(&calBuff[120], msgLeng);
	SHA512_CalcBlock(this->intermediateHash, calBuff);
	UInt8 *res = (UInt8*)this->intermediateHash;
	i = 64;
	while (i > 0)
	{
		i -= 8;
		buff[i + 0] = res[i + 7];
		buff[i + 1] = res[i + 6];
		buff[i + 2] = res[i + 5];
		buff[i + 3] = res[i + 4];
		buff[i + 4] = res[i + 3];
		buff[i + 5] = res[i + 2];
		buff[i + 6] = res[i + 1];
		buff[i + 7] = res[i + 0];
	}
	MemCopyNO(this->intermediateHash, intHash, 64);
}

UOSInt Crypto::Hash::SHA512::GetBlockSize()
{
	return 128;
}

UOSInt Crypto::Hash::SHA512::GetResultSize()
{
	return 64;
}
