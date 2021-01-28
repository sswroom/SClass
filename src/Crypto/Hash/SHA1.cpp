#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "Crypto/Hash/SHA1.h"
#include "Text/MyString.h"

#define SHA1HashSize 20

extern "C"
{
	void SHA1_CalcBlock(UInt32 *Intermediate_Hash, const UInt8 *Message_Block);
}

Crypto::Hash::SHA1::SHA1()
{
	this->Intermediate_Hash = MemAlloc(UInt32, SHA1HashSize / sizeof(UInt32));
	this->Message_Block = MemAlloc(UInt8, 64);
	Clear();
}

Crypto::Hash::SHA1::~SHA1()
{
	MemFree(this->Intermediate_Hash);
	MemFree(this->Message_Block);
}

UTF8Char *Crypto::Hash::SHA1::GetName(UTF8Char *sbuff)
{
	return Text::StrConcat(sbuff, (const UTF8Char*)"SHA-1");
}

Crypto::Hash::IHash *Crypto::Hash::SHA1::Clone()
{
	Crypto::Hash::SHA1 *sha1;
	NEW_CLASS(sha1, Crypto::Hash::SHA1());
	sha1->Message_Length = this->Message_Length;
	sha1->Message_Block_Index = this->Message_Block_Index;
    
	sha1->Intermediate_Hash[0] = this->Intermediate_Hash[0];
	sha1->Intermediate_Hash[1] = this->Intermediate_Hash[1];
	sha1->Intermediate_Hash[2] = this->Intermediate_Hash[2];
	sha1->Intermediate_Hash[3] = this->Intermediate_Hash[3];
	sha1->Intermediate_Hash[4] = this->Intermediate_Hash[4];
	return sha1;
}
void Crypto::Hash::SHA1::Clear()
{
	this->Message_Length         = 0;
	this->Message_Block_Index    = 0;
    
	this->Intermediate_Hash[0]   = 0x67452301;
	this->Intermediate_Hash[1]   = 0xEFCDAB89;
	this->Intermediate_Hash[2]   = 0x98BADCFE;
	this->Intermediate_Hash[3]   = 0x10325476;
	this->Intermediate_Hash[4]   = 0xC3D2E1F0;
}

void Crypto::Hash::SHA1::Calc(const UInt8 *buff, UOSInt buffSize)
{
	this->Message_Length += (buffSize << 3);
	if ((buffSize + this->Message_Block_Index) < 64)
	{
		MemCopyNO(&this->Message_Block[this->Message_Block_Index], buff, buffSize);
		this->Message_Block_Index += buffSize;
		return;
	}
    
	if (this->Message_Block_Index > 0)
	{
		MemCopyNO(&this->Message_Block[this->Message_Block_Index], buff, 64 - this->Message_Block_Index);
		SHA1_CalcBlock(this->Intermediate_Hash, this->Message_Block);
		buff += 64 - this->Message_Block_Index;
		buffSize -= 64 - this->Message_Block_Index;
		this->Message_Block_Index = 0;
	}

	while (buffSize >= 64)
	{
		SHA1_CalcBlock(this->Intermediate_Hash, buff);
		buff += 64;
		buffSize -= 64;
	}
	if (buffSize > 0)
	{
		MemCopyNO(this->Message_Block, buff, this->Message_Block_Index = (Int32)buffSize);
	}
}

void Crypto::Hash::SHA1::GetValue(UInt8 *buff)
{
	UInt8 calBuff[64];
	UInt32 intHash[5];
	MemCopyNO(intHash, this->Intermediate_Hash, 20);

	OSInt i;
	if (this->Message_Block_Index < 55)
	{
		MemCopyNO(calBuff, this->Message_Block, Message_Block_Index);
		i = Message_Block_Index;
		calBuff[i++] = 0x80;
		while (i < 56)
		{
			calBuff[i++] = 0;
		}

	}
	else
	{
		MemCopyNO(calBuff, this->Message_Block, Message_Block_Index);
		i = Message_Block_Index;
		calBuff[i++] = 0x80;
		while (i < 64)
		{
			calBuff[i++] = 0;
		}
		SHA1_CalcBlock(this->Intermediate_Hash, calBuff);

		MemClear(calBuff, 56);
	}

	Int64 msgLeng = this->Message_Length;
	calBuff[56] = ((UInt8*)&msgLeng)[7];
	calBuff[57] = ((UInt8*)&msgLeng)[6];
	calBuff[58] = ((UInt8*)&msgLeng)[5];
	calBuff[59] = ((UInt8*)&msgLeng)[4];
	calBuff[60] = ((UInt8*)&msgLeng)[3];
	calBuff[61] = ((UInt8*)&msgLeng)[2];
	calBuff[62] = ((UInt8*)&msgLeng)[1];
	calBuff[63] = ((UInt8*)&msgLeng)[0];
	SHA1_CalcBlock(this->Intermediate_Hash, calBuff);
	UInt8 *res = (UInt8*)this->Intermediate_Hash;
	i = 20;
	while (i > 0)
	{
		i -= 4;
		buff[i + 0] = res[i + 3];
		buff[i + 1] = res[i + 2];
		buff[i + 2] = res[i + 1];
		buff[i + 3] = res[i + 0];
	}
	MemCopyNO(this->Intermediate_Hash, intHash, 20);
}

UOSInt Crypto::Hash::SHA1::GetBlockSize()
{
	return 64;
}

UOSInt Crypto::Hash::SHA1::GetResultSize()
{
	return 20;
}
