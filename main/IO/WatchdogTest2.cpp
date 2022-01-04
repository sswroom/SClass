#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "IO/FileStream.h"
#include "IO/Watchdog.h"
#include "IO/Device/AM2315.h"
#include "Net/HTTPClient.h"
#include "Net/OSSocketFactory.h"
#include "Net/SSLEngine.h"
#include "Net/SSLEngineFactory.h"
#include "Sync/Thread.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"

#define TESTURL (const UTF8Char*)"http://abc.com/kareq"
#define USERAGENT (const UTF8Char*)"HTTPClient/1.0"

IO::Watchdog *wd;
Bool running;
Bool httpRunning;
Bool toStop;
Sync::Event *evt;
Sync::Event *httpEvt;
IO::Device::AM2315 *am2315;
Net::SocketFactory *sockf;
Net::SSLEngine *ssl;
IO::ConsoleWriter *consoleWriter;

UInt32 __stdcall WatchdogThread(void *userObj)
{
	Int32 i = 15;
	Single tempVal;
	Single rhVal;
	Text::StringBuilderUTF8 *sb;
	NEW_CLASS(sb, Text::StringBuilderUTF8());
	running = true;
	while (!toStop)
	{
		if (i-- > 0)
		{
			consoleWriter->WriteLine((const UTF8Char*)"Keep Alive");
			wd->Keepalive();
		}
		if (am2315->ReadTemperature(&tempVal))
		{
			if (am2315->ReadRH(&rhVal))
			{
				sb->ClearStr();
				sb->AppendC(UTF8STRC("AM2315: Temp = "));
				Text::SBAppendF64(sb, tempVal);
				sb->AppendC(UTF8STRC(", RH = "));
				Text::SBAppendF64(sb, rhVal);
				consoleWriter->WriteLine(sb->ToString());
			}
			else
			{
				sb->ClearStr();
				sb->AppendC(UTF8STRC("AM2315: Temp = "));
				Text::SBAppendF64(sb, tempVal);
				sb->AppendC(UTF8STRC(", RH = error"));
				consoleWriter->WriteLine(sb->ToString());
			}
		}
		else
		{
			consoleWriter->WriteLine((const UTF8Char*)"Fail in reading from AM2315");
		}
		
		evt->Wait(2000);
	}
	DEL_CLASS(sb);
	running = false;
	return 0;
}

UInt32 __stdcall HTTPThread(void *userObj)
{
	Net::HTTPClient *cli;
	Text::StringBuilderUTF8 *sb;
	NEW_CLASS(sb, Text::StringBuilderUTF8());
	httpRunning = true;
	while (!toStop)
	{
		consoleWriter->Write((const UTF8Char*)"Requesting to ");
		consoleWriter->WriteLine(TESTURL);
		cli = Net::HTTPClient::CreateClient(sockf, ssl, USERAGENT, false, Text::StrStartsWith(TESTURL, (const UTF8Char*)"https://"));
		cli->Connect(TESTURL, "GET", 0, 0, false);
		cli->AddHeader((const UTF8Char*)"User-Agent", USERAGENT);
		cli->AddHeader((const UTF8Char*)"Accept", (const UTF8Char*)"*/*");
		cli->AddHeader((const UTF8Char*)"Accept-Charset", (const UTF8Char*)"*");
		cli->AddHeader((const UTF8Char*)"Connection", (const UTF8Char*)"close");
		
		if (cli->IsError())
		{
			consoleWriter->WriteLine((const UTF8Char*)"Error in requesting to server");
		}
		else
		{
			cli->EndRequest(0, 0);
			Int32 respCode = cli->GetRespStatus();
			sb->ClearStr();
			sb->AppendC(UTF8STRC("Resp Code = "));
			sb->AppendI32(respCode);
			consoleWriter->WriteLine(sb->ToString());
		}
		DEL_CLASS(cli);
		httpEvt->Wait(1000);
	}
	DEL_CLASS(sb);
	httpRunning = false;
	return 0;
}

Bool EchoFile(const UTF8Char *fileName, const Char *msg)
{
	IO::FileStream *fs;
	NEW_CLASS(fs, IO::FileStream(fileName, IO::FileMode::CreateWrite, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
	if (fs->IsError())
	{
		DEL_CLASS(fs);
		return false;
	}
	UOSInt fileSize = Text::StrCharCnt(msg);
	fs->Write((const UInt8*)msg, fileSize);
	DEL_CLASS(fs);
	return true;
}

Int32 MyMain(Core::IProgControl *progCtrl)
{
	IO::ConsoleWriter console;
	EchoFile((const UTF8Char*)"/sys/class/gpio/export", "71");
	EchoFile((const UTF8Char*)"/sys/class/gpio/gpio71/direction", "out");
	EchoFile((const UTF8Char*)"/sys/class/gpio/gpio71/value", "1");
	Sync::Thread::Sleep(3000);
	consoleWriter = &console;

	running = false;
	httpRunning = false;
	toStop = false;
	IO::I2CChannel *channel = IO::Device::AM2315::CreateDefChannel(1);
	if (channel == 0)
	{
		console.WriteLine((const UTF8Char*)"I2C Bus not found");
		return 0;
	}
	NEW_CLASS(am2315, IO::Device::AM2315(channel, true));
	if (am2315->IsError())
	{
		DEL_CLASS(am2315);
		am2315 = 0;
		console.WriteLine((const UTF8Char*)"AM2315 not found");
		return 0;
	}

	NEW_CLASS(evt, Sync::Event(true, (const UTF8Char*)"evt"));
	NEW_CLASS(httpEvt, Sync::Event(true, (const UTF8Char*)"evt"));
	NEW_CLASS(sockf, Net::OSSocketFactory(false));
	ssl = Net::SSLEngineFactory::Create(sockf, true);

	wd = IO::Watchdog::Create(1);
	if (wd && wd->IsError())
	{
		DEL_CLASS(wd);
		wd = 0;
	}
	
	if (wd == 0)
	{
		console.WriteLine((const UTF8Char*)"Watchdog not found");
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
			console.WriteLine(sb.ToString());
		}
		else
		{
			console.WriteLine((const UTF8Char*)"Error in getting timeout value");
		}

		if (wd->Enable())
		{
			console.WriteLine((const UTF8Char*)"Watchdog enabled");
			Sync::Thread::Create(WatchdogThread, 0);
			Sync::Thread::Create(HTTPThread, 0);
			while (!running)
			{
				Sync::Thread::Sleep(10);
			}
			console.WriteLine((const UTF8Char*)"Running");
			progCtrl->WaitForExit(progCtrl);
			console.WriteLine((const UTF8Char*)"Exiting");
			toStop = true;
			evt->Set();
			httpEvt->Set();
			while (running || httpRunning)
			{
				Sync::Thread::Sleep(10);
			}
		}
		else
		{
			console.WriteLine((const UTF8Char*)"Error in enabling watchdog");
		}
		DEL_CLASS(wd);
	}
	DEL_CLASS(evt);
	DEL_CLASS(httpEvt);
	SDEL_CLASS(ssl);
	DEL_CLASS(sockf);
	DEL_CLASS(am2315);
	return 0;
}

