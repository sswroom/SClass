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
	console.WriteLineC(UTF8STRC("d14a028c2a3a2bc9476102bb288234c415a2b01f828ea62ac5b3e42f"));
	console.WriteLine();

	const UInt8 testBlock[] = "abc";
	hash->Calc(testBlock, sizeof(testBlock) - 1);
	hash->GetValue(hashVal);
	Text::StrHexBytes(sbuff, hashVal, 28, 0);
	console.WriteLine(sbuff);
	console.WriteLineC(UTF8STRC("23097d223405d8228642a477bda255b32aadbce4bda0b3f7e36c9da7"));
	console.WriteLine();

	const UInt8 testBlock2[] = "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq";
	hash->Clear();
	hash->Calc(testBlock2, sizeof(testBlock2) - 1);
	hash->GetValue(hashVal);
	Text::StrHexBytes(sbuff, hashVal, 28, 0);
	console.WriteLine(sbuff);
	console.WriteLineC(UTF8STRC("75388b16512776cc5dba5da1fd890150b0c6455cb4f58b1952522525"));

	DEL_CLASS(hash);
	return 0;
}
