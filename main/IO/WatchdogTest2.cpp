#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "IO/FileStream.h"
#include "IO/IOTool.h"
#include "IO/Watchdog.h"
#include "IO/Device/AM2315.h"
#include "Net/HTTPClient.h"
#include "Net/OSSocketFactory.h"
#include "Net/SSLEngine.h"
#include "Net/SSLEngineFactory.h"
#include "Sync/SimpleThread.h"
#include "Sync/ThreadUtil.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"

#define TESTURL "http://abc.com/kareq"
#define USERAGENT "HTTPClient/1.0"

Optional<IO::Watchdog> wd;
Bool running;
Bool httpRunning;
Bool toStop;
Sync::Event *evt;
Sync::Event *httpEvt;
IO::Device::AM2315 *am2315;
NN<Net::SocketFactory> sockf;
NN<Net::TCPClientFactory> clif;
Optional<Net::SSLEngine> ssl;
IO::ConsoleWriter *consoleWriter;

UInt32 __stdcall WatchdogThread(AnyType userObj)
{
	Int32 i = 15;
	Single tempVal;
	Single rhVal;
	NN<IO::Watchdog> nnwd;
	if (wd.SetTo(nnwd))
	{
		Text::StringBuilderUTF8 sb;
		running = true;
		while (!toStop)
		{
			if (i-- > 0)
			{
				consoleWriter->WriteLine(CSTR("Keep Alive"));
				nnwd->Keepalive();
			}
			if (am2315->ReadTemperature(tempVal))
			{
				if (am2315->ReadRH(rhVal))
				{
					sb.ClearStr();
					sb.AppendC(UTF8STRC("AM2315: Temp = "));
					sb.AppendDouble(tempVal);
					sb.AppendC(UTF8STRC(", RH = "));
					sb.AppendDouble(rhVal);
					consoleWriter->WriteLine(sb.ToCString());
				}
				else
				{
					sb.ClearStr();
					sb.AppendC(UTF8STRC("AM2315: Temp = "));
					sb.AppendDouble(tempVal);
					sb.AppendC(UTF8STRC(", RH = error"));
					consoleWriter->WriteLine(sb.ToCString());
				}
			}
			else
			{
				consoleWriter->WriteLine(CSTR("Fail in reading from AM2315"));
			}
			
			evt->Wait(2000);
		}
	}
	running = false;
	return 0;
}

UInt32 __stdcall HTTPThread(AnyType userObj)
{
	NN<Net::HTTPClient> cli;
	{
		Text::StringBuilderUTF8 sb;
		httpRunning = true;
		while (!toStop)
		{
			consoleWriter->Write(CSTR("Requesting to "));
			consoleWriter->WriteLine(CSTR(TESTURL));
			cli = Net::HTTPClient::CreateClient(clif, ssl, CSTR(USERAGENT), false, Text::StrStartsWithC(UTF8STRC(TESTURL), UTF8STRC("https://")));
			cli->Connect(CSTR(TESTURL), Net::WebUtil::RequestMethod::HTTP_GET, 0, 0, false);
			cli->AddHeaderC(CSTR("User-Agent"), CSTR(USERAGENT));
			cli->AddHeaderC(CSTR("Accept"), CSTR("*/*"));
			cli->AddHeaderC(CSTR("Accept-Charset"), CSTR("*"));
			cli->AddHeaderC(CSTR("Connection"), CSTR("close"));
			
			if (cli->IsError())
			{
				consoleWriter->WriteLine(CSTR("Error in requesting to server"));
			}
			else
			{
				cli->EndRequest(0, 0);
				Int32 respCode = cli->GetRespStatus();
				sb.ClearStr();
				sb.AppendC(UTF8STRC("Resp Code = "));
				sb.AppendI32(respCode);
				consoleWriter->WriteLine(sb.ToCString());
			}
			cli.Delete();
			httpEvt->Wait(1000);
		}
	}
	httpRunning = false;
	return 0;
}

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
	NN<IO::Watchdog> nnwd;
	IO::ConsoleWriter console;
	IO::IOTool::EchoFile(CSTR("/sys/class/gpio/export"), CSTR("71"));
	IO::IOTool::EchoFile(CSTR("/sys/class/gpio/gpio71/direction"), CSTR("out"));
	IO::IOTool::EchoFile(CSTR("/sys/class/gpio/gpio71/value"), CSTR("1"));
	Sync::SimpleThread::Sleep(3000);
	consoleWriter = &console;

	running = false;
	httpRunning = false;
	toStop = false;
	NN<IO::I2CChannel> channel;
	if (!IO::Device::AM2315::CreateDefChannel(1).SetTo(channel))
	{
		console.WriteLine(CSTR("I2C Bus not found"));
		return 0;
	}
	NEW_CLASS(am2315, IO::Device::AM2315(channel, true));
	if (am2315->IsError())
	{
		DEL_CLASS(am2315);
		am2315 = 0;
		console.WriteLine(CSTR("AM2315 not found"));
		return 0;
	}

	NEW_CLASS(evt, Sync::Event(true));
	NEW_CLASS(httpEvt, Sync::Event(true));
	NEW_CLASSNN(sockf, Net::OSSocketFactory(false));
	NEW_CLASSNN(clif, Net::TCPClientFactory(sockf));
	ssl = Net::SSLEngineFactory::Create(clif, true);

	wd = IO::Watchdog::Create(1);
	if (wd.SetTo(nnwd) && nnwd->IsError())
	{
		wd.Delete();
	}
	
	if (!wd.SetTo(nnwd))
	{
		console.WriteLine(CSTR("Watchdog not found"));
	}
	else
	{
		Text::StringBuilderUTF8 sb;
		Int32 timeoutSec;
		nnwd->SetTimeoutSec(15);
		if (nnwd->GetTimeoutSec(timeoutSec))
		{
			sb.ClearStr();
			sb.AppendC(UTF8STRC("Timeout = "));
			sb.AppendI32(timeoutSec);
			console.WriteLine(sb.ToCString());
		}
		else
		{
			console.WriteLine(CSTR("Error in getting timeout value"));
		}

		if (nnwd->Enable())
		{
			console.WriteLine(CSTR("Watchdog enabled"));
			Sync::ThreadUtil::Create(WatchdogThread, 0);
			Sync::ThreadUtil::Create(HTTPThread, 0);
			while (!running)
			{
				Sync::SimpleThread::Sleep(10);
			}
			console.WriteLine(CSTR("Running"));
			progCtrl->WaitForExit(progCtrl);
			console.WriteLine(CSTR("Exiting"));
			toStop = true;
			evt->Set();
			httpEvt->Set();
			while (running || httpRunning)
			{
				Sync::SimpleThread::Sleep(10);
			}
		}
		else
		{
			console.WriteLine(CSTR("Error in enabling watchdog"));
		}
		nnwd.Delete();
	}
	DEL_CLASS(evt);
	DEL_CLASS(httpEvt);
	ssl.Delete();
	clif.Delete();
	sockf.Delete();
	DEL_CLASS(am2315);
	return 0;
}

