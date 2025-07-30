#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/DateTime.h"
#include "IO/BuildTime.h"
#include "IO/Path.h"
#include "IO/MTFileLog.h"
#include "Sync/MutexUsage.h"
#include "Sync/SimpleThread.h"
#include "Sync/ThreadUtil.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"

UnsafeArray<UTF8Char> IO::MTFileLog::GetNewName(UnsafeArray<UTF8Char> buff, NN<Data::DateTimeUtil::TimeValue> time, UInt32 nanosec, OptOut<Int32> lastVal)
{
	UnsafeArray<UTF8Char> currName;
	NN<Text::String> extName;
	switch (this->groupStyle)
	{
	case IO::LogHandler::LogGroup::PerYear:
		currName = Data::DateTimeUtil::ToString(this->fileName->ConcatTo(buff), time, 0, nanosec, (const UTF8Char*)"yyyy");
		if (!IO::Path::IsDirectoryExist(CSTRP(buff, currName)))
		{
			IO::Path::CreateDirectory(CSTRP(buff, currName));
		}
		*currName++ = IO::Path::PATH_SEPERATOR;
		if (this->extName.SetTo(extName))
			currName = extName->ConcatTo(currName);
		break;
	case IO::LogHandler::LogGroup::PerMonth:
		currName = Data::DateTimeUtil::ToString(this->fileName->ConcatTo(buff), time, 0, nanosec, (const UTF8Char*)"yyyyMM");
		if (!IO::Path::IsDirectoryExist(CSTRP(buff, currName)))
		{
			IO::Path::CreateDirectory(CSTRP(buff, currName));
		}
		*currName++ = IO::Path::PATH_SEPERATOR;
		if (this->extName.SetTo(extName))
			currName = extName->ConcatTo(currName);
		break;
	case IO::LogHandler::LogGroup::PerDay:
		currName = Data::DateTimeUtil::ToString(this->fileName->ConcatTo(buff), time, 0, nanosec, (const UTF8Char*)"yyyyMMdd");
		if (!IO::Path::IsDirectoryExist(CSTRP(buff, currName)))
		{
			IO::Path::CreateDirectory(CSTRP(buff, currName));
		}
		*currName++ = IO::Path::PATH_SEPERATOR;
		if (this->extName.SetTo(extName))
			currName = extName->ConcatTo(currName);
		break;
	case IO::LogHandler::LogGroup::NoGroup:
	default:
		currName = this->fileName->ConcatTo(buff);
		break;
	}

	switch (this->logStyle)
	{
	case IO::LogHandler::LogType::PerHour:
		lastVal.Set(time->day * 24 + time->hour);
		currName = Text::StrConcatC(Data::DateTimeUtil::ToString(currName, time, 0, nanosec, (const UTF8Char*)"yyyyMMddHH"), UTF8STRC(".log"));
		break;
	case IO::LogHandler::LogType::PerDay:
		lastVal.Set(time->day);
		currName = Text::StrConcatC(Data::DateTimeUtil::ToString(currName, time, 0, nanosec, (const UTF8Char*)"yyyyMMdd"), UTF8STRC(".log"));
		break;
	case IO::LogHandler::LogType::PerMonth:
		lastVal.Set(time->month);
		currName = Text::StrConcatC(Data::DateTimeUtil::ToString(currName, time, 0, nanosec, (const UTF8Char*)"yyyyMM"), UTF8STRC(".log"));
		break;
	case IO::LogHandler::LogType::PerYear:
		lastVal.Set(time->year);
		currName = Text::StrConcatC(Data::DateTimeUtil::ToString(currName, time, 0, nanosec, (const UTF8Char*)"yyyy"), UTF8STRC(".log"));
		break;
	case IO::LogHandler::LogType::SingleFile:
	default:
		break;
	}
	return currName;
}

