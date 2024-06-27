#include "Stdafx.h"
#include "MyMemory.h"
#include "Crypto/Hash/RIPEMD160.h"
#include "IO/Console.h"
#include "Text/MyString.h"
#include "Text/StringBuilder.h"

extern "C"
{
	void RMD160_CalcBlock(UInt32 *keys, const UInt8 *block);
}

Crypto::Hash::RIPEMD160::RIPEMD160()
{
	Clear();
}

Crypto::Hash::RIPEMD160::~RIPEMD160()
{
}

UnsafeArray<UTF8Char> Crypto::Hash::RIPEMD160::GetName(UnsafeArray<UTF8Char> sbuff) const
{
	return Text::StrConcatC(sbuff, UTF8STRC("RIPEMD-160"));
}

NN<Crypto::Hash::IHash> Crypto::Hash::RIPEMD160::Clone() const
{
	NN<Crypto::Hash::RIPEMD160> rmd160;
	NEW_CLASSNN(rmd160, Crypto::Hash::RIPEMD160());
	rmd160->msgLeng = this->msgLeng;
	rmd160->h0 = this->h0;
	rmd160->h1 = this->h1;
	rmd160->h2 = this->h2;
	rmd160->h3 = this->h3;
	rmd160->h4 = this->h4;
	rmd160->buffSize = this->buffSize;
	return rmd160;
}

void Crypto::Hash::RIPEMD160::Clear()
{
	msgLeng = 0;
	h0 = 0x67452301;
	h1 = 0xEFCDAB89;
	h2 = 0x98BADCFE;
	h3 = 0x10325476;
	h4 = 0xC3D2E1F0;
	buffSize = 0;
}

void Crypto::Hash::RIPEMD160::Calc(UnsafeArray<const UInt8> buff, UOSInt buffSize)
{
	this->msgLeng += (buffSize << 3);
	if ((buffSize + this->buffSize) < 64)
	{
		MemCopyNO(&this->buff[this->buffSize], buff.Ptr(), buffSize);
		this->buffSize += (UInt32)buffSize;
		return;
	}

	UInt32 keys[5] = {h0, h1, h2, h3, h4};
	if (this->buffSize > 0)
	{
		MemCopyNO(&this->buff[this->buffSize], buff.Ptr(), 64 - this->buffSize);
		RMD160_CalcBlock(keys, this->buff);
		buff += 64 - this->buffSize;
		buffSize -= 64 - this->buffSize;
		this->buffSize = 0;
	}

	while (buffSize >= 64)
	{
		RMD160_CalcBlock(keys, buff.Ptr());
		buff += 64;
		buffSize -= 64;
	}
	if (buffSize > 0)
	{
		MemCopyNO(this->buff, buff.Ptr(), this->buffSize = (UInt32)buffSize);
	}

	h0 = keys[0];
	h1 = keys[1];
	h2 = keys[2];
	h3 = keys[3];
	h4 = keys[4];
}

void Crypto::Hash::RIPEMD160::GetValue(UnsafeArray<UInt8> buff) const
{
	UInt8 calBuff[64];
	*(UInt32*)&buff[0] = h0;
	*(UInt32*)&buff[4] = h1;
	*(UInt32*)&buff[8] = h2;
	*(UInt32*)&buff[12] = h3;
	*(UInt32*)&buff[16] = h4;
	UInt32 i;
	if (buffSize < 56)
	{
		MemCopyNO(calBuff, this->buff, buffSize);
		i = buffSize;
		calBuff[i++] = 0x80;
		while (i < 56)
		{
			calBuff[i++] = 0;
		}
		*(UInt64*)&calBuff[56] = msgLeng;
		RMD160_CalcBlock((UInt32*)buff.Ptr(), calBuff);
	}
	else
	{
		MemCopyNO(calBuff, this->buff, buffSize);
		i = buffSize;
		calBuff[i++] = 0x80;
		while (i < 64)
		{
			calBuff[i++] = 0;
		}
		RMD160_CalcBlock((UInt32*)buff.Ptr(), calBuff);

		MemClear(calBuff, 56);
		*(UInt64*)&calBuff[56] = msgLeng;
		RMD160_CalcBlock((UInt32*)buff.Ptr(), calBuff);
	}
}

UOSInt Crypto::Hash::RIPEMD160::GetBlockSize() const
{
	return 64;
}

UOSInt Crypto::Hash::RIPEMD160::GetResultSize() const
{
	return 20;
}
