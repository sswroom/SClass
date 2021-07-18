#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
/*#include "Data/ByteTool.h"
#include "IO/BTManager.h"
#include "IO/BTUtil.h"
#include "IO/ConsoleWriter.h"
#include "Net/MACInfo.h"
#include "Sync/Thread.h"
#include "Text/StringBuilderUTF8.h"

IO::ConsoleWriter *console;

void __stdcall OnScanResult(void *userObj, UInt64 mac, Int32 rssi, const Char *name)
{
	UInt8 buff[8];
	WriteMUInt64(buff, mac);
	Text::StringBuilderUTF8 sb;
	sb.AppendHexBuff(&buff[2], 6, ':', Text::LBT_NONE);
	sb.Append((const UTF8Char*)" RSSI=");
	sb.AppendI32(rssi);
	sb.Append((const UTF8Char*)", name=");
	if (name)
	{
		sb.Append((const UTF8Char*)name);
	}
	sb.Append((const UTF8Char*)", vendor=");
	sb.Append((const UTF8Char*)Net::MACInfo::GetMACInfo(mac)->name);
	console->WriteLine(sb.ToString());
}

Int32 MyMain(Core::IProgControl *progCtrl)
{
	IO::BTManager manager;
	IO::BTController *ctrl;
	Data::ArrayList<IO::BTController*> ctrlList;
	NEW_CLASS(console, IO::ConsoleWriter());
	manager.CreateControllers(&ctrlList);
	UOSInt i;
	Text::StringBuilderUTF8 sb;
	i = ctrlList.GetCount();
	while (i-- > 0)
	{
		ctrl = ctrlList.GetItem(i);
		sb.ClearStr();
		sb.Append((const UTF8Char*)"Using ");
		sb.Append(ctrl->GetName());
		sb.Append((const UTF8Char*)" (");
		sb.Append(IO::BTUtil::GetManufacturerName(ctrl->GetManufacturer()));
		sb.Append((const UTF8Char*)")");
		console->WriteLine(sb.ToString());

		ctrl->LEScanHandleResult(OnScanResult, 0);
		if (ctrl->LEScanBegin())
		{
			console->WriteLine((const UTF8Char*)"Started scanning");
			Sync::Thread::Sleep(10000);
			ctrl->LEScanEnd();
		}
		DEL_CLASS(ctrl);
	}
	DEL_CLASS(console);
	return 0;
}*/

#include "IO/BTLog.h"
#include "IO/Path.h"
#include "IO/ProgCtrl/BluetoothCtlProgCtrl.h"
#include "Sync/Thread.h"
#include "Text/StringBuilderUTF8.h"

#include <stdio.h>

const UTF8Char *lastFileName;
Bool threadRunning;
Bool threadToStop;
Sync::Event *threadEvt;

void StoreFile(IO::ProgCtrl::BluetoothCtlProgCtrl *bt)
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	IO::BTLog btLog;
	Sync::MutexUsage mutUsage;
	Data::UInt64Map<IO::ProgCtrl::BluetoothCtlProgCtrl::DeviceInfo*> *devMap = bt->GetDeviceMap(&mutUsage);
	btLog.AppendList(devMap);
	Data::DateTime dt;
	dt.SetCurrTime();
	sptr = IO::Path::GetProcessFileName(sbuff);
	sptr = IO::Path::AppendPath(sbuff, (const UTF8Char*)"bt");
	sptr = Text::StrInt64(sptr, dt.ToTicks());
	sptr = Text::StrConcat(sptr, (const UTF8Char*)".txt");
	if (btLog.Store(sbuff))
	{
		if (lastFileName)
		{
			IO::Path::DeleteFile(lastFileName);
			Text::StrDelNew(lastFileName);
		}
		lastFileName = Text::StrCopyNew(sbuff);
		printf("Store as %s\r\n", sbuff);
	}
}

UInt32 __stdcall TimerThread(void *userObj)
{
	Data::DateTime *dt;
	Int64 lastTime;
	threadRunning = true;
	NEW_CLASS(dt, Data::DateTime());
	dt->SetCurrTimeUTC();
	lastTime = dt->ToTicks();
	while (!threadToStop)
	{
		dt->SetCurrTimeUTC();
		if ((dt->ToTicks() - lastTime) >= 300000)
		{
			lastTime = dt->ToTicks();
			StoreFile((IO::ProgCtrl::BluetoothCtlProgCtrl*)userObj);
		}
		threadEvt->Wait(10000);
	}
	DEL_CLASS(dt);
	threadRunning = false;
	return 0;
}

Int32 MyMain(Core::IProgControl *progCtrl)
{
	lastFileName = 0;
	IO::ProgCtrl::BluetoothCtlProgCtrl bt;
	printf("Waiting...\r\n");
	if (bt.WaitForCmdReady())
	{
		threadRunning = false;
		threadToStop = false;
		NEW_CLASS(threadEvt, Sync::Event(true, (const UTF8Char*)"threadEvt"));
		Sync::Thread::Create(TimerThread, &bt);

		printf("Scan On...\r\n");
		bt.ScanOn();
		progCtrl->WaitForExit(progCtrl);

		StoreFile(&bt);
		threadToStop = true;
		threadEvt->Set();
		while (threadRunning)
		{
			Sync::Thread::Sleep(10);
		}
		DEL_CLASS(threadEvt);

		printf("Scan Off...\r\n");
		bt.ScanOff();
	}
	else
	{
		printf("Bluetooth daemon is not running\r\n");
	}

	printf("Closing...\r\n");
	bt.Exit();

	SDEL_TEXT(lastFileName);
	return 0;
}
