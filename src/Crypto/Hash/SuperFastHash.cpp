#include "Stdafx.h"
#include "MyMemory.h"
#include "Crypto/Hash/SuperFastHash.h"
#include "Text/MyString.h"

extern "C"
{
	UInt32 SuperFastHash_Calc(const UInt8 *buff, UOSInt buffSize, UInt32 currVal);
}

Crypto::Hash::SuperFastHash::SuperFastHash(UInt32 len)
{
	this->currVal = len;
}

Crypto::Hash::SuperFastHash::~SuperFastHash()
{
}

UTF8Char *Crypto::Hash::SuperFastHash::GetName(UTF8Char *sbuff)
{
	return Text::StrConcat(sbuff, (const UTF8Char*)"SuperFastHash");
}

Crypto::Hash::IHash *Crypto::Hash::SuperFastHash::Clone()
{
	Crypto::Hash::SuperFastHash *sfh;
	NEW_CLASS(sfh, Crypto::Hash::SuperFastHash(this->currVal));
	return sfh;
}

void Crypto::Hash::SuperFastHash::Clear()
{
	this->currVal = 0;
}

void Crypto::Hash::SuperFastHash::Calc(const UInt8 *buff, UOSInt buffSize)
{
	this->currVal = SuperFastHash_Calc(buff, buffSize, this->currVal);
}

void Crypto::Hash::SuperFastHash::GetValue(UInt8 *buff)
{
	UInt32 hash = this->currVal;
    hash ^= hash << 3;
    hash += hash >> 5;
    hash ^= hash << 4;
    hash += hash >> 17;
    hash ^= hash << 25;
	hash += hash >> 6;

	*(UInt32*)buff = hash;
}

UOSInt Crypto::Hash::SuperFastHash::GetBlockSize()
{
	return 4;
}

UOSInt Crypto::Hash::SuperFastHash::GetResultSize()
{
	return 4;
}
