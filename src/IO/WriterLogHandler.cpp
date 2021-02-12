#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/WriterLogHandler.h"

IO::WriterLogHandler::WriterLogHandler(IO::Writer *writer, Bool toRelease)
{
	this->writer = writer;
	this->toRelease = toRelease;
}

IO::WriterLogHandler::~WriterLogHandler()
{
	if (this->toRelease)
	{
		DEL_CLASS(this->writer);
		this->writer = 0;
		this->toRelease = false;
	}
}

void IO::WriterLogHandler::LogClosed()
{
	DEL_CLASS(this);
}
void IO::WriterLogHandler::LogAdded(Data::DateTime *time, const UTF8Char *logMsg, LogLevel logLev)
{
	this->writer->WriteLine(logMsg);
}
