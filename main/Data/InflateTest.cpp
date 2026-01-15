#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "Data/Compress/Deflater.h"
#include "Data/Compress/DeflateStream.h"
#include "Data/Compress/Inflate.h"
#include "Data/Compress/Inflater.h"
#include "Data/Compress/InflateStream.h"
#include "IO/ConsoleWriter.h"
#include "IO/FileStream.h"
#include "IO/MemoryStream.h"
#include "IO/StmData/MemoryDataRef.h"

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
	UInt8 *buff2;
	Text::CStringNN fileName = CSTR("/home/sswroom/Temp/OruxMapsImages.db");
//	Text::CStringNN fileName = CSTR("/home/sswroom/Temp/Hiking20240804.gpx");
	IO::MemoryStream oriStm;
	IO::MemoryStream srcStm;
	UInt8 *srcBuff;
	UIntOS srcSize2 = 0;
	UIntOS srcSize3 = 0;
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
		Data::Compress::DeflateStream dstm(oriStm, oriStm.GetLength(), nullptr, Data::Compress::DeflateStream::CompLevel::MaxCompression, true);
		dstm.ReadToEnd(srcStm, 65536);
	}
	{
		srcBuff = MemAlloc(UInt8, (UIntOS)oriStm.GetLength());
		if (Data::Compress::Deflater::CompressDirect(Data::ByteArray(srcBuff, (UIntOS)oriStm.GetLength()), srcSize2, oriStm.GetArray(), Data::Compress::Deflater::CompLevel::BestCompression, true))
		{
			console.WriteLine(CSTR("Deflater CompressDirect success"));
		}
		else
		{
			console.WriteLine(CSTR("Deflater CompressDirect failed"));
		}
		MemFree(srcBuff);
	}
	{
		IO::MemoryStream tmpStm;
		oriStm.SeekFromBeginning(0);
		Data::Compress::Deflater dstm(oriStm, oriStm.GetLength(), nullptr, Data::Compress::Deflater::CompLevel::BestCompression, true);
		dstm.ReadToEnd(tmpStm, 65536);
		srcSize3 = (UIntOS)tmpStm.GetLength();
		if (srcSize3 > 0)
		{
			console.WriteLine(CSTR("Deflater Compress success"));
		}
		else
		{
			console.WriteLine(CSTR("Deflater Compress failed"));
		}
	}
	printf("srcSize = %d\r\n", (UInt32)srcStm.GetLength());
	printf("srcSize2 = %d\r\n", (UInt32)srcSize2);
	printf("srcSize3 = %d\r\n", (UInt32)srcSize3);
	UIntOS buffSize2;
	IO::MemoryStream mstm1((UIntOS)oriStm.GetLength());
	IO::MemoryStream mstm2((UIntOS)oriStm.GetLength());
	IO::MemoryStream mstm3((UIntOS)oriStm.GetLength());
	{
		Data::Compress::InflateStream istm(mstm1, true);
		if (istm.Write(srcStm.GetArray()) == srcStm.GetLength())
		{
			console.WriteLine(CSTR("InflateStream decompress completed"));
		}
	}
	if (Text::StrEqualsC(oriStm.GetBuff(), (UIntOS)oriStm.GetLength(), mstm1.GetBuff(), (UIntOS)mstm1.GetLength()))
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
		if (istm.Write(srcStm.GetArray()) == srcStm.GetLength() && istm.IsEnd())
		{
			console.WriteLine(CSTR("Inflater decompress completed"));
		}
	}
	if (Text::StrEqualsC(oriStm.GetBuff(), (UIntOS)oriStm.GetLength(), mstm2.GetBuff(), (UIntOS)mstm2.GetLength()))
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
		if (inf.Decompress(mstm3, mfd))
		{
			console.WriteLine(CSTR("Inflate decompress completed"));
		}
	}
	if (Text::StrEqualsC(oriStm.GetBuff(), (UIntOS)oriStm.GetLength(), mstm3.GetBuff(), (UIntOS)mstm3.GetLength()))
	{
		console.WriteLine(CSTR("Inflate decompress correctly"));
	}
	else
	{
		console.WriteLine(CSTR("Inflate decompress failed"));
	}

	buff2 = MemAlloc(UInt8, (UIntOS)oriStm.GetLength() * 2);
	{
		if (Data::Compress::Inflater::DecompressDirect(Data::ByteArray(buff2, (UIntOS)oriStm.GetLength() * 2), buffSize2, srcStm.GetArray(), true))
		{
			console.WriteLine(CSTR("Inflater decompressDirect completed"));
		}
	}
	if (Text::StrEqualsC(oriStm.GetBuff(), (UIntOS)oriStm.GetLength(), buff2, buffSize2))
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

