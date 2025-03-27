#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteBuffer.h"
#include "Data/Compress/Inflate.h"
#include "Data/Compress/Inflater.h"
#include "Data/Compress/Deflater.h"

Data::Compress::Inflate::Inflate(Bool hasHeader)
{
	this->hasHeader = hasHeader;
}

Data::Compress::Inflate::~Inflate()
{
}

Bool Data::Compress::Inflate::Decompress(Data::ByteArray destBuff, OutParam<UOSInt> outDestBuffSize, Data::ByteArrayR srcBuff)
{
	return Data::Compress::Inflater::DecompressDirect(destBuff,  outDestBuffSize, srcBuff, this->hasHeader);
}

Bool Data::Compress::Inflate::Decompress(NN<IO::Stream> destStm, NN<IO::StreamData> srcData)
{
	Data::Compress::Inflater inf(destStm, this->hasHeader);
	return inf.WriteFromData(srcData, 1048576) && inf.IsEnd();
}

UOSInt Data::Compress::Inflate::TestCompress(UnsafeArray<const UInt8> srcBuff, UOSInt srcBuffSize, Bool hasHeader)
{
	return 0;
}

UOSInt Data::Compress::Inflate::Compress(UnsafeArray<const UInt8> srcBuff, UOSInt srcBuffSize, UnsafeArray<UInt8> destBuff, Bool hasHeader, CompressionLevel level)
{
	UOSInt compSize;
	if (!Data::Compress::Deflater::CompressDirect(Data::ByteArray(destBuff, srcBuffSize), compSize, Data::ByteArrayR(srcBuff, srcBuffSize), (Data::Compress::Deflater::CompLevel)level, hasHeader))
		return 0;
	return compSize;
}
