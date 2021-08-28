#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "Crypto/Encrypt/Blowfish.h"
#include "Data/ByteTool.h"
#include "IO/ConsoleWriter.h"
#include "Text/StringBuilderUTF8.h"

IO::ConsoleWriter *console;
void BFTest(UInt64 key, UInt64 plainText, UInt64 cipherText)
{
	UInt8 keyBuff[8];
	UInt8 plainBuff[8];
	UInt8 cipherBuff[8];
	WriteMUInt64(keyBuff, key);
	WriteMUInt64(plainBuff, plainText);
	Text::StringBuilderUTF8 sb;
	sb.Append((const UTF8Char*)"Testing Key = ");
	sb.AppendHex64(key);
	console->WriteLine(sb.ToString());
	sb.ClearStr();
	sb.Append((const UTF8Char*)"OriText = ");
	sb.AppendHex64(plainText);
	console->WriteLine(sb.ToString());
	sb.ClearStr();
	sb.Append((const UTF8Char*)"ExpText = ");
	sb.AppendHex64(cipherText);
	console->WriteLine(sb.ToString());
	Crypto::Encrypt::Blowfish bf(keyBuff, 8);
	bf.Encrypt(plainBuff, 8, cipherBuff, 0);
	sb.ClearStr();
	sb.Append((const UTF8Char*)"EncText = ");
	sb.AppendHexBuff(cipherBuff, 8, 0, Text::LBT_NONE);
	console->WriteLine(sb.ToString());
	bf.SetKey(keyBuff, 8);
	bf.Decrypt(cipherBuff, 8, plainBuff, 0);
	sb.ClearStr();
	sb.Append((const UTF8Char*)"DecText = ");
	sb.AppendHexBuff(plainBuff, 8, 0, Text::LBT_NONE);
	console->WriteLine(sb.ToString());
	console->WriteLine();	
}

Int32 MyMain(Core::IProgControl *progCtrl)
{
	NEW_CLASS(console, IO::ConsoleWriter());
	BFTest(0x0000000000000000, 0x0000000000000000, 0x4EF997456198DD78);
	BFTest(0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 0x51866FD5B85ECB8A);
	BFTest(0x3000000000000000, 0x1000000000000001, 0x7D856F9A613063F2);
	BFTest(0x1111111111111111, 0x1111111111111111, 0x2466DD878B963C9D);
	BFTest(0x0123456789ABCDEF, 0x1111111111111111, 0x61F9C3802281B096);
	BFTest(0x1111111111111111, 0x0123456789ABCDEF, 0x7D0CC630AFDA1EC7);
	BFTest(0xFEDCBA9876543210, 0x0123456789ABCDEF, 0x0ACEAB0FC6A0A28D);
	BFTest(0x7CA110454A1A6E57, 0x01A1D6D039776742, 0x59C68245EB05282B);
	DEL_CLASS(console);
	return 0;
}
