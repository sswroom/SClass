#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/DateTime.h"
#include "IO/Path.h"
#include "IO/MTFileLog.h"
#include "Sync/MutexUsage.h"
#include "Sync/Thread.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"

UTF8Char *IO::MTFileLog::GetNewName(UTF8Char *buff, Data::DateTime *time)
{
	UTF8Char *currName;

	if (this->groupStyle == IO::ILogHandler::LOG_GROUP_TYPE_NO_GROUP)
	{
		currName = Text::StrConcat(buff, this->fileName);
	}
	else if (this->groupStyle == IO::ILogHandler::LOG_GROUP_TYPE_PER_YEAR)
	{
		currName = time->ToString(Text::StrConcat(buff, this->fileName), "yyyy");
		if (!IO::Path::IsDirectoryExist(buff))
		{
			IO::Path::CreateDirectory(buff);
		}
		*currName++ = IO::Path::PATH_SEPERATOR;
		currName = Text::StrConcat(currName, this->extName);
	}
	else if (this->groupStyle == IO::ILogHandler::LOG_GROUP_TYPE_PER_MONTH)
	{
		currName = time->ToString(Text::StrConcat(buff, this->fileName), "yyyyMM");
		if (!IO::Path::IsDirectoryExist(buff))
		{
			IO::Path::CreateDirectory(buff);
		}
		*currName++ = IO::Path::PATH_SEPERATOR;
		currName = Text::StrConcat(currName, this->extName);
	}
	else if (this->groupStyle == IO::ILogHandler::LOG_GROUP_TYPE_PER_DAY)
	{
		currName = time->ToString(Text::StrConcat(buff, this->fileName), "yyyyMMdd");
		if (!IO::Path::IsDirectoryExist(buff))
		{
			IO::Path::CreateDirectory(buff);
		}
		*currName++ = IO::Path::PATH_SEPERATOR;
		currName = Text::StrConcat(currName, this->extName);
	}
	else
	{
		currName = Text::StrConcat(buff, this->fileName);
	}

	if (this->logStyle == IO::ILogHandler::LOG_TYPE_SINGLE_FILE)
	{
	}
	else if (this->logStyle == IO::ILogHandler::LOG_TYPE_PER_HOUR)
	{
		lastVal = time->GetDay() * 24 + time->GetHour();
		currName = Text::StrConcat(time->ToString(currName, "yyyyMMddHH"), (const UTF8Char*)".log");
	}
	else if (this->logStyle == IO::ILogHandler::LOG_TYPE_PER_DAY)
	{
		lastVal = time->GetDay();
		currName = Text::StrConcat(time->ToString(currName, "yyyyMMdd"), (const UTF8Char*)".log");
	}
	else if (this->logStyle == IO::ILogHandler::LOG_TYPE_PER_MONTH)
	{
		lastVal = time->GetMonth();
		currName = Text::StrConcat(time->ToString(currName, "yyyyMM"), (const UTF8Char*)".log");
	}
	else if (this->logStyle == IO::ILogHandler::LOG_TYPE_PER_YEAR)
	{
		lastVal = time->GetYear();
		currName = Text::StrConcat(time->ToString(currName, "yyyy"), (const UTF8Char*)".log");
	}
	return currName;
}

void IO::MTFileLog::WriteArr(const UTF8Char **msgArr, Int64 *dateArr, UOSInt arrCnt)
{
	Bool newFile = false;
	UTF8Char buff[256];
	Data::DateTime time;
	Text::StringBuilderUTF8 sb;

	UOSInt i;
	i = 0;
	while (i < arrCnt)
	{
		time.SetTicks(dateArr[i]);
		time.ToLocalTime();
		if (logStyle == ILogHandler::LOG_TYPE_PER_DAY)
		{
			if (time.GetDay() != lastVal)
			{
				newFile = true;
				GetNewName(buff, &time);
			}
		}
		else if (logStyle == ILogHandler::LOG_TYPE_PER_MONTH)
		{
			if (time.GetMonth() != lastVal)
			{
				newFile = true;
				GetNewName(buff, &time);
			}
		}
		else if (logStyle == ILogHandler::LOG_TYPE_PER_YEAR)
		{
			if (time.GetYear() != lastVal)
			{
				newFile = true;
				GetNewName(buff, &time);
			}
		}
		else if (logStyle == ILogHandler::LOG_TYPE_PER_HOUR)
		{
			if (lastVal != (time.GetDay() * 24 + time.GetHour()))
			{
				newFile = true;
				GetNewName(buff, &time);
			}
		}

		if (newFile)
		{
			log->Close();
			DEL_CLASS(log);
			DEL_CLASS(fileStm);

			NEW_CLASS(fileStm, FileStream(buff, FileStream::FILE_MODE_APPEND, FileStream::FILE_SHARE_DENY_NONE, IO::FileStream::BT_NORMAL));
			NEW_CLASS(log, Text::UTF8Writer(fileStm));
			log->WriteSignature();

			Text::StrConcat(time.ToString(buff, this->dateFormat), (const UTF8Char*)"Program running");
			log->WriteLine(buff);
			newFile = false;
		}

		time.ToString(buff, this->dateFormat);
		sb.ClearStr();
		sb.Append(buff);
		sb.Append(msgArr[i]);
		log->WriteLine(sb.ToString());

		Text::StrDelNew(msgArr[i]);
		i++;
	}
	if (fileStm)
	{
		fileStm->Flush();
	}
}

