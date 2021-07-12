#include "Stdafx.h"
#include "MyMemory.h"
#include "Crypto/HOTP.h"
#include "Crypto/Hash/HMAC.h"
#include "Crypto/Hash/SHA1.h"
#include "Data/ByteTool.h"

//RFC 4226
Crypto::HOTP::HOTP(const UInt8 *key, UOSInt keySize, UInt64 counter)
{
	this->key = MemAlloc(UInt8, keySize);
	this->keySize = keySize;
	MemCopyNO(this->key, key, keySize);
	this->nDigits = 6;
	this->counter = 0;
}

Crypto::HOTP::~HOTP()
{
	MemFree(this->key);
}

UInt64 Crypto::HOTP::GetCounter()
{
	return this->counter;
}

UInt32 Crypto::HOTP::NextCode()
{
	return CalcCode(this->key, this->keySize, this->counter++, this->nDigits);
}

Bool Crypto::HOTP::IsValid(UInt64 code)
{
	return false;
}

UInt32 Crypto::HOTP::CalcCode(const UInt8 *key, UOSInt keySize, UInt64 counter, UInt32 nDigits)
{
	UInt8 buff[20];
	Crypto::Hash::HMAC *hmac;
	Crypto::Hash::SHA1 *hash;
	NEW_CLASS(hash, Crypto::Hash::SHA1());
	NEW_CLASS(hmac, Crypto::Hash::HMAC(hash, key, keySize));
	WriteMUInt64(buff, counter);
	hmac->Calc(buff, 8);
	hmac->GetValue(buff);
	DEL_CLASS(hmac);
	DEL_CLASS(hash);
	UInt32 v = ReadMUInt32(&buff[buff[19] & 15]) & 0x7fffffff;
	if (nDigits == 6)
	{
		return v % 1000000;
	}
	else if (nDigits == 8)
	{
		return v % 100000000;
	}
	else
	{
		return v;
	}
}