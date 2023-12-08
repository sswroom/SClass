#include "Stdafx.h"
#include "IO/ConsoleLogHandler.h"

IO::ConsoleLogHandler::ConsoleLogHandler(NotNullPtr<IO::ConsoleWriter> console)
{
	this->console = console;
}

IO::ConsoleLogHandler::~ConsoleLogHandler()
{
}

void IO::ConsoleLogHandler::LogAdded(const Data::Timestamp &logTime, Text::CStringNN logMsg, LogLevel logLev)
{
	UTF8Char sbuff[16];
	UTF8Char *sptr;
	sptr = logTime.ToString(sbuff, "HH:mm:ss.fff ");
	this->console->SetBGColor(Text::StandardColor::Black);
	this->console->SetTextColor(Text::StandardColor::Gray);
	this->console->WriteStr(CSTRP(sbuff, sptr));
	if (logLev == IO::LogHandler::LogLevel::Raw)
	{
		this->console->SetTextColor(Text::StandardColor::White);
		this->console->WriteLineCStr(logMsg);
	}
	else if (logLev == IO::LogHandler::LogLevel::Command)
	{
		this->console->SetTextColor(Text::StandardColor::Lime);
		this->console->WriteLineCStr(logMsg);
	}
	else if (logLev == IO::LogHandler::LogLevel::Action)
	{
		this->console->SetTextColor(Text::StandardColor::Yellow);
		this->console->WriteLineCStr(logMsg);
	}
	else if (logLev == IO::LogHandler::LogLevel::Error)
	{
		this->console->SetTextColor(Text::StandardColor::Red);
		this->console->WriteLineCStr(logMsg);
	}
	else if (logLev == IO::LogHandler::LogLevel::ErrorDetail)
	{
		this->console->SetTextColor(Text::StandardColor::DarkRed);
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
