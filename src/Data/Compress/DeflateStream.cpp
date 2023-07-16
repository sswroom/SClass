#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/Compress/DeflateStream.h"
#include "miniz.h"

#define BUFFSIZE 16384

struct Data::Compress::DeflateStream::ClassData
{
	IO::Stream *srcStm;
	UInt64 srcLeng;
	Crypto::Hash::IHash *hash;
	mz_stream stm;
	UInt8 buff[BUFFSIZE];
};

Data::Compress::DeflateStream::DeflateStream(IO::Stream *srcStm, UInt64 srcLeng, Crypto::Hash::IHash *hash, CompLevel level, Bool hasHeader) : Stream(srcStm->GetSourceNameObj())
{
	this->clsData = MemAlloc(ClassData, 1);
	this->clsData->srcStm = srcStm;
	this->clsData->srcLeng = srcLeng;
	this->clsData->hash = hash;
	MemClear(&this->clsData->stm, sizeof(mz_stream));
	this->clsData->stm.next_in = this->clsData->buff;
	this->clsData->stm.avail_in = 0;
	this->clsData->stm.next_out = this->clsData->buff;
	this->clsData->stm.avail_out = 0;
	int ilevel;
	switch (level)
	{
	case CompLevel::MaxCompression:
		ilevel = MZ_BEST_COMPRESSION;
		break;
	case CompLevel::MaxSpeed:
		ilevel = MZ_BEST_SPEED;
		break;
	case CompLevel::Default:
	default:
		ilevel = MZ_DEFAULT_LEVEL;
		break;
	}
	mz_deflateInit2(&this->clsData->stm, ilevel, MZ_DEFLATED, hasHeader?MZ_DEFAULT_WINDOW_BITS:-MZ_DEFAULT_WINDOW_BITS, 1, MZ_DEFAULT_STRATEGY);
}

Data::Compress::DeflateStream::~DeflateStream()
{
	mz_deflateEnd(&this->clsData->stm);
	MemFree(this->clsData);
}

Bool Data::Compress::DeflateStream::IsDown() const
{
	return this->clsData->srcStm->IsDown();
}

UOSInt Data::Compress::DeflateStream::Read(const Data::ByteArray &buff)
{
	UOSInt initSize = size;
	int ret;
	this->clsData->stm.next_out = buff;
	this->clsData->stm.avail_out = (unsigned int)size;
	while (this->clsData->stm.avail_out == initSize)
	{
		if (this->clsData->stm.avail_in == 0 || this->clsData->srcLeng == 0)
		{
			UOSInt readSize = BUFFSIZE;
			if (this->clsData->srcLeng > 0)
			{
				this->clsData->stm.next_in = this->clsData->buff;
				if (this->clsData->srcLeng < BUFFSIZE)
				{
					readSize = (UOSInt)this->clsData->srcLeng;
				}
				readSize = this->clsData->srcStm->Read(this->clsData->buff, readSize);
				if (readSize > 0 && this->clsData->hash)
				{
					this->clsData->hash->Calc(this->clsData->buff, readSize);
				}
				this->clsData->srcLeng -= readSize;
				this->clsData->stm.avail_in = (unsigned int)readSize;
			}

			if (this->clsData->srcLeng == 0 || readSize == 0)
			{
				ret = mz_deflate(&this->clsData->stm, MZ_FINISH);
				if (this->clsData->stm.avail_out == initSize)
				{
					return 0;
				}
				else
				{
					return initSize - this->clsData->stm.avail_out;
				}
			}
		}
		ret = mz_deflate(&this->clsData->stm, MZ_NO_FLUSH);
		if (ret != 0)
		{
			break;
		}
	}
	return initSize - this->clsData->stm.avail_out;
}

UOSInt Data::Compress::DeflateStream::Write(const UInt8 *buff, UOSInt size)
{
	return 0;
}

Int32 Data::Compress::DeflateStream::Flush()
{
	return 0;
}

void Data::Compress::DeflateStream::Close()
{
	this->clsData->srcStm->Close();
}

Bool Data::Compress::DeflateStream::Recover()
{
	return this->clsData->srcStm->Recover();
}

IO::StreamType Data::Compress::DeflateStream::GetStreamType() const
{
	return IO::StreamType::Deflate;
}
