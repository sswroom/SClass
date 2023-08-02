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
	Clear();
}

Crypto::Hash::SHA1::~SHA1()
{
}

UTF8Char *Crypto::Hash::SHA1::GetName(UTF8Char *sbuff) const
{
	return Text::StrConcatC(sbuff, UTF8STRC("SHA-1"));
}

NotNullPtr<Crypto::Hash::IHash> Crypto::Hash::SHA1::Clone() const
{
	NotNullPtr<Crypto::Hash::SHA1> sha1;
	NEW_CLASSNN(sha1, Crypto::Hash::SHA1());
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

void Crypto::Hash::SHA1::GetValue(UInt8 *buff) const
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
		SHA1_CalcBlock(intHash, calBuff);

		MemClear(calBuff, 56);
	}

	WriteMInt64(&calBuff[56], this->messageLength);
	SHA1_CalcBlock(intHash, calBuff);
	i = 20;
	while (i > 0)
	{
		i -= 4;
		WriteMUInt32(&buff[i], intHash[i >> 2]);
	}
}

UOSInt Crypto::Hash::SHA1::GetBlockSize() const
{
	return 64;
}

UOSInt Crypto::Hash::SHA1::GetResultSize() const
{
	return 20;
}
