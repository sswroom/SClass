#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteBuffer.h"
#include "Data/Compress/LZWDecompressor.h"
#include "Data/Compress/LZWDecStream.h"
#include "IO/MemoryReadingStream.h"
#include "IO/StreamDataStream.h"

Data::Compress::LZWDecompressor::LZWDecompressor()
{
}

Data::Compress::LZWDecompressor::~LZWDecompressor()
{
}

Bool Data::Compress::LZWDecompressor::Decompress(Data::ByteArray destBuff, OutParam<UOSInt> outDestBuffSize, Data::ByteArrayR srcBuff)
{
	IO::MemoryReadingStream mstm(srcBuff);
	Data::Compress::LZWDecStream lzw(&mstm, false, 8, 15, 1);
	UOSInt writeSize = 0;
	UOSInt thisSize;
	while ((thisSize = lzw.Read(destBuff)) != 0)
	{
		writeSize += thisSize;
		destBuff += thisSize;
	}
	outDestBuffSize.Set(writeSize);
	return true;
}

Bool Data::Compress::LZWDecompressor::Decompress(NN<IO::Stream> destStm, NN<IO::StreamData> srcData)
{
	IO::StreamDataStream *srcStm;
	Data::ByteBuffer tmpBuff(65536);
	UOSInt thisSize;
	NEW_CLASS(srcStm, IO::StreamDataStream(srcData));
	while ((thisSize = srcStm->Read(tmpBuff)) != 0)
	{
		destStm->Write(tmpBuff.Ptr(), thisSize);
	}
	DEL_CLASS(srcStm);
	return true;
}
