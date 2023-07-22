#include "Stdafx.h"
#include "MyMemory.h"
#include "Sync/Mutex.h"
#include "Sync/ThreadUtil.h"

//#define MUTEX_DEBUG
#if defined(_MSC_VER) || defined(__MINGW32__)
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
void Sync::Mutex_Create(Sync::MutexData *data)
{
	data->debName = 0;
#ifdef MUTEX_DEBUG
	data->locked = 0;
#endif
	data->hand = malloc(sizeof(CRITICAL_SECTION));
	InitializeCriticalSection((LPCRITICAL_SECTION)data->hand);
}

void Sync::Mutex_Destroy(Sync::MutexData *data)
{
	DeleteCriticalSection((LPCRITICAL_SECTION)data->hand);
	free(data->hand);
	data->debName = 0;
}

void Sync::Mutex_Lock(Sync::MutexData *data)
{
#ifdef MUTEX_DEBUG
	if (data->debName)
		printf("Mutex %s Locking...", data->debName);
	if (data->locked)
		if (data->lockId == Sync::ThreadUtil::GetThreadId())
		{
			printf("Mutex: Lock again\r\n");
			//Unlock();
			data->locked++;
			return;
		}
#endif
	EnterCriticalSection((LPCRITICAL_SECTION)data->hand);
#ifdef MUTEX_DEBUG
	data->locked = 1;
	data->lockId = Sync::ThreadUtil::GetThreadId();
	if (data->debName)
		printf("Locked\r\n");
#endif
}

void Sync::Mutex_Unlock(Sync::MutexData *data)
{
#ifdef MUTEX_DEBUG
	if (data->debName)
		printf("Mutex %s Unlocking...", data->debName);
	if (!data->locked)
	{
		printf("Mutex: Mutex error!\r\n");
	}
	if (--data->locked == 0)
	{
		data->lockId = 0;
		if (data->debName)
		{
			LeaveCriticalSection((LPCRITICAL_SECTION)data->hand);
			printf("Mutex: Unlocked\r\n");
		}
		else
		{
			LeaveCriticalSection((LPCRITICAL_SECTION)data->hand);
		}
	}
#else
	LeaveCriticalSection((LPCRITICAL_SECTION)data->hand);
#endif
}

Bool Sync::Mutex_TryLock(Sync::MutexData *data)
{
#ifdef MUTEX_DEBUG
	if (data->debName)
		printf("Mutex %s Locking...", data->debName);
	if (data->locked)
		if (data->lockId == Sync::ThreadUtil::GetThreadId())
		{
			printf("Mutex: Lock Again\r\n");
			//Unlock();
			data->locked++;
			return true;
		}
#endif
	if (TryEnterCriticalSection((LPCRITICAL_SECTION)data->hand) == 0)
		return false;
#ifdef MUTEX_DEBUG
	data->locked = 1;
	data->lockId = Sync::ThreadUtil::GetThreadId();
	if (data->debName)
		printf("Locked\r\n");
#endif
	return true;
}

#elif defined(__linux__)
#include <sys/types.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/syscall.h>
#define gettid() (pid_t)syscall(SYS_gettid)
//#include <wchar.h>

void Sync::Mutex_Create(Sync::MutexData *data)
{
	data->debName = 0;
	data->hand = malloc(sizeof(pthread_mutex_t));
	data->lockId = 0;
	data->locked = 0;

/*	pthread_mutexattr_t attr;
	pthread_mutexattr_init(&attr);
	pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE_NP);
	pthread_mutex_init(&mut->mut, &attr);
	pthread_mutexattr_destroy(&attr);*/
	pthread_mutex_init((pthread_mutex_t*)data->hand, 0);
}

void Sync::Mutex_Destroy(Sync::MutexData *data)
{
	pthread_mutex_destroy((pthread_mutex_t*)data->hand);
	free(data->hand);
}

void Sync::Mutex_Lock(Sync::MutexData *data)
{
	pid_t currTID = gettid();
	if (data->lockId == (UInt32)currTID)
	{
		data->locked++;
//		printf("Mutex Relocked cnt = %d, tid = %d\r\n", mut->lockCnt, currTID);
	}
	else
	{
		pthread_mutex_lock((pthread_mutex_t*)data->hand);
		data->lockId = (UInt32)currTID;
		data->locked = 1;
	}
}

void Sync::Mutex_Unlock(Sync::MutexData *data)
{
	if (data->locked-- <= 1)
	{
		data->lockId = 0;
		data->locked = 0;
		pthread_mutex_unlock((pthread_mutex_t*)data->hand);
	}
	else
	{
//		printf("Mutex Relock released\r\n");
	}
}

