#include "Stdafx.h"
#include "IO/ZIPMTBuilder.h"
#include "Sync/ThreadUtil.h"
#define VERBOSE 1
#if defined(VERBOSE)
#include <stdio.h>
#endif
void __stdcall IO::ZIPMTBuilder::ThreadProc(NotNullPtr<Sync::Thread> thread)
{
	NotNullPtr<IO::ZIPMTBuilder> me = thread->GetUserObj().GetNN<IO::ZIPMTBuilder>();
	FileTask *task;
	while (!thread->IsStopping())
	{
		task = (FileTask*)me->taskList.Get();
		if (task)
		{
			me->zip.AddFile(task->fileName->ToCString(), task->fileBuff, task->fileSize, task->lastModTime, task->lastAccessTime, task->createTime, task->compLevel, task->unixAttr);
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
		me->zip.AddFile(task->fileName->ToCString(), task->fileBuff, task->fileSize, task->lastModTime, task->lastAccessTime, task->createTime, task->compLevel, task->unixAttr);
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

IO::ZIPMTBuilder::ZIPMTBuilder(NotNullPtr<IO::SeekableStream> stm, IO::ZIPOS os) : zip(stm, os)
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

Bool IO::ZIPMTBuilder::AddFile(Text::CStringNN fileName, NotNullPtr<IO::SeekableStream> stm, Data::Timestamp lastModTime, Data::Timestamp lastAccessTime, Data::Timestamp createTime, Data::Compress::Inflate::CompressionLevel compLevel, UInt32 unixAttr)
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
#if defined(VERBOSE)
			printf("Error in reading file from stream: file size = %lld, total read = %lld\r\n", (UInt64)task->fileSize, (UInt64)totalSize);
#endif
			MemFree(task->fileBuff);
			MemFree(task);
			return false;
		}
		totalSize += readSize;
	}
	task->fileName = Text::String::New(fileName);
	task->lastModTime = lastModTime;
	task->lastAccessTime = lastAccessTime;
	task->createTime = createTime;
	task->compLevel = compLevel;
	task->unixAttr = unixAttr;
	this->AddTask(task);
	return true;
}

Bool IO::ZIPMTBuilder::AddFile(Text::CStringNN fileName, NotNullPtr<IO::StreamData> fd, Data::Timestamp lastModTime, Data::Timestamp lastAccessTime, Data::Timestamp createTime, Data::Compress::Inflate::CompressionLevel compLevel, UInt32 unixAttr)
{
	UOSInt readSize;
	FileTask *task = MemAlloc(FileTask, 1);
	task->fileSize = (UOSInt)fd->GetDataSize();
	task->fileBuff = MemAlloc(UInt8, (UOSInt)task->fileSize);
	if ((readSize = fd->GetRealData(0, (UOSInt)task->fileSize, Data::ByteArray(task->fileBuff, task->fileSize))) != task->fileSize)
	{
#if defined(VERBOSE)
		printf("Error in reading file from file data: file size = %lld, total read = %lld, fileName = %s\r\n", (UInt64)task->fileSize, (UInt64)readSize, fileName.v);
#endif
		MemFree(task->fileBuff);
		MemFree(task);
		return false;
	}
	task->fileName = Text::String::New(fileName);
	task->lastModTime = lastModTime;
	task->lastAccessTime = lastAccessTime;
	task->createTime = createTime;
	task->compLevel = compLevel;
	task->unixAttr = unixAttr;
	this->AddTask(task);
	return true;
}

Bool IO::ZIPMTBuilder::AddDir(Text::CStringNN dirName, Data::Timestamp lastModTime, Data::Timestamp lastAccessTime, Data::Timestamp createTime, UInt32 unixAttr)
{
	return this->zip.AddDir(dirName, lastModTime, lastAccessTime, createTime, unixAttr);
}

Bool IO::ZIPMTBuilder::AddDeflate(Text::CStringNN fileName, Data::ByteArrayR buff, UInt64 decSize, UInt32 crcVal, Data::Timestamp lastModTime, Data::Timestamp lastAccessTime, Data::Timestamp createTime, UInt32 unixAttr)
{
	return this->zip.AddDeflate(fileName, buff, decSize, crcVal, lastModTime, lastAccessTime, createTime, unixAttr);	
}
