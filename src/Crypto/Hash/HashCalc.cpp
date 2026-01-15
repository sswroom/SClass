#include "Stdafx.h"
#include "MyMemory.h"
#include "Crypto/Hash/HashCalc.h"
#include "Sync/MutexUsage.h"
#include "Text/MyString.h"

Crypto::Hash::HashCalc::HashCalc(NN<Crypto::Hash::HashAlgorithm> hash)
{
	this->hash = hash;
}

Crypto::Hash::HashCalc::~HashCalc()
{
	this->hash.Delete();
}

void Crypto::Hash::HashCalc::Calc(UnsafeArray<const UInt8> buff, UIntOS size, UnsafeArray<UInt8> hashVal)
{
	Sync::MutexUsage mutUsage(this->mut);
	this->hash->Clear();
	this->hash->Calc(buff, size);
	this->hash->GetValue(hashVal);
}

void Crypto::Hash::HashCalc::CalcStr(Text::CStringNN s, UnsafeArray<UInt8> hashVal)
{
	this->Calc(s.v, s.leng, hashVal);
}
