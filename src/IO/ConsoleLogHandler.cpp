#include "Stdafx.h"
#include "IO/ConsoleLogHandler.h"

IO::ConsoleLogHandler::ConsoleLogHandler(IO::ConsoleWriter *console)
{
	this->console = console;
}

IO::ConsoleLogHandler::~ConsoleLogHandler()
{
}

void IO::ConsoleLogHandler::LogAdded(Data::Timestamp logTime, Text::CString logMsg, LogLevel logLev)
{
	UTF8Char sbuff[16];
	UTF8Char *sptr;
	sptr = logTime.ToString(sbuff, "HH:mm:ss.fff ");
	this->console->SetTextColor(IO::ConsoleWriter::CC_GRAY, IO::ConsoleWriter::CC_BLACK);
	this->console->WriteStr(CSTRP(sbuff, sptr));
	if (logLev == IO::ILogHandler::LogLevel::Raw)
	{
		this->console->SetTextColor(IO::ConsoleWriter::CC_WHITE, IO::ConsoleWriter::CC_BLACK);
		this->console->WriteLineCStr(logMsg);
	}
	else if (logLev == IO::ILogHandler::LogLevel::Command)
	{
		this->console->SetTextColor(IO::ConsoleWriter::CC_GREEN, IO::ConsoleWriter::CC_BLACK);
		this->console->WriteLineCStr(logMsg);
	}
	else if (logLev == IO::ILogHandler::LogLevel::Action)
	{
		this->console->SetTextColor(IO::ConsoleWriter::CC_YELLOW, IO::ConsoleWriter::CC_BLACK);
		this->console->WriteLineCStr(logMsg);
	}
	else if (logLev == IO::ILogHandler::LogLevel::Error)
	{
		this->console->SetTextColor(IO::ConsoleWriter::CC_RED, IO::ConsoleWriter::CC_BLACK);
		this->console->WriteLineCStr(logMsg);
	}
	else if (logLev == IO::ILogHandler::LogLevel::ErrorDetail)
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
