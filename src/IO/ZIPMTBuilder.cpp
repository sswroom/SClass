#include "Stdafx.h"
#include "IO/ZIPMTBuilder.h"
#include "Sync/ThreadUtil.h"

void __stdcall IO::ZIPMTBuilder::ThreadProc(NotNullPtr<Sync::Thread> thread)
{
	IO::ZIPMTBuilder *me = (IO::ZIPMTBuilder*)thread->GetUserObj();
	FileTask *task;
	while (!thread->IsStopping())
	{
		task = (FileTask*)me->taskList.Get();
		if (task)
		{
			me->zip.AddFile(task->fileName->ToCString(), task->fileBuff, task->fileSize, task->fileTimeTicks, task->compLevel);
			FreeTask(task);			
			me->mainEvt.Set();
		}
		else
		{
			thread->Wait(1000);
		}
	}
	while (true)
	{
		task = (FileTask*)me->taskList.Get();
		if (task == 0)
			break;
		me->zip.AddFile(task->fileName->ToCString(), task->fileBuff, task->fileSize, task->fileTimeTicks, task->compLevel);
		FreeTask(task);			
	}
	me->mainEvt.Set();
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
		if (this->threads[i]->IsWaiting())
		{
			this->threads[i]->Notify();
			return;
		}
	}
}

IO::ZIPMTBuilder::ZIPMTBuilder(NotNullPtr<IO::SeekableStream> stm) : zip(stm)
{
	UTF8Char sbuff[32];
	UTF8Char *sptr;
	this->toStop = false;
	this->threadCnt = Sync::ThreadUtil::GetThreadCnt();
	this->threads = MemAlloc(Sync::Thread*, this->threadCnt);
	UOSInt i = this->threadCnt;
	while (i-- > 0)
	{
		sptr = Text::StrUOSInt(Text::StrConcatC(sbuff, UTF8STRC("ZIPMTBuilder")), i);
		NEW_CLASS(this->threads[i], Sync::Thread(ThreadProc, this, CSTRP(sbuff, sptr)));
		this->threads[i]->Start();
	}
}

IO::ZIPMTBuilder::~ZIPMTBuilder()
{
	this->toStop = true;
	UOSInt i = this->threadCnt;
	while (i-- > 0)
	{
		this->threads[i]->BeginStop();
	}
	i = this->threadCnt;
	while (i-- > 0)
	{
		this->threads[i]->WaitForEnd();
		DEL_CLASS(this->threads[i]);
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
		readSize = stm->Read(Data::ByteArray(&task->fileBuff[totalSize], task->fileSize - totalSize));
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

Bool IO::ZIPMTBuilder::AddFile(Text::CString fileName, NotNullPtr<IO::StreamData> fd, Int64 fileTimeTicks, Data::Compress::Inflate::CompressionLevel compLevel)
{
	FileTask *task = MemAlloc(FileTask, 1);
	task->fileSize = (UOSInt)fd->GetDataSize();
	task->fileBuff = MemAlloc(UInt8, (UOSInt)task->fileSize);
	if (fd->GetRealData(0, (UOSInt)task->fileSize, Data::ByteArray(task->fileBuff, task->fileSize)) != task->fileSize)
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
