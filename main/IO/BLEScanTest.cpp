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

#include "IO/ProgCtrl/BluetoothCtlProgCtrl.h"
#include "Text/StringBuilderUTF8.h"

#include <stdio.h>

Int32 MyMain(Core::IProgControl *progCtrl)
{
	IO::ProgCtrl::BluetoothCtlProgCtrl bt;
	printf("Waiting...\r\n");
	if (bt.WaitForCmdReady())
	{
		printf("Scan On...\r\n");
		bt.ScanOn();
		progCtrl->WaitForExit(progCtrl);
		printf("Scan Off...\r\n");
		bt.ScanOff();
	}
	else
	{
		printf("Bluetooth daemon is not running\r\n");
	}
	printf("Closing...\r\n");
	bt.Exit();

	printf("\r\nDetected devices:\r\n");
	Sync::MutexUsage mutUsage;
	Data::UInt64Map<IO::ProgCtrl::BluetoothCtlProgCtrl::DeviceInfo*> *devMap = bt.GetDeviceMap(&mutUsage);
	Data::ArrayList<IO::ProgCtrl::BluetoothCtlProgCtrl::DeviceInfo*> *devList = devMap->GetValues();
	IO::ProgCtrl::BluetoothCtlProgCtrl::DeviceInfo *dev;
	UOSInt i = 0;
	UOSInt j = devList->GetCount();
	Text::StringBuilderUTF8 sb;
	while (i < j)
	{
		dev = devList->GetItem(i);
		sb.ClearStr();
		sb.AppendHexBuff(dev->mac, 6, ':', Text::LBT_NONE);
		sb.AppendChar(' ', 1);
		if (dev->name)
		{
			sb.Append(dev->name);
		}
		if (sb.GetLength() < 50)
		{
			sb.AppendChar(' ', 50 - sb.GetLength());
		}
		else
		{
			sb.AppendChar(' ', 1);
		}
		sb.AppendI32(dev->rssi);
		if (sb.GetLength() < 56)
		{
			sb.AppendChar(' ', 56 - sb.GetLength());
		}
		else
		{
			sb.AppendChar(' ', 1);
		}
		sb.AppendI32(dev->txPower);
		if (sb.GetLength() < 61)
		{
			sb.AppendChar(' ', 61 - sb.GetLength());
		}
		else
		{
			sb.AppendChar(' ', 1);
		}
		sb.Append((const UTF8Char*)(dev->connected?"y":"n"));
		printf("%s\r\n", sb.ToString());
		i++;
	}
	return 0;
}
