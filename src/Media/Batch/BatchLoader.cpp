#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/StmData/FileData.h"
#include "Media/Batch/BatchLoader.h"
#include "Sync/Event.h"
#include "Sync/MutexUsage.h"
#include "Sync/Thread.h"

UInt32 __stdcall Media::Batch::BatchLoader::ThreadProc(void *userObj)
{
	UTF8Char sbuff[256];
	const UTF8Char *fileName = 0;
	DataInfo *info = 0;
	Bool found;
	UOSInt i;
	IO::ParsedObject *pobj;
	IO::ParsedObject::ParserType pt;

	ThreadState *state = (ThreadState*)userObj;
	state->running = true;
	state->me->mainEvt->Set();
	while (!state->toStop)
	{
		while (true)
		{
			found = false;
			Sync::MutexUsage mutUsage(state->me->reqMut);
			if (state->me->fileNames->HasItems())
			{
				fileName = (const UTF8Char*)state->me->fileNames->Get();
				info = 0;
				found = true;
				state->processing = true;
			}
			else if (state->me->datas->HasItems())
			{
				fileName = 0;
				info = (DataInfo*)state->me->datas->Get();
				found = true;
			}
			else
			{
				state->processing = false;
			}
			mutUsage.EndUse();
			if (!found)
				break;

			if (fileName)
			{
				IO::StmData::FileData *fd;

				Sync::MutexUsage mutUsage(state->me->ioMut);
				NEW_CLASS(fd, IO::StmData::FileData(fileName, false));
				pobj = state->me->parsers->ParseFile(fd, &pt);
				DEL_CLASS(fd);
				if (pobj)
				{
					if (pt == IO::ParsedObject::PT_IMAGE_LIST_PARSER)
					{
						Media::ImageList *imgList = (Media::ImageList*)pobj;
						i = imgList->GetCount();
						while (i-- > 0)
						{
							imgList->ToStaticImage(i);
						}
						mutUsage.EndUse();
						
						Text::StrConcat(sbuff, fileName);
						i = Text::StrLastIndexOf(sbuff, '.');
						if (i != INVALID_INDEX)
						{
							sbuff[i] = 0;
						}
						state->me->hdlr->ImageOutput(imgList, sbuff, (const UTF8Char*)"");
					}
					else
					{
						mutUsage.EndUse();
					}
					DEL_CLASS(pobj);
				}
				else
				{
					mutUsage.EndUse();
				}

				Text::StrDelNew(fileName);
			}
			else
			{
				Sync::MutexUsage mutUsage(state->me->ioMut);
				pobj = state->me->parsers->ParseFile(info->data, &pt);
				if (pobj)
				{
					if (pt == IO::ParsedObject::PT_IMAGE_LIST_PARSER)
					{
						Media::ImageList *imgList = (Media::ImageList*)pobj;
						i = imgList->GetCount();
						while (i-- > 0)
						{
							imgList->ToStaticImage(i);
						}
						mutUsage.EndUse();
						
						state->me->hdlr->ImageOutput(imgList, info->fileId, (const UTF8Char*)"");
					}
					else
					{
						mutUsage.EndUse();
					}
					DEL_CLASS(pobj);
				}
				else
				{
					mutUsage.EndUse();
				}

				DEL_CLASS(info->data);
				Text::StrDelNew(info->fileId);
				MemFree(info);
			}
		}
		state->evt->Wait(1000);
	}
	state->running = false;
	state->me->mainEvt->Set();
	return 0;
}

