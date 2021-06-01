#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "Crypto/Hash/SHA256.h"
#include "IO/ConsoleWriter.h"
#include "Text/MyString.h"

Int32 MyMain(Core::IProgControl *progCtrl)
{
	UInt8 hashVal[32];
	UTF8Char sbuff[65];
	Crypto::Hash::IHash *hash;
	IO::ConsoleWriter console;

	NEW_CLASS(hash, Crypto::Hash::SHA256());

	hash->GetValue(hashVal);
	Text::StrHexBytes(sbuff, hashVal, 32, 0);
	console.WriteLine(sbuff);
	console.WriteLine((const UTF8Char*)"a7ffc6f8bf1ed76651c14756a061d662f580ff4de43b49fa82d80a4b80f8434a");
	console.WriteLine();

	const UInt8 testBlock[] = "abc";
	hash->Calc(testBlock, sizeof(testBlock) - 1);
	hash->GetValue(hashVal);
	Text::StrHexBytes(sbuff, hashVal, 32, 0);
	console.WriteLine(sbuff);
	console.WriteLine((const UTF8Char*)"3a985da74fe225b2045c172d6bd390bd855f086e3e9d525b46bfe24511431532");
	console.WriteLine();

	const UInt8 testBlock2[] = "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq";
	hash->Clear();
	hash->Calc(testBlock2, sizeof(testBlock2) - 1);
	hash->GetValue(hashVal);
	Text::StrHexBytes(sbuff, hashVal, 32, 0);
	console.WriteLine(sbuff);
	console.WriteLine((const UTF8Char*)"41c0dba2a9d6240849100376a8235e2c82e1b9998a999e21db32dd97496d3376");

	DEL_CLASS(hash);
	return 0;
}
