#include "Stdafx.h"
#include "MyMemory.h"
#include "Sync/Event.h"
#include "Sync/Interlocked.h"
#include "Sync/Thread.h"
#if defined(_MSC_VER) || defined(__MINGW32__)
#include <windows.h>

Sync::Event::Event(const UTF8Char *name)
{
	this->hand = CreateEvent(0, FALSE, FALSE, 0);//name);
	this->isSet = false;
	this->isAuto = true;
}

Sync::Event::Event(Bool isAuto, const UTF8Char *name)
{
	this->hand = CreateEvent(0, !isAuto, FALSE, 0);//name);
	this->isSet = false;
	this->isAuto = isAuto;
}

Sync::Event::~Event()
{
	CloseHandle(this->hand);
}

void Sync::Event::Wait()
{
	DWORD ret = WAIT_TIMEOUT;
	while (ret == WAIT_TIMEOUT)
	{
		ret = WaitForSingleObject(this->hand, 1000);
	}
}

Bool Sync::Event::Wait(UOSInt timeout)
{
	return WaitForSingleObject(this->hand, (DWORD)timeout) == WAIT_TIMEOUT;
}

void Sync::Event::Set()
{
	SetEvent(this->hand);
}

void Sync::Event::Clear()
{
	ResetEvent(this->hand);
}

Bool Sync::Event::IsSet()
{
	UInt32 ret = WaitForSingleObject(this->hand, 0);
	return ret == WAIT_OBJECT_0;
}

void *Sync::Event::GetHandle()
{
	return this->hand;
}
#elif defined(AVR)
Sync::Event::Event(const UTF8Char *name)
{
	this->hand = 0;
	this->isSet = false;
	this->isAuto = true;
}

Sync::Event::Event(Bool isAuto, const UTF8Char *name)
{
	this->hand = 0;
	this->isSet = false;
	this->isAuto = isAuto;
}

Sync::Event::~Event()
{
}

void Sync::Event::Wait()
{
	if (this->isAuto)
	{
		if (this->isSet)
		{
			this->isSet = false;
		}
		else
		{
			while (!this->isSet);
			this->isSet = false;
		}
	}
	else
	{
		while (!this->isSet);
	}
}

Bool Sync::Event::Wait(UOSInt timeout)
{
	Bool isTO = false;
	if (this->isAuto)
	{
		if (this->isSet)
		{
			this->isSet = false;
		}
		else
		{
			while (!this->isSet)
			{
							
			}
			this->isSet = false;
			isTO = true;
		}
	}
	else
	{
		if (this->isSet)
		{
		}
		else
		{
		}
	}
	return isTO;
}

void Sync::Event::Set()
{
	if (this->isAuto)
	{
		this->isSet = true;
	}
	else
	{
		this->isSet = true;
	}
}

void Sync::Event::Clear()
{
	this->isSet = false;
}

Bool Sync::Event::IsSet()
{
	return this->isSet;
}

void *Sync::Event::GetHandle()
{
	return 0;
}
#else
#include <errno.h>
#include <pthread.h>
#include <sys/time.h>

typedef struct
{
	pthread_cond_t cond;
	pthread_mutex_t mutex;
	UInt32 useCnt;
} EventStatus;

Sync::Event::Event(const UTF8Char *name)
{
	EventStatus *status = MemAllocA(EventStatus, 1);
	this->hand = status;
	pthread_cond_init(&status->cond, 0);
	pthread_mutex_init(&status->mutex, 0);
	status->useCnt = 0;
	this->isSet = false;
	this->isAuto = true;
}

Sync::Event::Event(Bool isAuto, const UTF8Char *name)
{
	EventStatus *status = MemAllocA(EventStatus, 1);
	this->hand = status;
	pthread_cond_init(&status->cond, 0);
	pthread_mutex_init(&status->mutex, 0);
	status->useCnt = 0;
	this->isSet = false;
	this->isAuto = isAuto;
}

