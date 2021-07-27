#include "Stdafx.h"
#include "IO/BTCapturer.h"
#include "IO/BTLog.h"
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
	this->threadRunning = false;
	this->threadToStop = false;
	NEW_CLASS(this->threadEvt, Sync::Event(true, (const UTF8Char*)"threadEvt"));
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
	IO::BTLog btLog;
	Sync::MutexUsage mutUsage;
	Data::UInt64Map<IO::BTScanner::ScanRecord*> *devMap = this->bt->GetRecordMap(&mutUsage);
	UOSInt i;
	btLog.AppendList(devMap);
	Data::DateTime dt;
	dt.SetCurrTime();
	sptr = IO::Path::GetProcessFileName(sbuff);
	i = Text::StrLastIndexOf(sbuff, IO::Path::PATH_SEPERATOR);
	sptr = &sbuff[i + 1];
	sptr = Text::StrConcat(sptr, (const UTF8Char*)"bt");
	sptr = dt.ToString(sptr, "yyyyMMddHHmmss");
	sptr = Text::StrConcat(sptr, (const UTF8Char*)".txt");
	if (btLog.StoreFile(sbuff))
	{
		if (this->lastFileName)
		{
			IO::Path::DeleteFile(this->lastFileName);
			Text::StrDelNew(this->lastFileName);
		}
		this->lastFileName = Text::StrCopyNew(sbuff);
	}
}

Data::ArrayList<IO::BTScanner::ScanRecord*> *IO::BTCapturer::GetLogList(Sync::MutexUsage *mutUsage)
{
	return this->bt->GetRecordMap(mutUsage)->GetValues();
}

void IO::BTCapturer::SetUpdateHandler(IO::BTScanner::RecordHandler hdlr, void *userObj)
{
	this->bt->HandleRecordUpdate(hdlr, userObj);
}
