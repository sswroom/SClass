#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "Crypto/Encrypt/AES128.h"
#include "IO/ConsoleWriter.h"
#include "Manage/HiResClock.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"

Int32 MyMain(Core::IProgControl *progCtrl)
{
	Text::StringBuilderUTF8 sb;
	IO::ConsoleWriter console;
	Crypto::Encrypt::AES128 *aes;
	UInt8 key[16] = {0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c};
	UInt8 testVector1[16] = {0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96, 0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a};
	UInt8 testVector2[16] = {0xae, 0x2d, 0x8a, 0x57, 0x1e, 0x03, 0xac, 0x9c, 0x9e, 0xb7, 0x6f, 0xac, 0x45, 0xaf, 0x8e, 0x51};
	UInt8 testVector3[16] = {0x30, 0xc8, 0x1c, 0x46, 0xa3, 0x5c, 0xe4, 0x11, 0xe5, 0xfb, 0xc1, 0x19, 0x1a, 0x0a, 0x52, 0xef};
	UInt8 testVector4[16] = {0xf6, 0x9f, 0x24, 0x45, 0xdf, 0x4f, 0x9b, 0x17, 0xad, 0x2b, 0x41, 0x7b, 0xe6, 0x6c, 0x37, 0x10};
	NEW_CLASS(aes, Crypto::Encrypt::AES128(key));
	UInt8 cipherText[16];
	UInt8 decText[16];
	
	sb.ClearStr();
	sb.Append((const UTF8Char*)"Encryption key: ");
	sb.AppendHexBuff(key, 16, 0, Text::LBT_NONE);
	console.WriteLine(sb.ToString());
	console.WriteLine();

	sb.ClearStr();
	sb.Append((const UTF8Char*)"Test vector: ");
	sb.AppendHexBuff(testVector1, 16, 0, Text::LBT_NONE);
	console.WriteLine(sb.ToString());

	aes->Encrypt(testVector1, 16, cipherText, 0);
	sb.ClearStr();
	sb.Append((const UTF8Char*)"Cipher text: ");
	sb.AppendHexBuff(cipherText, 16, 0, Text::LBT_NONE);
	console.WriteLine(sb.ToString());

	sb.ClearStr();
	sb.Append((const UTF8Char*)"Expected:    ");
	sb.Append((const UTF8Char*)"3ad77bb40d7a3660a89ecaf32466ef97");
	console.WriteLine(sb.ToString());

	aes->Decrypt(cipherText, 16, decText, 0);
	sb.ClearStr();
	sb.Append((const UTF8Char*)"DecryptText: ");
	sb.AppendHexBuff(decText, 16, 0, Text::LBT_NONE);
	console.WriteLine(sb.ToString());
	console.WriteLine();

	sb.ClearStr();
	sb.Append((const UTF8Char*)"Test vector: ");
	sb.AppendHexBuff(testVector2, 16, 0, Text::LBT_NONE);
	console.WriteLine(sb.ToString());

	aes->Encrypt(testVector2, 16, cipherText, 0);
	sb.ClearStr();
	sb.Append((const UTF8Char*)"Cipher text: ");
	sb.AppendHexBuff(cipherText, 16, 0, Text::LBT_NONE);
	console.WriteLine(sb.ToString());

	sb.ClearStr();
	sb.Append((const UTF8Char*)"Expected:    ");
	sb.Append((const UTF8Char*)"f5d3d58503b9699de785895a96fdbaaf");
	console.WriteLine(sb.ToString());

	aes->Decrypt(cipherText, 16, decText, 0);
	sb.ClearStr();
	sb.Append((const UTF8Char*)"DecryptText: ");
	sb.AppendHexBuff(decText, 16, 0, Text::LBT_NONE);
	console.WriteLine(sb.ToString());
	console.WriteLine();

	sb.ClearStr();
	sb.Append((const UTF8Char*)"Test vector: ");
	sb.AppendHexBuff(testVector3, 16, 0, Text::LBT_NONE);
	console.WriteLine(sb.ToString());

	aes->Encrypt(testVector3, 16, cipherText, 0);
	sb.ClearStr();
	sb.Append((const UTF8Char*)"Cipher text: ");
	sb.AppendHexBuff(cipherText, 16, 0, Text::LBT_NONE);
	console.WriteLine(sb.ToString());

	sb.ClearStr();
	sb.Append((const UTF8Char*)"Expected:    ");
	sb.Append((const UTF8Char*)"43b1cd7f598ece23881b00e3ed030688");
	console.WriteLine(sb.ToString());

	aes->Decrypt(cipherText, 16, decText, 0);
	sb.ClearStr();
	sb.Append((const UTF8Char*)"DecryptText: ");
	sb.AppendHexBuff(decText, 16, 0, Text::LBT_NONE);
	console.WriteLine(sb.ToString());
	console.WriteLine();

	sb.ClearStr();
	sb.Append((const UTF8Char*)"Test vector: ");
	sb.AppendHexBuff(testVector4, 16, 0, Text::LBT_NONE);
	console.WriteLine(sb.ToString());

	aes->Encrypt(testVector4, 16, cipherText, 0);
	sb.ClearStr();
	sb.Append((const UTF8Char*)"Cipher text: ");
	sb.AppendHexBuff(cipherText, 16, 0, Text::LBT_NONE);
	console.WriteLine(sb.ToString());

	sb.ClearStr();
	sb.Append((const UTF8Char*)"Expected:    ");
	sb.Append((const UTF8Char*)"7b0c785e27e8ad3f8223207104725dd4");
	console.WriteLine(sb.ToString());
	aes->Decrypt(cipherText, 16, decText, 0);

	sb.ClearStr();
	sb.Append((const UTF8Char*)"DecryptText: ");
	sb.AppendHexBuff(decText, 16, 0, Text::LBT_NONE);
	console.WriteLine(sb.ToString());
	console.WriteLine();

	UInt8 *tempBuff = MemAlloc(UInt8, 1048576);
	MemClear(tempBuff, 1048576);
	Manage::HiResClock clk;
	UOSInt i = 100;
	while (i-- > 0)
	{
		aes->Encrypt(tempBuff, 1048576, tempBuff, 0);
	}
	Double t = clk.GetTimeDiff();
	sb.ClearStr();
	sb.Append((const UTF8Char*)"Encrypt rate = ");
	Text::SBAppendF64(&sb, 104857600 / t);
	console.WriteLine(sb.ToString());

	clk.Start();
	i = 100;
	while (i-- > 0)
	{
		aes->Decrypt(tempBuff, 1048576, tempBuff, 0);
	}
	t = clk.GetTimeDiff();
	sb.ClearStr();
	sb.Append((const UTF8Char*)"Decrypt rate = ");
	Text::SBAppendF64(&sb, 104857600 / t);
	console.WriteLine(sb.ToString());
	MemFree(tempBuff);

	DEL_CLASS(aes);
	return 0;
}
