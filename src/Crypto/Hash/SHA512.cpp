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
	Clear();
}

Crypto::Hash::SHA512::~SHA512()
{
}

UnsafeArray<UTF8Char> Crypto::Hash::SHA512::GetName(UnsafeArray<UTF8Char> sbuff) const
{
	return Text::StrConcatC(sbuff, UTF8STRC("SHA-512"));
}

NN<Crypto::Hash::HashAlgorithm> Crypto::Hash::SHA512::Clone() const
{
	NN<Crypto::Hash::SHA512> sha512;
	NEW_CLASSNN(sha512, Crypto::Hash::SHA512());
	sha512->messageLength = this->messageLength;
	sha512->messageBlockIndex = this->messageBlockIndex;
    MemCopyNO(sha512->intermediateHash, this->intermediateHash, 64);
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

void Crypto::Hash::SHA512::Calc(UnsafeArray<const UInt8> buff, UOSInt buffSize)
{
	this->messageLength += buffSize;
	if ((buffSize + messageBlockIndex) < 128)
	{
		MemCopyNO(&this->messageBlock[messageBlockIndex], buff.Ptr(), buffSize);
		this->messageBlockIndex += buffSize;
		return;
	}
    
	if (messageBlockIndex > 0)
	{
		MemCopyNO(&this->messageBlock[messageBlockIndex], buff.Ptr(), 128 - messageBlockIndex);
		SHA512_CalcBlock(this->intermediateHash, this->messageBlock);
		buff += 128 - messageBlockIndex;
		buffSize -= 128 - messageBlockIndex;
		this->messageBlockIndex = 0;
	}

	while (buffSize >= 128)
	{
		SHA512_CalcBlock(this->intermediateHash, buff.Ptr());
		buff += 128;
		buffSize -= 128;
	}
	if (buffSize > 0)
	{
		MemCopyNO(this->messageBlock, buff.Ptr(), this->messageBlockIndex = buffSize);
	}
}

void Crypto::Hash::SHA512::GetValue(UnsafeArray<UInt8> buff) const
{
	UInt8 calBuff[128];
	UInt64 intHash[8];
	MemCopyNO(intHash, this->intermediateHash, 64);

	UOSInt i;
	if (messageBlockIndex < 111)
	{
		MemCopyNO(calBuff, this->messageBlock, messageBlockIndex);
		i = messageBlockIndex;
		calBuff[i++] = 0x80;
		if (i < 120)
		{
			MemClear(&calBuff[i], 120 - i);
		}
	}
	else
	{
		MemCopyNO(calBuff, this->messageBlock, messageBlockIndex);
		i = messageBlockIndex;
		calBuff[i++] = 0x80;
		if (i < 128)
		{
			MemClear(&calBuff[i], 128 - i);
		}
		SHA512_CalcBlock(intHash, calBuff);

		MemClear(calBuff, 120);
	}

	UInt64 msgLeng = this->messageLength << 3;
	WriteMUInt64(&calBuff[120], msgLeng);
	SHA512_CalcBlock(intHash, calBuff);
	i = 64;
	while (i > 0)
	{
		i -= 8;
		WriteMUInt64(&buff[i], intHash[i >> 3]);
	}
}

UOSInt Crypto::Hash::SHA512::GetBlockSize() const
{
	return 128;
}

UOSInt Crypto::Hash::SHA512::GetResultSize() const
{
	return 64;
}
