#include "Stdafx.h"
#include "Core/Core.h"
#include "IO/ATCommandChannel.h"
#include "IO/BTCapturer.h"
#include "IO/ConsoleWriter.h"
#include "IO/GSMCellCapturer.h"
#include "IO/IniFile.h"
#include "IO/Path.h"
#include "IO/RadioSignalLogger.h"
#include "IO/SerialPort.h"
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
		Bool failed = false;
		Bool enableWiFi = true;
		Bool enableBT = true;
		UOSInt gsmPort = 0;
		NN<IO::ConfigFile> cfg;
		NN<Text::String> s;
		if (IO::IniFile::ParseProgConfig(65001).SetTo(cfg))
		{
			if (cfg->GetValue(CSTR("EnableWiFi")).SetTo(s))
			{
				enableWiFi = s->ToInt32() != 0;
			}
			if (cfg->GetValue(CSTR("EnableBT")).SetTo(s))
			{
				enableBT = s->ToInt32() != 0;
			}
			if (cfg->GetValue(CSTR("GSMPort")).SetTo(s))
			{
				gsmPort = s->ToUOSInt();
			}
			cfg.Delete();
		}
		Optional<Net::WiFiCapturer> wifiCapturer = nullptr;
		Optional<IO::BTCapturer> btCapturer = nullptr;
		Optional<IO::GSMCellCapturer> gsmCapturer = nullptr;
		NN<Net::WiFiCapturer> nnwifiCapturer;
		NN<IO::BTCapturer> nnbtCapturer;
		NN<IO::GSMCellCapturer> nngsmCapturer;
		if (enableWiFi)
		{
			NEW_CLASSNN(nnwifiCapturer, Net::WiFiCapturer());
			if (nnwifiCapturer->IsError())
			{
				console.WriteLine(CSTR("Error in initializing WiFi"));
				nnwifiCapturer.Delete();
				failed = true;
			}
			else
			{
				wifiCapturer = nnwifiCapturer;
			}
		}
		if (enableBT)
		{
			NEW_CLASSNN(nnbtCapturer, IO::BTCapturer(true));
			if (nnbtCapturer->IsError())
			{
				console.WriteLine(CSTR("Error in initializing Bluetooth"));
				nnbtCapturer.Delete();
				failed = true;
			}
			else
			{
				btCapturer = nnbtCapturer;
			}
		}
		if (gsmPort != 0)
		{
			NN<IO::ATCommandChannel> channel;
			NN<IO::SerialPort> port;
			NEW_CLASSNN(port, IO::SerialPort(gsmPort, 115200, IO::SerialPort::PARITY_NONE, false));
			if (port->IsError())
			{
				console.WriteLine(CSTR("Error in opening gsm serial port"));
				port.Delete();
				failed = true;
			}
			else
			{
				NEW_CLASSNN(channel, IO::ATCommandChannel(port, true));
				NEW_CLASSNN(nngsmCapturer, IO::GSMCellCapturer(channel, true));
				if (nngsmCapturer->IsError())
				{
					console.WriteLine(CSTR("Error in opening gsm serial port"));
					nngsmCapturer.Delete();
					failed = true;
				}
				else
				{
					gsmCapturer = nngsmCapturer;
				}
			}
		}
		if (!failed)
		{
			Text::StringBuilderUTF8 sb;
			IO::RadioSignalLogger radioLogger;
			if (btCapturer.SetTo(nnbtCapturer))
			{
				radioLogger.CaptureBT(nnbtCapturer);
			}
			if (wifiCapturer.SetTo(nnwifiCapturer))
			{
				radioLogger.CaptureWiFi(nnwifiCapturer);
			}
			if (gsmCapturer.SetTo(nngsmCapturer))
			{
				radioLogger.CaptureGSM(nngsmCapturer);
			}
			Net::OSSocketFactory sockf(true);
			Net::TCPClientFactory clif(sockf);
			{
				Net::WebServer::CapturerWebHandler webHdlr(wifiCapturer, btCapturer, radioLogger);
				webHdlr.SetGSMCapturer(gsmCapturer);
				Net::WebServer::WebListener listener(clif, nullptr, webHdlr, webPort, 120, 1, 4, CSTR("WiFiCapture/1.0"), false, Net::WebServer::KeepAlive::Default, true);
				if (listener.IsError())
				{
					sb.AppendC(UTF8STRC("Error in starting web server at port "));
					sb.AppendI32(webPort);
					console.WriteLine(sb.ToCString());
				}
				else
				{
					if (wifiCapturer.SetTo(nnwifiCapturer) && !nnwifiCapturer->Start())
					{
						console.WriteLine(CSTR("No WiFi interface found"));
					}
					else if (btCapturer.SetTo(nnbtCapturer) && !nnbtCapturer->Start())
					{
						console.WriteLine(CSTR("No bluetooth interface found"));
					}
					else if (gsmCapturer.SetTo(nngsmCapturer) && !nngsmCapturer->Start())
					{
						console.WriteLine(CSTR("Error in starting GSM Capturer"));
					}
					else
					{
						console.WriteLine(CSTR("RadioCapture started"));
						progCtrl->WaitForExit(progCtrl);
						if (wifiCapturer.SetTo(nnwifiCapturer))
						{
							nnwifiCapturer->StoreStatus();
							nnwifiCapturer->Stop();
						}
						if (btCapturer.SetTo(nnbtCapturer))
						{
							nnbtCapturer->StoreStatus();
							nnbtCapturer->Stop();
						}
						if (gsmCapturer.SetTo(nngsmCapturer))
						{
							nngsmCapturer->Stop();
						}
					}
				}
			}
		}
		wifiCapturer.Delete();
		btCapturer.Delete();
		gsmCapturer.Delete();
	}
	return 0;
}