Sync::Event::~Event()
{
	EventStatus *status = (EventStatus*)this->hand;
	while (status->useCnt != 0)
	{
		Sync::Thread::Sleep(1);
	}
	pthread_mutex_lock(&status->mutex);
	pthread_cond_destroy(&status->cond);
	pthread_mutex_unlock(&status->mutex);
	pthread_mutex_destroy(&status->mutex);
	MemFreeA(status);
}

void Sync::Event::Wait()
{
	EventStatus *status = (EventStatus*)this->hand;
	if (this->isAuto)
	{
		pthread_mutex_lock(&status->mutex);
		if (this->isSet)
		{
			this->isSet = false;
		}
		else
		{
			pthread_cond_wait(&status->cond, &status->mutex);
			this->isSet = false;
		}
		pthread_mutex_unlock(&status->mutex);
	}
	else
	{
		pthread_mutex_lock(&status->mutex);
		if (this->isSet)
		{
		}
		else
		{
			pthread_cond_wait(&status->cond, &status->mutex);
		}
		pthread_mutex_unlock(&status->mutex);
	}
}

Bool Sync::Event::Wait(UOSInt timeout)
{
	EventStatus *status = (EventStatus*)this->hand;
	Bool isTO = false;
	struct timespec outtime;
	int ret;

	if (this->isAuto)
	{
		pthread_mutex_lock(&status->mutex);
		if (this->isSet)
		{
			this->isSet = false;
			pthread_mutex_unlock(&status->mutex);
		}
		else
		{
			clock_gettime(CLOCK_REALTIME, &outtime);
			outtime.tv_sec += (time_t)(timeout / 1000);
			outtime.tv_nsec += (long)((timeout % 1000) * 1000000);
			if (outtime.tv_nsec >= 1000000000)
			{
				outtime.tv_nsec -= 1000000000;
				outtime.tv_sec++;
			}
			ret = pthread_cond_timedwait(&status->cond, &status->mutex, &outtime);
			if (ret == ETIMEDOUT)
			{
				isTO = true;
			}
			else if (ret == 0)
			{
			}
			else
			{
			}
			pthread_mutex_unlock(&status->mutex);
		}
	}
	else
	{
		pthread_mutex_lock(&status->mutex);
		if (this->isSet)
		{
		}
		else
		{
			clock_gettime(CLOCK_REALTIME, &outtime);
			outtime.tv_sec += (time_t)(timeout / 1000);
			outtime.tv_nsec += (long)((timeout % 1000) * 1000000);
			if (outtime.tv_nsec >= 1000000000)
			{
				outtime.tv_nsec -= 1000000000;
				outtime.tv_sec++;
			}
			ret = pthread_cond_timedwait(&status->cond, &status->mutex, &outtime);
		}
		pthread_mutex_unlock(&status->mutex);
	}
	return isTO;
}

void Sync::Event::Set()
{
	EventStatus *status = (EventStatus*)this->hand;
	if (this->isAuto)
	{
		Interlocked_IncrementU32(&status->useCnt);
		pthread_mutex_lock(&status->mutex);
		this->isSet = true;
		pthread_cond_signal(&status->cond);
		pthread_mutex_unlock(&status->mutex);
		Interlocked_DecrementU32(&status->useCnt);
	}
	else
	{
		Interlocked_IncrementU32(&status->useCnt);
		pthread_mutex_lock(&status->mutex);
		this->isSet = true;
		pthread_cond_signal(&status->cond);
		pthread_cond_broadcast((pthread_cond_t*)this->hand);
		pthread_mutex_unlock(&status->mutex);
		Interlocked_DecrementU32(&status->useCnt);
	}
}

void Sync::Event::Clear()
{
	EventStatus *status = (EventStatus*)this->hand;
	pthread_mutex_lock(&status->mutex);
	this->isSet = false;
	pthread_mutex_unlock(&status->mutex);
}

Bool Sync::Event::IsSet()
{
	return this->isSet;
}

void *Sync::Event::GetHandle()
{
	return this->hand;
}
#endif
