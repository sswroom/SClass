#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/Compress/DeflateStream.h"

Data::Compress::DeflateStream::DeflateStream(NotNullPtr<IO::Stream> srcStm, UInt64 srcLeng, Crypto::Hash::IHash *hash, CompLevel level, Bool hasHeader) : Stream(srcStm->GetSourceNameObj())
{
}

Data::Compress::DeflateStream::~DeflateStream()
{
}

Bool Data::Compress::DeflateStream::IsDown() const
{
	return true;
}

UOSInt Data::Compress::DeflateStream::Read(const Data::ByteArray &buff)
{
	return 0;
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
}

Bool Data::Compress::DeflateStream::Recover()
{
	return false;
}

IO::StreamType Data::Compress::DeflateStream::GetStreamType() const
{
	return IO::StreamType::Deflate;
}
