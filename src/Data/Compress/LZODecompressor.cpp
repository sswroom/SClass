#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteBuffer.h"
#include "Data/Compress/LZODecompressor.h"
#include "Data/Compress/LZODecompressor_C.h"

Data::Compress::LZODecompressor::LZODecompressor()
{
}

Data::Compress::LZODecompressor::~LZODecompressor()
{
}

Bool Data::Compress::LZODecompressor::Decompress(Data::ByteArray destBuff, OutParam<UOSInt> outDestBuffSize, Data::ByteArrayR srcBuff)
{
	return LZODecompressor_Decompress(srcBuff.Arr().Ptr(), srcBuff.GetSize(), destBuff.Arr().Ptr(), outDestBuffSize.Ptr());
}

Bool Data::Compress::LZODecompressor::Decompress(NN<IO::Stream> destStm, NN<IO::StreamData> srcData)
{
	UInt64 srcSize = srcData->GetDataSize();
	Data::ByteBuffer srcBuff((UOSInt)srcSize);
	srcData->GetRealData(0, (UOSInt)srcSize, srcBuff);
	UOSInt destSize = LZODecompressor_CalcDecSize(srcBuff.Arr().Ptr(), (UOSInt)srcSize);
	if (destSize > 0)
	{
		UInt8 *destBuff = MemAlloc(UInt8, destSize);
		LZODecompressor_Decompress(srcBuff.Arr().Ptr(), (UOSInt)srcSize, destBuff, &destSize);
		destStm->Write(Data::ByteArrayR(destBuff, destSize));
		MemFree(destBuff);
		return true;
	}
	else
	{
		return false;
	}
}
