#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/Compress/LZWDecompressor.h"
#include "Data/Compress/LZWDecStream.h"
#include "IO/MemoryStream.h"
#include "IO/StreamDataStream.h"

Data::Compress::LZWDecompressor::LZWDecompressor()
{
}

Data::Compress::LZWDecompressor::~LZWDecompressor()
{
}

Bool Data::Compress::LZWDecompressor::Decompress(UInt8 *destBuff, UOSInt *destBuffSize, UInt8 *srcBuff, UOSInt srcBuffSize)
{
	IO::MemoryStream *mstm;
	NEW_CLASS(mstm, IO::MemoryStream(srcBuff, srcBuffSize, (const UTF8Char*)"Data.Compress.LZWDecompressor.Decompress"));
	Data::Compress::LZWDecStream lzw(mstm, false, 8, 15, 1);
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
	DEL_CLASS(mstm);
	return true;
}

Bool Data::Compress::LZWDecompressor::Decompress(IO::Stream *destStm, IO::IStreamData *srcData)
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
