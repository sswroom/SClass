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

UnsafeArray<UTF8Char> Crypto::Hash::SuperFastHash::GetName(UnsafeArray<UTF8Char> sbuff) const
{
	return Text::StrConcatC(sbuff, UTF8STRC("SuperFastHash"));
}

NN<Crypto::Hash::IHash> Crypto::Hash::SuperFastHash::Clone() const
{
	NN<Crypto::Hash::SuperFastHash> sfh;
	NEW_CLASSNN(sfh, Crypto::Hash::SuperFastHash(this->currVal));
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

void Crypto::Hash::SuperFastHash::GetValue(UnsafeArray<UInt8> buff) const
{
	UInt32 hash = this->currVal;
    hash ^= hash << 3;
    hash += hash >> 5;
    hash ^= hash << 4;
    hash += hash >> 17;
    hash ^= hash << 25;
	hash += hash >> 6;

	*(UInt32*)buff.Ptr() = hash;
}

UOSInt Crypto::Hash::SuperFastHash::GetBlockSize() const
{
	return 4;
}

UOSInt Crypto::Hash::SuperFastHash::GetResultSize() const
{
	return 4;
}
