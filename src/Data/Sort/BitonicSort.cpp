#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/Sort/BitonicSort.h"
#include "Sync/MutexUsage.h"
#include "Sync/ThreadUtil.h"
#include "Text/MyString.h"

void Data::Sort::BitonicSort::DoMergeInt32(NotNullPtr<ThreadStat> stat, Int32 *arr, OSInt n, Bool dir, OSInt m)
{
	Int32 v1;
	Int32 v2;
	OSInt i = 0;
	while (i < n - m)
	{
		v1 = arr[i];
		v2 = arr[i + m];
		if (dir == (v1 > v2))
		{
			arr[i] = v2;
			arr[i + m] = v1;
		}
		i++;
	}
	if (m > 1)
	{
		TaskInfo task1;
		task1.arr = arr;
		task1.arrLen = m;
		task1.dir = dir;
		task1.m = m >> 1;
		task1.arrType = AT_INT32;
		task1.notifyEvt = stat->evt;
		stat->me->tasks.Add(&task1);
		if (stat->nextThread && ((ThreadStat*)stat->nextThread)->state == 2)
		{
			((ThreadStat*)stat->nextThread)->evt->Set();
		}

		n -= m;
		if (n > 1)
		{
			arr = arr + m;
			while (m >= n)
			{
				m = m >> 1;
			}

			TaskInfo task2;
			task2.arr = arr;
			task2.arrLen = n;
			task2.dir = dir;
			task2.m = m;
			task2.arrType = AT_INT32;
			task2.notifyEvt = stat->evt;
			stat->me->tasks.Add(&task2);
			while (task1.arr != 0 || task2.arr != 0)
			{
				if (!stat->me->DoTask(stat))
				{
					if (task1.arr == 0 && task2.arr == 0)
						break;
					stat->state = 2;
					stat->evt->Wait(100);
					stat->state = 1;
				}
			}
		}
		else
		{
			while (task1.arr != 0)
			{
				if (!stat->me->DoTask(stat))
				{
					if (task1.arr == 0)
						break;
					stat->state = 2;
					stat->evt->Wait(100);
					stat->state = 1;
				}
			}
		}
	}
	else if (n > 2)
	{
		v1 = arr[1];
		v2 = arr[2];
		if (dir == (v1 > v2))
		{
			arr[1] = v2;
			arr[2] = v1;
		}
	}
}

void Data::Sort::BitonicSort::DoMergeUInt32(NotNullPtr<ThreadStat> stat, UInt32 *arr, OSInt n, Bool dir, OSInt m)
{
	UInt32 v1;
	UInt32 v2;
	OSInt i = 0;
	while (i < n - m)
	{
		v1 = arr[i];
		v2 = arr[i + m];
		if (dir == (v1 > v2))
		{
			arr[i] = v2;
			arr[i + m] = v1;
		}
		i++;
	}
	if (m > 1)
	{
		TaskInfo task1;
		task1.arr = arr;
		task1.arrLen = m;
		task1.dir = dir;
		task1.m = m >> 1;
		task1.arrType = AT_UINT32;
		task1.notifyEvt = stat->evt;
		stat->me->tasks.Add(&task1);
		if (stat->nextThread && ((ThreadStat*)stat->nextThread)->state == 2)
		{
			((ThreadStat*)stat->nextThread)->evt->Set();
		}

		n -= m;
		if (n > 1)
		{
			arr = arr + m;
			while (m >= n)
			{
				m = m >> 1;
			}

			TaskInfo task2;
			task2.arr = arr;
			task2.arrLen = n;
			task2.dir = dir;
			task2.m = m;
			task2.arrType = AT_UINT32;
			task2.notifyEvt = stat->evt;
			stat->me->tasks.Add(&task2);
			while (task1.arr != 0 || task2.arr != 0)
			{
				if (!stat->me->DoTask(stat))
				{
					if (task1.arr == 0 && task2.arr == 0)
						break;
					stat->state = 2;
					stat->evt->Wait(100);
					stat->state = 1;
				}
			}
		}
		else
		{
			while (task1.arr != 0)
			{
				if (!stat->me->DoTask(stat))
				{
					if (task1.arr == 0)
						break;
					stat->state = 2;
					stat->evt->Wait(100);
					stat->state = 1;
				}
			}
		}
	}
	else if (n > 2)
	{
		v1 = arr[1];
		v2 = arr[2];
		if (dir == (v1 > v2))
		{
			arr[1] = v2;
			arr[2] = v1;
		}
	}
}

Bool Data::Sort::BitonicSort::DoTask(NotNullPtr<ThreadStat> stat)
{
	TaskInfo *task = 0;
	task = this->tasks.RemoveLast();
	if (task)
	{
		if (task->arrType == AT_INT32)
		{
			DoMergeInt32(stat, (Int32*)task->arr, task->arrLen, task->dir, task->m);
		}
		else if (task->arrType == AT_UINT32)
		{
			DoMergeUInt32(stat, (UInt32*)task->arr, task->arrLen, task->dir, task->m);
		}
		else if (task->arrType == AT_STRC)
		{
		}
		Sync::Event *evt = task->notifyEvt;
		task->arr = 0;
		evt->Set();
		return true;
	}
	else
	{
		return false;
	}
}

