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

UTF8Char *Crypto::OTP::CodeString(UTF8Char *sbuff, UInt32 code)
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

const UTF8Char *Crypto::OTP::OTPTypeGetName(OTPType type)
{
	switch (type)
	{
	case OTPType::HOTP:
		return (const UTF8Char*)"HOTP";
	case OTPType::TOTP:
		return (const UTF8Char*)"TOTP";
	default:
		return (const UTF8Char*)"Unknown";
	}
}
