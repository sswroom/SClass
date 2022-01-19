#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "Exporter/MD5Exporter.h"
#include "IO/ConsoleWriter.h"
#include "IO/FileCheck.h"
#include "IO/FileStream.h"
#include "IO/Path.h"
#include "IO/StmData/FileData.h"
#include "Manage/HiResClock.h"
#include "Parser/FileParser/MD5Parser.h"
#include "Sync/Event.h"
#include "Sync/Mutex.h"
#include "Sync/MutexUsage.h"
#include "Sync/Thread.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"

IO::ConsoleWriter *console;
Bool showHelp;

class ProgressHandler : public IO::IProgressHandler
{
private:
	Sync::Mutex *mut;
	Text::String *name;
	const UTF8Char *fileName;
	UInt64 currCount;
	UInt64 lastCount;

	Bool threadRunning;
	Bool threadToStop;
	Sync::Event *evt;

	static UInt32 __stdcall CheckThread(void *userObj)
	{
		ProgressHandler *me = (ProgressHandler*)userObj;
		UInt64 lastDispCount = 0;
		Double lastDispTime = 0;
		UInt64 currCount;
		Double currTime;
		Manage::HiResClock *clk;
		Text::StringBuilderUTF8 *sb;
		NEW_CLASS(clk, Manage::HiResClock());
		NEW_CLASS(sb, Text::StringBuilderUTF8());

		me->threadRunning = true;
		while (!me->threadToStop)
		{
			me->evt->Wait(1000);

			currCount = me->currCount;
			currTime = clk->GetTimeDiff();

			sb->ClearStr();
			sb->AppendC(UTF8STRC("\rSpeed: "));
			if (currCount == lastDispCount || currTime == lastDispTime)
			{
				sb->AppendC(UTF8STRC("0"));
			}
			else
			{
				Text::SBAppendF64(sb, (Double)(currCount - lastDispCount)  / (currTime - lastDispTime));
				lastDispCount = currCount;
				lastDispTime = currTime;
			}
			sb->AppendC(UTF8STRC("Bytes/s"));
			Sync::MutexUsage mutUsage(me->mut);
			if (me->fileName)
			{
				sb->AppendC(UTF8STRC(" ("));
				sb->Append(me->fileName);
				sb->AppendC(UTF8STRC(")"));
			}
			mutUsage.EndUse();
			console->WriteStrC(sb->ToString(), sb->GetLength());
		}
		DEL_CLASS(clk);
		DEL_CLASS(sb);
		me->threadRunning = false;
		return 0;
	}
public:
	ProgressHandler()
	{
		NEW_CLASS(mut, Sync::Mutex());
		this->currCount = 0;
		this->lastCount = 0;
		this->name = 0;
		this->fileName = 0;

		this->threadRunning = false;
		this->threadToStop = false;
		NEW_CLASS(this->evt, Sync::Event(true, (const UTF8Char*)"ProgressHandler.evt"));
		Sync::Thread::Create(CheckThread, this);
		while (!this->threadRunning)
		{
			Sync::Thread::Sleep(1);
		}
	}

	virtual ~ProgressHandler()
	{
		this->threadToStop = true;
		this->evt->Set();
		while (this->threadRunning)
		{
			Sync::Thread::Sleep(1);
		}
		DEL_CLASS(this->evt);
		SDEL_STRING(this->name);
		this->fileName = 0;
		DEL_CLASS(mut);
	}

	virtual void ProgressStart(const UTF8Char *name, UInt64 count)
	{
		UOSInt i;
		Sync::MutexUsage mutUsage(this->mut);
		SDEL_STRING(this->name);
		this->name = Text::String::NewNotNull(name);
		i = Text::StrLastIndexOf(this->name->v, IO::Path::PATH_SEPERATOR);
		this->fileName = &this->name->v[i + 1];
		this->lastCount = 0;
	}

	virtual void ProgressUpdate(UInt64 currCount, UInt64 newCount)
	{
		this->currCount += currCount - this->lastCount;
		this->lastCount = currCount;
	}

	virtual void ProgressEnd()
	{
		Sync::MutexUsage mutUsage(this->mut);
		SDEL_STRING(this->name);
		this->fileName = 0;
	}
};

