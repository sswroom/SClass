#include "Stdafx.h"
#include "Crypto/Encrypt/Blowfish.h"
#include "Crypto/Hash/BcryptValidator.h"
#include "Data/BinTool.h"
#include "Text/PString.h"

void Crypto::Hash::BcryptValidator::CalcHash(UInt32 cost, UnsafeArray<const UInt8> salt, Text::CStringNN password, UnsafeArray<UInt8> hashBuff)
{
	Crypto::Encrypt::Blowfish bf;
	bf.EksBlowfishSetup(cost, salt, password);
	bf.SetChainMode(Crypto::Encrypt::ChainMode::ECB);
	UInt32 tmpBuff[6];
	const UInt8 *initVal = (const UInt8*)"OrpheanBeholderScryDoubt";
	tmpBuff[0] = ReadMUInt32(&initVal[0]);
	tmpBuff[1] = ReadMUInt32(&initVal[4]);
	tmpBuff[2] = ReadMUInt32(&initVal[8]);
	tmpBuff[3] = ReadMUInt32(&initVal[12]);
	tmpBuff[4] = ReadMUInt32(&initVal[16]);
	tmpBuff[5] = ReadMUInt32(&initVal[20]);
	UOSInt i = 64;
	while (i-- > 0)
	{
		bf.EncryptBlk(&tmpBuff[0]);
		bf.EncryptBlk(&tmpBuff[2]);
		bf.EncryptBlk(&tmpBuff[4]);
	}
	WriteMUInt32(&hashBuff[0], tmpBuff[0]);
	WriteMUInt32(&hashBuff[4], tmpBuff[1]);
	WriteMUInt32(&hashBuff[8], tmpBuff[2]);
	WriteMUInt32(&hashBuff[12], tmpBuff[3]);
	WriteMUInt32(&hashBuff[16], tmpBuff[4]);
	WriteMUInt32(&hashBuff[20], tmpBuff[5]);
}

Crypto::Hash::BcryptValidator::BcryptValidator() : radix64("./ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789")
{
}

Crypto::Hash::BcryptValidator::~BcryptValidator()
{
}

NN<Crypto::Hash::HashValidatorSess> Crypto::Hash::BcryptValidator::CreateSess()
{
	return NN<Crypto::Hash::HashValidatorSess>::FromPtr((Crypto::Hash::HashValidatorSess*)-1);
}

void Crypto::Hash::BcryptValidator::DeleteSess(NN<HashValidatorSess> sess)
{
}

Bool Crypto::Hash::BcryptValidator::SetHash(UnsafeArray<const UTF8Char> hash, UOSInt hashLen)
{
	if (hash[0] != '$' || hashLen > 63)
	{
		return false;
	}
	UTF8Char hashBuff[64];
	Text::PString sarr[4];
	UOSInt sarrCnt; 
	Text::StrConcatC(hashBuff, hash + 1, hashLen - 1);
	sarrCnt = Text::StrSplitP(sarr, 4, {hashBuff, hashLen - 1}, '$');
	if (sarrCnt != 3)
	{
		return false;
	}
	if (sarr[0].v[0] == '2' && sarr[2].leng == 53)
	{
		UOSInt saltSize = this->radix64.DecodeBin(Text::CStringNN(sarr[2].v, 22), this->salt);
		this->hashCTxt[23] = 0;
		UOSInt hashSize = this->radix64.DecodeBin(Text::CStringNN(sarr[2].v + 22, 31), this->hashCTxt);
		return saltSize == 16 && (hashSize == 24 || hashSize == 23) && Text::StrToUInt32(sarr[1].v, this->cost);
	}
	else
	{
		return false;
	}
}

Bool Crypto::Hash::BcryptValidator::IsMatch(NN<HashValidatorSess> sess, UnsafeArray<const UTF8Char> password, UOSInt pwdLen)
{
	UInt8 myCTxt[24];
	this->CalcHash(this->cost, this->salt, Text::CStringNN(password, pwdLen), myCTxt);
	return Data::BinTool::Equals(hashCTxt, myCTxt, 23);
}
