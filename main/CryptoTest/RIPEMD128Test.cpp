#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "Crypto/Hash/RIPEMD128.h"
#include "IO/Console.h"
#include "Text/MyString.h"

#define HASH_BYTES 16
#define PRINT_HASH(block) hash->Clear(); hash->Calc((UInt8*)block, sizeof(block) - 1); hash->GetValue(hashVal); Text::StrHexBytes(sbuff, hashVal, HASH_BYTES, 0); IO::Console::PrintStrO(sbuff);

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
/*	UInt8 testBlock1[] = "a";
	UInt8 testBlock2[] = "abc";
	UInt8 testBlock3[] = "message digest";
	UInt8 testBlock4[] = "abcdefghijklmnopqrstuvwxyz";
	UInt8 testBlock5[] = "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq";
	UInt8 testBlock6[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
	UInt8 testBlock7[] = "12345678901234567890123456789012345678901234567890123456789012345678901234567890";*/
	UInt8 hashVal[32];
	UTF8Char sbuff[65];
	Crypto::Hash::HashAlgorithm *hash;

	NEW_CLASS(hash, Crypto::Hash::RIPEMD128());

	PRINT_HASH("");
	IO::Console::PrintStrO(U8STR("\ncdf26213a150dc3ecb610f18f6b38b46\n\n"));

	PRINT_HASH("a");
	IO::Console::PrintStrO(U8STR("\n86be7afa339d0fc7cfc785e72f578d33\n\n"));

	PRINT_HASH("abc");
	IO::Console::PrintStrO(U8STR("\nc14a12199c66e4ba84636b0f69144c77\n\n"));

	PRINT_HASH("message digest");
	IO::Console::PrintStrO(U8STR("\n9e327b3d6e523062afc1132d7df9d1b8\n\n"));

	PRINT_HASH("abcdefghijklmnopqrstuvwxyz");
	IO::Console::PrintStrO(U8STR("\nfd2aa607f71dc8f510714922b371834e\n\n"));

	PRINT_HASH("abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq");
	IO::Console::PrintStrO(U8STR("\na1aa0689d0fafa2ddc22e88b49133a06\n\n"));

	PRINT_HASH("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789");
	IO::Console::PrintStrO(U8STR("\nd1e959eb179c911faea4624c60c5c702\n\n"));

	PRINT_HASH("12345678901234567890123456789012345678901234567890123456789012345678901234567890");
	IO::Console::PrintStrO(U8STR("\n3f45ef194732c2dbb2c4a2c769795fa3\n"));

	DEL_CLASS(hash);

	return 0;
}
