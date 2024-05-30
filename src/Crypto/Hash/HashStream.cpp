#include "Stdafx.h"
#include "MyMemory.h"
#include "Crypto/Hash/HashStream.h"

Crypto::Hash::HashStream::HashStream(NN<IO::Stream> srcStm, NN<Crypto::Hash::IHash> hash) : IO::Stream(srcStm->GetSourceNameObj())
{
	this->srcStm = srcStm;
	this->hash = hash;
}

Crypto::Hash::HashStream::~HashStream()
{
}

Bool Crypto::Hash::HashStream::IsDown() const
{
	return this->srcStm->IsDown();
}

UOSInt Crypto::Hash::HashStream::Read(const Data::ByteArray &buff)
{
	UOSInt retSize = this->srcStm->Read(buff);
	if (retSize > 0)
	{
		this->hash->Calc(buff.Arr().Ptr(), retSize);
	}
	return retSize;
}

UOSInt Crypto::Hash::HashStream::Write(UnsafeArray<const UInt8> buff, UOSInt size)
{
	this->hash->Calc(buff.Ptr(), size);
	return this->srcStm->Write(buff, size);
}

Int32 Crypto::Hash::HashStream::Flush()
{
	return this->srcStm->Flush();
}

void Crypto::Hash::HashStream::Close()
{
	this->srcStm->Close();
}

Bool Crypto::Hash::HashStream::Recover()
{
	return this->srcStm->Recover();
}

IO::StreamType Crypto::Hash::HashStream::GetStreamType() const
{
	return IO::StreamType::Hash;
}