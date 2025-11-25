#include "Stdafx.h"
#include "Core/Core.h"
#include "Crypto/Encrypt/Blowfish.h"
#include "Core/ByteTool_C.h"

Bool BFTest(UInt64 key, UInt64 plainText, UInt64 cipherText)
{
	UInt8 keyBuff[8];
	UInt8 plainBuff[8];
	UInt8 cipherBuff[8];
	WriteMUInt64(keyBuff, key);
	WriteMUInt64(plainBuff, plainText);
	Crypto::Encrypt::Blowfish bf(keyBuff, 8);
	bf.Encrypt(plainBuff, 8, cipherBuff);
	if (ReadMUInt64(cipherBuff) != cipherText)
	{
		return false;
	}
	WriteMUInt64(plainBuff, 0);
	bf.SetKey(keyBuff, 8);
	bf.Decrypt(cipherBuff, 8, plainBuff);
	if (ReadMUInt64(plainBuff) != plainText)
	{
		return false;
	}
	return true;
}

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
	if (!BFTest(0x0000000000000000, 0x0000000000000000, 0x4EF997456198DD78)) return 1;
	if (!BFTest(0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 0x51866FD5B85ECB8A)) return 1;
	if (!BFTest(0x3000000000000000, 0x1000000000000001, 0x7D856F9A613063F2)) return 1;
	if (!BFTest(0x1111111111111111, 0x1111111111111111, 0x2466DD878B963C9D)) return 1;
	if (!BFTest(0x0123456789ABCDEF, 0x1111111111111111, 0x61F9C3802281B096)) return 1;
	if (!BFTest(0x1111111111111111, 0x0123456789ABCDEF, 0x7D0CC630AFDA1EC7)) return 1;
	if (!BFTest(0xFEDCBA9876543210, 0x0123456789ABCDEF, 0x0ACEAB0FC6A0A28D)) return 1;
	if (!BFTest(0x7CA110454A1A6E57, 0x01A1D6D039776742, 0x59C68245EB05282B)) return 1;
	return 0;
}
