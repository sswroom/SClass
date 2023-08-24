#include "Stdafx.h"
#include "IO/BTCapturer.h"
#include "IO/BTDevLog.h"
#include "IO/Path.h"

//#include <stdio.h>

void __stdcall IO::BTCapturer::CheckThread(NotNullPtr<Sync::Thread> thread)
{
	IO::BTCapturer *me = (IO::BTCapturer*)thread->GetUserObj();
	Int64 currTime;
	Int64 lastTime;
	lastTime = Data::DateTimeUtil::GetCurrTimeMillis();
	while (!thread->IsStopping())
	{
		currTime = Data::DateTimeUtil::GetCurrTimeMillis();
		if ((currTime - lastTime) >= 300000)
		{
			lastTime = currTime;
			if (me->autoStore)
			{
				me->StoreStatus();
			}
		}
		thread->Wait(10000);
	}
}

IO::BTCapturer::BTCapturer(Bool autoStore) : thread(CheckThread, this, CSTR("BTCapturer"))
{
	this->lastFileName = 0;
	this->autoStore = autoStore;
	this->bt = IO::BTScanner::CreateScanner();
	if (this->bt)
	{
		//this->bt->SetScanMode(IO::BTScanner::SM_ACTIVE);
	}
}

IO::BTCapturer::~BTCapturer()
{
	this->Stop();
	if (this->bt)
	{
		this->bt->Close();
		DEL_CLASS(this->bt);
	}
	SDEL_TEXT(this->lastFileName);
}

Bool IO::BTCapturer::IsError()
{
	return this->bt == 0;
}

Bool IO::BTCapturer::IsStarted()
{
	return this->thread.IsRunning();
}

Bool IO::BTCapturer::Start()
{
	if (this->IsError() || this->IsStarted())
	{
		return false;
	}
	this->thread.Start();
	this->bt->ScanOn();
	return true;
}

void IO::BTCapturer::Stop()
{
	//printf("BTCapturer: Stopping\r\n");
	if (this->thread.IsRunning())
	{
		this->thread.Stop();
		//printf("BTCapturer: Stopping 2\r\n");
		this->bt->ScanOff();
	}
	//printf("BTCapturer: Stopping 3\r\n");
}

void IO::BTCapturer::StoreStatus()
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	IO::BTDevLog btLog;
	Sync::MutexUsage mutUsage;
	btLog.AppendList(this->bt->GetPublicMap(mutUsage));
	btLog.AppendList(this->bt->GetRandomMap(mutUsage));
	UOSInt i;
	Data::DateTime dt;
	dt.SetCurrTime();
	sptr = IO::Path::GetProcessFileName(sbuff);
	i = Text::StrLastIndexOfCharC(sbuff, (UOSInt)(sptr - sbuff), IO::Path::PATH_SEPERATOR);
	sptr = &sbuff[i + 1];
	sptr = Text::StrConcatC(sptr, UTF8STRC("bt"));
	sptr = dt.ToString(sptr, "yyyyMMddHHmmss");
	sptr = Text::StrConcatC(sptr, UTF8STRC(".txt"));
	if (btLog.StoreFile(CSTRP(sbuff, sptr)))
	{
		if (this->lastFileName)
		{
			IO::Path::DeleteFile(this->lastFileName);
			Text::StrDelNew(this->lastFileName);
		}
		this->lastFileName = Text::StrCopyNewC(sbuff, (UOSInt)(sptr - sbuff)).Ptr();
	}
}

NotNullPtr<const Data::ReadingList<IO::BTScanLog::ScanRecord3*>> IO::BTCapturer::GetPublicList(NotNullPtr<Sync::MutexUsage> mutUsage) const
{
	return this->bt->GetPublicMap(mutUsage);
}

NotNullPtr<const Data::ReadingList<IO::BTScanLog::ScanRecord3*>> IO::BTCapturer::GetRandomList(NotNullPtr<Sync::MutexUsage> mutUsage) const
{
	return this->bt->GetRandomMap(mutUsage);
}

void IO::BTCapturer::SetUpdateHandler(IO::BTScanner::RecordHandler hdlr, void *userObj)
{
	this->bt->HandleRecordUpdate(hdlr, userObj);
}
