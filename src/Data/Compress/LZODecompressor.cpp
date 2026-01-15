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

Bool Data::Compress::LZODecompressor::Decompress(Data::ByteArray destBuff, OutParam<UIntOS> outDestBuffSize, Data::ByteArrayR srcBuff)
{
	return LZODecompressor_Decompress(srcBuff.Arr().Ptr(), srcBuff.GetSize(), destBuff.Arr().Ptr(), outDestBuffSize.Ptr());
}

Bool Data::Compress::LZODecompressor::Decompress(NN<IO::Stream> destStm, NN<IO::StreamData> srcData)
{
	UInt64 srcSize = srcData->GetDataSize();
	Data::ByteBuffer srcBuff((UIntOS)srcSize);
	srcData->GetRealData(0, (UIntOS)srcSize, srcBuff);
	UIntOS destSize = LZODecompressor_CalcDecSize(srcBuff.Arr().Ptr(), (UIntOS)srcSize);
	if (destSize > 0)
	{
		UInt8 *destBuff = MemAlloc(UInt8, destSize);
		LZODecompressor_Decompress(srcBuff.Arr().Ptr(), (UIntOS)srcSize, destBuff, &destSize);
		destStm->Write(Data::ByteArrayR(destBuff, destSize));
		MemFree(destBuff);
		return true;
	}
	else
	{
		return false;
	}
}
