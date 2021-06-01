#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "Crypto/Hash/SHA384.h"
#include "Text/MyString.h"

extern "C"
{
	void SHA512_CalcBlock(UInt64 *intermediateHash, const UInt8 *messageBlock);
}

Crypto::Hash::SHA384::SHA384()
{
	this->intermediateHash = MemAlloc(UInt64, 8);
	this->messageBlock = MemAlloc(UInt8, 128);
	Clear();
}

Crypto::Hash::SHA384::~SHA384()
{
	MemFree(this->intermediateHash);
	MemFree(this->messageBlock);
}

UTF8Char *Crypto::Hash::SHA384::GetName(UTF8Char *sbuff)
{
	return Text::StrConcat(sbuff, (const UTF8Char*)"SHA-384");
}

Crypto::Hash::IHash *Crypto::Hash::SHA384::Clone()
{
	Crypto::Hash::SHA384 *sha384;
	NEW_CLASS(sha384, Crypto::Hash::SHA384());
	sha384->messageLength = this->messageLength;
	sha384->messageBlockIndex = this->messageBlockIndex;
    
	sha384->intermediateHash[0] = this->intermediateHash[0];
	sha384->intermediateHash[1] = this->intermediateHash[1];
	sha384->intermediateHash[2] = this->intermediateHash[2];
	sha384->intermediateHash[3] = this->intermediateHash[3];
	sha384->intermediateHash[4] = this->intermediateHash[4];
	sha384->intermediateHash[5] = this->intermediateHash[5];
	sha384->intermediateHash[6] = this->intermediateHash[6];
	sha384->intermediateHash[7] = this->intermediateHash[7];
	return sha384;
}
void Crypto::Hash::SHA384::Clear()
{
	this->messageLength        = 0;
	this->messageBlockIndex    = 0;
    
	this->intermediateHash[0]   = 0xcbbb9d5dc1059ed8;
	this->intermediateHash[1]   = 0x629a292a367cd507;
	this->intermediateHash[2]   = 0x9159015a3070dd17;
	this->intermediateHash[3]   = 0x152fecd8f70e5939;
	this->intermediateHash[4]   = 0x67332667ffc00b31;
	this->intermediateHash[5]   = 0x8eb44a8768581511;
	this->intermediateHash[6]   = 0xdb0c2e0d64f98fa7;
	this->intermediateHash[7]   = 0x47b5481dbefa4fa4;
}

void Crypto::Hash::SHA384::Calc(const UInt8 *buff, UOSInt buffSize)
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

void Crypto::Hash::SHA384::GetValue(UInt8 *buff)
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
	i = 48;
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

UOSInt Crypto::Hash::SHA384::GetBlockSize()
{
	return 128;
}

UOSInt Crypto::Hash::SHA384::GetResultSize()
{
	return 48;
}
