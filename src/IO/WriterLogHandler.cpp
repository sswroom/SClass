#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/WriterLogHandler.h"

IO::WriterLogHandler::WriterLogHandler(NN<IO::Writer> writer, Bool toRelease)
{
	this->writer = writer;
	this->toRelease = toRelease;
}

IO::WriterLogHandler::~WriterLogHandler()
{
	if (this->toRelease)
	{
		this->writer.Delete();
		this->toRelease = false;
	}
}

void IO::WriterLogHandler::LogClosed()
{
	DEL_CLASS(this);
}
void IO::WriterLogHandler::LogAdded(const Data::Timestamp &time, Text::CStringNN logMsg, LogLevel logLev)
{
	this->writer->WriteLine(logMsg);
}
