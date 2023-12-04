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

IO::Watchdog *wd;
Bool running;
Bool httpRunning;
Bool toStop;
Sync::Event *evt;
Sync::Event *httpEvt;
IO::Device::AM2315 *am2315;
NotNullPtr<Net::SocketFactory> sockf;
Optional<Net::SSLEngine> ssl;
IO::ConsoleWriter *consoleWriter;

UInt32 __stdcall WatchdogThread(void *userObj)
{
	Int32 i = 15;
	Single tempVal;
	Single rhVal;
	{
		Text::StringBuilderUTF8 sb;
		running = true;
		while (!toStop)
		{
			if (i-- > 0)
			{
				consoleWriter->WriteLineC(UTF8STRC("Keep Alive"));
				wd->Keepalive();
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
					consoleWriter->WriteLineC(sb.ToString(), sb.GetLength());
				}
				else
				{
					sb.ClearStr();
					sb.AppendC(UTF8STRC("AM2315: Temp = "));
					sb.AppendDouble(tempVal);
					sb.AppendC(UTF8STRC(", RH = error"));
					consoleWriter->WriteLineC(sb.ToString(), sb.GetLength());
				}
			}
			else
			{
				consoleWriter->WriteLineC(UTF8STRC("Fail in reading from AM2315"));
			}
			
			evt->Wait(2000);
		}
	}
	running = false;
	return 0;
}

UInt32 __stdcall HTTPThread(void *userObj)
{
	NotNullPtr<Net::HTTPClient> cli;
	{
		Text::StringBuilderUTF8 sb;
		httpRunning = true;
		while (!toStop)
		{
			consoleWriter->WriteStrC(UTF8STRC("Requesting to "));
			consoleWriter->WriteLineC(UTF8STRC(TESTURL));
			cli = Net::HTTPClient::CreateClient(sockf, ssl, CSTR(USERAGENT), false, Text::StrStartsWithC(UTF8STRC(TESTURL), UTF8STRC("https://")));
			cli->Connect(CSTR(TESTURL), Net::WebUtil::RequestMethod::HTTP_GET, 0, 0, false);
			cli->AddHeaderC(CSTR("User-Agent"), CSTR(USERAGENT));
			cli->AddHeaderC(CSTR("Accept"), CSTR("*/*"));
			cli->AddHeaderC(CSTR("Accept-Charset"), CSTR("*"));
			cli->AddHeaderC(CSTR("Connection"), CSTR("close"));
			
			if (cli->IsError())
			{
				consoleWriter->WriteLineC(UTF8STRC("Error in requesting to server"));
			}
			else
			{
				cli->EndRequest(0, 0);
				Int32 respCode = cli->GetRespStatus();
				sb.ClearStr();
				sb.AppendC(UTF8STRC("Resp Code = "));
				sb.AppendI32(respCode);
				consoleWriter->WriteLineC(sb.ToString(), sb.GetLength());
			}
			cli.Delete();
			httpEvt->Wait(1000);
		}
	}
	httpRunning = false;
	return 0;
}

Int32 MyMain(NotNullPtr<Core::IProgControl> progCtrl)
{
	IO::ConsoleWriter console;
	IO::IOTool::EchoFile(CSTR("/sys/class/gpio/export"), CSTR("71"));
	IO::IOTool::EchoFile(CSTR("/sys/class/gpio/gpio71/direction"), CSTR("out"));
	IO::IOTool::EchoFile(CSTR("/sys/class/gpio/gpio71/value"), CSTR("1"));
	Sync::SimpleThread::Sleep(3000);
	consoleWriter = &console;

	running = false;
	httpRunning = false;
	toStop = false;
	NotNullPtr<IO::I2CChannel> channel;
	if (!channel.Set(IO::Device::AM2315::CreateDefChannel(1)))
	{
		console.WriteLineC(UTF8STRC("I2C Bus not found"));
		return 0;
	}
	NEW_CLASS(am2315, IO::Device::AM2315(channel, true));
	if (am2315->IsError())
	{
		DEL_CLASS(am2315);
		am2315 = 0;
		console.WriteLineC(UTF8STRC("AM2315 not found"));
		return 0;
	}

	NEW_CLASS(evt, Sync::Event(true));
	NEW_CLASS(httpEvt, Sync::Event(true));
	NEW_CLASSNN(sockf, Net::OSSocketFactory(false));
	ssl = Net::SSLEngineFactory::Create(sockf, true);

	wd = IO::Watchdog::Create(1);
	if (wd && wd->IsError())
	{
		DEL_CLASS(wd);
		wd = 0;
	}
	
	if (wd == 0)
	{
		console.WriteLineC(UTF8STRC("Watchdog not found"));
	}
	else
	{
		Text::StringBuilderUTF8 sb;
		Int32 timeoutSec;
		wd->SetTimeoutSec(15);
		if (wd->GetTimeoutSec(&timeoutSec))
		{
			sb.ClearStr();
			sb.AppendC(UTF8STRC("Timeout = "));
			sb.AppendI32(timeoutSec);
			console.WriteLineC(sb.ToString(), sb.GetLength());
		}
		else
		{
			console.WriteLineC(UTF8STRC("Error in getting timeout value"));
		}

		if (wd->Enable())
		{
			console.WriteLineC(UTF8STRC("Watchdog enabled"));
			Sync::ThreadUtil::Create(WatchdogThread, 0);
			Sync::ThreadUtil::Create(HTTPThread, 0);
			while (!running)
			{
				Sync::SimpleThread::Sleep(10);
			}
			console.WriteLineC(UTF8STRC("Running"));
			progCtrl->WaitForExit(progCtrl);
			console.WriteLineC(UTF8STRC("Exiting"));
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
			console.WriteLineC(UTF8STRC("Error in enabling watchdog"));
		}
		DEL_CLASS(wd);
	}
	DEL_CLASS(evt);
	DEL_CLASS(httpEvt);
	ssl.Delete();
	sockf.Delete();
	DEL_CLASS(am2315);
	return 0;
}

