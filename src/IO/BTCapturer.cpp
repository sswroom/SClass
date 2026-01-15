#include "Stdafx.h"
#include "IO/BTCapturer.h"
#include "IO/BTDevLog.h"
#include "IO/Path.h"

//#include <stdio.h>

void __stdcall IO::BTCapturer::CheckThread(NN<Sync::Thread> thread)
{
	NN<IO::BTCapturer> me = thread->GetUserObj().GetNN<IO::BTCapturer>();
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
	this->lastFileName = nullptr;
	this->autoStore = autoStore;
	this->bt = IO::BTScanner::CreateScanner();
	if (this->bt.NotNull())
	{
		//this->bt->SetScanMode(IO::BTScanner::SM_ACTIVE);
	}
}

IO::BTCapturer::~BTCapturer()
{
	this->Stop();
	NN<IO::BTScanner> bt;
	if (this->bt.SetTo(bt))
	{
		bt->Close();
		this->bt.Delete();
	}
	SDEL_TEXT(this->lastFileName);
}

Bool IO::BTCapturer::IsError()
{
	return this->bt.IsNull();
}

Bool IO::BTCapturer::IsStarted()
{
	return this->thread.IsRunning();
}

Bool IO::BTCapturer::Start()
{
	NN<IO::BTScanner> bt;
	if (!this->bt.SetTo(bt) || this->IsStarted())
	{
		return false;
	}
	this->thread.Start();
	bt->ScanOn();
	return true;
}

void IO::BTCapturer::Stop()
{
	//printf("BTCapturer: Stopping\r\n");
	NN<IO::BTScanner> bt;
	if (this->thread.IsRunning() && this->bt.SetTo(bt))
	{
		this->thread.Stop();
		//printf("BTCapturer: Stopping 2\r\n");
		bt->ScanOff();
	}
	//printf("BTCapturer: Stopping 3\r\n");
}

void IO::BTCapturer::StoreStatus()
{
	NN<IO::BTScanner> bt;
	if (this->bt.SetTo(bt))
	{
		UTF8Char sbuff[512];
		UnsafeArray<UTF8Char> sptr;
		IO::BTDevLog btLog;
		Sync::MutexUsage mutUsage;
		btLog.AppendList(bt->GetPublicMap(mutUsage));
		btLog.AppendList(bt->GetRandomMap(mutUsage));
		UOSInt i;
		Data::DateTime dt;
		dt.SetCurrTime();
		sptr = IO::Path::GetProcessFileName(sbuff).Or(sbuff);
		i = Text::StrLastIndexOfCharC(sbuff, (UOSInt)(sptr - sbuff), IO::Path::PATH_SEPERATOR);
		sptr = &sbuff[i + 1];
		sptr = Text::StrConcatC(sptr, UTF8STRC("bt"));
		sptr = dt.ToString(sptr, "yyyyMMddHHmmss");
		sptr = Text::StrConcatC(sptr, UTF8STRC(".txt"));
		if (btLog.StoreFile(CSTRP(sbuff, sptr)))
		{
			UnsafeArray<const UTF8Char> lastFileName;
			if (this->lastFileName.SetTo(lastFileName))
			{
				IO::Path::DeleteFile(lastFileName);
				Text::StrDelNew(lastFileName);
			}
			this->lastFileName = Text::StrCopyNewC(sbuff, (UOSInt)(sptr - sbuff));
		}
	}
}

NN<const Data::ReadingListNN<IO::BTScanLog::ScanRecord3>> IO::BTCapturer::GetPublicList(NN<Sync::MutexUsage> mutUsage) const
{
	return this->bt.OrNull()->GetPublicMap(mutUsage);
}

NN<const Data::ReadingListNN<IO::BTScanLog::ScanRecord3>> IO::BTCapturer::GetRandomList(NN<Sync::MutexUsage> mutUsage) const
{
	return this->bt.OrNull()->GetRandomMap(mutUsage);
}

void IO::BTCapturer::SetUpdateHandler(IO::BTScanner::RecordHandler hdlr, void *userObj)
{
	NN<IO::BTScanner> bt;
	if (this->bt.SetTo(bt))
		bt->HandleRecordUpdate(hdlr, userObj);
}
