#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/DateTime.h"
#include "IO/Path.h"
#include "IO/MTFileLog.h"
#include "Sync/MutexUsage.h"
#include "Sync/Thread.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"

UTF8Char *IO::MTFileLog::GetNewName(UTF8Char *buff, Data::DateTime *time, Int32 *lastVal)
{
	UTF8Char *currName;
	switch (this->groupStyle)
	{
	case IO::ILogHandler::LOG_GROUP_TYPE_PER_YEAR:
		currName = time->ToString(this->fileName->ConcatTo(buff), "yyyy");
		if (!IO::Path::IsDirectoryExist(CSTRP(buff, currName)))
		{
			IO::Path::CreateDirectory(CSTRP(buff, currName));
		}
		*currName++ = IO::Path::PATH_SEPERATOR;
		currName = this->extName->ConcatTo(currName);
		break;
	case IO::ILogHandler::LOG_GROUP_TYPE_PER_MONTH:
		currName = time->ToString(this->fileName->ConcatTo(buff), "yyyyMM");
		if (!IO::Path::IsDirectoryExist(CSTRP(buff, currName)))
		{
			IO::Path::CreateDirectory(CSTRP(buff, currName));
		}
		*currName++ = IO::Path::PATH_SEPERATOR;
		currName = this->extName->ConcatTo(currName);
		break;
	case IO::ILogHandler::LOG_GROUP_TYPE_PER_DAY:
		currName = time->ToString(this->fileName->ConcatTo(buff), "yyyyMMdd");
		if (!IO::Path::IsDirectoryExist(CSTRP(buff, currName)))
		{
			IO::Path::CreateDirectory(CSTRP(buff, currName));
		}
		*currName++ = IO::Path::PATH_SEPERATOR;
		currName = this->extName->ConcatTo(currName);
		break;
	case IO::ILogHandler::LOG_GROUP_TYPE_NO_GROUP:
	default:
		currName = this->fileName->ConcatTo(buff);
		break;
	}

	switch (this->logStyle)
	{
	case IO::ILogHandler::LOG_TYPE_PER_HOUR:
		if (lastVal) *lastVal = time->GetDay() * 24 + time->GetHour();
		currName = Text::StrConcatC(time->ToString(currName, "yyyyMMddHH"), UTF8STRC(".log"));
		break;
	case IO::ILogHandler::LOG_TYPE_PER_DAY:
		if (lastVal) *lastVal = time->GetDay();
		currName = Text::StrConcatC(time->ToString(currName, "yyyyMMdd"), UTF8STRC(".log"));
		break;
	case IO::ILogHandler::LOG_TYPE_PER_MONTH:
		if (lastVal) *lastVal = time->GetMonth();
		currName = Text::StrConcatC(time->ToString(currName, "yyyyMM"), UTF8STRC(".log"));
		break;
	case IO::ILogHandler::LOG_TYPE_PER_YEAR:
		if (lastVal) *lastVal = time->GetYear();
		currName = Text::StrConcatC(time->ToString(currName, "yyyy"), UTF8STRC(".log"));
		break;
	case IO::ILogHandler::LOG_TYPE_SINGLE_FILE:
	default:
		break;
	}
	return currName;
}

