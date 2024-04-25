#include "Stdafx.h"
#include "Core/Core.h"
#include "Crypto/Hash/SHA512.h"
#include "Text/MyString.h"

Int32 MyMain(NN<Core::IProgControl> progCtrl)
{
/*	UInt8 hashVal[64];
	UTF8Char sbuff[129];
	UTF8Char *sptr;
	Crypto::Hash::SHA512 hash;

	hash.GetValue(hashVal);
	sptr = Text::StrHexBytes(sbuff, hashVal, 64, 0);
	if (!Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("A69F73CCA23A9AC5C8B567DC185A756E97C982164FE25859E0D1DCC1475C80A615B2123AF1F5F94C11E3E9402C3AC558F500199D95B6D3E301758586281DCD26")))
	{
		return 1;
	}

	const UInt8 testBlock[] = "abc";
	hash.Calc(testBlock, sizeof(testBlock) - 1);
	hash.GetValue(hashVal);
	sptr = Text::StrHexBytes(sbuff, hashVal, 64, 0);
	if (!Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("B751850B1A57168A5693CD924B6B096E08F621827444F70D884F5D0240D2712E10E116E9192AF3C91A7EC57647E3934057340B4CF408D5A56592F8274EEC53F0")))
	{
		return 1;
	}

	const UInt8 testBlock2[] = "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq";
	hash.Clear();
	hash.Calc(testBlock2, sizeof(testBlock2) - 1);
	hash.GetValue(hashVal);
	sptr = Text::StrHexBytes(sbuff, hashVal, 64, 0);
	if (!Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("04A371E84ECFB5B8B77CB48610FCA8182DD457CE6F326A0FD3D7EC2F1E91636DEE691FBE0C985302BA1B0D8DC78C086346B533B49C030D99A27DAF1139D6E75E")))
	{
		return 1;
	}*/

	return 0;
}
