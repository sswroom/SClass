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

Int32 MyMain(Core::IProgControl *progCtrl)
{
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
	sptr = IO::Path::AppendPath(sbuff, sptr, CSTR("Error.txt"));
	NEW_CLASS(exHdlr, Manage::ExceptionRecorder(CSTRP(sbuff, sptr), Manage::ExceptionRecorder::EA_RESTART));
	NEW_CLASS(capturer, Net::WiFiCapturer());
	if (capturer->IsError())
	{
		console.WriteLineC(UTF8STRC("Error in initializing WiFi"));
	}
	else
	{
		Text::StringBuilderUTF8 sb;
		NEW_CLASS(sockf, Net::OSSocketFactory(true));
		NEW_CLASS(webHdlr, Net::WebServer::CapturerWebHandler(capturer, 0, 0));
		NEW_CLASS(listener, Net::WebServer::WebListener(sockf, 0, webHdlr, webPort, 120, 4, CSTR("WiFiCapture/1.0"), false, true, true));
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
				console.WriteLineC(UTF8STRC("No WiFi interface found"));
			}
			else
			{
				console.WriteLineC(UTF8STRC("WiFiCapture started"));
				progCtrl->WaitForExit(progCtrl);
				capturer->StoreStatus();
				capturer->Stop();
			}
		}
		DEL_CLASS(listener);
		DEL_CLASS(webHdlr);
		DEL_CLASS(sockf);
	}

	DEL_CLASS(capturer);
	DEL_CLASS(exHdlr);
	return 0;
}
