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
	Net::WebServer::CaptuererWebHandler *webHdlr;
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
	NEW_CLASS(capturer, Net::WiFiCapturer());
	if (capturer->IsError())
	{
		console.WriteLine((const UTF8Char*)"Error in initializing WiFi");
	}
	else
	{
		Text::StringBuilderUTF8 sb;
		NEW_CLASS(sockf, Net::OSSocketFactory(true));
		NEW_CLASS(webHdlr, Net::WebServer::CaptuererWebHandler(capturer, 0, 0));
		NEW_CLASS(listener, Net::WebServer::WebListener(sockf, webHdlr, webPort, 120, 4, (const UTF8Char*)"WiFiCapture/1.0", false, true));
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
				console.WriteLine((const UTF8Char*)"No WiFi interface found");
			}
			else
			{
				console.WriteLine((const UTF8Char*)"WiFiCapture started");
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
