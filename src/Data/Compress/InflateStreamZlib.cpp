#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/Compress/InflateStream.h"
#include <zlib.h>

//#define VERBOSE
#define BUFFSIZE 1048576
#if defined(VERBOSE)
#include <stdio.h>
#endif

Data::Compress::InflateStream::InflateStream(NotNullPtr<IO::Stream> outStm, UOSInt headerSize, Bool zlibHeader) : IO::Stream(CSTR("InflateStream"))
{
	this->outStm = outStm;
	this->writeBuff = MemAlloc(UInt8, BUFFSIZE);
	this->headerSize = headerSize;
	z_stream *mzstm = MemAlloc(z_stream, 1);
	this->cmpInfo = mzstm;
	MemClear(mzstm, sizeof(z_stream));
	mzstm->next_in = this->writeBuff;
	mzstm->avail_in = 0;
	mzstm->next_out = this->writeBuff;
	mzstm->avail_out = BUFFSIZE;
	if (zlibHeader)
		inflateInit2(mzstm, 15);
	else
		inflateInit2(mzstm, -15);
}

Data::Compress::InflateStream::InflateStream(NotNullPtr<IO::Stream> outStm, Bool zlibHeader) : IO::Stream(CSTR("InflateStream"))
{
	this->outStm = outStm;
	this->writeBuff = MemAlloc(UInt8, BUFFSIZE);
	this->headerSize = 0;
	z_stream *mzstm = MemAlloc(z_stream, 1);
	this->cmpInfo = mzstm;
	MemClear(mzstm, sizeof(z_stream));
	mzstm->next_in = this->writeBuff;
	mzstm->avail_in = 0;
	mzstm->next_out = this->writeBuff;
	mzstm->avail_out = BUFFSIZE;
	if (zlibHeader)
		inflateInit2(mzstm, 15);
	else
		inflateInit2(mzstm, -15);
}

Data::Compress::InflateStream::~InflateStream()
{
	z_stream *mzstm = (z_stream *)this->cmpInfo;
	inflateEnd(mzstm);
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
	z_stream *mzstm = (z_stream *)this->cmpInfo;
//	UInt32 lastSize;
//	int ret;
	if (size <= headerSize)
	{
		headerSize -= size;
		return size;
	}
	else if (headerSize > 0)
	{
		mzstm->next_in = (Bytef*)buff + headerSize;
		mzstm->avail_in = (UInt32)(size - headerSize);
		headerSize = 0;
	}
	else
	{
		mzstm->next_in = (Bytef*)buff;
		mzstm->avail_in = (UInt32)size;
	}
	while (true || mzstm->avail_in > 0)
	{
//		lastSize = mzstm->avail_in;
//		ret = mz_inflate(mzstm, MZ_SYNC_FLUSH);
#if defined(VERBOSE)
		int ret = inflate(mzstm, Z_SYNC_FLUSH);
#else
		inflate(mzstm, Z_SYNC_FLUSH);
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
