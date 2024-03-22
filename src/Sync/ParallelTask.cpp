#include "Stdafx.h"
#include "MyMemory.h"
#include "Sync/ParallelTask.h"
#include "Sync/ThreadUtil.h"

UInt32 __stdcall Sync::ParallelTask::WorkerThread(AnyType userObj)
{
	NotNullPtr<ThreadStatus> stat = userObj.GetNN<ThreadStatus>();
	UTF8Char sbuff[16];
	UTF8Char *sptr;
	sptr = Text::StrUOSInt(Text::StrConcatC(sbuff, UTF8STRC("ParallelTask")), stat->index);
	Sync::ThreadUtil::SetName(CSTRP(sbuff, sptr));
	{
		Sync::Event evt;
		stat->evt = &evt;
		stat->running = true;
		stat->me->mainEvt.Set();
		while (!stat->toStop)
		{
			if (stat->currTaskFunc)
			{
				stat->currTaskFunc(stat->currTaskObj);
				stat->currTaskObj = 0;
				stat->currTaskFunc = 0;
				stat->me->mainEvt.Set();
			}
			stat->evt->Wait(1000);
		}
	}
	stat->running = false;
	stat->me->mainEvt.Set();
	return 0;
}

Sync::ParallelTask::ParallelTask(UOSInt threadCnt, Bool taskQueue)
{
	if (threadCnt == 0)
	{
		this->threadCnt = Sync::ThreadUtil::GetThreadCnt();
	}
	else
	{
		this->threadCnt = threadCnt;
	}
	if (this->threadCnt > 1)
	{
		this->stats = MemAlloc(Sync::ParallelTask::ThreadStatus, this->threadCnt);
		UOSInt i = this->threadCnt;
		while (i-- > 0)
		{
			this->stats[i].me = this;
			this->stats[i].index = i;
			this->stats[i].running = false;
			this->stats[i].toStop = false;
			this->stats[i].currTaskFunc = 0;
			this->stats[i].currTaskObj = 0;
			Sync::ThreadUtil::Create(WorkerThread, &this->stats[i]);
		}

		Bool inited = false;
		while (!inited)
		{
			inited = true;
			i = this->threadCnt;
			while (i-- > 0)
			{
				if (!this->stats[i].running)
				{
					inited = false;
					break;
				}
			}
			if (inited)
				break;
			this->mainEvt.Wait(100);
		}
	}
}

Sync::ParallelTask::~ParallelTask()
{
	UOSInt i;
	if (this->threadCnt > 1)
	{
#if 1 || defined(WIN32)
		Bool exited;
		i = this->threadCnt;
		while (i-- > 0)
		{
			this->stats[i].toStop = true;
			this->stats[i].evt->Set();
		}
		while (true)
		{
			exited = true;
			i = this->threadCnt;
			while (i-- > 0)
			{
				if (this->stats[i].running)
				{
					exited = false;
					break;
				}
			}
			if (exited)
				break;

			this->mainEvt.Wait(100);
		}
#else
		i = this->threadCnt;
		while (i-- > 0)
		{
			this->stats[i].toStop = true;
			this->stats[i].evt->Set();
			while (this->stats[i].running)
			{
				this->mainEvt.Wait(100);
			}
		}
#endif
		MemFree(this->stats);
	}
}

UOSInt Sync::ParallelTask::GetThreadCnt()
{
	return this->threadCnt;
}

void Sync::ParallelTask::AddTask(TaskFunc func, AnyType taskObj)
{
	if (this->threadCnt > 1)
	{
		UOSInt i;
		Bool added = false;
		while (!added)
		{
			i = this->threadCnt;
			while (i-- > 0)
			{
				if (this->stats[i].currTaskFunc == 0)
				{
					this->stats[i].currTaskObj = taskObj;
					this->stats[i].currTaskFunc = func;
					this->stats[i].evt->Set();
					added = true;
					break;
				}
			}
			if (!added)
			{
				this->mainEvt.Wait(1000);
			}
		}
	}
	else
	{
		func(taskObj);
	}
}

void Sync::ParallelTask::WaitForIdle()
{
	if (this->threadCnt > 1)
	{
		UOSInt i;
		Bool idle = false;
		while (!idle)
		{
			idle = true;
			i = this->threadCnt;
			while (i-- > 0)
			{
				if (this->stats[i].currTaskFunc != 0)
				{
					idle = false;
					break;
				}
			}
			if (!idle)
			{
				this->mainEvt.Wait(1000);
			}
		}
	}
}
