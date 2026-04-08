#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "Crypto/Hash/CRC32R.h"
#include "Data/Compress/LZODecompressor.h"
#include "IO/ConsoleWriter.h"
#include "IO/FileStream.h"
#include "IO/MemoryStream.h"
#include "IO/StmData/FileData.h"
#include "Manage/HiResClock.h"
#include "Text/StringBuilderUTF8.h"

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
	NN<IO::StmData::FileData> fd;
	UInt8 *srcBuff = 0;
	UInt8 *destBuff = 0;
	UInt64 fileSize;
	IO::ConsoleWriter console;
	NEW_CLASSNN(fd, IO::StmData::FileData(CSTR("LZOBlock.dat"), false));
	fileSize = fd->GetDataSize();
	if (fileSize > 0)
	{
		srcBuff = MemAlloc(UInt8, fileSize);
		fd->GetRealData(0, fileSize, Data::ByteArray(srcBuff, fileSize));
	}
	if (srcBuff)
	{
		Data::Compress::LZODecompressor *decomp;
		UInt64 decSize = 0;
		Manage::HiResClock clk;
		destBuff = MemAlloc(UInt8, fileSize * 3);
		NEW_CLASS(decomp, Data::Compress::LZODecompressor());
		clk.Start();
		if (decomp->Decompress(Data::ByteArray(destBuff, fileSize * 3), decSize, Data::ByteArrayR(srcBuff, fileSize)))
		{
			UIntOS i = 300;
			UInt32 crcVal;
			clk.Start();
			while (i-- > 0)
			{
				decomp->Decompress(Data::ByteArray(destBuff, fileSize * 3), decSize, Data::ByteArrayR(srcBuff, fileSize));
			}
			Double t = clk.GetTimeDiff();
			Text::StringBuilderUTF8 sb;
			Crypto::Hash::CRC32R crc;
			crc.Calc(destBuff, decSize);
			crc.GetValue((UInt8*)&crcVal);
			sb.Append(CSTR("Decompress: size = "));
			sb.AppendU64(decSize);
			sb.Append(CSTR(", crc = "));
			sb.AppendHex32(crcVal);
			if (crcVal == 0x5108BE1D)
			{
				sb.Append(CSTR(" (ok)"));
			}
			else
			{
				sb.Append(CSTR(" (err)"));
			}
			sb.Append(CSTR(", t = "));
			sb.AppendDouble(t);
			console.WriteLine(sb.ToCString());

			IO::MemoryStream mstm;
			UnsafeArray<UInt8> tmpBuff;
			UInt64 tmpBuffSize;
			if (decomp->Decompress(mstm, fd))
			{
				tmpBuff = mstm.GetBuff(tmpBuffSize);
				crc.Clear();
				crc.Calc(tmpBuff, tmpBuffSize);
				crc.GetValue((UInt8*)&crcVal);
				sb.ClearStr();
				sb.Append(CSTR("Decompress: streamdata decompress success, size = "));
				sb.AppendU64(tmpBuffSize);
				sb.Append(CSTR(", crc = "));
				sb.AppendHex32(crcVal);
				console.WriteLine(sb.ToCString());
			}
			else
			{
				console.WriteLine(CSTR("Decompress: streamdata decompress failed"));
			}
		}
		else
		{
			console.WriteLine(CSTR("Error in decompressing"));
		}
		DEL_CLASS(decomp);
		MemFree(destBuff);
		MemFree(srcBuff);
	}
	fd.Delete();
	return 0;
}