Bool ParseFile(const UTF8Char *fileName, UOSInt fileNameLen)
{
	if (Text::StrEndsWithICaseC(fileName, fileNameLen, UTF8STRC(".MD5")))
	{
		Parser::FileParser::MD5Parser parser;
		Bool succ = true;
		IO::StmData::FileData *fd;
		IO::FileCheck *fileChk;
		NEW_CLASS(fd, IO::StmData::FileData(fileName, false));
		fileChk = (IO::FileCheck *)parser.ParseFile(fd, 0, IO::ParserType::FileCheck);
		DEL_CLASS(fd);
		if (fileChk == 0)
		{
			console->WriteLineC(UTF8STRC("Error in parsing the file"));
		}
		else
		{
			UInt8 hash[16];
			UOSInt i;
			UOSInt j;
			
			i = 0;
			j = fileChk->GetCount();
			while (i < j)
			{
				if (!fileChk->CheckEntryHash(i, hash))
				{
					succ = false;
					console->WriteLineC(UTF8STRC("File validation failed"));
					break;
				}
			}
			showHelp = false;
			DEL_CLASS(fileChk);
		}
		return succ;
	}
	else
	{
		ProgressHandler *progress;
		NEW_CLASS(progress, ProgressHandler());
		IO::FileCheck *fileChk = IO::FileCheck::CreateCheck(fileName, IO::FileCheck::CheckType::MD5, progress, false);
		DEL_CLASS(progress);
		console->WriteLine();
		if (fileChk)
		{
			Text::StringBuilderUTF8 sb;
			IO::FileStream *fs;
			Exporter::MD5Exporter exporter;
			sb.Append(fileName);
			sb.AppendC(UTF8STRC(".md5"));
			NEW_CLASS(fs, IO::FileStream(sb.ToString(), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
			exporter.ExportFile(fs, sb.ToString(), fileChk, 0);
			DEL_CLASS(fs);
			DEL_CLASS(fileChk);
			showHelp = false;
			return true;
		}
		else
		{
			console->WriteLineC(UTF8STRC("Error in calculating MD5"));
			return false;
		}
	}
}

Int32 MyMain(Core::IProgControl *progCtrl)
{
	UOSInt cmdCnt;
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	UTF8Char **cmdLines = progCtrl->GetCommandLines(progCtrl, &cmdCnt);
	NEW_CLASS(console, IO::ConsoleWriter());
	showHelp = true;
	if (cmdCnt == 2)
	{
		UOSInt i = Text::StrIndexOf(cmdLines[1], '*');
		UOSInt j = Text::StrIndexOf(cmdLines[1], '?');
		if (i == INVALID_INDEX && j == INVALID_INDEX)
		{
			IO::Path::PathType pt = IO::Path::GetPathType(cmdLines[1]);
			if (pt == IO::Path::PathType::Unknown)
			{
				console->WriteLineC(UTF8STRC("File not found"));
			}
			else
			{
				ParseFile(cmdLines[1], Text::StrCharCnt(cmdLines[1]));
			}
		}
		else
		{
			IO::Path::FindFileSession *sess;
			Text::StringBuilderUTF8 sb;
			IO::Path::PathType pt;
			Text::StrConcat(sbuff, cmdLines[1]);
			i = Text::StrLastIndexOf(sbuff, IO::Path::PATH_SEPERATOR);
			sess = IO::Path::FindFile(sbuff);
			if (sess)
			{
				while (sptr = IO::Path::FindNextFile(&sbuff[i + 1], sess, 0, &pt, 0))
				{
					sb.ClearStr();
					sb.AppendC(UTF8STRC("Checking "));
					sb.Append(&sbuff[i + 1]);
					console->WriteLineC(sb.ToString(), sb.GetLength());
					ParseFile(sbuff, (UOSInt)(sptr - sbuff));
					console->WriteLine();
				}
				IO::Path::FindFileClose(sess);
			}
		}
	}
	if (showHelp)
	{
		console->WriteLineC(UTF8STRC("Usage: SMD5 [File to check]"));
	}
	DEL_CLASS(console);
	return 0;
}
