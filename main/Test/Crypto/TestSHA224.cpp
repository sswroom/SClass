#include "Stdafx.h"
#include "Core/Core.h"
#include "Crypto/Hash/SHA224.h"
#include "Text/MyString.h"

Int32 MyMain(NN<Core::IProgControl> progCtrl)
{
	UInt8 hashVal[28];
	UTF8Char sbuff[57];
	UnsafeArray<UTF8Char> sptr;
	Crypto::Hash::SHA224 hash;

	hash.GetValue(hashVal);
	sptr = Text::StrHexBytes(sbuff, hashVal, 28, 0);
	if (!Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("D14A028C2A3A2BC9476102BB288234C415A2B01F828EA62AC5B3E42F")))
	{
		return 1;
	}

	const UInt8 testBlock[] = "abc";
	hash.Calc(testBlock, sizeof(testBlock) - 1);
	hash.GetValue(hashVal);
	sptr = Text::StrHexBytes(sbuff, hashVal, 28, 0);
	if (!Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("23097D223405D8228642A477BDA255B32AADBCE4BDA0B3F7E36C9DA7")))
	{
		return 1;
	}

	const UInt8 testBlock2[] = "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq";
	hash.Clear();
	hash.Calc(testBlock2, sizeof(testBlock2) - 1);
	hash.GetValue(hashVal);
	sptr = Text::StrHexBytes(sbuff, hashVal, 28, 0);
	if (!Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("75388B16512776CC5DBA5DA1FD890150B0C6455CB4F58B1952522525")))
	{
		return 1;
	}
	return 0;
}
