#include "Stdafx.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "IO/Path.h"
#include "Manage/ExceptionRecorder.h"
#include "Net/OSSocketFactory.h"
#include "Net/WiFiCapturer.h"
#include "Net/WebServer/WebListener.h"
#include "Net/WebServer/CapturerWebHandler.h"

Net::WiFiCapturer *capturer;

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
	IO::ConsoleWriter console;
	Net::WebServer::WebListener *listener;
	UInt16 webPort = 8080;
	Manage::ExceptionRecorder *exHdlr;
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
	NEW_CLASS(exHdlr, Manage::ExceptionRecorder(CSTRP(sbuff, sptr), Manage::ExceptionRecorder::EA_RESTART));
	NEW_CLASS(capturer, Net::WiFiCapturer());
	if (capturer->IsError())
	{
		console.WriteLine(CSTR("Error in initializing WiFi"));
	}
	else
	{
		Text::StringBuilderUTF8 sb;
		Net::OSSocketFactory sockf(true);
		Net::TCPClientFactory clif(sockf);
		Net::WebServer::CapturerWebHandler webHdlr(capturer, nullptr, nullptr);
		NEW_CLASS(listener, Net::WebServer::WebListener(clif, nullptr, webHdlr, webPort, 120, 1, 4, CSTR("WiFiCapture/1.0"), false, Net::WebServer::KeepAlive::Default, true));
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
				console.WriteLine(CSTR("No WiFi interface found"));
			}
			else
			{
				console.WriteLine(CSTR("WiFiCapture started"));
				progCtrl->WaitForExit(progCtrl);
				capturer->StoreStatus();
				capturer->Stop();
			}
		}
		DEL_CLASS(listener);
	}

	DEL_CLASS(capturer);
	DEL_CLASS(exHdlr);
	return 0;
}
