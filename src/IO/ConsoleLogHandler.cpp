#include "Stdafx.h"
#include "IO/ConsoleLogHandler.h"

IO::ConsoleLogHandler::ConsoleLogHandler(NN<IO::ConsoleWriter> console)
{
	this->console = console;
}

IO::ConsoleLogHandler::~ConsoleLogHandler()
{
}

void IO::ConsoleLogHandler::LogAdded(const Data::Timestamp &logTime, Text::CStringNN logMsg, LogLevel logLev)
{
	UTF8Char sbuff[16];
	UnsafeArray<UTF8Char> sptr;
	sptr = logTime.ToString(sbuff, CHSTR("HH:mm:ss.fff "));
	this->console->SetBGColor(Text::StandardColor::Black);
	this->console->SetTextColor(Text::StandardColor::Gray);
	this->console->Write(CSTRP(sbuff, sptr));
	if (logLev == IO::LogHandler::LogLevel::Raw)
	{
		this->console->SetTextColor(Text::StandardColor::White);
		this->console->WriteLine(logMsg);
	}
	else if (logLev == IO::LogHandler::LogLevel::Command)
	{
		this->console->SetTextColor(Text::StandardColor::Lime);
		this->console->WriteLine(logMsg);
	}
	else if (logLev == IO::LogHandler::LogLevel::Action)
	{
		this->console->SetTextColor(Text::StandardColor::Yellow);
		this->console->WriteLine(logMsg);
	}
	else if (logLev == IO::LogHandler::LogLevel::Error)
	{
		this->console->SetTextColor(Text::StandardColor::Red);
		this->console->WriteLine(logMsg);
	}
	else if (logLev == IO::LogHandler::LogLevel::ErrorDetail)
	{
		this->console->SetTextColor(Text::StandardColor::DarkRed);
		this->console->WriteLine(logMsg);
	}
	else
	{
		this->console->WriteLine(logMsg);
	}
}

void IO::ConsoleLogHandler::LogClosed()
{

}
