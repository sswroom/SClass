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
	UTF8Char *sptr;
	
	UOSInt argc;
	UTF8Char **argv = progCtrl->GetCommandLines(progCtrl, &argc);
	if (argc >= 2)
	{
		Text::StrToUInt16(argv[1], &webPort);
	}
	sptr = IO::Path::GetProcessFileName(sbuff);
	sptr = IO::Path::AppendPathC(sbuff, sptr, UTF8STRC("Error.txt"));
	NEW_CLASS(exHdlr, Manage::ExceptionRecorder(CSTRP(sbuff, sptr), Manage::ExceptionRecorder::EA_RESTART));
	NEW_CLASS(wifiCapturer, Net::WiFiCapturer());
	NEW_CLASS(btCapturer, IO::BTCapturer(true));
	if (wifiCapturer->IsError())
	{
		console.WriteLineC(UTF8STRC("Error in initializing WiFi"));
	}
	else if (btCapturer->IsError())
	{
		console.WriteLineC(UTF8STRC("Error in initializing Bluetooth"));
	}
	else
	{
		Text::StringBuilderUTF8 sb;
		NEW_CLASS(radioLogger, IO::RadioSignalLogger());
		radioLogger->CaptureBT(btCapturer);
		radioLogger->CaptureWiFi(wifiCapturer);
		NEW_CLASS(sockf, Net::OSSocketFactory(true));
		NEW_CLASS(webHdlr, Net::WebServer::CapturerWebHandler(wifiCapturer, btCapturer, radioLogger));
		NEW_CLASS(listener, Net::WebServer::WebListener(sockf, 0, webHdlr, webPort, 120, 4, CSTR("WiFiCapture/1.0"), false, true));
		if (listener->IsError())
		{
			sb.AppendC(UTF8STRC("Error in starting web server at port "));
			sb.AppendI32(webPort);
			console.WriteLineC(sb.ToString(), sb.GetLength());
		}
		else
		{
			if (!wifiCapturer->Start())
			{
				console.WriteLineC(UTF8STRC("No WiFi interface found"));
			}
			else if (!btCapturer->Start())
			{
				console.WriteLineC(UTF8STRC("No bluetooth interface found"));
			}
			else
			{
				console.WriteLineC(UTF8STRC("RadioCapture started"));
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