void IO::MTFileLog::WriteArr(Text::String **msgArr, Int64 *dateArr, UOSInt arrCnt)
{
	Bool newFile = false;
	UTF8Char buff[256];
	UTF8Char *sptr;
	Data::DateTime time;
	Text::StringBuilderUTF8 sb;

	UOSInt i;
	i = 0;
	while (i < arrCnt)
	{
		time.SetTicks(dateArr[i]);
		time.ToLocalTime();
		switch (logStyle)
		{
		case ILogHandler::LOG_TYPE_PER_DAY:
			if (time.GetDay() != lastVal)
			{
				newFile = true;
				sptr = GetNewName(buff, &time, &lastVal);
			}
			break;
		case ILogHandler::LOG_TYPE_PER_MONTH:
			if (time.GetMonth() != lastVal)
			{
				newFile = true;
				sptr = GetNewName(buff, &time, &lastVal);
			}
			break;
		case ILogHandler::LOG_TYPE_PER_YEAR:
			if (time.GetYear() != lastVal)
			{
				newFile = true;
				sptr = GetNewName(buff, &time, &lastVal);
			}
			break;
		case ILogHandler::LOG_TYPE_PER_HOUR:
			if (lastVal != (time.GetDay() * 24 + time.GetHour()))
			{
				newFile = true;
				sptr = GetNewName(buff, &time, &lastVal);
			}
			break;
		case ILogHandler::LOG_TYPE_SINGLE_FILE:
		default:
			break;
		}

		if (newFile)
		{
			log->Close();
			DEL_CLASS(log);
			DEL_CLASS(cstm);
			DEL_CLASS(fileStm);

			NEW_CLASS(fileStm, FileStream({buff, (UOSInt)(sptr - buff)}, IO::FileMode::Append, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
			NEW_CLASS(cstm, IO::BufferedOutputStream(fileStm, 4096));
			NEW_CLASS(log, Text::UTF8Writer(cstm));
			log->WriteSignature();

			sptr = Text::StrConcatC(time.ToString(buff, this->dateFormat), UTF8STRC("Program running"));
			log->WriteLineC(buff, (UOSInt)(sptr - buff));
			newFile = false;
			this->hasNewFile = true;
		}

		sptr = time.ToString(buff, this->dateFormat);
		sb.ClearStr();
		sb.AppendC(buff, (UOSInt)(sptr - buff));
		sb.Append(msgArr[i]);
		log->WriteLineC(sb.ToString(), sb.GetLength());

		msgArr[i]->Release();
		i++;
	}
	if (cstm)
	{
		cstm->Flush();
	}
}

UInt32 __stdcall IO::MTFileLog::FileThread(void *userObj)
{
	IO::MTFileLog *me = (IO::MTFileLog*)userObj;
	UOSInt arrCnt;
	Text::String **msgArr = 0;
	Int64 *dateArr = 0;
	me->running = true;
	while (!me->closed)
	{
		Sync::MutexUsage mutUsage(me->mut);
		if ((arrCnt = me->msgList->GetCount()) > 0)
		{
			msgArr = MemAlloc(Text::String *, arrCnt);
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
		msgArr = MemAlloc(Text::String *, arrCnt);
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

void IO::MTFileLog::Init(LogType style, LogGroup groupStyle, const Char *dateFormat)
{
	UTF8Char buff[256];
	UTF8Char *sptr;
	Char cbuff[256];
	UOSInt i;
	NEW_CLASS(mut, Sync::Mutex());
	NEW_CLASS(evt, Sync::Event(true));
	NEW_CLASS(dateList, Data::ArrayListInt64());
	NEW_CLASS(msgList, Data::ArrayListString());
	if (dateFormat == 0)
	{
		this->dateFormat = Text::StrCopyNew("yyyy-MM-dd HH:mm:ss\t");
	}
	else
	{
		Text::StrConcatC(Text::StrConcat(cbuff, dateFormat), "\t", 1);
		this->dateFormat = Text::StrCopyNew(cbuff);
	}
	this->logStyle = style;
	this->groupStyle = groupStyle;
	this->closed = false;
	this->hasNewFile = false;


	if (this->groupStyle != IO::ILogHandler::LOG_GROUP_TYPE_NO_GROUP)
	{
		i = this->fileName->LastIndexOf(IO::Path::PATH_SEPERATOR);
		this->extName = Text::String::New(this->fileName->ToCString().Substring(i + 1));
	}
	else
	{
		this->extName = 0;
		i = this->fileName->LastIndexOf(IO::Path::PATH_SEPERATOR);
		if (i != INVALID_INDEX)
		{
			this->fileName->v[i] = 0;
			if (!IO::Path::IsDirectoryExist(this->fileName->ToCString()))
			{
				IO::Path::CreateDirectory(this->fileName->ToCString());
			}
			this->fileName->v[i] = IO::Path::PATH_SEPERATOR;
		}
	}

	Data::DateTime dt;
	dt.SetCurrTime();
	sptr = GetNewName(buff, &dt, &this->lastVal);

	NEW_CLASS(fileStm, FileStream({buff, (UOSInt)(sptr - buff)}, IO::FileMode::Append, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
	NEW_CLASS(cstm, IO::BufferedOutputStream(fileStm, 4096));
	NEW_CLASS(log, Text::UTF8Writer(cstm));
	if (fileStm->GetPosition() == 0)
	{
		this->hasNewFile = true;
	}
	log->WriteSignature();
	Sync::Thread::Create(FileThread, this, 0x20000);
}

IO::MTFileLog::MTFileLog(Text::String *fileName, LogType style, LogGroup groupStyle, const Char *dateFormat)
{
	this->fileName = fileName->Clone();
	this->Init(style, groupStyle, dateFormat);
}

IO::MTFileLog::MTFileLog(Text::CString fileName, LogType style, LogGroup groupStyle, const Char *dateFormat)
{
	this->fileName = Text::String::New(fileName);
	this->Init(style, groupStyle, dateFormat);
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

	fileName->Release();
	fileName = 0;
	SDEL_STRING(this->extName);

	SDEL_CLASS(log);
	SDEL_CLASS(cstm);
	SDEL_CLASS(fileStm);

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
void IO::MTFileLog::LogAdded(Data::DateTime *time, Text::CString logMsg, LogLevel logLev)
{
	if (closed)
		return;

	Sync::MutexUsage mutUsage(mut);
	this->msgList->Add(Text::String::New(logMsg));
	this->dateList->Add(time->ToTicks());
	mutUsage.EndUse();
	evt->Set();
}

Bool IO::MTFileLog::HasNewFile()
{
	return this->hasNewFile;
}
UTF8Char *IO::MTFileLog::GetLastFileName(UTF8Char *sbuff)
{
	this->hasNewFile = false;
	Data::DateTime dt;
	dt.SetCurrTime();
	switch (this->logStyle)
	{
	case IO::ILogHandler::LOG_TYPE_PER_HOUR:
		dt.AddHour(-1);
		break;
	case IO::ILogHandler::LOG_TYPE_PER_DAY:
		dt.AddDay(-1);
		break;
	case IO::ILogHandler::LOG_TYPE_PER_MONTH:
		dt.AddMonth(-1);
		break;
	case IO::ILogHandler::LOG_TYPE_PER_YEAR:
		dt.AddMonth(-12);
		break;
	case IO::ILogHandler::LOG_TYPE_SINGLE_FILE:
	default:
		break;
	}

	return GetNewName(sbuff, &dt, 0);
}
