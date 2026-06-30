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

Bool Data::Compress::LZWDecompressor::Decompress(Data::ByteArray destBuff, OutParam<UIntOS> outDestBuffSize, Data::ByteArrayR srcBuff)
{
	IO::MemoryReadingStream mstm(srcBuff);
	Data::Compress::LZWDecStream lzw(mstm, false, 8, 15, 1);
	UIntOS writeSize = 0;
	UIntOS thisSize;
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
	NN<IO::StreamDataStream> srcStm;
	Data::ByteBuffer tmpBuff(65536);
	UIntOS thisSize;
	NEW_CLASSNN(srcStm, IO::StreamDataStream(srcData));
	while ((thisSize = srcStm->Read(tmpBuff)) != 0)
	{
		destStm->Write(tmpBuff.WithSize(thisSize));
	}
	srcStm.Delete();
	return true;
}
