#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "Crypto/Hash/CRC16R.h"
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

	NEW_CLASS(hash, Crypto::Hash::CRC16R());

	hash->GetValue(hashVal);
	Text::StrHexBytes(sbuff, hashVal, 2, 0);
	IO::Console::PrintStrO(sbuff);
	IO::Console::PrintStrO(U8STR("\n0000\n\n"));

	hash->Calc(testBlock, sizeof(testBlock) - 1);
	hash->GetValue(hashVal);
	Text::StrHexBytes(sbuff, hashVal, 2, 0);
	IO::Console::PrintStrO(sbuff);
	IO::Console::PrintStrO(U8STR("\nB4C8\n\n"));

	hash->Clear();
	hash->Calc(testBlock2, sizeof(testBlock2) - 1);
	hash->GetValue(hashVal);
	Text::StrHexBytes(sbuff, hashVal, 2, 0);
	IO::Console::PrintStrO(sbuff);
	IO::Console::PrintStrO(U8STR("\n802D\n\n"));

	hash->Clear();
	hash->Calc(testBlock3, sizeof(testBlock3) - 1);
	hash->GetValue(hashVal);
	Text::StrHexBytes(sbuff, hashVal, 2, 0);
	IO::Console::PrintStrO(sbuff);
	IO::Console::PrintStrO(U8STR("\nE1B0\n"));

	DEL_CLASS(hash);
	return 0;
}
