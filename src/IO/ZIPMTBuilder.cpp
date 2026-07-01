#include "Stdafx.h"
#include "IO/ZIPMTBuilder.h"
#include "Sync/ThreadUtil.h"
#define VERBOSE 1
#if defined(VERBOSE)
#include <stdio.h>
#endif
void __stdcall IO::ZIPMTBuilder::ThreadProc(NN<Sync::Thread> thread)
{
	NN<IO::ZIPMTBuilder> me = thread->GetUserObj().GetNN<IO::ZIPMTBuilder>();
	NN<FileTask> task;
	NN<IO::SeekableStream> stm;
	UnsafeArray<UInt8> fileBuff;
	while (!thread->IsStopping())
	{
		if (me->taskList.Get().GetOpt<FileTask>().SetTo(task))
		{
			if (task->fileStm.SetTo(stm))
			{
				me->zip.AddFile(task->fileName->ToCString(), stm, task->lastModTime, task->lastAccessTime, task->createTime, task->compLevel, task->unixAttr);
			}
			else if (task->fileBuff.SetTo(fileBuff))
			{
				me->zip.AddFile(task->fileName->ToCString(), fileBuff, task->fileSize, task->lastModTime, task->lastAccessTime, task->createTime, task->compLevel, task->unixAttr);
			}
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
		if (!me->taskList.Get().GetOpt<FileTask>().SetTo(task))
			break;
		if (task->fileStm.SetTo(stm))
		{
			me->zip.AddFile(task->fileName->ToCString(), stm, task->lastModTime, task->lastAccessTime, task->createTime, task->compLevel, task->unixAttr);
		}
		else if (task->fileBuff.SetTo(fileBuff))
		{
			me->zip.AddFile(task->fileName->ToCString(), fileBuff, task->fileSize, task->lastModTime, task->lastAccessTime, task->createTime, task->compLevel, task->unixAttr);
		}
		FreeTask(task);
	}
	me->mainEvt.Set();
}

void IO::ZIPMTBuilder::FreeTask(NN<FileTask> task)
{
	task->fileName->Release();
	NN<IO::SeekableStream> stm;
	UnsafeArray<UInt8> fileBuff;
	if (task->fileBuff.SetTo(fileBuff))
	{
		MemFreeArr(fileBuff);
	}
	else if (task->fileStm.SetTo(stm))
	{
		stm.Delete();
	}
	MemFreeNN(task);
}

void IO::ZIPMTBuilder::AddTask(NN<FileTask> task)
{
	while (this->taskList.GetCount() >= 4)
	{
		this->mainEvt.Wait(1000);
	}
	this->taskList.Put(task);
	UIntOS i = this->threadCnt;
	while (i-- > 0)
	{
		if (this->threads[i]->IsWaiting())
		{
			this->threads[i]->Notify();
			return;
		}
	}
}

IO::ZIPMTBuilder::ZIPMTBuilder(NN<IO::SeekableStream> stm, IO::ZIPOS os) : zip(stm, os)
{
	UTF8Char sbuff[32];
	UnsafeArray<UTF8Char> sptr;
	this->toStop = false;
	this->threadCnt = Sync::ThreadUtil::GetThreadCnt();
	this->threads = MemAllocArr(NN<Sync::Thread>, this->threadCnt);
	UIntOS i = this->threadCnt;
	while (i-- > 0)
	{
		sptr = Text::StrUIntOS(Text::StrConcatC(sbuff, UTF8STRC("ZIPMTBuilder")), i);
		NEW_CLASSNN(this->threads[i], Sync::Thread(ThreadProc, this, CSTRP(sbuff, sptr)));
		this->threads[i]->Start();
	}
}

IO::ZIPMTBuilder::~ZIPMTBuilder()
{
	this->toStop = true;
	UIntOS i = this->threadCnt;
	while (i-- > 0)
	{
		this->threads[i]->BeginStop();
	}
	i = this->threadCnt;
	while (i-- > 0)
	{
		this->threads[i]->WaitForEnd();
		this->threads[i].Delete();
	}
	MemFreeArr(this->threads);
}

Bool IO::ZIPMTBuilder::AddFile(Text::CStringNN fileName, NN<IO::SeekableStream> stm, Data::Timestamp lastModTime, Data::Timestamp lastAccessTime, Data::Timestamp createTime, Data::Compress::Deflater::CompLevel compLevel, UInt32 unixAttr)
{
	NN<FileTask> task = MemAllocNN(FileTask);
	UnsafeArray<UInt8> fileBuff;
	task->fileSize = (UIntOS)stm->GetLength();
	task->fileBuff = fileBuff = MemAllocArr(UInt8, (UIntOS)task->fileSize);
	task->fileStm = nullptr;
	stm->SeekFromBeginning(0);
	UIntOS readSize;
	UIntOS totalSize = 0;
	while (totalSize < task->fileSize)
	{
		readSize = stm->Read(Data::ByteArray(&fileBuff[totalSize], task->fileSize - totalSize));
		if (readSize == 0)
		{
#if defined(VERBOSE)
			printf("Error in reading file from stream: file size = %lld, total read = %lld\r\n", (UInt64)task->fileSize, (UInt64)totalSize);
#endif
			MemFreeArr(fileBuff);
			MemFreeNN(task);
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

Bool IO::ZIPMTBuilder::AddFile(Text::CStringNN fileName, NN<IO::StreamData> fd, Data::Timestamp lastModTime, Data::Timestamp lastAccessTime, Data::Timestamp createTime, Data::Compress::Deflater::CompLevel compLevel, UInt32 unixAttr)
{
	UIntOS readSize;
	UnsafeArray<UInt8> fileBuff;
	NN<FileTask> task = MemAllocNN(FileTask);
	task->fileSize = (UIntOS)fd->GetDataSize();
	task->fileBuff = fileBuff = MemAllocArr(UInt8, (UIntOS)task->fileSize);
	task->fileStm = nullptr;
	if ((readSize = fd->GetRealData(0, (UIntOS)task->fileSize, Data::ByteArray(fileBuff, task->fileSize))) != task->fileSize)
	{
#if defined(VERBOSE)
		printf("Error in reading file from file data: file size = %lld, total read = %lld, fileName = %s\r\n", (UInt64)task->fileSize, (UInt64)readSize, fileName.v.Ptr());
#endif
		MemFreeArr(fileBuff);
		MemFreeNN(task);
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

Bool IO::ZIPMTBuilder::AddFileOTF(Text::CStringNN fileName, NN<IO::SeekableStream> stm, Data::Timestamp lastModTime, Data::Timestamp lastAccessTime, Data::Timestamp createTime, Data::Compress::Deflater::CompLevel compLevel, UInt32 unixAttr)
{
	NN<FileTask> task = MemAllocNN(FileTask);
	task->fileSize = (UIntOS)stm->GetLength();
	task->fileBuff = nullptr;
	task->fileStm = stm;
	stm->SeekFromBeginning(0);
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
