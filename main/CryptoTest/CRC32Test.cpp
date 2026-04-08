#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "Crypto/Hash/CRC32.h"
#include "IO/Console.h"
#include "Text/MyString.h"

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
	UInt8 testBlock[] = "123456789";
	UInt8 testBlock2[] = "12345678901234567";
	UInt8 testBlock3[] = "123456789012345678901234567890123";
	UInt8 hashVal[32];
	UTF8Char sbuff[65];
	Crypto::Hash::HashAlgorithm *hash;

	NEW_CLASS(hash, Crypto::Hash::CRC32());

	hash->GetValue(hashVal);
	Text::StrHexBytes(sbuff, hashVal, 4, 0);
	IO::Console::PrintStrO(sbuff);
	IO::Console::PrintStrO(U8STR("\n00000000\n\n"));

	hash->Calc(testBlock, sizeof(testBlock) - 1);
	hash->GetValue(hashVal);
	Text::StrHexBytes(sbuff, hashVal, 4, 0);
	IO::Console::PrintStrO(sbuff);
	IO::Console::PrintStrO(U8STR("\n89A1897F\n\n"));

	hash->Clear();
	hash->Calc(testBlock2, sizeof(testBlock2) - 1);
	hash->GetValue(hashVal);
	Text::StrHexBytes(sbuff, hashVal, 4, 0);
	IO::Console::PrintStrO(sbuff);
	IO::Console::PrintStrO(U8STR("\n797C7470\n\n"));

	hash->Clear();
	hash->Calc(testBlock3, sizeof(testBlock3) - 1);
	hash->GetValue(hashVal);
	Text::StrHexBytes(sbuff, hashVal, 4, 0);
	IO::Console::PrintStrO(sbuff);
	IO::Console::PrintStrO(U8STR("\n43F7F3FB\n"));

	DEL_CLASS(hash);
	return 0;
}
