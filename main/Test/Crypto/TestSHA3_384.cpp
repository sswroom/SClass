#include "Stdafx.h"
#include "Core/Core.h"
#include "Crypto/Hash/SHA384.h"
#include "Text/MyString.h"

Int32 MyMain(NN<Core::IProgControl> progCtrl)
{
/*	UInt8 hashVal[48];
	UTF8Char sbuff[97];
	UTF8Char *sptr;
	Crypto::Hash::SHA384 hash;

	hash.GetValue(hashVal);
	sptr = Text::StrHexBytes(sbuff, hashVal, 48, 0);
	if (!Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("0C63A75B845E4F7D01107D852E4C2485C51A50AAAA94FC61995E71BBEE983A2AC3713831264ADB47FB6BD1E058D5F004")))
	{
		return 1;
	}

	const UInt8 testBlock[] = "abc";
	hash.Calc(testBlock, sizeof(testBlock) - 1);
	hash.GetValue(hashVal);
	sptr = Text::StrHexBytes(sbuff, hashVal, 48, 0);
	if (!Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("EC01498288516FC926459F58E2C6AD8DF9B473CB0FC08C2596DA7CF0E49BE4B298D88CEA927AC7F539F1EDF228376D25")))
	{
		return 1;
	}

	const UInt8 testBlock2[] = "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq";
	hash.Clear();
	hash.Calc(testBlock2, sizeof(testBlock2) - 1);
	hash.GetValue(hashVal);
	sptr = Text::StrHexBytes(sbuff, hashVal, 48, 0);
	if (!Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("991C665755EB3A4B6BBDFB75C78A492E8C56A22C5C4D7E429BFDBC32B9D4AD5AA04A1F076E62FEA19EEF51ACD0657C22")))
	{
		return 1;
	}*/

	return 0;
}
