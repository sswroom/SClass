#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteBuffer.h"
#include "Data/Compress/Inflate.h"

Data::Compress::Inflate::Inflate(Bool hasHeader)
{
	this->hasHeader = hasHeader;
}

Data::Compress::Inflate::~Inflate()
{
}

Bool Data::Compress::Inflate::Decompress(Data::ByteArray destBuff, OutParam<UOSInt> outDestBuffSize, Data::ByteArrayR srcBuff)
{
	return false;
}

Bool Data::Compress::Inflate::Decompress(NN<IO::Stream> destStm, NN<IO::StreamData> srcData)
{
	return false;
}

UOSInt Data::Compress::Inflate::TestCompress(const UInt8 *srcBuff, UOSInt srcBuffSize, Bool hasHeader)
{
	return 0;
}

UOSInt Data::Compress::Inflate::Compress(const UInt8 *srcBuff, UOSInt srcBuffSize, UInt8 *destBuff, Bool hasHeader, CompressionLevel level)
{
	return 0;
}
