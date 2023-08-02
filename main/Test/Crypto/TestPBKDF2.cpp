#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "Crypto/PBKDF2.h"
#include "Crypto/Hash/HMAC.h"
#include "Crypto/Hash/SHA1.h"
#include "IO/ConsoleWriter.h"
#include "Text/StringBuilderUTF8.h"

UTF8Char *Test(UTF8Char *sbuff, const UTF8Char *pwd, UOSInt pwdLen, const UTF8Char *salt, UOSInt saltLen, UOSInt cnt, UOSInt dkLen)
{
	UInt8 *dk = MemAlloc(UInt8, dkLen);
	Crypto::Hash::SHA1 sha1;
	Crypto::Hash::HMAC hmac(sha1, pwd, pwdLen);
	Crypto::PBKDF2::Calc(salt, saltLen, cnt, dkLen, hmac, dk);
	sbuff = Text::StrHexBytes(sbuff, dk, dkLen, 0);
	MemFree(dk);
	return sbuff;
}

Int32 MyMain(NotNullPtr<Core::IProgControl> progCtrl)
{
	UTF8Char sbuff[64];
	UTF8Char *sptr;
	//rfc6070
	sptr = Test(sbuff, UTF8STRC("password"), UTF8STRC("salt"), 1, 20);
	if (!Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("0C60C80F961F0E71F3A9B524AF6012062FE037A6")))
	{
		return 1;
	}
	sptr = Test(sbuff, UTF8STRC("password"), UTF8STRC("salt"), 2, 20);
	if (!Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("EA6C014DC72D6F8CCD1ED92ACE1D41F0D8DE8957")))
	{
		return 1;
	}
	sptr = Test(sbuff, UTF8STRC("password"), UTF8STRC("salt"), 4096, 20);
	if (!Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("4B007901B765489ABEAD49D926F721D065A429C1")))
	{
		return 1;
	}
/*	sptr = Test(sbuff, UTF8STRC("password"), UTF8STRC("salt"), 16777216, 20);
	if (!Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("EEFE3D61CD4DA4E4E9945B3D6BA2158C2634E984")))
	{
		return 1;
	}*/
	sptr = Test(sbuff, UTF8STRC("passwordPASSWORDpassword"), UTF8STRC("saltSALTsaltSALTsaltSALTsaltSALTsalt"), 4096, 25);
	if (!Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("3D2EEC4FE41C849B80C8D83662C0E44A8B291A964CF2F07038")))
	{
		return 1;
	}
	return 0;
}
