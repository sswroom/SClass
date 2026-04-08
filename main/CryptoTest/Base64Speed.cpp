#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "Crypto/Encrypt/Base64.h"
#include "IO/FileStream.h"
#include "Manage/HiResClock.h"

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
	UnsafeArray<UInt8> oriBytes;
	UnsafeArray<UInt8> outBytes;
	UnsafeArray<UInt8> decBytes;
	Crypto::Encrypt::Base64 b64;
	Manage::HiResClock clk;
	Double t1;
	Double t2;
	UInt64 leng;
	UIntOS outSize;
	UIntOS decSize;
	UIntOS i;
	IO::FileStream fs(CSTR("Tramway_Server.jar"), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
	oriBytes = MemAllocArr(UInt8, leng = fs.GetLength());
	outBytes = MemAllocArr(UInt8, leng << 1);
	decBytes = MemAllocArr(UInt8, leng);
	fs.Read(Data::ByteArray(oriBytes, (UIntOS)leng));
	clk.Start();
	i = 100;
	while (i-- > 0)
		outSize = b64.Encrypt(oriBytes, (UIntOS)leng, outBytes);
	t1 = clk.GetTimeDiff();
	clk.Start();
	i = 100;
	while (i-- > 0)
		decSize = b64.Decrypt(outBytes, outSize, decBytes);
	t2 = clk.GetTimeDiff();
	printf("Ori: %llu, Enc: %u, Dec: %u, t1: %lf, t2: %lf\n", leng, (UInt32)outSize, (UInt32)decSize, t1, t2);
	i = decSize;
	while (i-- > 0)
	{
		if (decBytes[i] != oriBytes[i])
		{
			printf("Not equal at %d", (UInt32)i);
			break;
		}
	}
	MemFreeArr(oriBytes);
	MemFreeArr(outBytes);
	MemFreeArr(decBytes);
	return 0;
}