void IO::MTFileLog::WriteArr(UnsafeArray<NN<Text::String>> msgArr, UnsafeArray<Data::Timestamp> dateArr, UOSInt arrCnt)
{
	Bool newFile = false;
	UTF8Char buff[256];
	UnsafeArray<UTF8Char> sptr;
	Data::Timestamp time;
	Data::DateTimeUtil::TimeValue tval;
	Text::StringBuilderUTF8 sb;

	UOSInt i;
	i = 0;
	while (i < arrCnt)
	{
		time = dateArr[i].ToLocalTime();
		Data::DateTimeUtil::Instant2TimeValue(time.inst.sec, time.inst.nanosec, tval, time.tzQhr);
		switch (logStyle)
		{
		case LogHandler::LogType::PerDay:
			if (tval.day != lastVal)
			{
				newFile = true;
				sptr = GetNewName(buff, tval, time.inst.nanosec, lastVal);
			}
			break;
		case LogHandler::LogType::PerMonth:
			if (tval.month != lastVal)
			{
				newFile = true;
				sptr = GetNewName(buff, tval, time.inst.nanosec, lastVal);
			}
			break;
		case LogHandler::LogType::PerYear:
			if (tval.year != lastVal)
			{
				newFile = true;
				sptr = GetNewName(buff, tval, time.inst.nanosec, lastVal);
			}
			break;
		case LogHandler::LogType::PerHour:
			if (lastVal != (tval.day * 24 + tval.hour))
			{
				newFile = true;
				sptr = GetNewName(buff, tval, time.inst.nanosec, lastVal);
			}
			break;
		case LogHandler::LogType::SingleFile:
		default:
			break;
		}

		if (newFile)
		{
			log->Close();
			log.Delete();
			cstm.Delete();
			fileStm.Delete();

			NEW_CLASSNN(fileStm, FileStream({buff, (UOSInt)(sptr - buff)}, IO::FileMode::Append, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
			NEW_CLASSNN(cstm, IO::BufferedOutputStream(fileStm, 4096));
			NEW_CLASSNN(log, Text::UTF8Writer(cstm));
			log->WriteSignature();

			sptr = IO::BuildTime::GetBuildTime().ToLocalTime().ToStringNoZone(Text::StrConcatC(time.ToString(buff, UnsafeArray<const Char>::ConvertFrom(this->dateFormat)), UTF8STRC("Program running: ")));
			log->WriteLine(CSTRP(buff, sptr));
			newFile = false;
			this->hasNewFile = true;
		}

		sptr = time.ToString(buff, UnsafeArray<const Char>::ConvertFrom(this->dateFormat));
		sb.ClearStr();
		sb.AppendC(buff, (UOSInt)(sptr - buff));
		sb.Append(msgArr[i]);
		log->WriteLine(sb.ToCString());

		msgArr[i]->Release();
		i++;
	}
	cstm->Flush();
}

UInt32 __stdcall IO::MTFileLog::FileThread(AnyType userObj)
{
	NN<IO::MTFileLog> me = userObj.GetNN<IO::MTFileLog>();
	Sync::ThreadUtil::SetName(CSTR("MTFileLog"));
	UOSInt arrCnt;
	UnsafeArray<NN<Text::String>> msgArr;
	UnsafeArray<Data::Timestamp> dateArr;
	while (!me->closed)
	{
		Sync::MutexUsage mutUsage(me->mut);
		if ((arrCnt = me->msgList.GetCount()) > 0)
		{
			msgArr = MemAllocArr(NN<Text::String>, arrCnt);
			dateArr = MemAllocArr(Data::Timestamp, arrCnt);
			me->msgList.GetRange(msgArr, 0, arrCnt);
			me->dateList.GetRange(dateArr, 0, arrCnt);
			me->msgList.RemoveRange(0, arrCnt);
			me->dateList.RemoveRange(0, arrCnt);
			mutUsage.EndUse();
			me->WriteArr(msgArr, dateArr, arrCnt);
			MemFreeArr(msgArr);
			MemFreeArr(dateArr);
		}
		mutUsage.EndUse();
		me->evt.Wait(1000);
	}

	if ((arrCnt = me->msgList.GetCount()) > 0)
	{
		msgArr = MemAllocArr(NN<Text::String>, arrCnt);
		dateArr = MemAllocArr(Data::Timestamp, arrCnt);
		me->msgList.GetRange(msgArr, 0, arrCnt);
		me->dateList.GetRange(dateArr, 0, arrCnt);
		me->msgList.RemoveRange(0, arrCnt);
		me->dateList.RemoveRange(0, arrCnt);
		me->WriteArr(msgArr, dateArr, arrCnt);
		MemFreeArr(msgArr);
		MemFreeArr(dateArr);
	}

	me->running = false;
	return 0;
}

void IO::MTFileLog::Init(LogType style, LogGroup groupStyle, UnsafeArrayOpt<const Char> dateFormat)
{
	UTF8Char buff[256];
	UnsafeArray<UTF8Char> sptr;
	UOSInt i;
	UnsafeArray<const Char> nndateFormat;
	if (!dateFormat.SetTo(nndateFormat))
	{
		this->dateFormat = Text::StrCopyNewC(UTF8STRC("yyyy-MM-dd HH:mm:ss\t"));
	}
	else
	{
		sptr = Text::StrConcatC(Text::StrConcat(buff, UnsafeArray<const UTF8Char>::ConvertFrom(nndateFormat)), UTF8STRC("\t"));
		this->dateFormat = Text::StrCopyNewC(buff, (UOSInt)(sptr - buff));
	}
	this->logStyle = style;
	this->groupStyle = groupStyle;
	this->closed = false;
	this->hasNewFile = false;


	if (this->groupStyle != IO::LogHandler::LogGroup::NoGroup)
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

	Data::Timestamp ts = Data::Timestamp::Now();
	Data::DateTimeUtil::TimeValue tval;
	Data::DateTimeUtil::Instant2TimeValue(ts.inst.sec, ts.inst.nanosec, tval, ts.tzQhr);
	sptr = GetNewName(buff, tval, ts.inst.nanosec, this->lastVal);

	NEW_CLASSNN(fileStm, FileStream({buff, (UOSInt)(sptr - buff)}, IO::FileMode::Append, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
	NEW_CLASSNN(cstm, IO::BufferedOutputStream(fileStm, 4096));
	NEW_CLASSNN(log, Text::UTF8Writer(cstm));
	if (fileStm->GetPosition() == 0)
	{
		this->hasNewFile = true;
	}
	log->WriteSignature();
	this->running = true;
	Sync::ThreadUtil::Create(FileThread, this, 0x20000);
}

IO::MTFileLog::MTFileLog(NN<Text::String> fileName, LogType style, LogGroup groupStyle, UnsafeArrayOpt<const Char> dateFormat)
{
	this->fileName = fileName->Clone();
	this->Init(style, groupStyle, dateFormat);
}

IO::MTFileLog::MTFileLog(Text::CStringNN fileName, LogType style, LogGroup groupStyle, UnsafeArrayOpt<const Char> dateFormat)
{
	this->fileName = Text::String::New(fileName);
	this->Init(style, groupStyle, dateFormat);
}

IO::MTFileLog::~MTFileLog()
{
	if (!closed)
	{
		closed = true;
		this->evt.Set();
	}

	while (running)
	{
		Sync::SimpleThread::Sleep(10);
	}
	Sync::MutexUsage mutUsage(this->mut);
	log->Close();
	mutUsage.EndUse();

	this->fileName->Release();
	OPTSTR_DEL(this->extName);

	log.Delete();
	cstm.Delete();
	fileStm.Delete();

	Text::StrDelNew(this->dateFormat);
}

void IO::MTFileLog::LogClosed()
{
	if (!closed)
	{
		closed = true;
		this->evt.Set();
	}
}
void IO::MTFileLog::LogAdded(const Data::Timestamp &time, Text::CStringNN logMsg, LogLevel logLev)
{
	if (closed)
		return;

	Sync::MutexUsage mutUsage(this->mut);
	this->msgList.Add(Text::String::New(logMsg));
	this->dateList.Add(time);
	mutUsage.EndUse();
	this->evt.Set();
}

Bool IO::MTFileLog::HasNewFile()
{
	return this->hasNewFile;
}
UnsafeArray<UTF8Char> IO::MTFileLog::GetLastFileName(UnsafeArray<UTF8Char> sbuff)
{
	this->hasNewFile = false;
	Data::Timestamp ts = Data::Timestamp::Now();
	Data::DateTimeUtil::TimeValue tval;
	switch (this->logStyle)
	{
	case IO::LogHandler::LogType::PerHour:
		ts = ts.AddHour(-1);
		Data::DateTimeUtil::Instant2TimeValue(ts.inst.sec, ts.inst.nanosec, tval, ts.tzQhr);
		break;
	case IO::LogHandler::LogType::PerDay:
		ts = ts.AddDay(-1);
		Data::DateTimeUtil::Instant2TimeValue(ts.inst.sec, ts.inst.nanosec, tval, ts.tzQhr);
		break;
	case IO::LogHandler::LogType::PerMonth:
		Data::DateTimeUtil::Instant2TimeValue(ts.inst.sec, ts.inst.nanosec, tval, ts.tzQhr);
		tval.month = (UInt8)(tval.month - 1);
		if (tval.month == 0)
		{
			tval.month = 12;
			tval.year--;
		}
		break;
	case IO::LogHandler::LogType::PerYear:
		Data::DateTimeUtil::Instant2TimeValue(ts.inst.sec, ts.inst.nanosec, tval, ts.tzQhr);
		tval.year--;
		break;
	case IO::LogHandler::LogType::SingleFile:
	default:
		Data::DateTimeUtil::Instant2TimeValue(ts.inst.sec, ts.inst.nanosec, tval, ts.tzQhr);
		break;
	}

	return GetNewName(sbuff, tval, ts.inst.nanosec, 0);
}
