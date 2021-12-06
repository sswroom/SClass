#include "Stdafx.h"
#include "MyMemory.h"
#include "Crypto/HOTP.h"
#include "Crypto/TOTP.h"
#include "Data/DateTime.h"
#include "Text/TextBinEnc/Base32Enc.h"
#include "Text/TextEnc/URIEncoding.h"

//RFC 6238
Crypto::TOTP::TOTP(const UInt8 *key, UOSInt keySize) : OTP(6)
{
	this->keySize = keySize;
	this->key = MemAlloc(UInt8, keySize);
	MemCopyNO(this->key, key, keySize);
	this->intervalMS = 30000;
}

Crypto::TOTP::~TOTP()
{
	MemFree(this->key);
}

Crypto::OTP::OTPType Crypto::TOTP::GetType()
{
	return Crypto::OTP::OTPType::TOTP;
}

UInt64 Crypto::TOTP::GetCounter()
{
	Data::DateTime dt;
	dt.SetCurrTimeUTC();
	return ((UInt64)dt.ToTicks() / this->intervalMS);
}

UInt32 Crypto::TOTP::NextCode()
{
	return Crypto::HOTP::CalcCode(this->key, this->keySize, this->GetCounter(), this->nDigits);
}

Bool Crypto::TOTP::IsValid(UInt32 code)
{
	UOSInt i = 5;
	UInt64 counter = GetCounter() - (i >> 1);
	while (i-- > 0)
	{
		if (Crypto::HOTP::CalcCode(this->key, this->keySize, counter, this->nDigits) == code)
		{
			return true;
		}
		counter++;
	}
	return false;
}

void Crypto::TOTP::GenURI(Text::StringBuilderUTF *sb, const UTF8Char *name)
{
	UTF8Char sbuff[512];
	sb->Append((const UTF8Char*)"otpauth://totp/");
	Text::TextEnc::URIEncoding::URIEncode(sbuff, name);
	sb->Append(sbuff);
	sb->Append((const UTF8Char*)"?secret=");
	Text::TextBinEnc::Base32Enc b32;
	b32.EncodeBin(sb, this->key, this->keySize);
}
