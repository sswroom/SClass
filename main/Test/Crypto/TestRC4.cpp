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
	outSize = rc4.Encrypt(dataBuff, 16, outBuff);
	Text::StringBuilderUTF8 sb;
	sb.AppendC(UTF8STRC("DEC "));
	sb.AppendU32(val);
	sb.AppendChar(' ', 9 - sb.GetLength());
	sb.AppendHexBuff(outBuff, outSize, ' ', Text::LineBreakType::None);
	writer->WriteLine(sb.ToCString());
}

//RFC 6229
void TestAllForBit(UOSInt bitCnt, IO::Writer *writer)
{
	Text::StringBuilderUTF8 sb;
	sb.AppendC(UTF8STRC("Key Length: "));
	sb.AppendUOSInt(bitCnt);
	sb.AppendC(UTF8STRC(" bits"));
	writer->WriteLine(sb.ToCString());
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

Bool TestEncode2(UnsafeArray<const UTF8Char> key, UOSInt keyLen, UnsafeArray<const UTF8Char> val, UOSInt valLen, UnsafeArray<const UTF8Char> expRes, UOSInt expResLen)
{
	Crypto::Encrypt::RC4Cipher rc4(key, keyLen);
	UInt8 outBuff[16];
	UOSInt outSize;
	UInt8 decBuff[16];
	UOSInt decSize;
	UTF8Char sbuff[33];
	UnsafeArray<UTF8Char> sptr;
	outSize = rc4.Encrypt(val.Ptr(), valLen, outBuff);
	sptr = Text::StrHexBytes(sbuff, outBuff, outSize, 0);
	if (!Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), expRes, expResLen))
	{
		return false;
	}
	rc4.SetKey(key, keyLen);
	decSize = rc4.Decrypt(outBuff, outSize, decBuff);
	if (!Text::StrEqualsC(decBuff, decSize, val, valLen))
	{
		return false;
	}

	return true;
}


Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
	//TestAllForBit(40, &console);
	if (!TestEncode2(UTF8STRC("Key"), UTF8STRC("Plaintext"), UTF8STRC("BBF316E8D940AF0AD3"))) return 1;
	if (!TestEncode2(UTF8STRC("Wiki"), UTF8STRC("pedia"), UTF8STRC("1021BF0420"))) return 1;
	if (!TestEncode2(UTF8STRC("Secret"), UTF8STRC("Attack at dawn"), UTF8STRC("45A01F645FC35B383552544B9BF5"))) return 1;
	return 0;
}
