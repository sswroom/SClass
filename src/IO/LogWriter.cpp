#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/LogWriter.h"
#include "Sync/MutexUsage.h"
#include "Text/MyString.h"

void IO::LogWriter::CheckLines()
{
	if (this->sb.EndsWith('\r') || this->sb.EndsWith('\n'))
	{
		this->sb.RemoveChars(1);
		if (this->sb.EndsWith('\r') || this->sb.EndsWith('\n'))
		{
			this->sb.RemoveChars(1);
		}
		this->log->LogMessage(this->sb.ToCString(), this->logLev);
		this->sb.ClearStr();
	}
}

IO::LogWriter::LogWriter(NN<IO::LogTool> log, IO::LogHandler::LogLevel logLev)
{
	this->log = log;
	this->logLev = logLev;
}

IO::LogWriter::~LogWriter()
{
}

Bool IO::LogWriter::Write(Text::CStringNN str)
{
	{
		Sync::MutexUsage mutUsage(this->mut);
		this->sb.Append(str);
	}
	this->CheckLines();
	return true;
}

Bool IO::LogWriter::WriteLine(Text::CStringNN str)
{
	Sync::MutexUsage mutUsage(this->mut);
	this->sb.Append(str);
	this->CheckLines();
	if (this->sb.GetLength() > 0)
	{
		this->log->LogMessage(this->sb.ToCString(), this->logLev);
		this->sb.ClearStr();
	}
	return true;
}

Bool IO::LogWriter::WriteLine()
{
	Sync::MutexUsage mutUsage(this->mut);
	if (this->sb.GetLength() > 0)
	{
		this->log->LogMessage(this->sb.ToCString(), this->logLev);
		this->sb.ClearStr();
	}
	return true;
}
