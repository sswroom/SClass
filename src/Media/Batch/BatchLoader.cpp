#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/StmData/FileData.h"
#include "Media/Batch/BatchLoader.h"
#include "Sync/Event.h"
#include "Sync/MutexUsage.h"
#include "Sync/ThreadUtil.h"

UInt32 __stdcall Media::Batch::BatchLoader::ThreadProc(AnyType userObj)
{
	UTF8Char sbuff[256];
	UnsafeArray<UTF8Char> sptr;
	Optional<Text::String> fileName = 0;
	Optional<DataInfo> info = 0;
	Bool found;
	UOSInt i;
	NN<IO::ParsedObject> nnpobj;

	NN<ThreadState> state = userObj.GetNN<ThreadState>();
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

			NN<Text::String> fileNameStr;
			NN<DataInfo> nninfo;
			if (fileName.SetTo(fileNameStr))
			{
				Sync::MutexUsage mutUsage(state->me->ioMut);
				IO::StmData::FileData fd(fileNameStr, false);
				if (state->me->parsers->ParseFile(fd).SetTo(nnpobj))
				{
					if (nnpobj->GetParserType() == IO::ParserType::ImageList)
					{
						NN<Media::ImageList> imgList = NN<Media::ImageList>::ConvertFrom(nnpobj);
						i = imgList->GetCount();
						while (i-- > 0)
						{
							imgList->ToStaticImage(i);
						}
						mutUsage.EndUse();
						
						sptr = fileNameStr->ConcatTo(sbuff);
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
					nnpobj.Delete();
				}
				else
				{
					mutUsage.EndUse();
				}

				fileNameStr->Release();
			}
			else if (info.SetTo(nninfo))
			{
				Sync::MutexUsage mutUsage(state->me->ioMut);
				if (state->me->parsers->ParseFile(nninfo->data).SetTo(nnpobj))
				{
					if (nnpobj->GetParserType() == IO::ParserType::ImageList)
					{
						NN<Media::ImageList> imgList = NN<Media::ImageList>::ConvertFrom(nnpobj);
						i = imgList->GetCount();
						while (i-- > 0)
						{
							imgList->ToStaticImage(i);
						}
						mutUsage.EndUse();
						
						state->me->hdlr->ImageOutput(imgList, nninfo->fileId->ToCString(), CSTR(""));
					}
					else
					{
						mutUsage.EndUse();
					}
					nnpobj.Delete();
				}
				else
				{
					mutUsage.EndUse();
				}

				nninfo->data.Delete();
				nninfo->fileId->Release();
				MemFreeNN(nninfo);
			}
		}
		state->evt->Wait(1000);
	}
	state->running = false;
	state->me->mainEvt.Set();
	return 0;
}

Media::Batch::BatchLoader::BatchLoader(NN<Parser::ParserList> parsers, NN<Media::Batch::BatchHandler> hdlr)
{
	UOSInt i;
	Bool started;
	this->parsers = parsers;
	this->hdlr = hdlr;

	this->nextThread = 0;
	i = this->threadCnt = Sync::ThreadUtil::GetThreadCnt();
	this->threadStates = MemAllocArr(ThreadState, i);
	while (i-- > 0)
	{
		this->threadStates[i].toStop = false;
		this->threadStates[i].running = false;
		this->threadStates[i].processing = false;
		this->threadStates[i].me = *this;
		NEW_CLASSNN(this->threadStates[i].evt, Sync::Event(true));
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
	NN<DataInfo> data;

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
		this->threadStates[i].evt.Delete();
	}
	MemFreeArr(this->threadStates);


	NN<Text::String> fileName;
	while (this->fileNames.Get().SetTo(fileName))
	{
		fileName->Release();
	}

	while (this->datas.Get().SetTo(data))
	{
		data->data.Delete();
		data->fileId->Release();
		MemFreeNN(data);
	}
}

void Media::Batch::BatchLoader::AddFileName(Text::CStringNN fileName)
{
	Sync::MutexUsage mutUsage(this->reqMut);
	this->fileNames.Put(Text::String::New(fileName));
	this->threadStates[this->nextThread].evt->Set();
	this->nextThread = (this->nextThread + 1) % this->threadCnt;
	mutUsage.EndUse();
}

void Media::Batch::BatchLoader::AddImageData(NN<IO::StreamData> data, Text::CStringNN fileId)
{
	NN<DataInfo> info;
	info = MemAllocNN(DataInfo);
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
