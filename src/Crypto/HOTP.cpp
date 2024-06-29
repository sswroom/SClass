#include "Stdafx.h"
#include "MyMemory.h"
#include "Crypto/HOTP.h"
#include "Crypto/Hash/HMAC.h"
#include "Crypto/Hash/SHA1.h"
#include "Data/ByteTool.h"
#include "Text/TextBinEnc/Base32Enc.h"
#include "Text/TextBinEnc/URIEncoding.h"

//RFC 4226
Crypto::HOTP::HOTP(UnsafeArray<const UInt8> key, UOSInt keySize, UInt64 counter) : OTP(6)
{
	this->key = MemAllocArr(UInt8, keySize);
	this->keySize = keySize;
	MemCopyNO(this->key.Ptr(), key.Ptr(), keySize);
	this->counter = counter;
}

Crypto::HOTP::~HOTP()
{
	MemFreeArr(this->key);
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

UInt32 Crypto::HOTP::CalcCode(UnsafeArray<const UInt8> key, UOSInt keySize, UInt64 counter, UInt32 nDigits)
{
	UInt8 buff[20];
	NN<Crypto::Hash::SHA1> hash;
	NEW_CLASSNN(hash, Crypto::Hash::SHA1());
	{
		Crypto::Hash::HMAC hmac(hash, key, keySize);
		WriteMUInt64(buff, counter);
		hmac.Calc(buff, 8);
		hmac.GetValue(buff);
	}
	hash.Delete();
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

void Crypto::HOTP::GenURI(NN<Text::StringBuilderUTF8> sb, UnsafeArray<const UTF8Char> name)
{
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	sb->AppendC(UTF8STRC("otpauth://hotp/"));
	sptr = Text::TextBinEnc::URIEncoding::URIEncode(sbuff, name);
	sb->AppendP(sbuff, sptr);
	sb->AppendC(UTF8STRC("?secret="));
	Text::TextBinEnc::Base32Enc b32;
	b32.EncodeBin(sb, this->key, this->keySize);

}
