#include "Stdafx.h"
#include "IO/BTCapturer.h"
#include "IO/BTLog.h"
#include "IO/Path.h"
#include "Sync/Thread.h"

struct IO::BTCapturer::ClassData
{
	IO::ProgCtrl::BluetoothCtlProgCtrl *bt;
	Bool running;
};

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
	this->clsData = MemAlloc(ClassData, 1);
	this->lastFileName = 0;
	this->clsData->running = false;
	NEW_CLASS(this->clsData->bt, IO::ProgCtrl::BluetoothCtlProgCtrl());
	this->threadRunning = false;
	this->threadToStop = false;
	NEW_CLASS(this->threadEvt, Sync::Event(true, (const UTF8Char*)"threadEvt"));
	if (this->clsData->bt->WaitForCmdReady())
	{
		this->clsData->running = true;
	}
}

IO::BTCapturer::~BTCapturer()
{
	this->Stop();
	this->clsData->bt->Exit();
	DEL_CLASS(this->threadEvt);
	DEL_CLASS(this->clsData->bt);
	MemFree(this->clsData);
	SDEL_TEXT(this->lastFileName);
}

Bool IO::BTCapturer::IsError()
{
	return !this->clsData->running;
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
	this->clsData->bt->ScanOn();
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
		this->clsData->bt->ScanOff();
	}
}

void IO::BTCapturer::StoreStatus()
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	IO::BTLog btLog;
	Sync::MutexUsage mutUsage;
	Data::UInt64Map<IO::ProgCtrl::BluetoothCtlProgCtrl::DeviceInfo*> *devMap = this->clsData->bt->GetDeviceMap(&mutUsage);
	OSInt si;
	btLog.AppendList(devMap);
	Data::DateTime dt;
	dt.SetCurrTime();
	sptr = IO::Path::GetProcessFileName(sbuff);
	si = Text::StrLastIndexOf(sbuff, IO::Path::PATH_SEPERATOR);
	sptr = &sbuff[si + 1];
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

Data::ArrayList<IO::ProgCtrl::BluetoothCtlProgCtrl::DeviceInfo*> *IO::BTCapturer::GetLogList(Sync::MutexUsage *mutUsage)
{
	return this->clsData->bt->GetDeviceMap(mutUsage)->GetValues();
}

void IO::BTCapturer::SetUpdateHandler(IO::ProgCtrl::BluetoothCtlProgCtrl::DeviceHandler hdlr, void *userObj)
{
	this->clsData->bt->HandleDeviceUpdate(hdlr, userObj);
}
