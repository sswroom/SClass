#include "Stdafx.h"
#include "MemTool.h"
#include "MyMemory.h"
#include "IO/MTStream.h"
#include "Sync/MutexUsage.h"

void __stdcall IO::MTStream::OutputThread(NN<Sync::Thread> thread)
{
	NN<IO::MTStream> me = thread->GetUserObj().GetNN<IO::MTStream>();
	UnsafeArray<UInt8> tmpBuff;
	UOSInt size;
	while (!thread->IsStopping())
	{
		if (me->cacheSize > 0)
		{
			{
				Sync::MutexUsage mutUsage(me->cacheMut);
				size = me->cacheSize;
				tmpBuff = me->cacheBuff1;
				me->cacheBuff1 = me->cacheBuff2;
				me->cacheBuff2 = tmpBuff;
				me->cacheSize = 0;
			}
			me->outStm->Write(Data::ByteArrayR(tmpBuff, size));
		}
		thread->Wait(1000);
	}
	if (me->cacheSize > 0)
	{
		{
			Sync::MutexUsage mutUsage(me->cacheMut);
			size = me->cacheSize;
			tmpBuff = me->cacheBuff1;
			me->cacheBuff1 = me->cacheBuff2;
			me->cacheBuff2 = tmpBuff;
			me->cacheSize = 0;
		}
		me->outStm->Write(Data::ByteArrayR(tmpBuff, size));
	}
}

IO::MTStream::MTStream(NN<IO::Stream> outStm, UOSInt buffSize) : IO::Stream(outStm->GetSourceNameObj()), thread(OutputThread, this, CSTR("MTStream"))
{
	this->outStm = outStm;
	this->cacheBuffSize = buffSize;
	this->cacheBuff1 = MemAllocArr(UInt8, buffSize);
	this->cacheBuff2 = MemAllocArr(UInt8, buffSize);
	this->cacheSize = 0;
	this->thread.Start();
}

IO::MTStream::~MTStream()
{
	this->thread.Stop();
	MemFreeArr(this->cacheBuff1);
	MemFreeArr(this->cacheBuff2);
}

Bool IO::MTStream::IsDown() const
{
	return this->outStm->IsDown();
}

UOSInt IO::MTStream::Read(const Data::ByteArray &buff)
{
	return this->outStm->Read(buff);
}

UOSInt IO::MTStream::Write(Data::ByteArrayR buff)
{
	if (!this->thread.IsRunning())
		return 0;
	Sync::MutexUsage mutUsage(this->cacheMut);
	if (this->cacheSize + buff.GetSize() < this->cacheBuffSize)
	{
		MemCopyNO(&this->cacheBuff1[this->cacheSize], buff.Arr().Ptr(), buff.GetSize());
		this->cacheSize += buff.GetSize();
		this->thread.Notify();
		return buff.GetSize();
	}
	UOSInt ret = 0;
	while (buff.GetSize() > 0)
	{
		if (this->cacheSize >= this->cacheBuffSize)
		{
			mutUsage.EndUse();
			Sync::ThreadUtil::SleepDur(10);
			mutUsage.BeginUse();
		}
		else
		{
			UOSInt copySize = this->cacheBuffSize - this->cacheSize;
			if (copySize > buff.GetSize())
			{
				copySize = buff.GetSize();
			}
			MemCopyNO(&this->cacheBuff1[this->cacheSize], buff.Arr().Ptr(), copySize);
			this->cacheSize += copySize;
			this->thread.Notify();
			buff += copySize;
			ret += copySize;
		}
	}
	return ret;
}

Int32 IO::MTStream::Flush()
{
	if (this->thread.IsRunning() && this->cacheSize > 0)
	{
		this->thread.Notify();
		while (this->cacheSize > 0)
		{
			Sync::ThreadUtil::SleepDur(10);
		}
	}
	return this->outStm->Flush();
}

void IO::MTStream::Close()
{
	if (this->thread.IsRunning() && this->cacheSize > 0)
	{
		this->thread.Notify();
		while (this->cacheSize > 0)
		{
			Sync::ThreadUtil::SleepDur(10);
		}
	}
	this->outStm->Close();
}

Bool IO::MTStream::Recover()
{
	return this->outStm->Recover();
}

IO::StreamType IO::MTStream::GetStreamType() const
{
	return IO::StreamType::BufferedOutput;
}
