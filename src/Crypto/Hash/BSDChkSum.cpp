#include "Stdafx.h"
#include "MyMemory.h"
#include "Crypto/Hash/BSDChkSum.h"
#ifdef HAS_INTRIN
#include <intrin.h>
#endif

Crypto::Hash::BSDChkSum::BSDChkSum(const BSDChkSum *bsdchksum)
{
	this->chksum = bsdchksum->chksum;
}

Crypto::Hash::BSDChkSum::BSDChkSum()
{
	this->chksum = 0;
}

Crypto::Hash::BSDChkSum::~BSDChkSum()
{
}

NN<Crypto::Hash::HashAlgorithm> Crypto::Hash::BSDChkSum::Clone()
{
	Crypto::Hash::BSDChkSum *bsdchksum;
	NEW_CLASS(bsdchksum, Crypto::Hash::BSDChkSum(this));
	return bsdchksum;
}

void Crypto::Hash::BSDChkSum::Clear()
{
	this->chksum = 0;
}

void Crypto::Hash::BSDChkSum::Calc(UnsafeArray<const UInt8> buff, UOSInt buffSize)
{
	UInt16 chksum = this->chksum;

	while (buffSize-- > 0)
	{
#ifdef HAS_INTRIN
		chksum = _rotr16(chksum, 1) + *buff++;
#else
		chksum = ((chksum >> 1) | (chksum << 15)) + *buff++;
#endif
	}
	this->chksum = chksum;
}

void Crypto::Hash::BSDChkSum::GetValue(UInt8 *buff)
{
	*(UInt16*)buff = chksum;
}

UOSInt Crypto::Hash::BSDChkSum::GetBlockSize()
{
	return 1;
}

UOSInt Crypto::Hash::BSDChkSum::GetResultSize()
{
	return 2;
}
