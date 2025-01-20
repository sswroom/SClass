#include "Stdafx.h"
#include "Crypto/Hash/BatchHashValidator.h"
#include "Data/BinTool.h"

Crypto::Hash::BatchHashValidator::BatchHashValidator(NN<Crypto::Hash::HashAlgorithm> hash, Bool toRelease)
{
	this->hash = hash;
	this->toRelease = toRelease;
	this->hashSize = 0;
}

Crypto::Hash::BatchHashValidator::~BatchHashValidator()
{
	if (this->toRelease)
	{
		this->hash.Delete();
	}
}

NN<Crypto::Hash::HashValidatorSess> Crypto::Hash::BatchHashValidator::CreateSess()
{
	return NN<Crypto::Hash::HashValidatorSess>::ConvertFrom(this->hash->Clone());
}

void Crypto::Hash::BatchHashValidator::DeleteSess(NN<HashValidatorSess> sess)
{
	NN<Crypto::Hash::HashAlgorithm> hash = NN<Crypto::Hash::HashAlgorithm>::ConvertFrom(sess);
	hash.Delete();
}

Bool Crypto::Hash::BatchHashValidator::SetHash(UnsafeArray<const UTF8Char> hash, UOSInt hashLen)
{
	if (hashLen > 128)
	{
		return false;
	}
	this->hashSize = Text::StrHex2Bytes(hash, this->hashBuff);
	return this->hashSize == this->hash->GetResultSize();
}

Bool Crypto::Hash::BatchHashValidator::IsMatch(NN<HashValidatorSess> sess, UnsafeArray<const UTF8Char> password, UOSInt pwdLen)
{
	UInt8 hashBuff[64];
	NN<Crypto::Hash::HashAlgorithm> hash = NN<Crypto::Hash::HashAlgorithm>::ConvertFrom(sess);
	hash->Clear();
	hash->Calc(password, pwdLen);
	hash->GetValue(hashBuff);
	return Data::BinTool::Equals(hashBuff, this->hashBuff, this->hashSize);
}
