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
	const UTF8Char *name;
	const UTF8Char *fileName;
	Int64 currCount;
	Int64 lastCount;

	Bool threadRunning;
	Bool threadToStop;
	Sync::Event *evt;

	static UInt32 __stdcall CheckThread(void *userObj)
	{
		ProgressHandler *me = (ProgressHandler*)userObj;
		Int64 lastDispCount = 0;
		Double lastDispTime = 0;
		Int64 currCount;
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
			sb->Append((const UTF8Char*)"\rSpeed: ");
			if (currCount == lastDispCount || currTime == lastDispTime)
			{
				sb->Append((const UTF8Char*)"0");
			}
			else
			{
				Text::SBAppendF64(sb, (currCount - lastDispCount)  / (currTime - lastDispTime));
				lastDispCount = currCount;
				lastDispTime = currTime;
			}
			sb->Append((const UTF8Char*)"Bytes/s");
			me->mut->Lock();
			if (me->fileName)
			{
				sb->Append((const UTF8Char*)" (");
				sb->Append(me->fileName);
				sb->Append((const UTF8Char*)")");
			}
			me->mut->Unlock();
			console->Write(sb->ToString());
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
		SDEL_TEXT(this->name);
		this->fileName = 0;
		DEL_CLASS(mut);
	}

	virtual void ProgressStart(const UTF8Char *name, Int64 count)
	{
		OSInt i;
		this->mut->Lock();
		SDEL_TEXT(this->name);
		this->name = Text::StrCopyNew(name);
		i = Text::StrLastIndexOf(this->name, IO::Path::PATH_SEPERATOR);
		this->fileName = &this->name[i + 1];
		this->lastCount = 0;
		this->mut->Unlock();
	}

	virtual void ProgressUpdate(Int64 currCount, Int64 newCount)
	{
		this->currCount += currCount - this->lastCount;
		this->lastCount = currCount;
	}

	virtual void ProgressEnd()
	{
		this->mut->Lock();
		SDEL_TEXT(this->name);
		this->fileName = 0;
		this->mut->Unlock();
	}
};

Bool ParseFile(const UTF8Char *fileName)
{
	if (Text::StrEndsWithICase(fileName, (const UTF8Char*)".MD5"))
	{
		Parser::FileParser::MD5Parser parser;
		Bool succ = true;
		IO::StmData::FileData *fd;
		IO::FileCheck *fileChk;
		NEW_CLASS(fd, IO::StmData::FileData(fileName, false));
		fileChk = (IO::FileCheck *)parser.ParseFile(fd, 0, IO::ParsedObject::PT_FILE_CHECK);
		DEL_CLASS(fd);
		if (fileChk == 0)
		{
			console->WriteLine((const UTF8Char*)"Error in parsing the file");
		}
		else
		{
			UInt8 hash[16];
			OSInt i;
			OSInt j;
			
			i = 0;
			j = fileChk->GetCount();
			while (i < j)
			{
				if (!fileChk->CheckEntryHash(i, hash))
				{
					succ = false;
					console->WriteLine((const UTF8Char*)"File validation failed");
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
		IO::FileCheck *fileChk = IO::FileCheck::CreateCheck(fileName, IO::FileCheck::CT_MD5, progress, false);
		DEL_CLASS(progress);
		console->WriteLine();
		if (fileChk)
		{
			Text::StringBuilderUTF8 sb;
			IO::FileStream *fs;
			Exporter::MD5Exporter exporter;
			sb.Append(fileName);
			sb.Append((const UTF8Char*)".md5");
			NEW_CLASS(fs, IO::FileStream(sb.ToString(), IO::FileStream::FILE_MODE_CREATE, IO::FileStream::FILE_SHARE_DENY_NONE, IO::FileStream::BT_NORMAL));
			exporter.ExportFile(fs, sb.ToString(), fileChk, 0);
			DEL_CLASS(fs);
			DEL_CLASS(fileChk);
			showHelp = false;
			return true;
		}
		else
		{
			console->WriteLine((const UTF8Char*)"Error in calculating MD5");
			return false;
		}
	}
}

Int32 MyMain(Core::IProgControl *progCtrl)
{
	OSInt cmdCnt;
	UTF8Char sbuff[512];
	UTF8Char **cmdLines = progCtrl->GetCommandLines(progCtrl, &cmdCnt);
	NEW_CLASS(console, IO::ConsoleWriter());
	showHelp = true;
	if (cmdCnt == 2)
	{
		OSInt i = Text::StrIndexOf(cmdLines[1], '*');
		OSInt j = Text::StrIndexOf(cmdLines[1], '?');
		if (i < 0 && j < 0)
		{
			IO::Path::PathType pt = IO::Path::GetPathType(cmdLines[1]);
			if (pt == IO::Path::PT_UNKNOWN)
			{
				console->WriteLine((const UTF8Char*)"File not found");
			}
			else
			{
				ParseFile(cmdLines[1]);
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
				while (IO::Path::FindNextFile(&sbuff[i + 1], sess, 0, &pt, 0))
				{
					sb.ClearStr();
					sb.Append((const UTF8Char*)"Checking ");
					sb.Append(&sbuff[i + 1]);
					console->WriteLine(sb.ToString());
					ParseFile(sbuff);
					console->WriteLine();
				}
				IO::Path::FindFileClose(sess);
			}
		}
	}
	if (showHelp)
	{
		console->WriteLine((const UTF8Char*)"Usage: SMD5 [File to check]");
	}
	DEL_CLASS(console);
	return 0;
}
