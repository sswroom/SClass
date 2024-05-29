#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteBuffer.h"
#include "Data/Compress/Inflate.h"
//#define _fseeki64 fseek
//#define _ftelli64 ftell
#include "miniz.h"

Data::Compress::Inflate::Inflate(Bool hasHeader)
{
	this->hasHeader = hasHeader;
}

Data::Compress::Inflate::~Inflate()
{
}

Bool Data::Compress::Inflate::Decompress(Data::ByteArray destBuff, OutParam<UOSInt> outDestBuffSize, Data::ByteArrayR srcBuff)
{
	mz_ulong buffSize = (mz_ulong)destBuff.GetSize();
	Int32 ret = mz_uncompress(destBuff.Arr().Ptr(), &buffSize, srcBuff.Arr().Ptr(), (mz_ulong)srcBuff.GetSize());
	if (ret == MZ_OK)
	{
		outDestBuffSize.Set(buffSize);
		return true;
	}
	return false;
}

Bool Data::Compress::Inflate::Decompress(NN<IO::Stream> destStm, NN<IO::StreamData> srcData)
{
	UInt64 srcOfst = 0;
	UInt64 srcLen = srcData->GetDataSize();
	UOSInt srcSize;
	UInt8 *writeBuff;
	Bool error = false;
	mz_stream stm;
	Data::ByteBuffer readBuff(1048576);
	writeBuff = MemAlloc(UInt8, 1048576);

	MemClear(&stm, sizeof(stm));
	stm.next_in = readBuff.Arr().Ptr();
	stm.avail_in = 0;
	stm.next_out = writeBuff;
	stm.avail_out = 1048576;
	mz_inflateInit2(&stm, this->hasHeader?MZ_DEFAULT_WINDOW_BITS:-MZ_DEFAULT_WINDOW_BITS);
	while (!error)
	{
		srcSize = srcData->GetRealData(srcOfst, 1048576, readBuff);
		if (srcSize == 0)
			break;
		srcOfst += srcSize;

		stm.next_in = readBuff.Arr().Ptr();
		stm.avail_in = (unsigned int)srcSize;
		int ret = MZ_STREAM_END;
		while (stm.avail_in > 0)
		{
			ret = mz_inflate(&stm, MZ_SYNC_FLUSH);
			if (stm.avail_out == 1048576)
			{
				error = true;
				break;
			}
			else
			{
				if ((UOSInt)destStm->Write(writeBuff, 1048576 - stm.avail_out) != (1048576 - stm.avail_out))
				{
					error = true;
					break;
				}
				if (ret == MZ_STREAM_END)
				{
					break;
				}
				stm.avail_out = 1048576;
				stm.next_out = writeBuff;
			}
//			if (ret == MZ_STREAM_END)
//				break;
		}
		while (!error && srcOfst >= srcLen && ret != MZ_STREAM_END)
		{
			ret = mz_inflate(&stm, MZ_FINISH);
			if (stm.avail_out == 1048576)
			{
				error = true;
				break;
			}
			else
			{
				if ((UOSInt)destStm->Write(writeBuff, 1048576 - stm.avail_out) != (1048576 - stm.avail_out))
				{
					error = true;
					break;
				}
				stm.avail_out = 1048576;
				stm.next_out = writeBuff;
			}
		}
	}
	mz_inflateEnd(&stm);
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
	mz_stream stream;
	MemClear(&stream, sizeof(stream));

	if (srcBuffSize > 0xFFFFFFFFU)
		return 0;

	stream.next_in = srcBuff;
	stream.avail_in = (mz_uint32)srcBuffSize;
	stream.next_out = destBuff;
	stream.avail_out = (mz_uint32)srcBuffSize + 11;

	status = mz_deflateInit2(&stream, (int)level, MZ_DEFLATED, hasHeader?MZ_DEFAULT_WINDOW_BITS:-MZ_DEFAULT_WINDOW_BITS, 1, 0);
	if (status != MZ_OK)
		return 0;

	status = mz_deflate(&stream, MZ_FINISH);
	mz_deflateEnd(&stream);
	if (status == MZ_OK || status == MZ_STREAM_END)
	{
		return stream.total_out;
	}
	return 0;
}
