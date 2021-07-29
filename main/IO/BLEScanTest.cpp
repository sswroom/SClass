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

#include "IO/BTCapturer.h"
#include "IO/ConsoleWriter.h"
#include "Net/OSSocketFactory.h"
#include "Net/WebServer/CapturerWebHandler.h"
#include "Net/WebServer/WebListener.h"
#include "Text/StringBuilderUTF8.h"

#include <stdio.h>

Int32 MyMain(Core::IProgControl *progCtrl)
{
	IO::ConsoleWriter console;
	IO::RadioSignalLogger *radioLogger;
	Net::SocketFactory *sockf;
	Net::WebServer::CapturerWebHandler *webHdlr;
	Net::WebServer::WebListener *listener;
	UInt16 webPort = 8081;
	IO::BTCapturer *capturer;
	NEW_CLASS(capturer, IO::BTCapturer());
	if (capturer->IsError())
	{
		console.WriteLine((const UTF8Char*)"Error in initializing Bluetooth");
	}
	else
	{
		Text::StringBuilderUTF8 sb;
		NEW_CLASS(radioLogger, IO::RadioSignalLogger());
		radioLogger->CaptureBT(capturer);
		NEW_CLASS(sockf, Net::OSSocketFactory(true));
		NEW_CLASS(webHdlr, Net::WebServer::CapturerWebHandler(0, capturer, radioLogger));
		NEW_CLASS(listener, Net::WebServer::WebListener(sockf, 0, webHdlr, webPort, 120, 4, (const UTF8Char*)"BLEScanTest/1.0", false, true));
		if (listener->IsError())
		{
			sb.Append((const UTF8Char*)"Error in starting web server at port ");
			sb.AppendI32(webPort);
			console.WriteLine(sb.ToString());
		}
		else
		{
			if (!capturer->Start())
			{
				console.WriteLine((const UTF8Char*)"No BT interface found");
			}
			else
			{
				console.WriteLine((const UTF8Char*)"BLEScanTest started");
				progCtrl->WaitForExit(progCtrl);
				capturer->StoreStatus();
				capturer->Stop();
			}
		}
		DEL_CLASS(listener);
		DEL_CLASS(webHdlr);
		DEL_CLASS(sockf);
		DEL_CLASS(radioLogger);
	}

	DEL_CLASS(capturer);
	return 0;
}
