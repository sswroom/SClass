#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "Crypto/Hash/MD5.h"
#include "IO/ConsoleWriter.h"
#include "Text/MyString.h"

Int32 MyMain(Core::IProgControl *progCtrl)
{
	UInt8 testBlock[] = "The quick brown fox jumps over the lazy dog";
	UInt8 hashVal[32];
	UTF8Char sbuff[65];
	Crypto::Hash::IHash *hash;
	IO::ConsoleWriter console;

	NEW_CLASS(hash, Crypto::Hash::MD5());

	hash->GetValue(hashVal);
	Text::StrHexBytes(sbuff, hashVal, 16, 0);
	console.WriteLine(sbuff);
	console.WriteLineC(UTF8STRC("D41D8CD98F00B204E9800998ECF8427E"));
	console.WriteLine();

	hash->Calc(testBlock, sizeof(testBlock) - 1);
	hash->GetValue(hashVal);
	Text::StrHexBytes(sbuff, hashVal, 16, 0);
	console.WriteLine(sbuff);
	console.WriteLineC(UTF8STRC("9E107D9D372BB6826BD81D3542A419D6"));
	console.WriteLine();

	hash->Calc(testBlock, sizeof(testBlock) - 1);
	hash->GetValue(hashVal);
	Text::StrHexBytes(sbuff, hashVal, 16, 0);
	console.WriteLine(sbuff);
	console.WriteLineC(UTF8STRC("D27C6D8BCAA695E377D32387E115763C"));
	console.WriteLine();

	hash->Calc(testBlock, sizeof(testBlock) - 1);
	hash->GetValue(hashVal);
	Text::StrHexBytes(sbuff, hashVal, 16, 0);
	console.WriteLine(sbuff);
	console.WriteLineC(UTF8STRC("4E67DB4A7A406B0CFDADD887CDE7888E"));

	DEL_CLASS(hash);
	return 0;
}
