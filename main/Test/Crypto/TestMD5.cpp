#include "Stdafx.h"
#include "Core/Core.h"
#include "Crypto/Hash/MD5.h"
#include "Text/MyString.h"

Int32 MyMain(NotNullPtr<Core::IProgControl> progCtrl)
{
	UInt8 testBlock[] = "The quick brown fox jumps over the lazy dog";
	UInt8 hashVal[32];
	UTF8Char sbuff[65];
	UTF8Char *sptr;
	Crypto::Hash::MD5 hash;

	hash.GetValue(hashVal);
	sptr = Text::StrHexBytes(sbuff, hashVal, 16, 0);
	if (!Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("D41D8CD98F00B204E9800998ECF8427E")))
	{
		return 1;
	}

	hash.Calc(testBlock, sizeof(testBlock) - 1);
	hash.GetValue(hashVal);
	sptr = Text::StrHexBytes(sbuff, hashVal, 16, 0);
	if (!Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("9E107D9D372BB6826BD81D3542A419D6")))
	{
		return 1;
	}

	hash.Calc(testBlock, sizeof(testBlock) - 1);
	hash.GetValue(hashVal);
	sptr = Text::StrHexBytes(sbuff, hashVal, 16, 0);
	if (!Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("D27C6D8BCAA695E377D32387E115763C")))
	{
		return 1;
	}

	hash.Calc(testBlock, sizeof(testBlock) - 1);
	hash.GetValue(hashVal);
	sptr = Text::StrHexBytes(sbuff, hashVal, 16, 0);
	if (!Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("4E67DB4A7A406B0CFDADD887CDE7888E")))
	{
		return 1;
	}

	return 0;
}
