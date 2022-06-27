#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "Crypto/Hash/SHA224.h"
#include "Text/MyString.h"

extern "C"
{
	void SHA256_CalcBlock(UInt32 *intermediateHash, const UInt8 *messageBlock);
}

Crypto::Hash::SHA224::SHA224()
{
	Clear();
}

Crypto::Hash::SHA224::~SHA224()
{
}

UTF8Char *Crypto::Hash::SHA224::GetName(UTF8Char *sbuff) const
{
	return Text::StrConcatC(sbuff, UTF8STRC("SHA-224"));
}

Crypto::Hash::IHash *Crypto::Hash::SHA224::Clone() const
{
	Crypto::Hash::SHA224 *sha224;
	NEW_CLASS(sha224, Crypto::Hash::SHA224());
	sha224->messageLength = this->messageLength;
	sha224->messageBlockIndex = this->messageBlockIndex;
    
	sha224->intermediateHash[0] = this->intermediateHash[0];
	sha224->intermediateHash[1] = this->intermediateHash[1];
	sha224->intermediateHash[2] = this->intermediateHash[2];
	sha224->intermediateHash[3] = this->intermediateHash[3];
	sha224->intermediateHash[4] = this->intermediateHash[4];
	sha224->intermediateHash[5] = this->intermediateHash[5];
	sha224->intermediateHash[6] = this->intermediateHash[6];
	sha224->intermediateHash[7] = this->intermediateHash[7];
	return sha224;
}
void Crypto::Hash::SHA224::Clear()
{
	this->messageLength        = 0;
	this->messageBlockIndex    = 0;
    
	this->intermediateHash[0]   = 0xc1059ed8;
	this->intermediateHash[1]   = 0x367cd507;
	this->intermediateHash[2]   = 0x3070dd17;
	this->intermediateHash[3]   = 0xf70e5939;
	this->intermediateHash[4]   = 0xffc00b31;
	this->intermediateHash[5]   = 0x68581511;
	this->intermediateHash[6]   = 0x64f98fa7;
	this->intermediateHash[7]   = 0xbefa4fa4;
}

void Crypto::Hash::SHA224::Calc(const UInt8 *buff, UOSInt buffSize)
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

void Crypto::Hash::SHA224::GetValue(UInt8 *buff) const
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
		if (i < 56)
		{
			MemClear(&calBuff[i], 56 - i);
		}
	}
	else
	{
		MemCopyNO(calBuff, this->messageBlock, messageBlockIndex);
		i = messageBlockIndex;
		calBuff[i++] = 0x80;
		if (i < 64)
		{
			MemClear(&calBuff[i], 64 - i);
		}
		SHA256_CalcBlock(intHash, calBuff);

		MemClear(calBuff, 56);
	}

	UInt64 msgLeng = this->messageLength;
	WriteMUInt64(&calBuff[56], msgLeng);
	SHA256_CalcBlock(intHash, calBuff);
	i = 28;
	while (i > 0)
	{
		i -= 4;
		WriteMUInt32(&buff[i], intHash[i >> 2]);
	}
}

UOSInt Crypto::Hash::SHA224::GetBlockSize() const
{
	return 64;
}

UOSInt Crypto::Hash::SHA224::GetResultSize() const
{
	return 28;
}
