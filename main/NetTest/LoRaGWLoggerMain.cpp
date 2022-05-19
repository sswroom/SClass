#include "Stdafx.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "IO/LogTool.h"
#include "IO/Path.h"
#include "Net/OSSocketFactory.h"
#include "Net/LoRaGWMonitor.h"
#include "Text/StringBuilderUTF8.h"

void __stdcall OnMessage(void *userObj, Bool toServer, UInt8 ver, UInt16 token, UInt8 msgType, const UInt8 *msg, UOSInt msgSize)
{
	IO::LogTool *log = (IO::LogTool*)userObj;
	Text::StringBuilderUTF8 sb;
	if (toServer)
	{
		sb.AppendC(UTF8STRC("To Server"));
	}
	else
	{
		sb.AppendC(UTF8STRC("Fr Server"));
	}
	sb.AppendC(UTF8STRC(", ver="));
	sb.AppendU16(ver);
	sb.AppendC(UTF8STRC(", token="));
	sb.AppendU16(token);
	sb.AppendC(UTF8STRC(", leng="));
	sb.AppendUOSInt(msgSize);
	sb.AppendC(UTF8STRC(", type="));
	switch (msgType)
	{
	case 0:
		sb.AppendC(UTF8STRC("PUSH_DATA"));
		sb.AppendC(UTF8STRC(", GWEUI="));
		if (msgSize >= 8)
		{
			sb.AppendHexBuff(msg, 8, 0, Text::LineBreakType::None);
		}
		sb.AppendC(UTF8STRC(", Payload="));
		if (msgSize > 8)
		{
			sb.AppendC(msg + 8, msgSize - 8);
		}
		break;
	case 1:
		sb.AppendC(UTF8STRC("PUSH_ACK"));
		break;
	case 2:
		sb.AppendC(UTF8STRC("PULL_DATA"));
		sb.AppendC(UTF8STRC(", GWEUI="));
		if (msgSize >= 8)
		{
			sb.AppendHexBuff(msg, 8, 0, Text::LineBreakType::None);
		}
		break;
	case 3:
		sb.AppendC(UTF8STRC("PULL_RESP"));
		sb.AppendC(UTF8STRC(", Payload="));
		sb.AppendC(msg, msgSize);
		break;
	case 4:
		sb.AppendC(UTF8STRC("PULL_ACK"));
		sb.AppendC(UTF8STRC(", GWEUI="));
		if (msgSize >= 8)
		{
			sb.AppendHexBuff(msg, 8, 0, Text::LineBreakType::None);
		}
		break;
	case 5:
		sb.AppendC(UTF8STRC("TX_ACK"));
		sb.AppendC(UTF8STRC(", res="));
		if (msgSize == 1 && msg[0] == 0)
		{
			sb.AppendC(UTF8STRC("ok"));
		}
		else
		{
			sb.AppendC(msg, msgSize);
		}
		break;
	default:
		sb.AppendC(UTF8STRC("UNK("));
		sb.AppendU16(msgType);
		sb.AppendUTF8Char(')');
		break;
	}
	log->LogMessage(sb.ToCString(), IO::ILogHandler::LOG_LEVEL_RAW);
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

		log.AddFileLog(CSTRP(sbuff, sptr), IO::ILogHandler::LOG_TYPE_PER_DAY, IO::ILogHandler::LOG_GROUP_TYPE_PER_MONTH, IO::ILogHandler::LOG_LEVEL_RAW, "yyyy-MM-dd HH:mm:ss.fff", false);
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
