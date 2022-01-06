#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "Crypto/Encrypt/RC4Cipher.h"
#include "Data/ByteTool.h"
#include "IO/ConsoleWriter.h"
#include "Text/StringBuilderUTF8.h"

void TestEncode(UInt32 val, UOSInt bitCnt, IO::Writer *writer)
{
	UInt8 keyBuff[16] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F};
	Crypto::Encrypt::RC4Cipher rc4(keyBuff, bitCnt >> 3);
	UInt8 dataBuff[16];
	UInt8 outBuff[16];
	UOSInt outSize;
	WriteUInt32(&dataBuff[0], val);
	WriteUInt32(&dataBuff[4], val);
	WriteUInt32(&dataBuff[8], val);
	WriteUInt32(&dataBuff[12], val);
	outSize = rc4.Encrypt(dataBuff, 16, outBuff, 0);
	Text::StringBuilderUTF8 sb;
	sb.AppendC(UTF8STRC("DEC "));
	sb.AppendU32(val);
	sb.AppendChar(' ', 9 - sb.GetLength());
	sb.AppendHexBuff(outBuff, outSize, ' ', Text::LineBreakType::None);
	writer->WriteLineC(sb.ToString(), sb.GetLength());
}

//RFC 6229
void TestAllForBit(UOSInt bitCnt, IO::Writer *writer)
{
	Text::StringBuilderUTF8 sb;
	sb.AppendC(UTF8STRC("Key Length: "));
	sb.AppendUOSInt(bitCnt);
	sb.AppendC(UTF8STRC(" bits"));
	writer->WriteLineC(sb.ToString(), sb.GetLength());
	TestEncode(0, bitCnt, writer);
	TestEncode(16, bitCnt, writer);
	TestEncode(240, bitCnt, writer);
	TestEncode(256, bitCnt, writer);
	TestEncode(496, bitCnt, writer);
	TestEncode(512, bitCnt, writer);
	TestEncode(752, bitCnt, writer);
	TestEncode(768, bitCnt, writer);
	TestEncode(1008, bitCnt, writer);
	TestEncode(1024, bitCnt, writer);
	TestEncode(1520, bitCnt, writer);
	TestEncode(1536, bitCnt, writer);
	TestEncode(2032, bitCnt, writer);
	TestEncode(2048, bitCnt, writer);
	TestEncode(3056, bitCnt, writer);
	TestEncode(3072, bitCnt, writer);
	TestEncode(4080, bitCnt, writer);
	TestEncode(4096, bitCnt, writer);
	writer->WriteLine();
}

void TestEncode2(const Char *key, const Char *val, IO::Writer *writer)
{
	Crypto::Encrypt::RC4Cipher rc4((const UInt8 *)key, Text::StrCharCnt(key));
	UInt8 outBuff[16];
	UOSInt outSize;
	outSize = rc4.Encrypt((const UInt8*)val, Text::StrCharCnt(val), outBuff, 0);
	Text::StringBuilderUTF8 sb;
	sb.AppendHexBuff(outBuff, outSize, ' ', Text::LineBreakType::None);
	writer->WriteLineC(sb.ToString(), sb.GetLength());
}


Int32 MyMain(Core::IProgControl *progCtrl)
{
	IO::ConsoleWriter console;
	//TestAllForBit(40, &console);
	TestEncode2("Key", "Plaintext", &console);
	TestEncode2("Wiki", "pedia", &console);
	TestEncode2("Secret", "Attack at dawn", &console);
	return 0;
}
