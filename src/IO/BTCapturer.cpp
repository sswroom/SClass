#include "Stdafx.h"
#include "IO/BTCapturer.h"
#include "IO/BTDevLog.h"
#include "IO/Path.h"
#include "Sync/Thread.h"

UInt32 __stdcall IO::BTCapturer::CheckThread(void *userObj)
{
	IO::BTCapturer *me = (IO::BTCapturer*)userObj;
	Data::DateTime *dt;
	Int64 lastTime;
	me->threadRunning = true;
	NEW_CLASS(dt, Data::DateTime());
	dt->SetCurrTimeUTC();
	lastTime = dt->ToTicks();
	while (!me->threadToStop)
	{
		dt->SetCurrTimeUTC();
		if ((dt->ToTicks() - lastTime) >= 300000)
		{
			lastTime = dt->ToTicks();
			me->StoreStatus();
		}
		me->threadEvt->Wait(10000);
	}
	DEL_CLASS(dt);
	me->threadRunning = false;
	return 0;
}

IO::BTCapturer::BTCapturer()
{
	this->lastFileName = 0;
	this->bt = IO::BTScanner::CreateScanner();
	if (this->bt)
	{
		//this->bt->SetScanMode(IO::BTScanner::SM_ACTIVE);
	}
	this->threadRunning = false;
	this->threadToStop = false;
	NEW_CLASS(this->threadEvt, Sync::Event(true));
}

IO::BTCapturer::~BTCapturer()
{
	this->Stop();
	if (this->bt)
	{
		this->bt->Close();
		DEL_CLASS(this->threadEvt);
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
	return this->threadRunning;
}

Bool IO::BTCapturer::Start()
{
	if (this->IsError() || this->IsStarted())
	{
		return false;
	}
	this->threadToStop = false;
	Sync::Thread::Create(CheckThread, this);
	this->bt->ScanOn();
	return true;
}

void IO::BTCapturer::Stop()
{
	if (this->threadRunning)
	{
		this->threadToStop = true;
		this->threadEvt->Set();
		while (this->threadRunning)
		{
			Sync::Thread::Sleep(10);
		}
		this->bt->ScanOff();
	}
}

void IO::BTCapturer::StoreStatus()
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	IO::BTDevLog btLog;
	Sync::MutexUsage mutUsage;
	btLog.AppendList(this->bt->GetPublicMap(&mutUsage));
	btLog.AppendList(this->bt->GetRandomMap(&mutUsage));
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
		this->lastFileName = Text::StrCopyNew(sbuff);
	}
}

Data::ArrayList<IO::BTScanLog::ScanRecord3*> *IO::BTCapturer::GetPublicList(Sync::MutexUsage *mutUsage)
{
	return this->bt->GetPublicMap(mutUsage)->GetValues();
}

Data::ArrayList<IO::BTScanLog::ScanRecord3*> *IO::BTCapturer::GetRandomList(Sync::MutexUsage *mutUsage)
{
	return this->bt->GetRandomMap(mutUsage)->GetValues();
}

void IO::BTCapturer::SetUpdateHandler(IO::BTScanner::RecordHandler hdlr, void *userObj)
{
	this->bt->HandleRecordUpdate(hdlr, userObj);
}
