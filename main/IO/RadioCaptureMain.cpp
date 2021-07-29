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

Int32 MyMain(Core::IProgControl *progCtrl)
{
	Net::WiFiCapturer *wifiCapturer;
	IO::BTCapturer *btCapturer;
	IO::RadioSignalLogger *radioLogger;
	IO::ConsoleWriter console;
	Net::SocketFactory *sockf;
	Net::WebServer::CapturerWebHandler *webHdlr;
	Net::WebServer::WebListener *listener;
	UInt16 webPort = 8080;
	Manage::ExceptionRecorder *exHdlr;
	UTF8Char sbuff[512];
	
	UOSInt argc;
	UTF8Char **argv = progCtrl->GetCommandLines(progCtrl, &argc);
	if (argc >= 2)
	{
		Text::StrToUInt16(argv[1], &webPort);
	}
	IO::Path::GetProcessFileName(sbuff);
	IO::Path::AppendPath(sbuff, (const UTF8Char*)"Error.txt");
	NEW_CLASS(exHdlr, Manage::ExceptionRecorder(sbuff, Manage::ExceptionRecorder::EA_RESTART));
	NEW_CLASS(wifiCapturer, Net::WiFiCapturer());
	NEW_CLASS(btCapturer, IO::BTCapturer());
	if (wifiCapturer->IsError())
	{
		console.WriteLine((const UTF8Char*)"Error in initializing WiFi");
	}
	else if (btCapturer->IsError())
	{
		console.WriteLine((const UTF8Char*)"Error in initializing Bluetooth");
	}
	else
	{
		Text::StringBuilderUTF8 sb;
		NEW_CLASS(radioLogger, IO::RadioSignalLogger());
		radioLogger->CaptureBT(btCapturer);
		radioLogger->CaptureWiFi(wifiCapturer);
		NEW_CLASS(sockf, Net::OSSocketFactory(true));
		NEW_CLASS(webHdlr, Net::WebServer::CapturerWebHandler(wifiCapturer, btCapturer, radioLogger));
		NEW_CLASS(listener, Net::WebServer::WebListener(sockf, 0, webHdlr, webPort, 120, 4, (const UTF8Char*)"WiFiCapture/1.0", false, true));
		if (listener->IsError())
		{
			sb.Append((const UTF8Char*)"Error in starting web server at port ");
			sb.AppendI32(webPort);
			console.WriteLine(sb.ToString());
		}
		else
		{
			if (!wifiCapturer->Start())
			{
				console.WriteLine((const UTF8Char*)"No WiFi interface found");
			}
			else if (!btCapturer->Start())
			{
				console.WriteLine((const UTF8Char*)"No bluetooth interface found");
			}
			else
			{
				console.WriteLine((const UTF8Char*)"RadioCapture started");
				progCtrl->WaitForExit(progCtrl);
				wifiCapturer->StoreStatus();
				btCapturer->StoreStatus();
				wifiCapturer->Stop();
				btCapturer->Stop();
			}
		}
		DEL_CLASS(listener);
		DEL_CLASS(webHdlr);
		DEL_CLASS(sockf);
		DEL_CLASS(radioLogger);
	}

	DEL_CLASS(wifiCapturer);
	DEL_CLASS(btCapturer);
	DEL_CLASS(exHdlr);
	return 0;
}
