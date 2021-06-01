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
	Crypto::Hash::IHash *hash;
	IO::ConsoleWriter console;

	NEW_CLASS(hash, Crypto::Hash::SHA384());

	hash->GetValue(hashVal);
	Text::StrHexBytes(sbuff, hashVal, 48, 0);
	console.WriteLine(sbuff);
	console.WriteLine((const UTF8Char*)"38b060a751ac96384cd9327eb1b1e36a21fdb71114be07434c0cc7bf63f6e1da274edebfe76f65fbd51ad2f14898b95b");
	console.WriteLine();

	const UInt8 testBlock[] = "abc";
	hash->Calc(testBlock, sizeof(testBlock) - 1);
	hash->GetValue(hashVal);
	Text::StrHexBytes(sbuff, hashVal, 48, 0);
	console.WriteLine(sbuff);
	console.WriteLine((const UTF8Char*)"cb00753f45a35e8bb5a03d699ac65007272c32ab0eded1631a8b605a43ff5bed8086072ba1e7cc2358baeca134c825a7");
	console.WriteLine();

	const UInt8 testBlock2[] = "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq";
	hash->Clear();
	hash->Calc(testBlock2, sizeof(testBlock2) - 1);
	hash->GetValue(hashVal);
	Text::StrHexBytes(sbuff, hashVal, 48, 0);
	console.WriteLine(sbuff);
	console.WriteLine((const UTF8Char*)"3391fdddfc8dc7393707a65b1b4709397cf8b1d162af05abfe8f450de5f36bc6b0455a8520bc4e6f5fe95b1fe3c8452b");

	DEL_CLASS(hash);
	return 0;
}
