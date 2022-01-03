#include "Stdafx.h"
#include "MyMemory.h"
#include "Crypto/HOTP.h"
#include "Crypto/Hash/HMAC.h"
#include "Crypto/Hash/SHA1.h"
#include "Data/ByteTool.h"
#include "Text/TextBinEnc/Base32Enc.h"
#include "Text/TextEnc/URIEncoding.h"

//RFC 4226
Crypto::HOTP::HOTP(const UInt8 *key, UOSInt keySize, UInt64 counter) : OTP(6)
{
	this->key = MemAlloc(UInt8, keySize);
	this->keySize = keySize;
	MemCopyNO(this->key, key, keySize);
	this->counter = counter;
}

Crypto::HOTP::~HOTP()
{
	MemFree(this->key);
}

Crypto::OTP::OTPType Crypto::HOTP::GetType()
{
	return Crypto::OTP::OTPType::HOTP;
}

UInt64 Crypto::HOTP::GetCounter()
{
	return this->counter;
}

UInt32 Crypto::HOTP::NextCode()
{
	return CalcCode(this->key, this->keySize, this->counter++, this->nDigits);
}

Bool Crypto::HOTP::IsValid(UInt32 code)
{
	UInt32 cnt = 0;
	UInt32 calCode;
	while (cnt < 10)
	{
		calCode = CalcCode(this->key, this->keySize, this->counter + cnt, this->nDigits);
		if (calCode == code)
		{
			this->counter += cnt + 1;
			return true;
		}
		cnt++;
	}
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

void Crypto::HOTP::GenURI(Text::StringBuilderUTF *sb, const UTF8Char *name)
{
	UTF8Char sbuff[512];
	sb->AppendC(UTF8STRC("otpauth://hotp/"));
	Text::TextEnc::URIEncoding::URIEncode(sbuff, name);
	sb->Append(sbuff);
	sb->AppendC(UTF8STRC("?secret="));
	Text::TextBinEnc::Base32Enc b32;
	b32.EncodeBin(sb, this->key, this->keySize);

}
