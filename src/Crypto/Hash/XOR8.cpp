#include "Stdafx.h"
#include "MyMemory.h"
#include "Crypto/Hash/XOR8.h"

Crypto::Hash::XOR8::XOR8(NN<const XOR8> xor8)
{
	this->val = xor8->val;
}

Crypto::Hash::XOR8::XOR8()
{
	this->val = 0;
}

Crypto::Hash::XOR8::~XOR8()
{
}

UnsafeArray<UTF8Char> Crypto::Hash::XOR8::GetName(UnsafeArray<UTF8Char> sbuff) const
{
	return Text::StrConcatC(sbuff, UTF8STRC("XOR8"));
}

NN<Crypto::Hash::HashAlgorithm> Crypto::Hash::XOR8::Clone() const
{
	NN<Crypto::Hash::XOR8> xor8;
	NEW_CLASSNN(xor8, Crypto::Hash::XOR8(*this));
	return xor8;
}

void Crypto::Hash::XOR8::Clear()
{
	this->val = 0;
}

void Crypto::Hash::XOR8::Calc(UnsafeArray<const UInt8> buff, UIntOS buffSize)
{
	UInt8 val = this->val;

	while (buffSize-- > 0)
	{
		val ^= *buff++;
	}
	this->val = val;
}

void Crypto::Hash::XOR8::GetValue(UnsafeArray<UInt8> buff) const
{
	*buff = this->val;
}

UIntOS Crypto::Hash::XOR8::GetBlockSize() const
{
	return 1;
}

UIntOS Crypto::Hash::XOR8::GetResultSize() const
{
	return 1;
}
