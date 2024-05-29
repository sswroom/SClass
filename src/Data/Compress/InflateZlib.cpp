#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteBuffer.h"
#include "Data/Compress/Inflate.h"
#include <zlib.h>

#define VERBOSE
#if defined(VERBOSE)
#include <stdio.h>
#endif

Data::Compress::Inflate::Inflate(Bool hasHeader)
{
	this->hasHeader = hasHeader;
}

Data::Compress::Inflate::~Inflate()
{
}

Bool Data::Compress::Inflate::Decompress(Data::ByteArray destBuff, OutParam<UOSInt> outDestBuffSize, Data::ByteArrayR srcBuff)
{
    int ret;
	z_stream strm;
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    strm.avail_in = 0;
    strm.next_in = Z_NULL;
    ret = inflateInit2(&strm, this->hasHeader?15:-15);
    if (ret != Z_OK)
	{
#if defined(VERBOSE)
		printf("Error in initializing Inflate: %d\r\n", ret);
#endif
        return false;
	}

	strm.avail_in = (uInt)srcBuff.GetSize();
	strm.next_in = (Bytef*)srcBuff.Arr().Ptr();
	strm.avail_out = (uInt)destBuff.GetSize();
	strm.next_out = (Bytef*)destBuff.Arr().Ptr();
	ret = inflate(&strm, Z_SYNC_FLUSH);
	inflateEnd(&strm);
	if (ret == Z_STREAM_END)
	{
		outDestBuffSize.Set(destBuff.GetSize() - strm.avail_out);
		return true;
	}
#if defined(VERBOSE)
	else
	{
		printf("Error during Inflate: %d\r\n", ret);
	}
#endif
	return false;
}

Bool Data::Compress::Inflate::Decompress(NN<IO::Stream> destStm, NN<IO::StreamData> srcData)
{
	UInt64 srcOfst = 0;
	UInt64 srcLen = srcData->GetDataSize();
	UOSInt srcSize;
	UInt8 *writeBuff;
	Bool error = false;
	z_stream strm;
	int ret;
	Data::ByteBuffer readBuff(1048576);
	writeBuff = MemAlloc(UInt8, 1048576);

	MemClear(&strm, sizeof(strm));
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    strm.avail_in = 0;
    strm.next_in = Z_NULL;
    ret = inflateInit2(&strm, this->hasHeader?15:-15);
    if (ret != Z_OK)
	{
#if defined(VERBOSE)
		printf("Error in initializing Inflate: %d\r\n", ret);
#endif
        return false;
	}
	while (!error)
	{
		srcSize = srcData->GetRealData(srcOfst, 1048576, readBuff);
		if (srcSize == 0)
			break;
		srcOfst += srcSize;

		strm.next_in = readBuff.Arr().Ptr();
		strm.avail_in = (unsigned int)srcSize;
		strm.next_out = writeBuff;
		strm.avail_out = 1048576;
		int ret = Z_STREAM_END;
		while (strm.avail_in > 0)
		{
			ret = inflate(&strm, Z_SYNC_FLUSH);
			if (strm.avail_out == 1048576)
			{
#if defined(VERBOSE)
				printf("Error during Inflate 1: %d, size=%d\r\n", ret, (UInt32)srcSize);
#endif
				error = true;
				break;
			}
			else
			{
				if ((UOSInt)destStm->Write(writeBuff, 1048576 - strm.avail_out) != (1048576 - strm.avail_out))
				{
					error = true;
					break;
				}
				if (ret == Z_STREAM_END)
				{
					break;
				}
				strm.avail_out = 1048576;
				strm.next_out = writeBuff;
			}
//			if (ret == MZ_STREAM_END)
//				break;
		}
		while (!error && srcOfst >= srcLen && ret != Z_STREAM_END)
		{
			ret = inflate(&strm, Z_FINISH);
			if (strm.avail_out == 1048576)
			{
#if defined(VERBOSE)
				printf("Error during Inflate 2: %d\r\n", ret);
#endif
				error = true;
				break;
			}
			else
			{
				if ((UOSInt)destStm->Write(writeBuff, 1048576 - strm.avail_out) != (1048576 - strm.avail_out))
				{
					error = true;
					break;
				}
				strm.avail_out = 1048576;
				strm.next_out = writeBuff;
			}
		}
	}
	inflateEnd(&strm);
	MemFree(writeBuff);
	return !error;
}

UOSInt Data::Compress::Inflate::TestCompress(const UInt8 *srcBuff, UOSInt srcBuffSize, Bool hasHeader)
{
	UInt8 *tmpBuff = MemAlloc(UInt8, srcBuffSize + 11);
	UOSInt outSize = Compress(srcBuff, srcBuffSize, tmpBuff, hasHeader, CompressionLevel::BestCompression);
	MemFree(tmpBuff);
	return outSize;
}

UOSInt Data::Compress::Inflate::Compress(const UInt8 *srcBuff, UOSInt srcBuffSize, UInt8 *destBuff, Bool hasHeader, CompressionLevel level)
{
	int status;
	z_stream stream;
	MemClear(&stream, sizeof(stream));
    stream.zalloc = Z_NULL;
    stream.zfree = Z_NULL;
    stream.opaque = Z_NULL;

	if (srcBuffSize > 0xFFFFFFFFU)
		return 0;

	stream.next_in = (Bytef*)srcBuff;
	stream.avail_in = (uInt)srcBuffSize;
	stream.next_out = destBuff;
	stream.avail_out = (uInt)srcBuffSize + 11;

	status = deflateInit2(&stream, (int)level, Z_DEFLATED, hasHeader?15:-15, 8, Z_DEFAULT_STRATEGY);
	if (status != Z_OK)
	{
		return 0;
	}

	status = deflate(&stream, Z_FINISH);
	deflateEnd(&stream);
	if (status == Z_OK || status == Z_STREAM_END)
	{
		return stream.total_out;
	}
	return 0;
}
