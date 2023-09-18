#include "Stdafx.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "IO/IniFile.h"
#include "IO/Path.h"
#include "Manage/ExceptionRecorder.h"
#include "Net/OSSocketFactory.h"
#include "Net/PushServer.h"
#include "Net/SSLEngineFactory.h"

Int32 MyMain(NotNullPtr<Core::IProgControl> progCtrl)
{
	Manage::ExceptionRecorder exHdlr(CSTR("PushServer.err"), Manage::ExceptionRecorder::EA_RESTART);
	IO::ConsoleWriter console;
	IO::ConfigFile *cfg = IO::IniFile::ParseProgConfig(0);
	if (cfg == 0)
	{
		console.WriteLineC(UTF8STRC("Config file not found"));
		return 1;
	}
	Text::String *sPort = cfg->GetValue(CSTR("Port"));
	Text::String *fcmKey = cfg->GetValue(CSTR("FCMKey"));
	if (sPort == 0)
	{
		DEL_CLASS(cfg);
		console.WriteLineC(UTF8STRC("Config Port missing"));
		return 2;
	}
	UInt16 port;
	if (!sPort->ToUInt16(port))
	{
		DEL_CLASS(cfg);
		console.WriteLineC(UTF8STRC("Error in parsing port number"));
		return 3;
	}
	if (fcmKey == 0)
	{
		DEL_CLASS(cfg);
		console.WriteLineC(UTF8STRC("Config FCMKey missing"));
		return 4;
	}
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	Net::OSSocketFactory sockf(true);
	Net::SSLEngine *ssl = Net::SSLEngineFactory::Create(sockf, true);
	IO::LogTool log;
	sptr = IO::Path::GetProcessFileName(sbuff);
	sptr = IO::Path::AppendPath(sbuff, sptr, CSTR("log"));
	*sptr++ = IO::Path::PATH_SEPERATOR;
	sptr = Text::StrConcatC(sptr, UTF8STRC("PushSvr"));
	log.AddFileLog(CSTRP(sbuff, sptr), IO::LogHandler::LogType::PerDay, IO::LogHandler::LogGroup::PerMonth, IO::LogHandler::LogLevel::Raw, "yyyy-MM-dd HH:mm:ss.fff", false);
	{
		Net::PushServer svr(sockf, ssl, port, fcmKey->ToCString(), log);
		if (svr.IsError())
		{
			console.WriteLineC(UTF8STRC("Error in listening to port"));
		}
		else
		{
			console.WriteLineC(UTF8STRC("PushServer started"));
			progCtrl->WaitForExit(progCtrl);
		}
	}
	SDEL_CLASS(ssl);
	DEL_CLASS(cfg);
	return 0;
}