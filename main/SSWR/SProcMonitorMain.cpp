#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "IO/FileStream.h"
#include "IO/LogTool.h"
#include "IO/Path.h"
#include "Manage/ExceptionRecorder.h"
#include "Manage/Process.h"
#include "Sync/Thread.h"
#include "Text/UTF8Reader.h"

typedef struct
{
	const UTF8Char *progName;
	const UTF8Char *progPath;
	UOSInt procId;
} ProgInfo;

IO::LogTool *myLog;
Data::ArrayList<ProgInfo*> *progList;
Bool threadRunning;
Bool threadToStop;
Sync::Event *threadEvt;

Bool SearchProcId(ProgInfo *prog)
{
	if (prog->progPath == 0)
		return false;

	UTF8Char sbuff[512];
	UOSInt i;
	Bool ret = false;
	Manage::Process::ProcessInfo info;
	i = Text::StrLastIndexOf(prog->progPath, IO::Path::PATH_SEPERATOR);
	Manage::Process::FindProcSess *sess = Manage::Process::FindProcess(&prog->progPath[i + 1]);
	if (sess)
	{
		Text::StringBuilderUTF8 sb;
		while (Manage::Process::FindProcessNext(sbuff, sess, &info))
		{
			Manage::Process proc(info.processId, false);
			sb.ClearStr();
			if (proc.GetFilename(&sb))
			{
				if (sb.Equals(prog->progPath))
				{
					Text::StringBuilderUTF8 sb;
					prog->procId = info.processId;
					ret = true;
					sb.AppendC(UTF8STRC("Prog "));
					sb.Append(prog->progName);
					sb.AppendC(UTF8STRC(": Updated procId as "));
					sb.AppendUOSInt(prog->procId);
					myLog->LogMessageC(sb.ToString(), sb.GetLength(), IO::ILogHandler::LOG_LEVEL_COMMAND);
					break;
				}
			}
		}
		Manage::Process::FindProcessClose(sess);
	}
	return ret;
}

void AddProg(const UTF8Char *progName, const UTF8Char *progPath)
{
	ProgInfo *prog;
	prog = MemAlloc(ProgInfo, 1);
	prog->progName = Text::StrCopyNew(progName);
	prog->procId = 0;
	if (progPath)
	{
		prog->progPath = Text::StrCopyNew(progPath);
	}
	else
	{
		prog->progPath = 0;
	}
	progList->Add(prog);

	if (progPath)
	{
		SearchProcId(prog);
	}
}

void LoadProgList()
{
	UTF8Char sbuff[512];
	UTF8Char *sarr[2];
	IO::FileStream *fs;
	Text::UTF8Reader *reader;
	Text::StringBuilderUTF8 sb;

	IO::Path::GetProcessFileName(sbuff);
	IO::Path::ReplaceExt(sbuff, (const UTF8Char*)"prg");
	NEW_CLASS(fs, IO::FileStream(sbuff, IO::FileMode::ReadOnly, IO::FileShare::DenyAll, IO::FileStream::BufferType::Normal));
	if (!fs->IsError())
	{
		NEW_CLASS(reader, Text::UTF8Reader(fs));
		
		while (true)
		{
			sb.ClearStr();
			if (!reader->ReadLine(&sb, 4096))
				break;
			if (Text::StrSplit(sarr, 2, sb.ToString(), ',') == 2)
			{
				if (sarr[1][0])
				{
					AddProg(sarr[0], sarr[1]);
				}
				else
				{
					AddProg(sarr[0], sarr[1]);
				}
			}
		}
		DEL_CLASS(reader);
	}
	DEL_CLASS(fs);
}

void __stdcall OnTimerTick(void *userObj)
{
	UOSInt i;
	ProgInfo *prog;
	i = progList->GetCount();
	while (i-- > 0)
	{
		prog = progList->GetItem(i);
		if (prog->progPath != 0)
		{
			if (prog->procId != 0)
			{
				Manage::Process proc(prog->procId, false);
				if (!proc.IsRunning())
				{
					prog->procId = 0;
					Text::StringBuilderUTF8 sb;
					sb.AppendC(UTF8STRC("Prog "));
					sb.Append(prog->progName);
					sb.AppendC(UTF8STRC(" stopped"));
					myLog->LogMessageC(sb.ToString(), sb.GetLength(), IO::ILogHandler::LOG_LEVEL_COMMAND);
				}
			}
			if (prog->procId == 0)
			{
				if (!SearchProcId(prog))
				{
					Manage::Process proc(prog->progPath);
					if (proc.IsRunning())
					{
						prog->procId = proc.GetProcId();
						Text::StringBuilderUTF8 sb;
						sb.AppendC(UTF8STRC("Prog "));
						sb.Append(prog->progName);
						sb.AppendC(UTF8STRC(" restarted, procId = "));
						sb.AppendUOSInt(prog->procId);
						myLog->LogMessageC(sb.ToString(), sb.GetLength(), IO::ILogHandler::LOG_LEVEL_COMMAND);
					}
				}
			}
		}
	}
}

static UInt32 __stdcall CheckThread(void *userObj)
{
	threadRunning = true;
	while (!threadToStop)
	{
		OnTimerTick(userObj);
		threadEvt->Wait(30000);
	}
	threadRunning = false;
	return 0;
}

Int32 MyMain(Core::IProgControl *progCtrl)
{
	Text::StringBuilderUTF8 sb;
	sb.AppendC(UTF8STRC("Log"));
	sb.AppendChar(IO::Path::PATH_SEPERATOR, 1);
	sb.AppendC(UTF8STRC("ProgLog"));
	NEW_CLASS(myLog, IO::LogTool());
	myLog->AddFileLog(sb.ToString(), IO::ILogHandler::LOG_TYPE_PER_DAY, IO::ILogHandler::LOG_GROUP_TYPE_PER_MONTH, IO::ILogHandler::LOG_LEVEL_RAW, "yyyy-MM-dd HH:mm:ss.fff", false);
	NEW_CLASS(progList, Data::ArrayList<ProgInfo*>());

	LoadProgList();

	if (progList->GetCount() > 0)
	{
		NEW_CLASS(threadEvt, Sync::Event(true, (const UTF8Char*)"threadEvt"));
		threadRunning = false;
		threadToStop = false;
		Sync::Thread::Create(CheckThread, 0);

		progCtrl->WaitForExit(progCtrl);

		threadToStop = true;
		threadEvt->Set();
		while (threadRunning)
		{
			Sync::Thread::Sleep(1);
		}
		DEL_CLASS(threadEvt);
	}


	ProgInfo *prog;
	UOSInt i = progList->GetCount();
	while (i-- > 0)
	{
		prog = progList->GetItem(i);
		SDEL_TEXT(prog->progPath);
		SDEL_TEXT(prog->progName);
		MemFree(prog);
	}
	DEL_CLASS(progList);
	DEL_CLASS(myLog);
	return 0;
}
