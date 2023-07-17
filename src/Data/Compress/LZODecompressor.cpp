#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteBuffer.h"
#include "Data/Compress/LZODecompressor.h"

Data::Compress::LZODecompressor::LZODecompressor()
{
}

Data::Compress::LZODecompressor::~LZODecompressor()
{
}

Bool Data::Compress::LZODecompressor::Decompress(UInt8 *destBuff, UOSInt *destBuffSize, const UInt8 *srcBuff, UOSInt srcBuffSize)
{
	return LZODecompressor_Decompress(srcBuff, srcBuffSize, destBuff, destBuffSize);
}

Bool Data::Compress::LZODecompressor::Decompress(IO::Stream *destStm, IO::StreamData *srcData)
{
	UInt64 srcSize = srcData->GetDataSize();
	Data::ByteBuffer srcBuff((UOSInt)srcSize);
	srcData->GetRealData(0, (UOSInt)srcSize, srcBuff);
	UOSInt destSize = LZODecompressor_CalcDecSize(srcBuff.Ptr(), (UOSInt)srcSize);
	if (destSize > 0)
	{
		UInt8 *destBuff = MemAlloc(UInt8, destSize);
		LZODecompressor_Decompress(srcBuff.Ptr(), (UOSInt)srcSize, destBuff, &destSize);
		destStm->Write(destBuff, destSize);
		MemFree(destBuff);
		return true;
	}
	else
	{
		return false;
	}
}
