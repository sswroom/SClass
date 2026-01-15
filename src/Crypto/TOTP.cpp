#include "Stdafx.h"
#include "MyMemory.h"
#include "Crypto/HOTP.h"
#include "Crypto/TOTP.h"
#include "Data/DateTime.h"
#include "Text/TextBinEnc/Base32Enc.h"
#include "Text/TextBinEnc/URIEncoding.h"

//RFC 6238
Crypto::TOTP::TOTP(UnsafeArray<const UInt8> key, UIntOS keySize) : OTP(6)
{
	this->keySize = keySize;
	this->key = MemAllocArr(UInt8, keySize);
	MemCopyNO(this->key.Ptr(), key.Ptr(), keySize);
	this->intervalMS = 30000;
}

Crypto::TOTP::~TOTP()
{
	MemFreeArr(this->key);
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
	UIntOS i = 5;
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

void Crypto::TOTP::GenURI(NN<Text::StringBuilderUTF8> sb, UnsafeArray<const UTF8Char> name)
{
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	sb->AppendC(UTF8STRC("otpauth://totp/"));
	sptr = Text::TextBinEnc::URIEncoding::URIEncode(sbuff, name);
	sb->AppendP(sbuff, sptr);
	sb->AppendC(UTF8STRC("?secret="));
	Text::TextBinEnc::Base32Enc b32;
	b32.EncodeBin(sb, this->key, this->keySize);
}
