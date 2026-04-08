#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "Crypto/Hash/RIPEMD160.h"
#include "IO/Console.h"
#include "Text/MyString.h"

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
	UInt8 testBlock[] = "The quick brown fox jumps over the lazy dog";
	UInt8 hashVal[32];
	UTF8Char sbuff[65];
	Crypto::Hash::HashAlgorithm *hash;

	NEW_CLASS(hash, Crypto::Hash::RIPEMD160());

	hash->GetValue(hashVal);
	Text::StrHexBytes(sbuff, hashVal, 20, 0);
	IO::Console::PrintStrO(sbuff);
	IO::Console::PrintStrO(U8STR("\n9c1185a5c5e9fc54612808977ee8f548b2258d31\n\n"));

	hash->Calc(testBlock, sizeof(testBlock) - 1);
	hash->GetValue(hashVal);
	Text::StrHexBytes(sbuff, hashVal, 20, 0);
	IO::Console::PrintStrO(sbuff);
	IO::Console::PrintStrO(U8STR("\n37f332f68db77bd9d7edd4969571ad671cf9dd3b\n"));

	DEL_CLASS(hash);
	return 0;
}
