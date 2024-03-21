#include "Stdafx.h"
#include "Core/Core.h"
#include "Crypto/Encrypt/AES128.h"
#include "IO/ConsoleWriter.h"
#include "Text/MyString.h"

Int32 MyMain(NotNullPtr<Core::IProgControl> progCtrl)
{
	UInt8 key[16] = {0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c};
	UInt8 testVector1[16] = {0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96, 0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a};
	UInt8 testVector2[16] = {0xae, 0x2d, 0x8a, 0x57, 0x1e, 0x03, 0xac, 0x9c, 0x9e, 0xb7, 0x6f, 0xac, 0x45, 0xaf, 0x8e, 0x51};
	UInt8 testVector3[16] = {0x30, 0xc8, 0x1c, 0x46, 0xa3, 0x5c, 0xe4, 0x11, 0xe5, 0xfb, 0xc1, 0x19, 0x1a, 0x0a, 0x52, 0xef};
	UInt8 testVector4[16] = {0xf6, 0x9f, 0x24, 0x45, 0xdf, 0x4f, 0x9b, 0x17, 0xad, 0x2b, 0x41, 0x7b, 0xe6, 0x6c, 0x37, 0x10};
	Crypto::Encrypt::AES128 aes(key);
	UInt8 cipherText[16];
	UInt8 decText[16];
	UTF8Char sbuff[64];
	UTF8Char *sptr;

	aes.Encrypt(testVector1, 16, cipherText);
	sptr = Text::StrHexBytes(sbuff, cipherText, 16, 0);
	if (!Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("3AD77BB40D7A3660A89ECAF32466EF97")))
	{
		return 1;
	}
	aes.Decrypt(cipherText, 16, decText);
	if (!Text::StrEqualsC(decText, 16, testVector1, 16))
	{
		return 1;
	}

	aes.Encrypt(testVector2, 16, cipherText);
	sptr = Text::StrHexBytes(sbuff, cipherText, 16, 0);
	if (!Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("F5D3D58503B9699DE785895A96FDBAAF")))
	{
		return 1;
	}
	aes.Decrypt(cipherText, 16, decText);
	if (!Text::StrEqualsC(decText, 16, testVector2, 16))
	{
		return 1;
	}

	aes.Encrypt(testVector3, 16, cipherText);
	sptr = Text::StrHexBytes(sbuff, cipherText, 16, 0);
	if (!Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("43B1CD7F598ECE23881B00E3ED030688")))
	{
		return 1;
	}
	aes.Decrypt(cipherText, 16, decText);
	if (!Text::StrEqualsC(decText, 16, testVector3, 16))
	{
		return 1;
	}

	aes.Encrypt(testVector4, 16, cipherText);
	sptr = Text::StrHexBytes(sbuff, cipherText, 16, 0);
	if (!Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("7B0C785E27E8AD3F8223207104725DD4")))
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
