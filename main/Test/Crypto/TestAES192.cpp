#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "Crypto/Encrypt/AES192.h"
#include "Text/MyString.h"

Int32 MyMain(NN<Core::IProgControl> progCtrl)
{
	UInt8 key[24] = {0x8e, 0x73, 0xb0, 0xf7, 0xda, 0x0e, 0x64, 0x52, 0xc8, 0x10, 0xf3, 0x2b, 0x80, 0x90, 0x79, 0xe5, 0x62, 0xf8, 0xea, 0xd2, 0x52, 0x2c, 0x6b, 0x7b};
	UInt8 testVector1[16] = {0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96, 0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a};
	UInt8 testVector2[16] = {0xae, 0x2d, 0x8a, 0x57, 0x1e, 0x03, 0xac, 0x9c, 0x9e, 0xb7, 0x6f, 0xac, 0x45, 0xaf, 0x8e, 0x51};
	UInt8 testVector3[16] = {0x30, 0xc8, 0x1c, 0x46, 0xa3, 0x5c, 0xe4, 0x11, 0xe5, 0xfb, 0xc1, 0x19, 0x1a, 0x0a, 0x52, 0xef};
	UInt8 testVector4[16] = {0xf6, 0x9f, 0x24, 0x45, 0xdf, 0x4f, 0x9b, 0x17, 0xad, 0x2b, 0x41, 0x7b, 0xe6, 0x6c, 0x37, 0x10};
	Crypto::Encrypt::AES192 aes(key);
	UInt8 cipherText[16];
	UInt8 decText[16];
	UTF8Char sbuff[64];
	UTF8Char *sptr;
	
	aes.Encrypt(testVector1, 16, cipherText);
	sptr = Text::StrHexBytes(sbuff, cipherText, 16, 0);
	if (!Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("BD334F1D6E45F25FF712A214571FA5CC")))
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
	if (!Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("974104846D0AD3AD7734ECB3ECEE4EEF")))
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
	if (!Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("EF7AFD2270E2E60ADCE0BA2FACE6444E")))
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
	if (!Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("9A4B41BA738D6C72FB16691603C18E0E")))
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
