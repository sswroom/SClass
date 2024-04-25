#include "Stdafx.h"
#include "Core/Core.h"
#include "Crypto/Hash/SHA224.h"
#include "Text/MyString.h"

Int32 MyMain(NN<Core::IProgControl> progCtrl)
{
/*	UInt8 hashVal[28];
	UTF8Char sbuff[57];
	UTF8Char *sptr;
	Crypto::Hash::SHA224 hash;

	hash.GetValue(hashVal);
	sptr = Text::StrHexBytes(sbuff, hashVal, 28, 0);
	if (!Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("6B4E03423667DBB73B6E15454F0EB1ABD4597F9A1B078E3F5B5A6BC7")))
	{
		return 1;
	}

	const UInt8 testBlock[] = "abc";
	hash.Calc(testBlock, sizeof(testBlock) - 1);
	hash.GetValue(hashVal);
	sptr = Text::StrHexBytes(sbuff, hashVal, 28, 0);
	if (!Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("E642824C3F8CF24AD09234EE7D3C766FC9A3A5168D0C94AD73B46FDF")))
	{
		return 1;
	}

	const UInt8 testBlock2[] = "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq";
	hash.Clear();
	hash.Calc(testBlock2, sizeof(testBlock2) - 1);
	hash.GetValue(hashVal);
	sptr = Text::StrHexBytes(sbuff, hashVal, 28, 0);
	if (!Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("8A24108B154ADA21C9FD5574494479BA5C7E7AB76EF264EAD0FCCE33")))
	{
		return 1;
	}*/

	return 0;
}
