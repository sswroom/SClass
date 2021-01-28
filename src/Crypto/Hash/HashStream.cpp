#include "Stdafx.h"
#include "MyMemory.h"
#include "Crypto/Hash/HashStream.h"

Crypto::Hash::HashStream::HashStream(IO::Stream *srcStm, Crypto::Hash::IHash *hash) : IO::Stream(srcStm->GetSourceNameObj())
{
	this->srcStm = srcStm;
	this->hash = hash;
}

Crypto::Hash::HashStream::~HashStream()
{
}

UOSInt Crypto::Hash::HashStream::Read(UInt8 *buff, UOSInt size)
{
	UOSInt retSize = this->srcStm->Read(buff, size);
	if (retSize > 0)
	{
		this->hash->Calc(buff, retSize);
	}
	return retSize;
}

UOSInt Crypto::Hash::HashStream::Write(const UInt8 *buff, UOSInt size)
{
	this->hash->Calc(buff, size);
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
