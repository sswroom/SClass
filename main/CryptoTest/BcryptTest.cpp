#include "Stdafx.h"
#include "Core/Core.h"
#include "Crypto/Hash/Bcrypt.h"
#include "IO/ConsoleWriter.h"
#include "Manage/HiResClock.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"

void BCryptTest(IO::ConsoleWriter *console, Crypto::Hash::Bcrypt *bcrypt, const UTF8Char *hash, const UTF8Char *password)
{
	console->WriteLine(hash);
	console->WriteLine(password);
	Manage::HiResClock clk;
	Bool result = bcrypt->Matches(hash, password);
	Double t = clk.GetTimeDiff();
	Text::StringBuilderUTF8 sb;
	if (result)
	{
		sb.AppendC(UTF8STRC("Match"));
	}
	else
	{
		sb.AppendC(UTF8STRC("Not Match"));
	}
	sb.AppendC(UTF8STRC(", t = "));
	Text::SBAppendF64(&sb, t);
	console->WriteLine(sb.ToString());
	console->WriteLine();
}

Int32 MyMain(Core::IProgControl *progCtrl)
{
	IO::ConsoleWriter console;
	Crypto::Hash::Bcrypt bcrypt;
	UInt8 radixTest[] = {0x85, 0x20, 0xAF, 0x9F, 0x03, 0x3D, 0xB3, 0x8C, 0x08, 0x5F, 0xD2, 0x5E, 0x2D, 0xAA, 0x5E, 0x84, 0xA2, 0xB9, 0x61, 0xD2, 0xF1, 0x29, 0xC9, 0xA4};
	UInt8 decTest[64];
	UOSInt decSize;
	Text::TextBinEnc::Radix64Enc *radix64;
	NEW_CLASS(radix64, Text::TextBinEnc::Radix64Enc("./ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789"));
	Text::StringBuilderUTF8 sb;
	radix64->EncodeBin(&sb, radixTest, sizeof(radixTest));
	console.WriteLine((const UTF8Char*)"Radix64 test:");
	console.Write((const UTF8Char*)"Get: ");
	console.WriteLine(sb.ToString());
	console.WriteLine((const UTF8Char*)"Exp: hSCvnwM9s4wIX9JeLapehKK5YdLxKcmk");
	decSize = radix64->DecodeBin(sb.ToString(), sb.GetCharCnt(), decTest);
	sb.ClearStr();
	sb.AppendC(UTF8STRC("Dec: "));
	sb.AppendHexBuff(decTest, decSize, ' ', Text::LineBreakType::None);
	console.WriteLine(sb.ToString());
	console.WriteLine();
	BCryptTest(&console, &bcrypt, (const UTF8Char*)"$2a$10$IIlT8V9XkWW5WxvoIjC/Q.hAGkGFd54hcUaGSBo8p1xL847drOwAa", (const UTF8Char*)"admin");
	BCryptTest(&console, &bcrypt, (const UTF8Char*)"$2y$11$hbIW55e9K0klAOTBidPNIeSszueX1nQbDZgGuNur5qAExTwX6pe8i", (const UTF8Char*)"admin");
	BCryptTest(&console, &bcrypt, (const UTF8Char*)"$2a$04$zVHmKQtGGQob.b/Nc7l9NO8UlrYcW05FiuCj/SxsFO/ZtiN9.mNzy", (const UTF8Char*)"");
	BCryptTest(&console, &bcrypt, (const UTF8Char*)"$2a$04$VYAclAMpaXY/oqAo9yUpkuWmoYywaPzyhu56HxXpVltnBIfmO9tgu", (const UTF8Char*)"<.S.2K(Zq'");
	BCryptTest(&console, &bcrypt, (const UTF8Char*)"$2a$12$aroG/pwwPj1tU5fl9a9pkO4rydAmkXRj/LqfHZOSnR6LGAZ.z.jwa", (const UTF8Char*)"ptAP\"mcg6oH.\";c0U2_oll.OKi<!ku");
	BCryptTest(&console, &bcrypt, (const UTF8Char*)"$2a$04$5DCebwootqWMCp59ISrMJ.l4WvgHIVg17ZawDIrDM2IjlE64GDNQS", (const UTF8Char*)"a");
	BCryptTest(&console, &bcrypt, (const UTF8Char*)"$2a$12$kQtGrSy5/39p96XsfTnpmuG1RiTw0KPKTSTsLuaooVr476.Ti9zcW", (const UTF8Char*)"admin");

	sb.ClearStr();
	sb.AppendC(UTF8STRC("Generating hash: "));
	bcrypt.GenHash(&sb, 12, (const UTF8Char*)"admin");
	console.WriteLine(sb.ToString());
	return 0;
}
