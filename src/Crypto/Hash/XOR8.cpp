#include "Stdafx.h"
#include "MyMemory.h"
#include "Crypto/Hash/XOR8.h"

Crypto::Hash::XOR8::XOR8(const XOR8 *xor8)
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

Crypto::Hash::IHash *Crypto::Hash::XOR8::Clone()
{
	Crypto::Hash::XOR8 *xor8;
	NEW_CLASS(xor8, Crypto::Hash::XOR8(this));
	return xor8;
}

void Crypto::Hash::XOR8::Clear()
{
	this->val = 0;
}

void Crypto::Hash::XOR8::Calc(const UInt8 *buff, UOSInt buffSize)
{
	UInt8 val = this->val;

	while (buffSize-- > 0)
	{
		val ^= *buff++;
	}
	this->val = val;
}

void Crypto::Hash::XOR8::GetValue(UInt8 *buff)
{
	*buff = this->val;
}

UOSInt Crypto::Hash::XOR8::GetBlockSize()
{
	return 1;
}

UOSInt Crypto::Hash::XOR8::GetResultSize()
{
	return 1;
}
