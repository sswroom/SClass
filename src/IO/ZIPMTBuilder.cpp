#include "Stdafx.h"
#include "IO/ZIPMTBuilder.h"
#include "Sync/ThreadUtil.h"

UInt32 __stdcall IO::ZIPMTBuilder::ThreadProc(void *userObj)
{
	ThreadStat *stat = (ThreadStat*)userObj;
	FileTask *task;
	{
		Sync::Event evt;
		stat->evt = &evt;
		stat->status = ThreadState::Idle;
		while (!stat->me->toStop)
		{
			task = (FileTask*)stat->me->taskList.Get();
			if (task)
			{
				stat->status = ThreadState::Processing;
				stat->me->zip.AddFile(task->fileName->ToCString(), task->fileBuff, task->fileSize, task->fileTimeTicks, task->compLevel);
				FreeTask(task);			
				stat->status = ThreadState::Idle;
				stat->me->mainEvt.Set();
			}
			else
			{
				evt.Wait(1000);
			}
		}
		while (true)
		{
			task = (FileTask*)stat->me->taskList.Get();
			if (task == 0)
				break;
			stat->me->zip.AddFile(task->fileName->ToCString(), task->fileBuff, task->fileSize, task->fileTimeTicks, task->compLevel);
			FreeTask(task);			
		}
	}
	stat->status = ThreadState::Stopped;
	stat->me->mainEvt.Set();
	return 0;
}

void IO::ZIPMTBuilder::FreeTask(FileTask *task)
{
	task->fileName->Release();
	MemFree(task->fileBuff);
	MemFree(task);
}

void IO::ZIPMTBuilder::AddTask(FileTask *task)
{
	while (this->taskList.GetCount() >= 4)
	{
		this->mainEvt.Wait(1000);
	}
	this->taskList.Put(task);
	UOSInt i = this->threadCnt;
	while (i-- > 0)
	{
		if (this->threads[i].status == ThreadState::Idle)
		{
			this->threads[i].evt->Set();
			return;
		}
	}
}

IO::ZIPMTBuilder::ZIPMTBuilder(IO::SeekableStream *stm) : zip(stm)
{
	this->toStop = false;
	this->threadCnt = Sync::ThreadUtil::GetThreadCnt();
	this->threads = MemAlloc(ThreadStat, this->threadCnt);
	UOSInt i = this->threadCnt;
	while (i-- > 0)
	{
		this->threads[i].me = this;
		this->threads[i].evt = 0;
		this->threads[i].status = ThreadState::NotRunning;
		Sync::ThreadUtil::Create(ThreadProc, &this->threads[i]);
	}
}

IO::ZIPMTBuilder::~ZIPMTBuilder()
{
	this->toStop = true;
	UOSInt i = this->threadCnt;
	while (i-- > 0)
	{
		this->threads[i].evt->Set();
	}
	Bool running;
	while (true)
	{
		running = false;
		i = this->threadCnt;
		while (i-- > 0)
		{
			if (this->threads[i].status != ThreadState::Stopped)
			{
				running = true;
				break;
			}
		}
		if (!running)
			break;
		this->mainEvt.Wait(1000);
	}
	MemFree(this->threads);
}

Bool IO::ZIPMTBuilder::AddFile(Text::CString fileName, IO::SeekableStream *stm, Int64 fileTimeTicks, Data::Compress::Inflate::CompressionLevel compLevel)
{
	FileTask *task = MemAlloc(FileTask, 1);
	task->fileSize = (UOSInt)stm->GetLength();
	task->fileBuff = MemAlloc(UInt8, (UOSInt)task->fileSize);
	stm->SeekFromBeginning(0);
	UOSInt readSize;
	UOSInt totalSize = 0;
	while (totalSize < task->fileSize)
	{
		readSize = stm->Read(&task->fileBuff[totalSize], task->fileSize - totalSize);
		if (readSize == 0)
		{
			MemFree(task->fileBuff);
			MemFree(task);
			return false;
		}
		totalSize += readSize;
	}
	task->fileName = Text::String::New(fileName);
	task->fileTimeTicks = fileTimeTicks;
	task->compLevel = compLevel;
	this->AddTask(task);
	return true;
}

Bool IO::ZIPMTBuilder::AddFile(Text::CString fileName, IO::StreamData *fd, Int64 fileTimeTicks, Data::Compress::Inflate::CompressionLevel compLevel)
{
	FileTask *task = MemAlloc(FileTask, 1);
	task->fileSize = (UOSInt)fd->GetDataSize();
	task->fileBuff = MemAlloc(UInt8, (UOSInt)task->fileSize);
	if (fd->GetRealData(0, (UOSInt)task->fileSize, task->fileBuff) != task->fileSize)
	{
		MemFree(task->fileBuff);
		MemFree(task);
		return false;
	}
	task->fileName = Text::String::New(fileName);
	task->fileTimeTicks = fileTimeTicks;
	task->compLevel = compLevel;
	this->AddTask(task);
	return true;
}
