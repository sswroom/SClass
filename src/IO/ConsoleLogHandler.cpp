#include "Stdafx.h"
#include "IO/ConsoleLogHandler.h"

IO::ConsoleLogHandler::ConsoleLogHandler(IO::ConsoleWriter *console)
{
	this->console = console;
}

IO::ConsoleLogHandler::~ConsoleLogHandler()
{
}

void IO::ConsoleLogHandler::LogAdded(Data::DateTime *logTime, Text::CString logMsg, LogLevel logLev)
{
	UTF8Char sbuff[16];
	UTF8Char *sptr;
	sptr = logTime->ToString(sbuff, "HH:mm:ss.fff ");
	this->console->SetTextColor(IO::ConsoleWriter::CC_GRAY, IO::ConsoleWriter::CC_BLACK);
	this->console->WriteStr(CSTRP(sbuff, sptr));
	if (logLev == IO::ILogHandler::LOG_LEVEL_RAW)
	{
		this->console->SetTextColor(IO::ConsoleWriter::CC_WHITE, IO::ConsoleWriter::CC_BLACK);
		this->console->WriteLineCStr(logMsg);
	}
	else if (logLev == IO::ILogHandler::LOG_LEVEL_COMMAND)
	{
		this->console->SetTextColor(IO::ConsoleWriter::CC_GREEN, IO::ConsoleWriter::CC_BLACK);
		this->console->WriteLineCStr(logMsg);
	}
	else if (logLev == IO::ILogHandler::LOG_LEVEL_ACTION)
	{
		this->console->SetTextColor(IO::ConsoleWriter::CC_YELLOW, IO::ConsoleWriter::CC_BLACK);
		this->console->WriteLineCStr(logMsg);
	}
	else if (logLev == IO::ILogHandler::LOG_LEVEL_ERROR)
	{
		this->console->SetTextColor(IO::ConsoleWriter::CC_RED, IO::ConsoleWriter::CC_BLACK);
		this->console->WriteLineCStr(logMsg);
	}
	else if (logLev == IO::ILogHandler::LOG_LEVEL_ERR_DETAIL)
	{
		this->console->SetTextColor(IO::ConsoleWriter::CC_BLACK, IO::ConsoleWriter::CC_DARK_RED);
		this->console->WriteLineCStr(logMsg);
	}
	else
	{
		this->console->WriteLineCStr(logMsg);
	}
}

void IO::ConsoleLogHandler::LogClosed()
{

}
