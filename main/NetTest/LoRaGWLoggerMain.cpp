#include "Stdafx.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "IO/LogTool.h"
#include "IO/Path.h"
#include "Net/OSSocketFactory.h"
#include "Net/LoRaGWMonitor.h"
#include "Net/LoRaGWUtil.h"
#include "Text/StringBuilderUTF8.h"

void __stdcall OnMessage(void *userObj, Bool toServer, UInt8 ver, UInt16 token, UInt8 msgType, const UInt8 *msg, UOSInt msgSize)
{
	IO::LogTool *log = (IO::LogTool*)userObj;
	Text::StringBuilderUTF8 sb;
	Net::LoRaGWUtil::ParseGWMPMessage(&sb, toServer, ver, token, msgType, msg, msgSize);
	log->LogMessage(sb.ToCString(), IO::ILogHandler::LogLevel::Raw);
}

Int32 MyMain(Core::IProgControl *progCtrl)
{
	Net::OSSocketFactory sockf(true);
	IO::ConsoleWriter console;
	IO::LogTool log;
	{
		UTF8Char sbuff[512];
		UTF8Char *sptr;
		sptr = IO::Path::GetProcessFileName(sbuff);
		sptr = IO::Path::AppendPath(sbuff, sptr, CSTR("loragw"));
		*sptr++ = IO::Path::PATH_SEPERATOR;
		sptr = Text::StrConcatC(sptr, UTF8STRC("log"));

		log.AddFileLog(CSTRP(sbuff, sptr), IO::ILogHandler::LogType::PerDay, IO::ILogHandler::LogGroup::PerMonth, IO::ILogHandler::LogLevel::Raw, "yyyy-MM-dd HH:mm:ss.fff", false);
		Net::LoRaGWMonitor lora(&sockf, 1700, OnMessage, &log);
		if (!lora.IsError())
		{
			console.WriteLineC(UTF8STRC("Listening to LoRa Gateway data"));
			progCtrl->WaitForExit(progCtrl);
			console.WriteLineC(UTF8STRC("End listening"));
		}
	}
	return 0;
}
