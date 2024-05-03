#include "Stdafx.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "IO/LogTool.h"
#include "IO/Path.h"
#include "Net/OSSocketFactory.h"
#include "Net/LoRaGWMonitor.h"
#include "Net/LoRaGWUtil.h"
#include "Text/StringBuilderUTF8.h"

void __stdcall OnMessage(AnyType userObj, Bool toServer, UInt8 ver, UInt16 token, UInt8 msgType, const UInt8 *msg, UOSInt msgSize)
{
	NN<IO::LogTool> log = userObj.GetNN<IO::LogTool>();
	Text::StringBuilderUTF8 sb;
	Net::LoRaGWUtil::ParseGWMPMessage(sb, toServer, ver, token, msgType, msg, msgSize);
	log->LogMessage(sb.ToCString(), IO::LogHandler::LogLevel::Raw);
}

Int32 MyMain(NN<Core::IProgControl> progCtrl)
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

		log.AddFileLog(CSTRP(sbuff, sptr), IO::LogHandler::LogType::PerDay, IO::LogHandler::LogGroup::PerMonth, IO::LogHandler::LogLevel::Raw, "yyyy-MM-dd HH:mm:ss.fff", false);
		Net::LoRaGWMonitor lora(sockf, 1700, OnMessage, &log);
		if (!lora.IsError())
		{
			console.WriteLine(CSTR("Listening to LoRa Gateway data"));
			progCtrl->WaitForExit(progCtrl);
			console.WriteLine(CSTR("End listening"));
		}
	}
	return 0;
}