UInt32 __stdcall IO::MTFileLog::FileThread(void *userObj)
{
	IO::MTFileLog *me = (IO::MTFileLog*)userObj;
	UOSInt arrCnt;
	const UTF8Char **msgArr = 0;
	Int64 *dateArr = 0;
	me->running = true;
	while (!me->closed)
	{
		Sync::MutexUsage mutUsage(me->mut);
		if ((arrCnt = me->msgList->GetCount()) > 0)
		{
			msgArr = MemAlloc(const UTF8Char *, arrCnt);
			dateArr = MemAlloc(Int64, arrCnt);
			me->msgList->GetRange(msgArr, 0, arrCnt);
			me->dateList->GetRange(dateArr, 0, arrCnt);
			me->msgList->RemoveRange(0, arrCnt);
			me->dateList->RemoveRange(0, arrCnt);
		}
		mutUsage.EndUse();
		
		if (arrCnt > 0)
		{
			me->WriteArr(msgArr, dateArr, arrCnt);
			MemFree(msgArr);
			MemFree(dateArr);
		}
		me->evt->Wait(1000);
	}

	if ((arrCnt = me->msgList->GetCount()) > 0)
	{
		msgArr = MemAlloc(const UTF8Char *, arrCnt);
		dateArr = MemAlloc(Int64, arrCnt);
		me->msgList->GetRange(msgArr, 0, arrCnt);
		me->dateList->GetRange(dateArr, 0, arrCnt);
		me->msgList->RemoveRange(0, arrCnt);
		me->dateList->RemoveRange(0, arrCnt);
		me->WriteArr(msgArr, dateArr, arrCnt);
		MemFree(msgArr);
		MemFree(dateArr);
	}

	me->running = false;
	return 0;
}

IO::MTFileLog::MTFileLog(const UTF8Char *fileName, LogType style, LogGroup groupStyle, const Char *dateFormat)
{
	UTF8Char buff[256];
	Char cbuff[256];
	UOSInt i;
	NEW_CLASS(mut, Sync::Mutex());
	NEW_CLASS(evt, Sync::Event(true, (const UTF8Char*)"IO.MTFileLog.evt"));
	NEW_CLASS(dateList, Data::ArrayListInt64());
	NEW_CLASS(msgList, Data::ArrayListStrUTF8());
	if (dateFormat == 0)
	{
		this->dateFormat = Text::StrCopyNew("yyyy-MM-dd HH:mm:ss\t");
	}
	else
	{
		Text::StrConcat(Text::StrConcat(cbuff, dateFormat), "\t");
		this->dateFormat = Text::StrCopyNew(cbuff);
	}
	this->logStyle = style;
	this->groupStyle = groupStyle;
	this->closed = false;


	this->fileName = Text::StrCopyNew(fileName);
	if (this->groupStyle != IO::ILogHandler::LOG_GROUP_TYPE_NO_GROUP)
	{
		i = Text::StrLastIndexOf(this->fileName, IO::Path::PATH_SEPERATOR);
		this->extName = Text::StrCopyNew(&this->fileName[i + 1]);
	}
	else
	{
		this->extName = 0;
		i = Text::StrLastIndexOf(this->fileName, IO::Path::PATH_SEPERATOR);
		if (i != INVALID_INDEX)
		{
			((UTF8Char*)this->fileName)[i] = 0;
			if (!IO::Path::IsDirectoryExist(this->fileName))
			{
				IO::Path::CreateDirectory(this->fileName);
			}
			((UTF8Char*)this->fileName)[i] = IO::Path::PATH_SEPERATOR;
		}
	}

	Data::DateTime dt;
	dt.SetCurrTime();
	GetNewName(buff, &dt);

	NEW_CLASS(fileStm, FileStream(buff, FileStream::FILE_MODE_APPEND, FileStream::FILE_SHARE_DENY_NONE, IO::FileStream::BT_NORMAL));
	NEW_CLASS(log, Text::UTF8Writer(fileStm));
	log->WriteSignature();
	Sync::Thread::Create(FileThread, this, 0x20000);
}

IO::MTFileLog::~MTFileLog()
{
	if (!closed)
	{
		closed = true;
		evt->Set();
	}

	while (running)
	{
		Sync::Thread::Sleep(10);
	}
	Sync::MutexUsage mutUsage(mut);
	log->Close();
	mutUsage.EndUse();


	Text::StrDelNew(fileName);
	fileName = 0;
	if (this->extName)
	{
		Text::StrDelNew(this->extName);
		this->extName = 0;
	}

	DEL_CLASS(log);
	log = 0;

	DEL_CLASS(fileStm);
	fileStm = 0;

	DEL_CLASS(dateList);
	DEL_CLASS(msgList);
	Text::StrDelNew(this->dateFormat);
	DEL_CLASS(evt);
	evt = 0;
	DEL_CLASS(mut);
	mut = 0;
}

void IO::MTFileLog::LogClosed()
{
	if (!closed)
	{
		closed = true;
		evt->Set();
	}
}
void IO::MTFileLog::LogAdded(Data::DateTime *time, const UTF8Char *logMsg, LogLevel logLev)
{
	if (closed)
		return;

	Sync::MutexUsage mutUsage(mut);
	this->msgList->Add(Text::StrCopyNew(logMsg));
	this->dateList->Add(time->ToTicks());
	mutUsage.EndUse();
	evt->Set();
}
