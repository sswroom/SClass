#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "IO/LogWriter.h"

void IO::LogWriter::CheckLines()
{

}

IO::LogWriter::LogWriter(IO::LogTool *log, IO::ILogHandler::LogLevel logLev)
{
	this->log = log;
	this->logLev = logLev;
	NEW_CLASS(this->mut, Sync::Mutex());
	NEW_CLASS(this->sb, Text::StringBuilderUTF8());
}

IO::LogWriter::~LogWriter()
{
	DEL_CLASS(this->mut);
	DEL_CLASS(this->sb);
}

Bool IO::LogWriter::WriteStrC(const UTF8Char *str, UOSInt nChar)
{
	this->mut->Lock();
	sb->AppendC(str, nChar);
	this->mut->Unlock();
	this->CheckLines();
	return true;
}

Bool IO::LogWriter::WriteStr(const UTF8Char *str)
{
	this->mut->Lock();
	sb->AppendSlow(str);
	this->mut->Unlock();
	this->CheckLines();
	return true;
}

Bool IO::LogWriter::WriteLineC(const UTF8Char *str, UOSInt nChar)
{
	this->mut->Lock();
	sb->AppendC(str, nChar);
	this->CheckLines();
	if (sb->GetLength() > 0)
	{
		this->log->LogMessageC(sb->ToString(), sb->GetLength(), this->logLev);
		sb->ClearStr();
	}
	this->mut->Unlock();
	return true;
}

Bool IO::LogWriter::WriteLine(const UTF8Char *str)
{
	this->mut->Lock();
	sb->AppendSlow(str);
	this->CheckLines();
	if (sb->GetLength() > 0)
	{
		this->log->LogMessageC(sb->ToString(), sb->GetLength(), this->logLev);
		sb->ClearStr();
	}
	this->mut->Unlock();
	return true;
}

Bool IO::LogWriter::WriteLine()
{
	this->mut->Lock();
	if (sb->GetLength() > 0)
	{
		this->log->LogMessageC(sb->ToString(), sb->GetLength(), this->logLev);
		sb->ClearStr();
	}
	this->mut->Unlock();
	return true;
}
