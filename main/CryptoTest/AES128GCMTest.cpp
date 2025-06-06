#include "Stdafx.h"
#include "Core/Core.h"
#include "Crypto/Encrypt/AES128GCM.h"
#include "IO/FileStream.h"

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
	UInt8 key[] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff};
	UInt8 iv[] = {1, 2, 3, 4, 5, 6, 7, 8, 0, 0, 0, 0};
	Crypto::Encrypt::AES128GCM aes(key, iv);
	UInt8 inBuff[4096];
	UOSInt inSize;
	UInt8 outBuff[4096];
	UOSInt outSize;

	{
		IO::FileStream fs(CSTR("/home/sswroom/Progs/Temp/FileTest/EncTest/api.js"), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
		inSize = fs.Read(BYTEARR(inBuff));
	}
	outSize = aes.Encrypt(inBuff, inSize, outBuff);
	{
		IO::FileStream fs(CSTR("/home/sswroom/Progs/Temp/FileTest/EncTest/api.js.aes128gcm.enc"), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
		fs.WriteCont(outBuff, outSize);
	}

	{
		IO::FileStream fs(CSTR("/home/sswroom/Progs/Temp/FileTest/EncTest/api.js.aes128gcm.enc"), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
		inSize = fs.Read(BYTEARR(inBuff));
	}
	outSize = aes.Decrypt(inBuff, inSize, outBuff);
	{
		IO::FileStream fs(CSTR("/home/sswroom/Progs/Temp/FileTest/EncTest/api.js.aes128gcm"), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
		fs.WriteCont(outBuff, outSize);
	}
	return 0;
}