Media::Batch::BatchLoader::BatchLoader(Parser::ParserList *parsers, Media::Batch::BatchHandler *hdlr)
{
	UOSInt i;
	Bool started;
	this->parsers = parsers;
	this->hdlr = hdlr;
	NEW_CLASS(this->ioMut, Sync::Mutex());
	NEW_CLASS(this->reqMut, Sync::Mutex());
	NEW_CLASS(this->fileNames, Data::LinkedList());
	NEW_CLASS(this->datas, Data::LinkedList());
	NEW_CLASS(this->mainEvt, Sync::Event(true, (const UTF8Char*)"Media.Batch.BatchLoader.mainEvt"));

	this->nextThread = 0;
	i = this->threadCnt = Sync::Thread::GetThreadCnt();
	this->threadStates = MemAlloc(ThreadState, i);
	while (i-- > 0)
	{
		this->threadStates[i].toStop = false;
		this->threadStates[i].running = false;
		this->threadStates[i].processing = false;
		this->threadStates[i].me = this;
		NEW_CLASS(this->threadStates[i].evt, Sync::Event(true, (const UTF8Char*)"Media.Batch.BatchLoader.threadStates.evt"));
		Sync::Thread::Create(ThreadProc, &this->threadStates[i]);
	}

	while (true)
	{
		this->mainEvt->Wait(100);

		started = true;
		i = this->threadCnt;
		while (i-- > 0)
		{
			if (!this->threadStates[i].running)
			{
				started = false;
				break;
			}
		}
		if (started)
			break;
	}
}

Media::Batch::BatchLoader::~BatchLoader()
{
	UOSInt i;
	Bool exited;
	DataInfo *data;

	i = this->threadCnt;
	while (i-- > 0)
	{
		this->threadStates[i].toStop = true;
		this->threadStates[i].evt->Set();
	}

	while (true)
	{
		this->mainEvt->Wait(1000);

		exited = true;
		i = threadCnt;
		while (i-- > 0)
		{
			if (this->threadStates[i].running)
			{
				exited = false;
				break;
			}
		}
		if (exited)
			break;
	}


	i = this->threadCnt;
	while (i-- > 0)
	{
		DEL_CLASS(this->threadStates[i].evt);
	}
	MemFree(this->threadStates);

	DEL_CLASS(this->ioMut);
	DEL_CLASS(this->reqMut);

	const UTF8Char *fileName;
	while ((fileName = (const UTF8Char*)this->fileNames->Get()) != 0)
	{
		Text::StrDelNew(fileName);
	}
	DEL_CLASS(this->fileNames);

	while ((data = (DataInfo*)this->datas->Get()) != 0)
	{
		DEL_CLASS(data->data);
		Text::StrDelNew(data->fileId);
		MemFree(data);
	}
	DEL_CLASS(this->datas);
	DEL_CLASS(this->mainEvt);
}

void Media::Batch::BatchLoader::AddFileName(const UTF8Char *fileName)
{
	Sync::MutexUsage mutUsage(this->reqMut);
	this->fileNames->Put((void*)Text::StrCopyNew(fileName));
	this->threadStates[this->nextThread].evt->Set();
	this->nextThread = (this->nextThread + 1) % this->threadCnt;
	mutUsage.EndUse();
}

void Media::Batch::BatchLoader::AddImageData(IO::IStreamData *data, const UTF8Char *fileId)
{
	DataInfo *info;
	info = MemAlloc(DataInfo, 1);
	info->data = data->GetPartialData(0, data->GetDataSize());
	info->fileId = Text::StrCopyNew(fileId);
	Sync::MutexUsage mutUsage(this->reqMut);
	this->datas->Put(info);
	this->threadStates[this->nextThread].evt->Set();
	this->nextThread = (this->nextThread + 1) % this->threadCnt;
	mutUsage.EndUse();
}

Bool Media::Batch::BatchLoader::IsProcessing()
{
	Bool proc = false;
	Sync::MutexUsage mutUsage(this->reqMut);
	if (this->fileNames->HasItems())
	{
		proc = true;
	}
	else if (this->datas->HasItems())
	{
		proc = true;
	}
	else
	{
		UOSInt i = this->threadCnt;
		while (i-- > 0)
		{
			if (this->threadStates[i].processing)
			{
				proc = true;
				break;
			}
		}
	}
	mutUsage.EndUse();
	return proc;
}
