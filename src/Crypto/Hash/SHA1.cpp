#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "Crypto/Hash/SHA1.h"
#include "Text/MyString.h"

#define SHA1HashSize 20

extern "C"
{
	void SHA1_CalcBlock(UInt32 *intermediateHash, const UInt8 *messageBlock);
}

Crypto::Hash::SHA1::SHA1()
{
	this->intermediateHash = MemAlloc(UInt32, SHA1HashSize / sizeof(UInt32));
	this->messageBlock = MemAlloc(UInt8, 64);
	Clear();
}

Crypto::Hash::SHA1::~SHA1()
{
	MemFree(this->intermediateHash);
	MemFree(this->messageBlock);
}

UTF8Char *Crypto::Hash::SHA1::GetName(UTF8Char *sbuff)
{
	return Text::StrConcatC(sbuff, UTF8STRC("SHA-1"));
}

Crypto::Hash::IHash *Crypto::Hash::SHA1::Clone()
{
	Crypto::Hash::SHA1 *sha1;
	NEW_CLASS(sha1, Crypto::Hash::SHA1());
	sha1->messageLength = this->messageLength;
	sha1->messageBlockIndex = this->messageBlockIndex;
    
	sha1->intermediateHash[0] = this->intermediateHash[0];
	sha1->intermediateHash[1] = this->intermediateHash[1];
	sha1->intermediateHash[2] = this->intermediateHash[2];
	sha1->intermediateHash[3] = this->intermediateHash[3];
	sha1->intermediateHash[4] = this->intermediateHash[4];
	return sha1;
}
void Crypto::Hash::SHA1::Clear()
{
	this->messageLength         = 0;
	this->messageBlockIndex    = 0;
    
	this->intermediateHash[0]   = 0x67452301;
	this->intermediateHash[1]   = 0xEFCDAB89;
	this->intermediateHash[2]   = 0x98BADCFE;
	this->intermediateHash[3]   = 0x10325476;
	this->intermediateHash[4]   = 0xC3D2E1F0;
}

void Crypto::Hash::SHA1::Calc(const UInt8 *buff, UOSInt buffSize)
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
		SHA1_CalcBlock(this->intermediateHash, this->messageBlock);
		buff += 64 - this->messageBlockIndex;
		buffSize -= 64 - this->messageBlockIndex;
		this->messageBlockIndex = 0;
	}

	while (buffSize >= 64)
	{
		SHA1_CalcBlock(this->intermediateHash, buff);
		buff += 64;
		buffSize -= 64;
	}
	if (buffSize > 0)
	{
		MemCopyNO(this->messageBlock, buff, this->messageBlockIndex = buffSize);
	}
}

void Crypto::Hash::SHA1::GetValue(UInt8 *buff)
{
	UInt8 calBuff[64];
	UInt32 intHash[5];
	MemCopyNO(intHash, this->intermediateHash, 20);

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
		SHA1_CalcBlock(this->intermediateHash, calBuff);

		MemClear(calBuff, 56);
	}

	UInt64 msgLeng = this->messageLength;
	calBuff[56] = ((UInt8*)&msgLeng)[7];
	calBuff[57] = ((UInt8*)&msgLeng)[6];
	calBuff[58] = ((UInt8*)&msgLeng)[5];
	calBuff[59] = ((UInt8*)&msgLeng)[4];
	calBuff[60] = ((UInt8*)&msgLeng)[3];
	calBuff[61] = ((UInt8*)&msgLeng)[2];
	calBuff[62] = ((UInt8*)&msgLeng)[1];
	calBuff[63] = ((UInt8*)&msgLeng)[0];
	SHA1_CalcBlock(this->intermediateHash, calBuff);
	UInt8 *res = (UInt8*)this->intermediateHash;
	i = 20;
	while (i > 0)
	{
		i -= 4;
		buff[i + 0] = res[i + 3];
		buff[i + 1] = res[i + 2];
		buff[i + 2] = res[i + 1];
		buff[i + 3] = res[i + 0];
	}
	MemCopyNO(this->intermediateHash, intHash, 20);
}

UOSInt Crypto::Hash::SHA1::GetBlockSize()
{
	return 64;
}

UOSInt Crypto::Hash::SHA1::GetResultSize()
{
	return 20;
}
