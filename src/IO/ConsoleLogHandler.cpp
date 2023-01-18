#include "Stdafx.h"
#include "IO/ConsoleLogHandler.h"

IO::ConsoleLogHandler::ConsoleLogHandler(IO::ConsoleWriter *console)
{
	this->console = console;
}

IO::ConsoleLogHandler::~ConsoleLogHandler()
{
}

void IO::ConsoleLogHandler::LogAdded(const Data::Timestamp &logTime, Text::CString logMsg, LogLevel logLev)
{
	UTF8Char sbuff[16];
	UTF8Char *sptr;
	sptr = logTime.ToString(sbuff, "HH:mm:ss.fff ");
	this->console->SetBGColor(Text::StyledTextWriter::StandardColor::Black);
	this->console->SetTextColor(Text::StyledTextWriter::StandardColor::Gray);
	this->console->WriteStr(CSTRP(sbuff, sptr));
	if (logLev == IO::ILogHandler::LogLevel::Raw)
	{
		this->console->SetTextColor(Text::StyledTextWriter::StandardColor::White);
		this->console->WriteLineCStr(logMsg);
	}
	else if (logLev == IO::ILogHandler::LogLevel::Command)
	{
		this->console->SetTextColor(Text::StyledTextWriter::StandardColor::Green);
		this->console->WriteLineCStr(logMsg);
	}
	else if (logLev == IO::ILogHandler::LogLevel::Action)
	{
		this->console->SetTextColor(Text::StyledTextWriter::StandardColor::Yellow);
		this->console->WriteLineCStr(logMsg);
	}
	else if (logLev == IO::ILogHandler::LogLevel::Error)
	{
		this->console->SetTextColor(Text::StyledTextWriter::StandardColor::Red);
		this->console->WriteLineCStr(logMsg);
	}
	else if (logLev == IO::ILogHandler::LogLevel::ErrorDetail)
	{
		this->console->SetBGColor(Text::StyledTextWriter::StandardColor::DarkRed);
		this->console->SetTextColor(Text::StyledTextWriter::StandardColor::Black);
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
