#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/Compress/LZODecompressor.h"

Data::Compress::LZODecompressor::LZODecompressor()
{
}

Data::Compress::LZODecompressor::~LZODecompressor()
{
}

Bool Data::Compress::LZODecompressor::Decompress(UInt8 *destBuff, UOSInt *destBuffSize, UInt8 *srcBuff, UOSInt srcBuffSize)
{
	return LZODecompressor_Decompress(srcBuff, srcBuffSize, destBuff, destBuffSize);
}

Bool Data::Compress::LZODecompressor::Decompress(IO::Stream *destStm, IO::IStreamData *srcData)
{
	UInt64 srcSize = srcData->GetDataSize();
	UInt8 *srcBuff = MemAlloc(UInt8, (UOSInt)srcSize);
	srcData->GetRealData(0, (UOSInt)srcSize, srcBuff);
	UOSInt destSize = LZODecompressor_CalcDecSize(srcBuff, (UOSInt)srcSize);
	if (destSize > 0)
	{
		UInt8 *destBuff = MemAlloc(UInt8, destSize);
		LZODecompressor_Decompress(srcBuff, (UOSInt)srcSize, destBuff, &destSize);
		destStm->Write(destBuff, destSize);
		MemFree(destBuff);
		MemFree(srcBuff);
		return true;
	}
	else
	{
		MemFree(srcBuff);
		return false;
	}
}
