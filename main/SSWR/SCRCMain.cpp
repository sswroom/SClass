#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "Exporter/SFVExporter.h"
#include "IO/ConsoleWriter.h"
#include "IO/FileCheck.h"
#include "IO/FileStream.h"
#include "IO/Path.h"
#include "IO/StmData/FileData.h"
#include "Manage/HiResClock.h"
#include "Parser/FileParser/SFVParser.h"
#include "Sync/Event.h"
#include "Sync/Mutex.h"
#include "Sync/MutexUsage.h"
#include "Sync/Thread.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"

IO::ConsoleWriter *console;

class ProgressHandler : public IO::IProgressHandler
{
private:
	Sync::Mutex *mut;
	const UTF8Char *name;
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
			sb->Append((const UTF8Char*)"\rSpeed: ");
			if (currCount == lastDispCount || currTime == lastDispTime)
			{
				sb->Append((const UTF8Char*)"0");
			}
			else
			{
				Text::SBAppendF64(sb, (Double)(currCount - lastDispCount)  / (currTime - lastDispTime));
				lastDispCount = currCount;
				lastDispTime = currTime;
			}
			sb->Append((const UTF8Char*)"Bytes/s");
			Sync::MutexUsage mutUsage(me->mut);
			if (me->fileName)
			{
				sb->Append((const UTF8Char*)" (");
				sb->Append(me->fileName);
				sb->Append((const UTF8Char*)")");
			}
			mutUsage.EndUse();
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

	virtual void ProgressStart(const UTF8Char *name, UInt64 count)
	{
		UOSInt i;
		Sync::MutexUsage mutUsage(this->mut);
		SDEL_TEXT(this->name);
		this->name = Text::StrCopyNew(name);
		i = Text::StrLastIndexOf(this->name, IO::Path::PATH_SEPERATOR);
		this->fileName = &this->name[i + 1];
		this->lastCount = 0;
		mutUsage.EndUse();
	}

	virtual void ProgressUpdate(UInt64 currCount, UInt64 newCount)
	{
		this->currCount += currCount - this->lastCount;
		this->lastCount = currCount;
	}

	virtual void ProgressEnd()
	{
		Sync::MutexUsage mutUsage(this->mut);
		SDEL_TEXT(this->name);
		this->fileName = 0;
		mutUsage.EndUse();
	}
};

Int32 MyMain(Core::IProgControl *progCtrl)
{
	Bool showHelp = true;
	UOSInt cmdCnt;
	UTF8Char **cmdLines = progCtrl->GetCommandLines(progCtrl, &cmdCnt);
	NEW_CLASS(console, IO::ConsoleWriter());
	if (cmdCnt == 2)
	{
		IO::Path::PathType pt = IO::Path::GetPathType(cmdLines[1]);
		if (pt == IO::Path::PathType::Unknown)
		{
			console->WriteLine((const UTF8Char*)"File not found");
		}
		else
		{
			if (Text::StrEndsWithICase(cmdLines[1], (const UTF8Char*)".SFV"))
			{
				Parser::FileParser::SFVParser parser;
				IO::StmData::FileData *fd;
				IO::FileCheck *fileChk;
				NEW_CLASS(fd, IO::StmData::FileData(cmdLines[1], false));
				fileChk = (IO::FileCheck *)parser.ParseFile(fd, 0, IO::ParserType::FileCheck);
				DEL_CLASS(fd);
				if (fileChk == 0)
				{
					console->WriteLine((const UTF8Char*)"Error in parsing the file");
				}
				else
				{
					UInt8 hash[4];
					UOSInt i;
					UOSInt j;
					i = 0;
					j = fileChk->GetCount();
					while (i < j)
					{
						fileChk->CheckEntryHash(i, hash);
					}
					showHelp = false;
					DEL_CLASS(fileChk);
				}
			}
			else
			{
				ProgressHandler *progress;
				NEW_CLASS(progress, ProgressHandler());
				IO::FileCheck *fileChk = IO::FileCheck::CreateCheck(cmdLines[1], IO::FileCheck::CheckType::CRC32, progress, false);
				DEL_CLASS(progress);
				console->WriteLine();
				if (fileChk)
				{
					Text::StringBuilderUTF8 sb;
					IO::FileStream *fs;
					Exporter::SFVExporter exporter;
					sb.Append(cmdLines[1]);
					sb.Append((const UTF8Char*)".sfv");
					NEW_CLASS(fs, IO::FileStream(sb.ToString(), IO::FileStream::FileMode::Create, IO::FileStream::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
					exporter.ExportFile(fs, sb.ToString(), fileChk, 0);
					DEL_CLASS(fs);
					DEL_CLASS(fileChk);
					showHelp = false;
				}
				else
				{
					console->WriteLine((const UTF8Char*)"Error in calculating CRC");
				}
			}
		}
	}
	if (showHelp)
	{
		console->WriteLine((const UTF8Char*)"Usage: SCRC [File to check]");
	}
	DEL_CLASS(console);
	return 0;
}
