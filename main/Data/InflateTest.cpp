#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "Data/Compress/DeflateStream.h"
#include "Data/Compress/Inflate.h"
#include "Data/Compress/Inflater.h"
#include "Data/Compress/InflateStream.h"
#include "IO/ConsoleWriter.h"
#include "IO/FileStream.h"
#include "IO/MemoryStream.h"
#include "IO/StmData/MemoryDataRef.h"

Int32 MyMain(NN<Core::IProgControl> progCtrl)
{
	UInt8 *buff2;
	Text::CStringNN fileName = CSTR("/mnt/raid2_3/GPS/RAW/Hiking20250111.gpx");
	IO::MemoryStream oriStm;
	IO::MemoryStream srcStm;
	IO::ConsoleWriter console;
	{
		IO::FileStream fs(fileName, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
		if (fs.IsError())
		{
			console.WriteLine(CSTR("Error in reading source file"));
		}
		fs.ReadToEnd(oriStm, 65536);
	}
	{
		oriStm.SeekFromBeginning(0);
		Data::Compress::DeflateStream dstm(oriStm, oriStm.GetLength(), 0, Data::Compress::DeflateStream::CompLevel::MaxCompression, true);
		dstm.ReadToEnd(srcStm, 65536);
	}
	printf("srcSize = %d\r\n", (UInt32)srcStm.GetLength());
	UOSInt buffSize2;
	IO::MemoryStream mstm1((UOSInt)oriStm.GetLength());
	IO::MemoryStream mstm2((UOSInt)oriStm.GetLength());
	IO::MemoryStream mstm3((UOSInt)oriStm.GetLength());
	{
		Data::Compress::InflateStream istm(mstm1, true);
		istm.Write(srcStm.GetArray());
	}
	if (Text::StrEqualsC(oriStm.GetBuff(), (UOSInt)oriStm.GetLength(), mstm1.GetBuff(), (UOSInt)mstm1.GetLength()))
	{
		console.WriteLine(CSTR("InflateStream decompress correctly"));
	}
	else
	{
		console.WriteLine(CSTR("InflateStream decompress failed"));
		printf("destSize = %d\r\n", (UInt32)mstm1.GetLength());
	}

	{
		Data::Compress::Inflater istm(mstm2, true);
		istm.Write(srcStm.GetArray());
	}
	if (Text::StrEqualsC(oriStm.GetBuff(), (UOSInt)oriStm.GetLength(), mstm2.GetBuff(), (UOSInt)mstm2.GetLength()))
	{
		console.WriteLine(CSTR("Inflater decompress correctly"));
	}
	else
	{
		console.WriteLine(CSTR("Inflater decompress failed"));
	}

	{
		Data::Compress::Inflate inf(true);
		IO::StmData::MemoryDataRef mfd(srcStm.GetArray());
		inf.Decompress(mstm3, mfd);
	}
	if (Text::StrEqualsC(oriStm.GetBuff(), (UOSInt)oriStm.GetLength(), mstm3.GetBuff(), (UOSInt)mstm3.GetLength()))
	{
		console.WriteLine(CSTR("Inflate decompress correctly"));
	}
	else
	{
		console.WriteLine(CSTR("Inflate decompress failed"));
	}

	buff2 = MemAlloc(UInt8, (UOSInt)oriStm.GetLength() * 2);
	{
		Data::Compress::Inflater::DecompressDirect(Data::ByteArray(buff2, (UOSInt)oriStm.GetLength() * 2), buffSize2, srcStm.GetArray(), true);
	}
	if (Text::StrEqualsC(oriStm.GetBuff(), (UOSInt)oriStm.GetLength(), buff2, buffSize2))
	{
		console.WriteLine(CSTR("Inflater decompressDirect correctly"));
	}
	else
	{
		console.WriteLine(CSTR("Inflater decompressDirect failed"));
	}
	MemFree(buff2);
	return 0;
}

