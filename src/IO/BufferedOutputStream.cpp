#include "Stdafx.h"
#include "MemTool.h"
#include "MyMemory.h"
#include "IO/BufferedOutputStream.h"

IO::BufferedOutputStream::BufferedOutputStream(NN<IO::Stream> outStm, UOSInt buffSize) : IO::Stream(outStm->GetSourceNameObj())
{
	this->outStm = outStm;
	this->cacheBuffSize = buffSize;
	this->cacheBuff = MemAlloc(UInt8, buffSize);
	this->cacheSize = 0;
	this->totalWrite = 0;
}

IO::BufferedOutputStream::~BufferedOutputStream()
{
	if (this->cacheSize > 0)
	{
		this->outStm->Write(Data::ByteArrayR(this->cacheBuff, this->cacheSize));
	}
	MemFree(this->cacheBuff);
	this->cacheBuff = 0;
}

Bool IO::BufferedOutputStream::IsDown() const
{
	return this->outStm->IsDown();
}

UOSInt IO::BufferedOutputStream::Read(const Data::ByteArray &buff)
{
	return this->outStm->Read(buff);
}

UOSInt IO::BufferedOutputStream::Write(Data::ByteArrayR buff)
{
	this->totalWrite += buff.GetSize();
	if (this->cacheSize + buff.GetSize() < this->cacheBuffSize)
	{
		MemCopyNO(&this->cacheBuff[this->cacheSize], buff.Arr().Ptr(), buff.GetSize());
		this->cacheSize += buff.GetSize();
		return buff.GetSize();
	}
	else if (buff.GetSize() >= this->cacheSize)
	{
		if (this->cacheSize > 0)
		{
			this->outStm->WriteCont(this->cacheBuff, this->cacheSize);
			this->cacheSize = 0;
		}
		return this->outStm->WriteCont(buff.Arr(), buff.GetSize());
	}
	
	UOSInt ret = this->cacheBuffSize - this->cacheSize;
	if (ret > 0)
	{
		MemCopyNO(&this->cacheBuff[this->cacheSize], buff.Arr().Ptr(), ret);
		buff += ret;
	}

	UOSInt writeSize = this->outStm->WriteCont(this->cacheBuff, this->cacheBuffSize);
	if (writeSize == 0)
	{
		return ret;
	}
	else if (writeSize < this->cacheBuffSize)
	{
		MemCopyO(this->cacheBuff, &this->cacheBuff[writeSize], this->cacheBuffSize - writeSize);
		this->cacheSize = this->cacheBuffSize - writeSize;
		if (this->cacheBuffSize - this->cacheSize < buff.GetSize())
		{
			MemCopyNO(&this->cacheBuff[this->cacheSize], buff.Arr().Ptr(), this->cacheBuffSize - this->cacheSize);
			ret += this->cacheBuffSize - this->cacheSize;
			this->cacheSize = this->cacheBuffSize;
			return ret;
		}
		else
		{
			MemCopyNO(&this->cacheBuff[this->cacheSize], buff.Arr().Ptr(), buff.GetSize());
			this->cacheSize += buff.GetSize();
			ret += buff.GetSize();
			return ret;
		}
	}
	this->cacheSize = 0;
	if (buff.GetSize() >= this->cacheBuffSize)
	{
		writeSize = this->outStm->WriteCont(buff.Arr(), buff.GetSize());
		ret += writeSize;
		if (writeSize == buff.GetSize())
		{
			return ret;
		}
		buff += writeSize;
		if (this->cacheBuffSize - this->cacheSize < buff.GetSize())
		{
			MemCopyNO(&this->cacheBuff[this->cacheSize], buff.Arr().Ptr(), this->cacheBuffSize - this->cacheSize);
			ret += this->cacheBuffSize - this->cacheSize;
			this->cacheSize = this->cacheBuffSize;
			return ret;
		}
		else
		{
			MemCopyNO(&this->cacheBuff[this->cacheSize], buff.Arr().Ptr(), buff.GetSize());
			this->cacheSize += buff.GetSize();
			ret += buff.GetSize();
			return ret;
		}
	}
	else
	{
		MemCopyNO(this->cacheBuff, buff.Arr().Ptr(), buff.GetSize());
		this->cacheSize += buff.GetSize();
		ret += buff.GetSize();
		return ret;
	}
}

Int32 IO::BufferedOutputStream::Flush()
{
	if (this->cacheSize > 0)
	{
		this->outStm->Write(Data::ByteArrayR(this->cacheBuff, this->cacheSize));
		this->cacheSize = 0;
	}
	return this->outStm->Flush();
}

void IO::BufferedOutputStream::Close()
{
	if (this->cacheSize > 0)
	{
		this->outStm->Write(Data::ByteArrayR(this->cacheBuff, this->cacheSize));
		this->cacheSize = 0;
	}
	this->outStm->Close();
}

Bool IO::BufferedOutputStream::Recover()
{
	return this->outStm->Recover();
}

IO::StreamType IO::BufferedOutputStream::GetStreamType() const
{
	return IO::StreamType::BufferedOutput;
}

UInt64 IO::BufferedOutputStream::GetPosition() const
{
	return this->totalWrite;
}
