#include "Stdafx.h"
#include "Sync/SimpleThread.h"
#include "Sync/Thread.h"
#include "Sync/ThreadUtil.h"

UInt32 __stdcall Sync::Thread::InnerThread(void *userObj)
{
	NotNullPtr<Thread> me = NotNullPtr<Thread>::FromPtr((Thread*)userObj);
	me->func(me);
	me->running = false;
	return 0;
}

Sync::Thread::Thread(ThreadFunc func, void *userObj)
{
	this->running = false;
	this->stopping = false;
	this->func = func;
	this->userObj = userObj;
}

Sync::Thread::~Thread()
{
	this->Stop();
}

void Sync::Thread::Start()
{
	if (this->running)
		return;
	this->stopping = false;
	this->running = true;
	Sync::ThreadUtil::Create(InnerThread, this);
}

void Sync::Thread::BeginStop()
{
	if (!this->running)
		return;
	this->stopping = true;
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
	this->evt.Wait(period);
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

void *Sync::Thread::GetUserObj() const
{
	return this->userObj;
}
