#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "Crypto/Hash/SHA384.h"
#include "IO/ConsoleWriter.h"
#include "Text/MyString.h"

Int32 MyMain(Core::IProgControl *progCtrl)
{
	UInt8 hashVal[48];
	UTF8Char sbuff[97];
	UTF8Char *sptr;
	Crypto::Hash::IHash *hash;
	IO::ConsoleWriter console;

	NEW_CLASS(hash, Crypto::Hash::SHA384());

	hash->GetValue(hashVal);
	sptr = Text::StrHexBytes(sbuff, hashVal, 48, 0);
	console.WriteLineC(sbuff, (UOSInt)(sptr - sbuff));
	console.WriteLineC(UTF8STRC("0c63a75b845e4f7d01107d852e4c2485c51a50aaaa94fc61995e71bbee983a2ac3713831264adb47fb6bd1e058d5f004"));
	console.WriteLine();

	const UInt8 testBlock[] = "abc";
	hash->Calc(testBlock, sizeof(testBlock) - 1);
	hash->GetValue(hashVal);
	sptr = Text::StrHexBytes(sbuff, hashVal, 48, 0);
	console.WriteLineC(sbuff, (UOSInt)(sptr - sbuff));
	console.WriteLineC(UTF8STRC("ec01498288516fc926459f58e2c6ad8df9b473cb0fc08c2596da7cf0e49be4b298d88cea927ac7f539f1edf228376d25"));
	console.WriteLine();

	const UInt8 testBlock2[] = "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq";
	hash->Clear();
	hash->Calc(testBlock2, sizeof(testBlock2) - 1);
	hash->GetValue(hashVal);
	sptr = Text::StrHexBytes(sbuff, hashVal, 48, 0);
	console.WriteLineC(sbuff, (UOSInt)(sptr - sbuff));
	console.WriteLineC(UTF8STRC("991c665755eb3a4b6bbdfb75c78a492e8c56a22c5c4d7e429bfdbc32b9d4ad5aa04a1f076e62fea19eef51acd0657c22"));

	DEL_CLASS(hash);
	return 0;
}
