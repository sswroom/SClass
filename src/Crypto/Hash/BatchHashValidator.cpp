#include "Stdafx.h"
#include "Crypto/Hash/BatchHashValidator.h"
#include "Data/BinTool.h"

Crypto::Hash::BatchHashValidator::BatchHashValidator(Crypto::Hash::IHash *hash, Bool toRelease)
{
	this->hash = hash;
	this->toRelease = toRelease;
	this->hashSize = 0;
}

Crypto::Hash::BatchHashValidator::~BatchHashValidator()
{
	if (this->toRelease)
	{
		DEL_CLASS(this->hash);
		this->hash = 0;
	}
}

Crypto::Hash::HashValidatorSess *Crypto::Hash::BatchHashValidator::CreateSess()
{
	return (Crypto::Hash::HashValidatorSess*)this->hash->Clone().Ptr();
}

void Crypto::Hash::BatchHashValidator::DeleteSess(HashValidatorSess *sess)
{
	Crypto::Hash::IHash *hash = (Crypto::Hash::IHash*)sess;
	DEL_CLASS(hash);
}

Bool Crypto::Hash::BatchHashValidator::SetHash(const UTF8Char *hash, UOSInt hashLen)
{
	if (hashLen > 128)
	{
		return false;
	}
	this->hashSize = Text::StrHex2Bytes(hash, this->hashBuff);
	return this->hashSize == this->hash->GetResultSize();
}

Bool Crypto::Hash::BatchHashValidator::IsMatch(HashValidatorSess *sess, const UTF8Char *password, UOSInt pwdLen)
{
	UInt8 hashBuff[64];
	Crypto::Hash::IHash *hash = (Crypto::Hash::IHash*)sess;
	hash->Clear();
	hash->Calc(password, pwdLen);
	hash->GetValue(hashBuff);
	return Data::BinTool::Equals(hashBuff, this->hashBuff, this->hashSize);
}
