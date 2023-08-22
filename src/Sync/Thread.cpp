#include "Stdafx.h"
#include "Sync/SimpleThread.h"
#include "Sync/Thread.h"

UInt32 __stdcall Sync::Thread::InnerThread(void *userObj)
{
	NotNullPtr<Thread> me = NotNullPtr<Thread>::FromPtr((Thread*)userObj);
	Sync::ThreadUtil::SetName(me->name->ToCString());
	me->func(me);
	me->running = false;
	return 0;
}

Sync::Thread::Thread(ThreadFunc func, void *userObj, Text::CStringNN name)
{
	this->running = false;
	this->stopping = false;
	this->name = Text::String::New(name);
	this->hand = 0;
	this->func = func;
	this->userObj = userObj;
}

Sync::Thread::~Thread()
{
	this->Stop();
	if (this->hand)
	{
		Sync::ThreadUtil::CloseHandle(this->hand);
		this->hand = 0;
	}
	this->name->Release();
}

Bool Sync::Thread::Start()
{
	if (this->running)
		return false;
	if (this->hand)
	{
		Sync::ThreadUtil::CloseHandle(this->hand);
	}
	this->stopping = false;
	this->running = true;
	this->hand = Sync::ThreadUtil::CreateWithHandle(InnerThread, this);
	if (this->hand == 0)
	{
		this->running = false;
		return false;
	}
	return true;
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
