#include "Stdafx.h"
#include "Core/Core.h"
#include "IO/BTCapturer.h"
#include "IO/ConsoleWriter.h"
#include "IO/Path.h"
#include "IO/RadioSignalLogger.h"
#include "Manage/ExceptionRecorder.h"
#include "Net/OSSocketFactory.h"
#include "Net/WiFiCapturer.h"
#include "Net/WebServer/WebListener.h"
#include "Net/WebServer/CapturerWebHandler.h"

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
	IO::ConsoleWriter console;
	UInt16 webPort = 8080;
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	
	UOSInt argc;
	UnsafeArray<UnsafeArray<UTF8Char>> argv = progCtrl->GetCommandLines(progCtrl, argc);
	if (argc >= 2)
	{
		Text::StrToUInt16(argv[1], webPort);
	}
	sptr = IO::Path::GetProcessFileName(sbuff).Or(sbuff);
	sptr = IO::Path::AppendPath(sbuff, sptr, CSTR("Error.txt"));
	Manage::ExceptionRecorder exHdlr(CSTRP(sbuff, sptr), Manage::ExceptionRecorder::EA_RESTART);
	{
		Net::WiFiCapturer wifiCapturer;
		IO::BTCapturer btCapturer(true);
		if (wifiCapturer.IsError())
		{
			console.WriteLine(CSTR("Error in initializing WiFi"));
		}
		else if (btCapturer.IsError())
		{
			console.WriteLine(CSTR("Error in initializing Bluetooth"));
		}
		else
		{
			Text::StringBuilderUTF8 sb;
			IO::RadioSignalLogger radioLogger;
			radioLogger.CaptureBT(btCapturer);
			radioLogger.CaptureWiFi(wifiCapturer);
			Net::OSSocketFactory sockf(true);
			Net::TCPClientFactory clif(sockf);
			{
				Net::WebServer::CapturerWebHandler webHdlr(&wifiCapturer, &btCapturer, &radioLogger);
				Net::WebServer::WebListener listener(clif, 0, webHdlr, webPort, 120, 1, 4, CSTR("WiFiCapture/1.0"), false, Net::WebServer::KeepAlive::Default, true);
				if (listener.IsError())
				{
					sb.AppendC(UTF8STRC("Error in starting web server at port "));
					sb.AppendI32(webPort);
					console.WriteLine(sb.ToCString());
				}
				else
				{
					if (!wifiCapturer.Start())
					{
						console.WriteLine(CSTR("No WiFi interface found"));
					}
					else if (!btCapturer.Start())
					{
						console.WriteLine(CSTR("No bluetooth interface found"));
					}
					else
					{
						console.WriteLine(CSTR("RadioCapture started"));
						progCtrl->WaitForExit(progCtrl);
						wifiCapturer.StoreStatus();
						btCapturer.StoreStatus();
						wifiCapturer.Stop();
						btCapturer.Stop();
					}
				}
			}
		}
	}
	return 0;
}