Bool Sync::Mutex_TryLock(Sync::MutexData *data)
{
	pid_t currTID = gettid();
	if (data->lockId == (UInt32)currTID)
	{
		data->locked++;
//		printf("Mutex Relocked cnt = %d, tid = %d\r\n", mut->lockCnt, currTID);
		return true;
	}
	else
	{
		if (pthread_mutex_lock((pthread_mutex_t*)data->hand) != 0)
			return false;
		data->lockId = (UInt32)currTID;
		data->locked = 1;
		return true;
	}
}
#elif defined(__FreeBSD__) || defined(__APPLE__)
#include <pthread.h>
#include <stdlib.h>
#include <wchar.h>

typedef struct
{
	pthread_mutex_t mut;
	pthread_t lockTID;
	OSInt lockCnt;
} MutexInfo;

void Sync::Mutex_Create(Sync::MutexData *data)
{
	data->debName = 0;
	data->hand = malloc(sizeof(MutexInfo));
	MutexInfo *mut = (MutexInfo*)data->hand;
	mut->lockTID = 0;
	mut->lockCnt = 0;

/*	pthread_mutexattr_t attr;
	pthread_mutexattr_init(&attr);
	pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE_NP);
	pthread_mutex_init(&mut->mut, &attr);
	pthread_mutexattr_destroy(&attr);*/
	pthread_mutex_init(&mut->mut, 0);
}

void Sync::Mutex_Destroy(Sync::MutexData *data)
{
	MutexInfo *mut = (MutexInfo*)data->hand;
	pthread_mutex_destroy(&mut->mut);
	free(mut);
}

void Sync::Mutex_Lock(Sync::MutexData *data)
{
	MutexInfo *mut = (MutexInfo*)data->hand;
	pthread_t currTID = pthread_self();
	if (mut->lockTID == currTID)
	{
		mut->lockCnt++;
		printf("Mutex Relocked cnt = %d, tid = %d\r\n", (Int32)mut->lockCnt, (Int32)(OSInt)currTID);
	}
	else
	{
		pthread_mutex_lock(&mut->mut);
		mut->lockTID = currTID;
		mut->lockCnt = 1;
	}
}

void Sync::Mutex_Unlock(Sync::MutexData *data)
{
	MutexInfo *mut = (MutexInfo*)data->hand;
	if (mut->lockCnt-- <= 1)
	{
		mut->lockTID = 0;
		mut->lockCnt = 0;
		pthread_mutex_unlock(&mut->mut);
	}
	else
	{
		printf("Mutex Relock released\r\n");
	}
}

Bool Sync::Mutex_TryLock(Sync::MutexData *data)
{
	MutexInfo *mut = (MutexInfo*)data->hand;
	pthread_t currTID = pthread_self();
	if (mut->lockTID == currTID)
	{
		mut->lockCnt++;
		printf("Mutex Relocked cnt = %d, tid = %d\r\n", (Int32)mut->lockCnt, (Int32)(OSInt)currTID);
		return true;
	}
	else
	{
		if (pthread_mutex_lock(&mut->mut) != 0)
			return false;
		mut->lockTID = currTID;
		mut->lockCnt = 1;
		return true;
	}
}
#elif defined(__solaris__)
#include <synch.h>
#include <thread.h>

void Sync::Mutex_Create(Sync::MutexData *data)
{
	data->debName = 0;
	data->hand = (void*)malloc(sizeof(struct mutex_t));
	mutex_init(data->hand);
}

void Sync::Mutex_Destroy(Sync::MutexData *data)
{
	mutex_destroy(data->hand);
	free(data->hand);
}

void Sync::Mutex_Lock(Sync::MutexData *data)
{
	mutex_lock((struct mutex*)data->hand);
}

void Sync::Mutex_Unlock(Sync::MutexData *data)
{
	mutex_unlock((struct mutex*)data->hand);
}

void Sync::Mutex_TryLock(Sync::MutexData *data)
{
	return mutex_trylock((struct mutex*)data->hand) == 0;
}
#else
void Sync::Mutex_Create(Sync::MutexData *data)
{
	data->debName = 0;
	data->hand = 0;
}

void Sync::Mutex_Destroy(Sync::MutexData *data)
{
}

void Sync::Mutex_Lock(Sync::MutexData *data)
{
}

void Sync::Mutex_Unlock(Sync::MutexData *data)
{
}

Bool Sync::Mutex_TryLock(Sync::MutexData *data)
{
	return true;
}
#endif

void Sync::Mutex_SetDebName(Sync::MutexData *data, const UTF8Char *name)
{
	data->debName = name;
}

Sync::Mutex::Mutex()
{
	Mutex_Create(&this->data);
}

Sync::Mutex::~Mutex()
{
	Mutex_Destroy(&this->data);
}

void Sync::Mutex::Lock() const
{
	Mutex_Lock((MutexData*)&this->data);
}

void Sync::Mutex::Unlock() const
{
	Mutex_Unlock((MutexData*)&this->data);
}

Bool Sync::Mutex::TryLock() const
{
	return Mutex_TryLock((MutexData*)&this->data);
}

void Sync::Mutex::SetDebName(const UTF8Char *name)
{
	Mutex_SetDebName(&this->data, name);
}

