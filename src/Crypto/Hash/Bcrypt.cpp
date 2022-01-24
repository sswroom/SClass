#include "Stdafx.h"
#include "Crypto/Encrypt/Blowfish.h"
#include "Crypto/Hash/Bcrypt.h"
#include "Data/BinTool.h"
#include "Data/ByteTool.h"
#include "Data/RandomBytesGenerator.h"

void Crypto::Hash::Bcrypt::CalcHash(UInt32 cost, const UInt8 *salt, const UTF8Char *password, UOSInt pwdLen, UInt8 *hashBuff)
{
	Crypto::Encrypt::Blowfish bf;
	bf.EksBlowfishSetup(cost, salt, password, pwdLen);
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

Crypto::Hash::Bcrypt::Bcrypt()
{
	NEW_CLASS(this->radix64, Text::TextBinEnc::Radix64Enc("./ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789"));
}

Crypto::Hash::Bcrypt::~Bcrypt()
{
	DEL_CLASS(this->radix64);
}

Bool Crypto::Hash::Bcrypt::Matches(const UTF8Char *hash, UOSInt hashLen, const UTF8Char *password, UOSInt pwdLen)
{
	if (hash[0] != '$' || hashLen > 63)
	{
		return false;
	}
	UTF8Char hashBuff[64];
	Text::PString sarr[4];
	UOSInt sarrCnt; 
	Text::StrConcatC(hashBuff, hash + 1, hashLen - 1);
	sarrCnt = Text::StrSplitP(sarr, 4, hashBuff, hashLen - 1, '$');
	if (sarrCnt != 3)
	{
		MemFree(hashBuff);
		return false;
	}
	if (sarr[0].v[0] == '2' && sarr[2].leng == 53)
	{
		UInt8 salt[16];
		UInt8 hashCTxt[24];
		UInt8 myCTxt[24];
		this->radix64->DecodeBin(sarr[2].v, 22, salt);
		this->radix64->DecodeBin(sarr[2].v + 22, 31, hashCTxt);
		this->CalcHash(Text::StrToUInt32(sarr[1].v), salt, password, pwdLen, myCTxt);
		return Data::BinTool::Equals(hashCTxt, myCTxt, 23);
	}
	else
	{
		return false;
	}
}

Bool Crypto::Hash::Bcrypt::GenHash(Text::StringBuilderUTF *sb, UInt32 cost, const UTF8Char *password, UOSInt pwdLen
)
{
	if (cost < 4 || cost > 31)
	{
		return false;
	}
	Data::RandomBytesGenerator rand;
	UInt8 salt[16];
	rand.NextBytes(salt, 16);
	return this->GenHash(sb, cost, salt, password, pwdLen);
}

Bool Crypto::Hash::Bcrypt::GenHash(Text::StringBuilderUTF *sb, UInt32 cost, const UInt8 *salt, const UTF8Char *password, UOSInt pwdLen)
{
	if (cost < 4 || cost > 31)
	{
		return false;
	}
	sb->AppendC(UTF8STRC("$2a$"));
	if (cost < 10)
	{
		sb->AppendChar('0', 1);
	}
	sb->AppendU32(cost);
	sb->AppendChar('$', 1);
	UInt8 hashCTxt[24];
	this->radix64->EncodeBin(sb, salt, 16);
	this->CalcHash(cost, salt, password, pwdLen, hashCTxt);
	this->radix64->EncodeBin(sb, hashCTxt, 23);
	return true;
}
