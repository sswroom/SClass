#include "Stdafx.h"
#include "Core/Core.h"
#include "Crypto/Hash/Bcrypt.h"
#include "Text/StringBuilderUTF8.h"

Bool BCryptTest(NotNullPtr<Crypto::Hash::Bcrypt> bcrypt, Text::CStringNN hash, Text::CStringNN password)
{
	return bcrypt->Matches(hash, password);
}

Int32 MyMain(NotNullPtr<Core::IProgControl> progCtrl)
{
	Crypto::Hash::Bcrypt bcrypt;
	UInt8 radixTest[] = {0x85, 0x20, 0xAF, 0x9F, 0x03, 0x3D, 0xB3, 0x8C, 0x08, 0x5F, 0xD2, 0x5E, 0x2D, 0xAA, 0x5E, 0x84, 0xA2, 0xB9, 0x61, 0xD2, 0xF1, 0x29, 0xC9, 0xA4};
	UInt8 decTest[64];
	UOSInt decSize;
	Text::TextBinEnc::Radix64Enc radix64("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/");
	Text::StringBuilderUTF8 sb;
	radix64.EncodeBin(sb, radixTest, sizeof(radixTest));
	if (!sb.Equals(UTF8STRC("hSCvnwM9s4wIX9JeLapehKK5YdLxKcmk")))
	{
		return 1;
	}
	decSize = radix64.DecodeBin(sb.ToString(), sb.GetCharCnt(), decTest);
	if (!Text::StrEqualsC(decTest, decSize, radixTest, sizeof(radixTest)))
	{
		return 1;
	}
	if (!BCryptTest(bcrypt, CSTR("$2a$10$IIlT8V9XkWW5WxvoIjC/Q.hAGkGFd54hcUaGSBo8p1xL847drOwAa"), CSTR("admin"))) return 1;
	if (!BCryptTest(bcrypt, CSTR("$2y$11$hbIW55e9K0klAOTBidPNIeSszueX1nQbDZgGuNur5qAExTwX6pe8i"), CSTR("admin"))) return 1;
	if (!BCryptTest(bcrypt, CSTR("$2a$04$zVHmKQtGGQob.b/Nc7l9NO8UlrYcW05FiuCj/SxsFO/ZtiN9.mNzy"), CSTR(""))) return 1;
	if (!BCryptTest(bcrypt, CSTR("$2a$04$VYAclAMpaXY/oqAo9yUpkuWmoYywaPzyhu56HxXpVltnBIfmO9tgu"), CSTR("<.S.2K(Zq'"))) return 1;
	if (!BCryptTest(bcrypt, CSTR("$2a$12$aroG/pwwPj1tU5fl9a9pkO4rydAmkXRj/LqfHZOSnR6LGAZ.z.jwa"), CSTR("ptAP\"mcg6oH.\";c0U2_oll.OKi<!ku"))) return 1;
	if (!BCryptTest(bcrypt, CSTR("$2a$04$5DCebwootqWMCp59ISrMJ.l4WvgHIVg17ZawDIrDM2IjlE64GDNQS"), CSTR("a"))) return 1;
	if (!BCryptTest(bcrypt, CSTR("$2a$12$kQtGrSy5/39p96XsfTnpmuG1RiTw0KPKTSTsLuaooVr476.Ti9zcW"), CSTR("admin"))) return 1;

	sb.ClearStr();
	bcrypt.GenHash(sb, 10, CSTR("admin"));
	if (!bcrypt.Matches(sb.ToCString(), CSTR("admin")))
	{
		return 1;
	}
	return 0;
}
