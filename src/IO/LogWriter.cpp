#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "IO/LogWriter.h"

IO::LogWriter::LogWriter(IO::LogTool *log, IO::ILogHandler::LogLevel logLev)
{
	this->log = log;
	this->logLev = logLev;
	NEW_CLASS(this->mut, Sync::Mutex());
	NEW_CLASS(this->sb, Text::StringBuilder());
}

IO::LogWriter::~LogWriter()
{
	DEL_CLASS(this->mut);
	DEL_CLASS(this->sb);
}

OSInt IO::LogWriter::Write(const WChar *str, OSInt nChar)
{
	this->mut->Lock();
	sb->Append(str, nChar);
	this->mut->Unlock();
	return nChar;
}

OSInt IO::LogWriter::Write(const WChar *str)
{
	OSInt nChar = Text::StrCharCnt(str);
	this->mut->Lock();
	sb->Append(str, nChar);
	this->mut->Unlock();
	return nChar;
}

OSInt IO::LogWriter::WriteLine(const WChar *str, OSInt nChar)
{
	this->mut->Lock();
	sb->Append(str, nChar);
	if (sb->GetLength() > 0)
	{
		this->log->LogMessage(sb->ToString(), this->logLev);
		sb->ClearStr();
	}
	this->mut->Unlock();
	return nChar;
}

OSInt IO::LogWriter::WriteLine(const WChar *str)
{
	OSInt nChar = Text::StrCharCnt(str);
	this->mut->Lock();
	sb->Append(str, nChar);
	if (sb->GetLength() > 0)
	{
		this->log->LogMessage(sb->ToString(), this->logLev);
		sb->ClearStr();
	}
	this->mut->Unlock();
	return nChar;
}

OSInt IO::LogWriter::WriteLine()
{
	this->mut->Lock();
	if (sb->GetLength() > 0)
	{
		this->log->LogMessage(sb->ToString(), this->logLev);
		sb->ClearStr();
	}
	this->mut->Unlock();
	return 0;
}
