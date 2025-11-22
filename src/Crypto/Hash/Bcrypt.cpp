#include "Stdafx.h"
#include "Crypto/Encrypt/Blowfish.h"
#include "Crypto/Hash/Bcrypt.h"
#include "Data/BinTool.h"
#include "Core/ByteTool_C.h"
#include "Data/RandomBytesGenerator.h"

void Crypto::Hash::Bcrypt::CalcHash(UInt32 cost, UnsafeArray<const UInt8> salt, Text::CStringNN password, UnsafeArray<UInt8> hashBuff) const
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

Crypto::Hash::Bcrypt::Bcrypt() : radix64("./ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789")
{
}

Crypto::Hash::Bcrypt::~Bcrypt()
{
}

Bool Crypto::Hash::Bcrypt::Matches(Text::CStringNN hash, Text::CStringNN password) const
{
	if (hash.v[0] != '$' || hash.leng > 63)
	{
		return false;
	}
	UTF8Char hashBuff[64];
	Text::PString sarr[4];
	UOSInt sarrCnt; 
	hash.Substring(1).ConcatTo(hashBuff);
	sarrCnt = Text::StrSplitP(sarr, 4, {hashBuff, hash.leng - 1}, '$');
	if (sarrCnt != 3)
	{
		return false;
	}
	if (sarr[0].v[0] == '2' && sarr[2].leng == 53)
	{
		UInt8 salt[16];
		UInt8 hashCTxt[24];
		UInt8 myCTxt[24];
		this->radix64.DecodeBin(Text::CStringNN(sarr[2].v, 22), salt);
		this->radix64.DecodeBin(Text::CStringNN(sarr[2].v + 22, 31), hashCTxt);
		this->CalcHash(Text::StrToUInt32(sarr[1].v), salt, password, myCTxt);
		return Data::BinTool::Equals(hashCTxt, myCTxt, 23);
	}
	else
	{
		return false;
	}
}

Bool Crypto::Hash::Bcrypt::GenHash(NN<Text::StringBuilderUTF8> sb, UInt32 cost, Text::CStringNN password)
{
	if (cost < 4 || cost > 31)
	{
		return false;
	}
	Data::RandomBytesGenerator rand;
	UInt8 salt[16];
	rand.NextBytes(salt, 16);
	return this->GenHash(sb, cost, salt, password);
}

Bool Crypto::Hash::Bcrypt::GenHash(NN<Text::StringBuilderUTF8> sb, UInt32 cost, UnsafeArray<const UInt8> salt, Text::CStringNN password) const
{
	if (cost < 4 || cost > 31)
	{
		return false;
	}
	sb->AppendC(UTF8STRC("$2a$"));
	if (cost < 10)
	{
		sb->AppendUTF8Char('0');
	}
	sb->AppendU32(cost);
	sb->AppendUTF8Char('$');
	UInt8 hashCTxt[24];
	this->radix64.EncodeBin(sb, salt, 16);
	this->CalcHash(cost, salt, password, hashCTxt);
	this->radix64.EncodeBin(sb, hashCTxt, 23);
	return true;
}
