#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/Compress/InflateStream.h"
//#define _fseeki64 fseek
//#define _ftelli64 ftell
#include "miniz.h"

//#define VERBOSE
#define BUFFSIZE 1048576
#if defined(VERBOSE)
#include <stdio.h>
#endif

Data::Compress::InflateStream::InflateStream(IO::Stream *outStm, UOSInt headerSize, Bool zlibHeader) : IO::Stream(CSTR("InflateStream"))
{
	this->outStm = outStm;
	this->writeBuff = MemAlloc(UInt8, BUFFSIZE);
	this->headerSize = headerSize;
	mz_stream *mzstm = MemAlloc(mz_stream, 1);
	this->cmpInfo = mzstm;
	MemClear(mzstm, sizeof(mz_stream));
	mzstm->next_in = this->writeBuff;
	mzstm->avail_in = 0;
	mzstm->next_out = this->writeBuff;
	mzstm->avail_out = BUFFSIZE;
	if (zlibHeader)
		mz_inflateInit2(mzstm, MZ_DEFAULT_WINDOW_BITS);
	else
		mz_inflateInit2(mzstm, -MZ_DEFAULT_WINDOW_BITS);
}

Data::Compress::InflateStream::InflateStream(IO::Stream *outStm, Bool zlibHeader) : IO::Stream(CSTR("InflateStream"))
{
	this->outStm = outStm;
	this->writeBuff = MemAlloc(UInt8, BUFFSIZE);
	this->headerSize = 0;
	mz_stream *mzstm = MemAlloc(mz_stream, 1);
	this->cmpInfo = mzstm;
	MemClear(mzstm, sizeof(mz_stream));
	mzstm->next_in = this->writeBuff;
	mzstm->avail_in = 0;
	mzstm->next_out = this->writeBuff;
	mzstm->avail_out = BUFFSIZE;
	if (zlibHeader)
		mz_inflateInit2(mzstm, MZ_DEFAULT_WINDOW_BITS);
	else
		mz_inflateInit2(mzstm, -MZ_DEFAULT_WINDOW_BITS);
}

Data::Compress::InflateStream::~InflateStream()
{
	mz_stream *mzstm = (mz_stream *)this->cmpInfo;
	mz_inflateEnd(mzstm);
	MemFree(this->writeBuff);
	MemFree(mzstm);
}

Bool Data::Compress::InflateStream::IsDown() const
{
	return this->outStm->IsDown();
}

UOSInt Data::Compress::InflateStream::Read(const Data::ByteArray &buff)
{
	return 0;
}

UOSInt Data::Compress::InflateStream::Write(const UInt8 *buff, UOSInt size)
{
	mz_stream *mzstm = (mz_stream *)this->cmpInfo;
//	UInt32 lastSize;
//	int ret;
	if (size <= headerSize)
	{
		headerSize -= size;
		return size;
	}
	else if (headerSize > 0)
	{
		mzstm->next_in = buff + headerSize;
		mzstm->avail_in = (UInt32)(size - headerSize);
		headerSize = 0;
	}
	else
	{
		mzstm->next_in = buff;
		mzstm->avail_in = (UInt32)size;
	}
	while (true || mzstm->avail_in > 0)
	{
//		lastSize = mzstm->avail_in;
//		ret = mz_inflate(mzstm, MZ_SYNC_FLUSH);
#if defined(VERBOSE)
		int ret = mz_inflate(mzstm, MZ_SYNC_FLUSH);
#else
		mz_inflate(mzstm, MZ_SYNC_FLUSH);
#endif
		if (mzstm->avail_out == BUFFSIZE)
		{
#if defined(VERBOSE)
			printf("InflateStream: Error = %d\r\n", ret);
#endif
//			error = true;
			break;
		}
		else //	if (ret == MZ_STREAM_END || mzstm->avail_in != lastSize)
		{
			this->outStm->Write(writeBuff, BUFFSIZE - mzstm->avail_out);
			mzstm->avail_out = BUFFSIZE;
			mzstm->next_out = writeBuff;
		}
	}
	return size - mzstm->avail_in;
}

Int32 Data::Compress::InflateStream::Flush()
{
	return 0;
}

void Data::Compress::InflateStream::Close()
{
}

Bool Data::Compress::InflateStream::Recover()
{
	return this->outStm->Recover();
}

IO::StreamType Data::Compress::InflateStream::GetStreamType() const
{
	return IO::StreamType::Inflate;
}
