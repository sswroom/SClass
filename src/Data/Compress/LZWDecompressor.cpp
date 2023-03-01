#include "Stdafx.h"
#include "MyMemory.h"
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

Bool Data::Compress::LZWDecompressor::Decompress(UInt8 *destBuff, UOSInt *destBuffSize, const UInt8 *srcBuff, UOSInt srcBuffSize)
{
	IO::MemoryReadingStream mstm(srcBuff, srcBuffSize);
	Data::Compress::LZWDecStream lzw(&mstm, false, 8, 15, 1);
	UOSInt writeSize = 0;
	UOSInt thisSize;
	while ((thisSize = lzw.Read(destBuff, 4096)) != 0)
	{
		writeSize += thisSize;
		destBuff += thisSize;
	}
	if (destBuffSize)
	{
		*destBuffSize = writeSize;
	}
	return true;
}

Bool Data::Compress::LZWDecompressor::Decompress(IO::Stream *destStm, IO::StreamData *srcData)
{
	IO::StreamDataStream *srcStm;
	UInt8 *tmpBuff = MemAlloc(UInt8, 65536);
	UOSInt thisSize;
	NEW_CLASS(srcStm, IO::StreamDataStream(srcData));
	while ((thisSize = srcStm->Read(tmpBuff, 65536)) != 0)
	{
		destStm->Write(tmpBuff, thisSize);
	}
	DEL_CLASS(srcStm);
	MemFree(tmpBuff);
	return true;
}
