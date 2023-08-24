#include "Stdafx.h"
#include "Sync/SimpleThread.h"
#include "Sync/Thread.h"
#include "Sync/Interlocked.h"

UInt32 __stdcall Sync::Thread::InnerThread(void *userObj)
{
	NotNullPtr<Thread> me = NotNullPtr<Thread>::FromPtr((Thread*)userObj);
	Sync::ThreadUtil::SetName(me->name->ToCString());
	me->func(me);
	Sync::Interlocked::DecrementU32(me->running);
	return 0;
}

Sync::Thread::Thread(ThreadFunc func, void *userObj, Text::CStringNN name)
{
	this->running = 0;
	this->stopping = 0;
	this->waiting = 0;
	this->name = Text::String::New(name);
	this->func = func;
	this->userObj = userObj;
}

Sync::Thread::~Thread()
{
	this->Stop();
	this->name->Release();
}

Bool Sync::Thread::Start()
{
	if (this->running)
		return false;
	this->stopping = 0;
	this->running = 1;
	Sync::ThreadHandle *hand = Sync::ThreadUtil::CreateWithHandle(InnerThread, this);
	if (hand == 0)
	{
		this->running = 0;
		return false;
	}
	Sync::ThreadUtil::CloseHandle(hand);
	return true;
}

UOSInt Sync::Thread::StartMulti(UOSInt cnt)
{
	if (this->running)
		return 0;
	UOSInt ret = 0;
	this->stopping = 0;
	while (cnt-- > 0)
	{
		Sync::Interlocked::IncrementU32(this->running);
		Sync::ThreadHandle *hand = Sync::ThreadUtil::CreateWithHandle(InnerThread, this);
		if (hand == 0)
		{
			Sync::Interlocked::DecrementU32(this->running);
		}
		else
		{
			Sync::ThreadUtil::CloseHandle(hand);
			ret++;
		}
	}
	return ret;

}

void Sync::Thread::BeginStop()
{
	if (!this->running)
		return;
	this->stopping = 1;
	this->evt.Set();
}

void Sync::Thread::WaitForEnd()
{
	while (this->running)
	{
		Sync::SimpleThread::Sleep(1);
	}
}

void Sync::Thread::Stop()
{
	this->BeginStop();
	this->WaitForEnd();
}

void Sync::Thread::Wait(Data::Duration period)
{
	this->waiting = 1;
	this->evt.Wait(period);
	this->waiting = 0;
}

void Sync::Thread::Notify()
{
	this->evt.Set();
}

Bool Sync::Thread::IsRunning() const
{
	return this->running;
}

Bool Sync::Thread::IsStopping() const
{
	return this->stopping;
}

Bool Sync::Thread::IsWaiting() const
{
	return this->waiting;
}

void *Sync::Thread::GetUserObj() const
{
	return this->userObj;
}
