#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/Compress/InflateStream.h"

Data::Compress::InflateStream::InflateStream(NotNullPtr<IO::Stream> outStm, UOSInt headerSize, Bool zlibHeader) : IO::Stream(CSTR("InflateStream"))
{
	this->outStm = outStm;
}

Data::Compress::InflateStream::InflateStream(NotNullPtr<IO::Stream> outStm, Bool zlibHeader) : IO::Stream(CSTR("InflateStream"))
{
	this->outStm = outStm;
}

Data::Compress::InflateStream::~InflateStream()
{
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
	return 0;
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
