#include "Stdafx.h"
#include "MyMemory.h"
#include "Crypto/Hash/MD5.h"
#include "Data/ByteTool.h"
#include "Text/MyString.h"

#include "Text/StringBuilder.h"
#include "IO/Console.h"

extern "C"
{
	void MD5_CalcBlock(UInt32 *hVals, const UInt8 *block, UOSInt blkCnt);
}

Crypto::Hash::MD5::MD5()
{
	buff = MemAlloc(UInt8, 64);
	Clear();
}

Crypto::Hash::MD5::~MD5()
{
	MemFree(buff);
}

UTF8Char *Crypto::Hash::MD5::GetName(UTF8Char *sbuff) const
{
	return Text::StrConcatC(sbuff, UTF8STRC("MD5"));
}

NN<Crypto::Hash::IHash> Crypto::Hash::MD5::Clone() const
{
	NN<Crypto::Hash::MD5> md5;
	NEW_CLASSNN(md5, Crypto::Hash::MD5());
	md5->msgLeng = this->msgLeng;
	md5->h[0] = this->h[0];
	md5->h[1] = this->h[1];
	md5->h[2] = this->h[2];
	md5->h[3] = this->h[3];
	md5->buffSize = this->buffSize;
	return md5;
}

void Crypto::Hash::MD5::Clear()
{
	msgLeng = 0;
	this->h[0] = 0x67452301;
	this->h[1] = 0xEFCDAB89;
	this->h[2] = 0x98BADCFE;
	this->h[3] = 0x10325476;
	buffSize = 0;
}

void Crypto::Hash::MD5::Calc(const UInt8 *buff, UOSInt buffSize)
{
	this->msgLeng += (buffSize << 3);
	if ((buffSize + this->buffSize) < 64)
	{
		MemCopyNO(&this->buff[this->buffSize], buff, buffSize);
		this->buffSize += buffSize;
		return;
	}

	UInt32 *v = this->h;
	if (this->buffSize > 0)
	{
		MemCopyNO(&this->buff[this->buffSize], buff, 64 - this->buffSize);
		MD5_CalcBlock(v, this->buff, 1);
		buff += 64 - this->buffSize;
		buffSize -= 64 - this->buffSize;
		this->buffSize = 0;
	}
	if (buffSize >= 64)
	{
		MD5_CalcBlock(v, buff, buffSize >> 6);
		buff += buffSize & (UOSInt)~63;
		buffSize = buffSize & 63;
	}
	if (buffSize > 0)
	{
		MemCopyNO(this->buff, buff, this->buffSize = buffSize);
	}
}

void Crypto::Hash::MD5::GetValue(UnsafeArray<UInt8> buff) const
{
	UInt8 calBuff[64];
	UInt32 v[4];
	v[0] = this->h[0];
	v[1] = this->h[1];
	v[2] = this->h[2];
	v[3] = this->h[3];
	UOSInt i;
	if (buffSize < 56)
	{
		MemCopyNO(calBuff, this->buff, buffSize);
		i = buffSize;
		calBuff[i++] = 0x80;
		while (i < 56)
		{
			calBuff[i++] = 0;
		}
		WriteUInt64(&calBuff[56], msgLeng);
		MD5_CalcBlock(v, calBuff, 1);
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
		MD5_CalcBlock(v, calBuff, 1);

		MemClear(calBuff, 56);
		WriteUInt64(&calBuff[56], msgLeng);
		MD5_CalcBlock(v, calBuff, 1);
	}
	WriteUInt32(&buff[0], v[0]);
	WriteUInt32(&buff[4], v[1]);
	WriteUInt32(&buff[8], v[2]);
	WriteUInt32(&buff[12], v[3]);
}

UOSInt Crypto::Hash::MD5::GetBlockSize() const
{
	return 64;
}

UOSInt Crypto::Hash::MD5::GetResultSize() const
{
	return 16;
}
