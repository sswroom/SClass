#include "Stdafx.h"
#include "MyMemory.h"
#include "Sync/Event.h"
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
//#define TIMEDWAIT_BUG
#if defined(TIMEDWAIT_BUG)
#include <unistd.h>
#endif
#include <errno.h>
#include <pthread.h>
#include <sys/time.h>

typedef struct
{
	pthread_cond_t cond;
	pthread_mutex_t mutex;
} EventStatus;

Sync::Event::Event(const UTF8Char *name)
{
	EventStatus *status = MemAlloc(EventStatus, 1);
	this->hand = status;
	pthread_cond_init(&status->cond, 0);
	pthread_mutex_init(&status->mutex, 0);
	this->isSet = false;
	this->isAuto = true;
}

Sync::Event::Event(Bool isAuto, const UTF8Char *name)
{
	EventStatus *status = MemAlloc(EventStatus, 1);
	this->hand = status;
	pthread_cond_init(&status->cond, 0);
	pthread_mutex_init(&status->mutex, 0);
	this->isSet = false;
	this->isAuto = isAuto;
}

Sync::Event::~Event()
{
	EventStatus *status = (EventStatus*)this->hand;
	pthread_mutex_lock(&status->mutex);
	pthread_cond_destroy(&status->cond);
	pthread_mutex_unlock(&status->mutex);
	pthread_mutex_destroy(&status->mutex);
	MemFree(status);
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
#if defined(TIMEDWAIT_BUG)
	struct timeval t;
	struct timezone tz;
	UInt64 stTime;
	UInt64 currTime;

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
			pthread_mutex_unlock(&status->mutex);
			gettimeofday(&t, &tz);
			stTime = t.tv_sec * (UInt64)1000000 + t.tv_usec;
			while (!(volatile Int32)this->isSet)
			{
				gettimeofday(&t, &tz);
				currTime = t.tv_sec * (UInt64)1000000 + t.tv_usec;
				if (currTime >= stTime)
				{
					currTime = (currTime - stTime) / 1000;
				}
				else
				{
					currTime = (86400000000 - (currTime - stTime)) / 1000;
				}
				if (currTime >= timeout)
				{
					isTO = true;
					break;
				}
				usleep(100);
			}
			this->isSet = false;
		}
	}
	else
	{
		if (this->isSet)
		{
		}
		else
		{
			gettimeofday(&t, &tz);
			stTime = t.tv_sec * (Int64)1000000 + t.tv_usec;
			while (!(volatile Int32)this->isSet)
			{
				gettimeofday(&t, &tz);
				currTime = t.tv_sec * (Int64)1000000 + t.tv_usec;
				if (currTime >= stTime)
				{
					currTime = (currTime - stTime) / 1000;
				}
				else
				{
					currTime = (86400000000 - (currTime - stTime)) / 1000;
				}
				if (currTime >= timeout)
				{
					isTO = true;
					break;
				}
				usleep(100);
			}
		}
	}
#else
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
#endif
	return isTO;
}

void Sync::Event::Set()
{
	EventStatus *status = (EventStatus*)this->hand;
	if (this->isAuto)
	{
		pthread_mutex_lock(&status->mutex);
		this->isSet = true;
		pthread_cond_signal(&status->cond);
		pthread_mutex_unlock(&status->mutex);
	}
	else
	{
		pthread_mutex_lock(&status->mutex);
		this->isSet = true;
		pthread_cond_signal(&status->cond);
		pthread_cond_broadcast((pthread_cond_t*)this->hand);
		pthread_mutex_unlock(&status->mutex);
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
