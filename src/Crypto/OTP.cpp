#include "Stdafx.h"
#include "Crypto/OTP.h"
#include "Text/MyString.h"

Crypto::OTP::OTP(UInt32 nDigits)
{
	this->nDigits = nDigits;
}

Crypto::OTP::~OTP()
{
}

UnsafeArray<UTF8Char> Crypto::OTP::CodeString(UnsafeArray<UTF8Char> sbuff, UInt32 code)
{
	if (this->nDigits == 6)
	{
		if (code < 10)
		{
			return Text::StrUInt32(Text::StrConcatC(sbuff, UTF8STRC("00000")), code);
		}
		else if (code < 100)
		{
			return Text::StrUInt32(Text::StrConcatC(sbuff, UTF8STRC("0000")), code);
		}
		else if (code < 1000)
		{
			return Text::StrUInt32(Text::StrConcatC(sbuff, UTF8STRC("000")), code);
		}
		else if (code < 10000)
		{
			return Text::StrUInt32(Text::StrConcatC(sbuff, UTF8STRC("00")), code);
		}
		else if (code < 100000)
		{
			return Text::StrUInt32(Text::StrConcatC(sbuff, UTF8STRC("0")), code);
		}
		else
		{
			return Text::StrUInt32(sbuff, code);
		}
	}
	else
	{
		return Text::StrUInt32(sbuff, code);
	}
}

Text::CString Crypto::OTP::OTPTypeGetName(OTPType type)
{
	switch (type)
	{
	case OTPType::HOTP:
		return CSTR("HOTP");
	case OTPType::TOTP:
		return CSTR("TOTP");
	default:
		return CSTR("Unknown");
	}
}
