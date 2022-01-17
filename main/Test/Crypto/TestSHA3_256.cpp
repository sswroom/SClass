#include "Stdafx.h"
#include "Core/Core.h"
#include "Crypto/Hash/SHA256.h"
#include "Text/MyString.h"

Int32 MyMain(Core::IProgControl *progCtrl)
{
/*	UInt8 hashVal[32];
	UTF8Char sbuff[65];
	UTF8Char *sptr;
	Crypto::Hash::SHA256 hash;

	hash.GetValue(hashVal);
	sptr = Text::StrHexBytes(sbuff, hashVal, 32, 0);
	if (!Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("A7FFC6F8BF1ED76651C14756A061D662F580FF4DE43B49FA82D80A4B80F8434A")))
	{
		return 1;
	}

	const UInt8 testBlock[] = "abc";
	hash.Calc(testBlock, sizeof(testBlock) - 1);
	hash.GetValue(hashVal);
	sptr = Text::StrHexBytes(sbuff, hashVal, 32, 0);
	if (!Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("3A985DA74FE225B2045C172D6BD390BD855F086E3E9D525B46BFE24511431532")))
	{
		return 1;
	}

	const UInt8 testBlock2[] = "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq";
	hash.Clear();
	hash.Calc(testBlock2, sizeof(testBlock2) - 1);
	hash.GetValue(hashVal);
	sptr = Text::StrHexBytes(sbuff, hashVal, 32, 0);
	if (!Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("41C0DBA2A9D6240849100376A8235E2C82E1B9998A999E21DB32DD97496D3376")))
	{
		return 1;
	}*/

	return 0;
}