void Data::Sort::BitonicSort::SortInnerInt32(Int32 *arr, OSInt n, Bool dir, OSInt pw2)
{
	OSInt m = n / 2;
	if (m > 1)
	{
		this->SortInnerInt32(arr, m, !dir, pw2 >> 1);
		this->SortInnerInt32(arr + m, n - m, dir, pw2 >> 1);
	}
	else if (n > 2)
	{
		Int32 v1 = arr[1];
		Int32 v2 = arr[2];
		if (dir == (v1 > v2))
		{
			arr[1] = v2;
			arr[2] = v1;
		}
	}
	TaskInfo task;
	task.arr = arr;
	task.arrLen = n;
	task.dir = dir;
	task.m = pw2;
	task.arrType = AT_INT32;
	task.notifyEvt = this->mainThread.evt;
	this->tasks.Add(&task);
	ThreadStat *nextThread = (ThreadStat*)this->mainThread.nextThread;
	while (nextThread)
	{
		if (nextThread->state == 2)
		{
			nextThread->evt->Set();
		}
		nextThread = (ThreadStat*)nextThread->nextThread;
	}
	while (task.arr != 0)
	{
		this->mainThread.evt->Wait(100);
	}
}

void Data::Sort::BitonicSort::SortInnerUInt32(UInt32 *arr, OSInt n, Bool dir, OSInt pw2)
{
	OSInt m = n / 2;
	if (m > 1)
	{
		this->SortInnerUInt32(arr, m, !dir, pw2 >> 1);
		this->SortInnerUInt32(arr + m, n - m, dir, pw2 >> 1);
	}
	else if (n > 2)
	{
		UInt32 v1 = arr[1];
		UInt32 v2 = arr[2];
		if (dir == (v1 > v2))
		{
			arr[1] = v2;
			arr[2] = v1;
		}
	}
	TaskInfo task;
	task.arr = arr;
	task.arrLen = n;
	task.dir = dir;
	task.m = pw2;
	task.arrType = AT_UINT32;
	task.notifyEvt = this->mainThread.evt;
	this->tasks.Add(&task);
	ThreadStat *nextThread = (ThreadStat*)this->mainThread.nextThread;
	while (nextThread)
	{
		if (nextThread->state == 2)
		{
			nextThread->evt->Set();
		}
		nextThread = (ThreadStat*)nextThread->nextThread;
	}
	while (task.arr != 0)
	{
		this->mainThread.evt->Wait(100);
	}
}

UInt32 __stdcall Data::Sort::BitonicSort::ProcessThread(AnyType userObj)
{
	NotNullPtr<ThreadStat> stat = userObj.GetNN<ThreadStat>();
	stat->state = 1;
	stat->me->mainThread.evt->Set();
	while (!stat->toStop)
	{
		if (!stat->me->DoTask(stat))
		{
			stat->state = 2;
			stat->evt->Wait(1000);
			stat->state = 1;
		}
	}
	stat->state = 0;
	stat->me->mainThread.evt->Set();
	return 0;
}

Data::Sort::BitonicSort::BitonicSort()
{
	this->threadCnt = Sync::ThreadUtil::GetThreadCnt();
	this->threads = MemAlloc(ThreadStat, this->threadCnt);
	mainThread.me = this;
	mainThread.toStop = false;
	mainThread.state = 1;
	NEW_CLASS(mainThread.evt, Sync::Event(true));
	UOSInt i = this->threadCnt;
	ThreadStat *lastThread = 0;
	while (i-- > 0)
	{
		this->threads[i].me = this;
		this->threads[i].toStop = false;
		this->threads[i].state = 0;
		NEW_CLASS(this->threads[i].evt, Sync::Event(true));
		this->threads[i].nextThread = lastThread;
		lastThread = &this->threads[i];
		Sync::ThreadUtil::Create(ProcessThread, &this->threads[i]);
	}
	mainThread.nextThread = lastThread;
	while (true)
	{
		Bool found = false;
		i = this->threadCnt;
		while (i-- > 0)
		{
			if (this->threads[i].state == 0)
			{
				found = true;
				break;
			}
		}
		if (!found)
			break;
		mainThread.evt->Wait(100);
	}
}

Data::Sort::BitonicSort::~BitonicSort()
{
	UOSInt i;
	i = this->threadCnt;
	while (i-- > 0)
	{
		this->threads[i].toStop = true;
		this->threads[i].evt->Set();
	}
	while (true)
	{
		Bool found = false;
		i = this->threadCnt;
		while (i-- > 0)
		{
			if (this->threads[i].state != 0)
			{
				found =true;
				break;
			}
		}
		if (!found)
			break;
		mainThread.evt->Wait(100);
	}

	i = this->threadCnt;
	while (i-- > 0)
	{
		DEL_CLASS(this->threads[i].evt);
	}
	MemFree(this->threads);
	DEL_CLASS(mainThread.evt);
}

void Data::Sort::BitonicSort::SortInt32(Int32 *arr, OSInt firstIndex, OSInt lastIndex)
{
	OSInt cnt = lastIndex - firstIndex + 1;
	if (cnt > 1)
	{
		OSInt m = 1;
		while (m < cnt)
			m = m << 1;
		this->SortInnerInt32(arr + firstIndex, cnt, true, m >> 1);
	}
}

void Data::Sort::BitonicSort::SortUInt32(UInt32 *arr, OSInt firstIndex, OSInt lastIndex)
{
	OSInt cnt = lastIndex - firstIndex + 1;
	if (cnt > 1)
	{
		OSInt m = 1;
		while (m < cnt)
			m = m << 1;
		this->SortInnerUInt32(arr + firstIndex, cnt, true, m >> 1);
	}
}

void Data::Sort::BitonicSort::SortStr(UTF8Char **arr, OSInt firstIndex, OSInt lastIndex)
{
}
