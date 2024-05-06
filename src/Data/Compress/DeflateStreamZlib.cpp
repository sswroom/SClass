#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/Compress/DeflateStream.h"
#include <zlib.h>

#define BUFFSIZE 16384

struct Data::Compress::DeflateStream::ClassData
{
	NN<IO::Stream> srcStm;
	UInt64 srcLeng;
	Crypto::Hash::IHash *hash;
	z_stream stm;
	UInt8 buff[BUFFSIZE];
};

Data::Compress::DeflateStream::DeflateStream(NN<IO::Stream> srcStm, UInt64 srcLeng, Crypto::Hash::IHash *hash, CompLevel level, Bool hasHeader) : Stream(srcStm->GetSourceNameObj())
{
	this->clsData = MemAlloc(ClassData, 1);
	this->clsData->srcStm = srcStm;
	this->clsData->srcLeng = srcLeng;
	this->clsData->hash = hash;
	MemClear(&this->clsData->stm, sizeof(z_stream));
	this->clsData->stm.next_in = this->clsData->buff;
	this->clsData->stm.avail_in = 0;
	this->clsData->stm.next_out = this->clsData->buff;
	this->clsData->stm.avail_out = 0;
	int ilevel;
	switch (level)
	{
	case CompLevel::MaxCompression:
		ilevel = Z_BEST_COMPRESSION;
		break;
	case CompLevel::MaxSpeed:
		ilevel = Z_BEST_SPEED;
		break;
	case CompLevel::Default:
	default:
		ilevel = Z_DEFAULT_COMPRESSION;
		break;
	}
	deflateInit2(&this->clsData->stm, ilevel, Z_DEFLATED, hasHeader?15:-15, 8, Z_DEFAULT_STRATEGY);
}

Data::Compress::DeflateStream::~DeflateStream()
{
	deflateEnd(&this->clsData->stm);
	MemFree(this->clsData);
}

Bool Data::Compress::DeflateStream::IsDown() const
{
	return this->clsData->srcStm->IsDown();
}

UOSInt Data::Compress::DeflateStream::Read(const Data::ByteArray &buff)
{
	UOSInt initSize = buff.GetSize();
	int ret;
	this->clsData->stm.next_out = buff.Ptr().Ptr();
	this->clsData->stm.avail_out = (unsigned int)buff.GetSize();
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
				readSize = this->clsData->srcStm->Read(Data::ByteArray(this->clsData->buff, readSize));
				if (readSize > 0 && this->clsData->hash)
				{
					this->clsData->hash->Calc(this->clsData->buff, readSize);
				}
				this->clsData->srcLeng -= readSize;
				this->clsData->stm.avail_in = (unsigned int)readSize;
			}

			if (this->clsData->srcLeng == 0 || readSize == 0)
			{
				ret = deflate(&this->clsData->stm, Z_FINISH);
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
		ret = deflate(&this->clsData->stm, Z_NO_FLUSH);
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
