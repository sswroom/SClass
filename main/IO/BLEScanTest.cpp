#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "Data/ByteTool.h"
#include "IO/BTManager.h"
#include "IO/BTUtil.h"
#include "IO/ConsoleWriter.h"
#include "Net/MACInfo.h"
#include "Sync/Thread.h"
#include "Text/StringBuilderUTF8.h"

IO::ConsoleWriter *console;

void __stdcall OnScanResult(void *userObj, Int64 mac, Int32 rssi, const Char *name)
{
	UInt8 buff[8];
	WriteMInt64(buff, mac);
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
	OSInt i;
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
}
