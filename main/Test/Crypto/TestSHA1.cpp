#include "Stdafx.h"
#include "Core/Core.h"
#include "Crypto/Hash/SHA1.h"
#include "Text/MyString.h"

Int32 MyMain(NN<Core::IProgControl> progCtrl)
{
	UInt8 hashVal[28];
	UTF8Char sbuff[57];
	UTF8Char *sptr;
	Crypto::Hash::SHA1 hash;

	hash.GetValue(hashVal);
	sptr = Text::StrHexBytes(sbuff, hashVal, 20, 0);
	if (!Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("DA39A3EE5E6B4B0D3255BFEF95601890AFD80709")))
	{
		return 1;
	}

	const UInt8 testBlock[] = "abc";
	hash.Calc(testBlock, sizeof(testBlock) - 1);
	hash.GetValue(hashVal);
	sptr = Text::StrHexBytes(sbuff, hashVal, 20, 0);
	if (!Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("A9993E364706816ABA3E25717850C26C9CD0D89D")))
	{
		return 1;
	}

	const UInt8 testBlock2[] = "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq";
	hash.Clear();
	hash.Calc(testBlock2, sizeof(testBlock2) - 1);
	hash.GetValue(hashVal);
	sptr = Text::StrHexBytes(sbuff, hashVal, 20, 0);
	if (!Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("84983E441C3BD26EBAAE4AA1F95129E5E54670F1")))
	{
		return 1;
	}
	return 0;
}
