#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
/*#include "Core/ByteTool_C.h"
#include "IO/BTManager.h"
#include "IO/BTUtil.h"
#include "IO/ConsoleWriter.h"
#include "Net/MACInfo.h"
#include "Sync/ThreadUtil.h"
#include "Text/StringBuilderUTF8.h"

NN<IO::ConsoleWriter> console;

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
	console->WriteLine(sb.ToCString());
}

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
	IO::BTManager manager;
	NN<IO::BTController> ctrl;
	Data::ArrayListNN<IO::BTController> ctrlList;
	NEW_CLASSNN(console, IO::ConsoleWriter());
	manager.CreateControllers(ctrlList);
	UIntOS i;
	Text::StringBuilderUTF8 sb;
	i = ctrlList.GetCount();
	while (i-- > 0)
	{
		ctrl = ctrlList.GetItemNoCheck(i);
		sb.ClearStr();
		sb.AppendC(UTF8STRC("Using "));
		sb.Append(ctrl->GetName());
		sb.AppendC(UTF8STRC(" ("));
		sb.Append(IO::BTUtil::GetManufacturerName(ctrl->GetManufacturer()));
		sb.AppendC(UTF8STRC(")"));
		console->WriteLine(sb.ToCString());

		ctrl->LEScanHandleResult(OnScanResult, 0);
		if (ctrl->LEScanBegin())
		{
			console->WriteLine(CSTR("Started scanning"));
			Sync::SimpleThread::Sleep(10000);
			ctrl->LEScanEnd();
		}
		ctrl.Delete();
	}
	console.Delete();
	return 0;
}*/

#include "IO/BTCapturer.h"
#include "IO/ConsoleWriter.h"
#include "Net/OSSocketFactory.h"
#include "Net/WebServer/CapturerWebHandler.h"
#include "Net/WebServer/WebListener.h"
#include "Text/StringBuilderUTF8.h"

#include <stdio.h>

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
	IO::ConsoleWriter console;
	NN<IO::RadioSignalLogger> radioLogger;
	NN<Net::WebServer::WebListener> listener;
	UInt16 webPort = 8081;
	NN<IO::BTCapturer> capturer;
	NEW_CLASSNN(capturer, IO::BTCapturer(true));
	if (capturer->IsError())
	{
		console.WriteLine(CSTR("Error in initializing Bluetooth"));
	}
	else
	{
		Text::StringBuilderUTF8 sb;
		NEW_CLASSNN(radioLogger, IO::RadioSignalLogger());
		radioLogger->CaptureBT(capturer);
		NN<Net::WebServer::CapturerWebHandler> webHdlr;
		Net::OSSocketFactory sockf(true);
		Net::TCPClientFactory clif(sockf);
		NEW_CLASSNN(webHdlr, Net::WebServer::CapturerWebHandler(nullptr, capturer, radioLogger));
		NEW_CLASSNN(listener, Net::WebServer::WebListener(clif, nullptr, webHdlr, webPort, 120, 1, 4, CSTR("BLEScanTest/1.0"), false, Net::WebServer::KeepAlive::Default, false));
		if (listener->IsError())
		{
			sb.AppendC(UTF8STRC("Error in starting web server at port "));
			sb.AppendI32(webPort);
			console.WriteLine(sb.ToCString());
		}
		else
		{
			if (!capturer->Start())
			{
				console.WriteLine(CSTR("No BT interface found"));
			}
			else
			{
				console.WriteLine(CSTR("BLEScanTest started"));
				progCtrl->WaitForExit(progCtrl);
				capturer->StoreStatus();
				capturer->Stop();
			}
		}
		listener.Delete();
		webHdlr.Delete();
		radioLogger.Delete();
	}

	capturer.Delete();
	return 0;
}
