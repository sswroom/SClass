#include "Stdafx.h"
#include "Core/Core.h"
#include "Crypto/Encrypt/AES256.h"
#include "Crypto/Encrypt/AES256GCM.h"
#include "Manage/HiResClock.h"
#include "Text/StringBuilderUTF8.h"

#define TEST_LENG 1048576

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
	UnsafeArray<UInt8> plainText = MemAllocArr(UInt8, TEST_LENG);
	UIntOS i = 0;
	UIntOS j = TEST_LENG;
	while (i < j)
	{
		plainText[i] = (UInt8)(i & 0xff);
		i++;
	}
	UInt8 key[] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff, 0xff, 0xee, 0xdd, 0xcc, 0xbb, 0xaa, 0x99, 0x88, 0x77, 0x66, 0x55, 0x44, 0x33, 0x22, 0x11, 0x00};
	UInt8 iv[] = {1, 2, 3, 4, 5, 6, 7, 8, 0, 0, 0, 0};
	UnsafeArray<UInt8> outBuff = MemAllocArr(UInt8, TEST_LENG + 16);
	UIntOS outSize;
	Manage::HiResClock clk;
	Double t;
	Crypto::Encrypt::AES256GCM aes(key);
	aes.SetChainMode(Crypto::Encrypt::ChainMode::GCM);
	aes.SetIV(iv);
	outSize = aes.Encrypt(plainText, TEST_LENG, outBuff);
	t = clk.GetTimeDiff();
	Text::StringBuilderUTF8 sb;
	sb.AppendHexBuff(Data::ByteArrayR(&outBuff[outSize - 16], 16), ' ', Text::LineBreakType::CRLF);
	printf("AES256GCM.Encrypt(%d) %lf: %s\r\n", (UInt32)outSize, t, sb.v.Ptr());

	clk.Start();
	Crypto::Encrypt::AES256 aes2(key);
	aes2.SetChainMode(Crypto::Encrypt::ChainMode::GCM);
	aes2.SetIV(iv);
	outSize = aes2.Encrypt(plainText, TEST_LENG, outBuff);
	t = clk.GetTimeDiff();
	sb.ClearStr();
	sb.AppendHexBuff(Data::ByteArrayR(&outBuff[outSize - 16], 16), ' ', Text::LineBreakType::CRLF);
	printf("AES256GCM.Encrypt2(%d): %lf, %s\r\n", (UInt32)outSize, t, sb.v.Ptr());

	MemFreeArr(outBuff);
	MemFreeArr(plainText);
	return 0;
}

