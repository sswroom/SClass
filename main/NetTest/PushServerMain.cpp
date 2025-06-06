#include "Stdafx.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "IO/IniFile.h"
#include "IO/Path.h"
#include "Manage/ExceptionRecorder.h"
#include "Net/OSSocketFactory.h"
#include "Net/PushServer.h"
#include "Net/SSLEngineFactory.h"

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
	Manage::ExceptionRecorder exHdlr(CSTR("PushServer.err"), Manage::ExceptionRecorder::EA_RESTART);
	IO::ConsoleWriter console;
	NN<IO::ConfigFile> cfg;
	if (!IO::IniFile::ParseProgConfig(0).SetTo(cfg))
	{
		console.WriteLine(CSTR("Config file not found"));
		return 1;
	}
	NN<Text::String> sPort;
	NN<Net::Google::GoogleServiceAccount> serviceAccount;
	if (!cfg->GetValue(CSTR("Port")).SetTo(sPort))
	{
		cfg.Delete();
		console.WriteLine(CSTR("Config Port missing"));
		return 2;
	}
	UInt16 port;
	if (!sPort->ToUInt16(port))
	{
		cfg.Delete();
		console.WriteLine(CSTR("Error in parsing port number"));
		return 3;
	}
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	sptr = IO::Path::GetProcessFileName(sbuff).Or(sbuff);
	sptr = IO::Path::AppendPath(sbuff, sptr, CSTR("serviceaccount.json"));
	if (!Net::Google::GoogleServiceAccount::FromFile(CSTRP(sbuff, sptr)).SetTo(serviceAccount))
	{
		console.WriteLine(CSTR("Error loading serviceaccount.json"));
		cfg.Delete();
		return 4;
	}
	Net::OSSocketFactory sockf(true);
	Net::TCPClientFactory clif(sockf);
	Optional<Net::SSLEngine> ssl = Net::SSLEngineFactory::Create(clif, true);
	IO::LogTool log;
	sptr = IO::Path::GetProcessFileName(sbuff).Or(sbuff);
	sptr = IO::Path::AppendPath(sbuff, sptr, CSTR("log"));
	*sptr++ = IO::Path::PATH_SEPERATOR;
	sptr = Text::StrConcatC(sptr, UTF8STRC("PushSvr"));
	log.AddFileLog(CSTRP(sbuff, sptr), IO::LogHandler::LogType::PerDay, IO::LogHandler::LogGroup::PerMonth, IO::LogHandler::LogLevel::Raw, "yyyy-MM-dd HH:mm:ss.fff", false);
	{
		Net::PushServer svr(clif, ssl, port, serviceAccount, log);
		if (svr.IsError())
		{
			console.WriteLine(CSTR("Error in listening to port"));
		}
		else
		{
			console.WriteLine(CSTR("PushServer started"));
			progCtrl->WaitForExit(progCtrl);
		}
	}
	serviceAccount.Delete();
	ssl.Delete();
	cfg.Delete();
	return 0;
}