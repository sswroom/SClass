#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "Crypto/Encrypt/AES256.h"
#include "Text/MyString.h"

Int32 MyMain(NN<Core::IProgControl> progCtrl)
{
	UInt8 key[32] = {0x60, 0x3d, 0xeb, 0x10, 0x15, 0xca, 0x71, 0xbe, 0x2b, 0x73, 0xae, 0xf0, 0x85, 0x7d, 0x77, 0x81, 0x1f, 0x35, 0x2c, 0x07, 0x3b, 0x61, 0x08, 0xd7, 0x2d, 0x98, 0x10, 0xa3, 0x09, 0x14, 0xdf, 0xf4};
	UInt8 testVector1[16] = {0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96, 0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a};
	UInt8 testVector2[16] = {0xae, 0x2d, 0x8a, 0x57, 0x1e, 0x03, 0xac, 0x9c, 0x9e, 0xb7, 0x6f, 0xac, 0x45, 0xaf, 0x8e, 0x51};
	UInt8 testVector3[16] = {0x30, 0xc8, 0x1c, 0x46, 0xa3, 0x5c, 0xe4, 0x11, 0xe5, 0xfb, 0xc1, 0x19, 0x1a, 0x0a, 0x52, 0xef};
	UInt8 testVector4[16] = {0xf6, 0x9f, 0x24, 0x45, 0xdf, 0x4f, 0x9b, 0x17, 0xad, 0x2b, 0x41, 0x7b, 0xe6, 0x6c, 0x37, 0x10};
	Crypto::Encrypt::AES256 aes(key);
	UInt8 cipherText[16];
	UInt8 decText[16];
	UTF8Char sbuff[64];
	UnsafeArray<UTF8Char> sptr;

	aes.Encrypt(testVector1, 16, cipherText);
	sptr = Text::StrHexBytes(sbuff, cipherText, 16, 0);
	if (!Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("F3EED1BDB5D2A03C064B5A7E3DB181F8")))
	{
		return 1;
	}
	aes.Decrypt(cipherText, 16, decText);
	if (!Text::StrEqualsC(decText, 16, testVector1, 16))
	{
		return 1;
	}

	aes.SetKey(key);
	aes.Encrypt(testVector2, 16, cipherText);
	sptr = Text::StrHexBytes(sbuff, cipherText, 16, 0);
	if (!Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("591CCB10D410ED26DC5BA74A31362870")))
	{
		return 1;
	}
	aes.Decrypt(cipherText, 16, decText);
	if (!Text::StrEqualsC(decText, 16, testVector2, 16))
	{
		return 1;
	}

	aes.SetKey(key);
	aes.Encrypt(testVector3, 16, cipherText);
	sptr = Text::StrHexBytes(sbuff, cipherText, 16, 0);
	if (!Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("B6ED21B99CA6F4F9F153E7B1BEAFED1D")))
	{
		return 1;
	}
	aes.Decrypt(cipherText, 16, decText);
	if (!Text::StrEqualsC(decText, 16, testVector3, 16))
	{
		return 1;
	}

	aes.SetKey(key);
	aes.Encrypt(testVector4, 16, cipherText);
	sptr = Text::StrHexBytes(sbuff, cipherText, 16, 0);
	if (!Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("23304B7A39F9F3FF067D8D8F9E24ECC7")))
	{
		return 1;
	}
	aes.Decrypt(cipherText, 16, decText);
	if (!Text::StrEqualsC(decText, 16, testVector4, 16))
	{
		return 1;
	}

	return 0;
}
