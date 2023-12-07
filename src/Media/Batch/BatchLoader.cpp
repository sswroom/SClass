#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/StmData/FileData.h"
#include "Media/Batch/BatchLoader.h"
#include "Sync/Event.h"
#include "Sync/MutexUsage.h"
#include "Sync/ThreadUtil.h"

UInt32 __stdcall Media::Batch::BatchLoader::ThreadProc(void *userObj)
{
	UTF8Char sbuff[256];
	UTF8Char *sptr;
	Text::String *fileName = 0;
	DataInfo *info = 0;
	Bool found;
	UOSInt i;
	IO::ParsedObject *pobj;
	NotNullPtr<IO::ParsedObject> nnpobj;

	ThreadState *state = (ThreadState*)userObj;
	state->running = true;
	state->me->mainEvt.Set();
	while (!state->toStop)
	{
		while (true)
		{
			found = false;
			Sync::MutexUsage mutUsage(state->me->reqMut);
			if (state->me->fileNames.HasItems())
			{
				fileName = state->me->fileNames.Get();
				info = 0;
				found = true;
				state->processing = true;
			}
			else if (state->me->datas.HasItems())
			{
				fileName = 0;
				info = state->me->datas.Get();
				found = true;
			}
			else
			{
				state->processing = false;
			}
			mutUsage.EndUse();
			if (!found)
				break;

			NotNullPtr<Text::String> fileNameStr;
			if (fileNameStr.Set(fileName))
			{
				Sync::MutexUsage mutUsage(state->me->ioMut);
				{
					IO::StmData::FileData fd(fileNameStr, false);
					pobj = state->me->parsers->ParseFile(fd);
				}
				if (nnpobj.Set(pobj))
				{
					if (nnpobj->GetParserType() == IO::ParserType::ImageList)
					{
						NotNullPtr<Media::ImageList> imgList = NotNullPtr<Media::ImageList>::ConvertFrom(nnpobj);
						i = imgList->GetCount();
						while (i-- > 0)
						{
							imgList->ToStaticImage(i);
						}
						mutUsage.EndUse();
						
						sptr = fileName->ConcatTo(sbuff);
						i = Text::StrLastIndexOfCharC(sbuff, (UOSInt)(sptr - sbuff), '.');
						if (i != INVALID_INDEX)
						{
							sbuff[i] = 0;
							sptr = &sbuff[i];
						}
						state->me->hdlr->ImageOutput(imgList, CSTRP(sbuff, sptr), CSTR(""));
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

				fileName->Release();
			}
			else
			{
				Sync::MutexUsage mutUsage(state->me->ioMut);
				pobj = state->me->parsers->ParseFile(info->data);
				if (nnpobj.Set(pobj))
				{
					if (nnpobj->GetParserType() == IO::ParserType::ImageList)
					{
						NotNullPtr<Media::ImageList> imgList = NotNullPtr<Media::ImageList>::ConvertFrom(nnpobj);
						i = imgList->GetCount();
						while (i-- > 0)
						{
							imgList->ToStaticImage(i);
						}
						mutUsage.EndUse();
						
						state->me->hdlr->ImageOutput(imgList, info->fileId->ToCString(), CSTR(""));
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

				info->data.Delete();
				info->fileId->Release();
				MemFree(info);
			}
		}
		state->evt->Wait(1000);
	}
	state->running = false;
	state->me->mainEvt.Set();
	return 0;
}

Media::Batch::BatchLoader::BatchLoader(NotNullPtr<Parser::ParserList> parsers, Media::Batch::BatchHandler *hdlr)
{
	UOSInt i;
	Bool started;
	this->parsers = parsers;
	this->hdlr = hdlr;

	this->nextThread = 0;
	i = this->threadCnt = Sync::ThreadUtil::GetThreadCnt();
	this->threadStates = MemAlloc(ThreadState, i);
	while (i-- > 0)
	{
		this->threadStates[i].toStop = false;
		this->threadStates[i].running = false;
		this->threadStates[i].processing = false;
		this->threadStates[i].me = this;
		NEW_CLASS(this->threadStates[i].evt, Sync::Event(true));
		Sync::ThreadUtil::Create(ThreadProc, &this->threadStates[i]);
	}

	while (true)
	{
		this->mainEvt.Wait(100);

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
		this->mainEvt.Wait(1000);

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


	Text::String *fileName;
	while ((fileName = this->fileNames.Get()) != 0)
	{
		fileName->Release();
	}

	while ((data = this->datas.Get()) != 0)
	{
		data->data.Delete();
		data->fileId->Release();
		MemFree(data);
	}
}

void Media::Batch::BatchLoader::AddFileName(Text::CString fileName)
{
	Sync::MutexUsage mutUsage(this->reqMut);
	this->fileNames.Put(Text::String::New(fileName).Ptr());
	this->threadStates[this->nextThread].evt->Set();
	this->nextThread = (this->nextThread + 1) % this->threadCnt;
	mutUsage.EndUse();
}

void Media::Batch::BatchLoader::AddImageData(NotNullPtr<IO::StreamData> data, Text::CStringNN fileId)
{
	DataInfo *info;
	info = MemAlloc(DataInfo, 1);
	info->data = data->GetPartialData(0, data->GetDataSize());
	info->fileId = Text::String::New(fileId);
	Sync::MutexUsage mutUsage(this->reqMut);
	this->datas.Put(info);
	this->threadStates[this->nextThread].evt->Set();
	this->nextThread = (this->nextThread + 1) % this->threadCnt;
	mutUsage.EndUse();
}

Bool Media::Batch::BatchLoader::IsProcessing()
{
	Bool proc = false;
	Sync::MutexUsage mutUsage(this->reqMut);
	if (this->fileNames.HasItems())
	{
		proc = true;
	}
	else if (this->datas.HasItems())
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
