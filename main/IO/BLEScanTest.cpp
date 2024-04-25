#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
/*#include "Data/ByteTool.h"
#include "IO/BTManager.h"
#include "IO/BTUtil.h"
#include "IO/ConsoleWriter.h"
#include "Net/MACInfo.h"
#include "Sync/ThreadUtil.h"
#include "Text/StringBuilderUTF8.h"

IO::ConsoleWriter *console;

void __stdcall OnScanResult(void *userObj, UInt64 mac, Int32 rssi, const Char *name)
{
	UInt8 buff[8];
	WriteMUInt64(buff, mac);
	Text::StringBuilderUTF8 sb;
	sb.AppendHexBuff(&buff[2], 6, ':', Text::LineBreakType::None);
	sb.AppendC(UTF8STRC(" RSSI="));
	sb.AppendI32(rssi);
	sb.AppendC(UTF8STRC(", name="));
	if (name)
	{
		sb.Append((const UTF8Char*)name);
	}
	sb.AppendC(UTF8STRC(", vendor="));
	sb.Append((const UTF8Char*)Net::MACInfo::GetMACInfo(mac)->name);
	console->WriteLineC(sb.ToString(), sb.GetLength());
}

Int32 MyMain(NN<Core::IProgControl> progCtrl)
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
		sb.AppendC(UTF8STRC("Using "));
		sb.Append(ctrl->GetName());
		sb.AppendC(UTF8STRC(" ("));
		sb.Append(IO::BTUtil::GetManufacturerName(ctrl->GetManufacturer()));
		sb.AppendC(UTF8STRC(")"));
		console->WriteLineC(sb.ToString(), sb.GetLength());

		ctrl->LEScanHandleResult(OnScanResult, 0);
		if (ctrl->LEScanBegin())
		{
			console->WriteLineC(UTF8STRC("Started scanning");
			Sync::SimpleThread::Sleep(10000);
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

Int32 MyMain(NN<Core::IProgControl> progCtrl)
{
	IO::ConsoleWriter console;
	IO::RadioSignalLogger *radioLogger;
	Net::WebServer::WebListener *listener;
	UInt16 webPort = 8081;
	IO::BTCapturer *capturer;
	NEW_CLASS(capturer, IO::BTCapturer(true));
	if (capturer->IsError())
	{
		console.WriteLineC(UTF8STRC("Error in initializing Bluetooth"));
	}
	else
	{
		Text::StringBuilderUTF8 sb;
		NEW_CLASS(radioLogger, IO::RadioSignalLogger());
		radioLogger->CaptureBT(capturer);
		NN<Net::WebServer::CapturerWebHandler> webHdlr;
		Net::OSSocketFactory sockf(true);
		NEW_CLASSNN(webHdlr, Net::WebServer::CapturerWebHandler(0, capturer, radioLogger));
		NEW_CLASS(listener, Net::WebServer::WebListener(sockf, 0, webHdlr, webPort, 120, 1, 4, CSTR("BLEScanTest/1.0"), false, Net::WebServer::KeepAlive::Default, false));
		if (listener->IsError())
		{
			sb.AppendC(UTF8STRC("Error in starting web server at port "));
			sb.AppendI32(webPort);
			console.WriteLineC(sb.ToString(), sb.GetLength());
		}
		else
		{
			if (!capturer->Start())
			{
				console.WriteLineC(UTF8STRC("No BT interface found"));
			}
			else
			{
				console.WriteLineC(UTF8STRC("BLEScanTest started"));
				progCtrl->WaitForExit(progCtrl);
				capturer->StoreStatus();
				capturer->Stop();
			}
		}
		DEL_CLASS(listener);
		webHdlr.Delete();
		DEL_CLASS(radioLogger);
	}

	DEL_CLASS(capturer);
	return 0;
}
