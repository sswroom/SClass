#include "Stdafx.h"
#include "MyMemory.h"
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

Bool Data::Compress::Inflate::Decompress(UInt8 *destBuff, UOSInt *destBuffSize, UInt8 *srcBuff, UOSInt srcBuffSize)
{
	mz_ulong buffSize = (mz_ulong)*destBuffSize;
	Int32 ret = mz_uncompress(destBuff, &buffSize, srcBuff, (mz_ulong)srcBuffSize);
	if (ret == MZ_OK)
	{
		*destBuffSize = buffSize;
		return true;
	}
	return false;
}

Bool Data::Compress::Inflate::Decompress(IO::Stream *destStm, IO::IStreamData *srcData)
{
	UInt64 srcOfst = 0;
	UOSInt srcSize;
	UInt8 *readBuff;
	UInt8 *writeBuff;
	Bool error = false;
	mz_stream stm;
	readBuff = MemAlloc(UInt8, 1048576);
	writeBuff = MemAlloc(UInt8, 1048576);

/*	UInt32 destLeng;
	readBuff[0] = 'a';
	mz_compress(writeBuff, (mz_ulong*)&destLeng, &readBuff[0], 1);

	readBuff[0] = 0x4b;
	readBuff[1] = 0x04;
	readBuff[2] = 0x00;
	readBuff[3] = 0x43;
	readBuff[4] = 0xbe;
	readBuff[5] = 0xb7;
	readBuff[6] = 0xe8;
	readBuff[7] = 0x01;
	readBuff[8] = 0x00;
	readBuff[9] = 0x00;
	readBuff[10] = 0x00;
	srcSize = 11;
	while (srcSize-- > 0)
	{
		mz_uncompress(writeBuff, (mz_ulong*)&destLeng, &readBuff[0], srcSize);
	}*/
	MemClear(&stm, sizeof(stm));
	stm.next_in = readBuff;
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

		stm.next_in = readBuff;
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
		while (!error && ret != MZ_STREAM_END)
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
	MemFree(readBuff);
	MemFree(writeBuff);
	return !error;
}

UOSInt Data::Compress::Inflate::TestCompress(const UInt8 *srcBuff, UOSInt srcBuffSize, Bool hasHeader)
{
	UInt8 *tmpBuff = MemAlloc(UInt8, srcBuffSize + 11);
	UOSInt outSize = Compress(srcBuff, srcBuffSize, tmpBuff, hasHeader);
	MemFree(tmpBuff);
	return outSize;
}

UOSInt Data::Compress::Inflate::Compress(const UInt8 *srcBuff, UOSInt srcBuffSize, UInt8 *destBuff, Bool hasHeader)
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

	status = mz_deflateInit2(&stream, MZ_BEST_COMPRESSION, MZ_DEFLATED, hasHeader?MZ_DEFAULT_WINDOW_BITS:-MZ_DEFAULT_WINDOW_BITS, 1, 0);
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
