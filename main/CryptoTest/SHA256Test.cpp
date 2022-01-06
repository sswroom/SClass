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
	console.WriteLineC(UTF8STRC("e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855"));
	console.WriteLine();

	const UInt8 testBlock[] = "abc";
	hash->Calc(testBlock, sizeof(testBlock) - 1);
	hash->GetValue(hashVal);
	Text::StrHexBytes(sbuff, hashVal, 32, 0);
	console.WriteLine(sbuff);
	console.WriteLineC(UTF8STRC("ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad"));
	console.WriteLine();

	const UInt8 testBlock2[] = "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq";
	hash->Clear();
	hash->Calc(testBlock2, sizeof(testBlock2) - 1);
	hash->GetValue(hashVal);
	Text::StrHexBytes(sbuff, hashVal, 32, 0);
	console.WriteLine(sbuff);
	console.WriteLineC(UTF8STRC("248d6a61d20638b8e5c026930c3e6039a33ce45964ff2167f6ecedd419db06c1"));

	DEL_CLASS(hash);
	return 0;
}
