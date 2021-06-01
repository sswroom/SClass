#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "Crypto/Hash/SHA224.h"
#include "IO/ConsoleWriter.h"
#include "Text/MyString.h"

Int32 MyMain(Core::IProgControl *progCtrl)
{
	UInt8 hashVal[28];
	UTF8Char sbuff[57];
	Crypto::Hash::IHash *hash;
	IO::ConsoleWriter console;

	NEW_CLASS(hash, Crypto::Hash::SHA224());

	hash->GetValue(hashVal);
	Text::StrHexBytes(sbuff, hashVal, 28, 0);
	console.WriteLine(sbuff);
	console.WriteLine((const UTF8Char*)"6b4e03423667dbb73b6e15454f0eb1abd4597f9a1b078e3f5b5a6bc7");
	console.WriteLine();

	const UInt8 testBlock[] = "abc";
	hash->Calc(testBlock, sizeof(testBlock) - 1);
	hash->GetValue(hashVal);
	Text::StrHexBytes(sbuff, hashVal, 28, 0);
	console.WriteLine(sbuff);
	console.WriteLine((const UTF8Char*)"e642824c3f8cf24ad09234ee7d3c766fc9a3a5168d0c94ad73b46fdf");
	console.WriteLine();

	const UInt8 testBlock2[] = "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq";
	hash->Clear();
	hash->Calc(testBlock2, sizeof(testBlock2) - 1);
	hash->GetValue(hashVal);
	Text::StrHexBytes(sbuff, hashVal, 28, 0);
	console.WriteLine(sbuff);
	console.WriteLine((const UTF8Char*)"8a24108b154ada21c9fd5574494479ba5c7e7ab76ef264ead0fcce33");

	DEL_CLASS(hash);
	return 0;
}
