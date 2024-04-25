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
#include "Sync/SimpleThread.h"
#include "Sync/ThreadUtil.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"

IO::ConsoleWriter *console;

class ProgressHandler : public IO::ProgressHandler
{
private:
	Sync::Mutex mut;
	Text::String *name;
	const UTF8Char *fileName;
	UInt64 currCount;
	UInt64 lastCount;

	Bool threadRunning;
	Bool threadToStop;
	Sync::Event *evt;

	static UInt32 __stdcall CheckThread(AnyType userObj)
	{
		NN<ProgressHandler> me = userObj.GetNN<ProgressHandler>();
		UInt64 lastDispCount = 0;
		Double lastDispTime = 0;
		UInt64 currCount;
		Double currTime;
		{
			Manage::HiResClock clk;
			Text::StringBuilderUTF8 sb;

			me->threadRunning = true;
			while (!me->threadToStop)
			{
				me->evt->Wait(1000);

				currCount = me->currCount;
				currTime = clk.GetTimeDiff();

				sb.ClearStr();
				sb.AppendC(UTF8STRC("\rSpeed: "));
				if (currCount == lastDispCount || currTime == lastDispTime)
				{
					sb.AppendC(UTF8STRC("0"));
				}
				else
				{
					sb.AppendDouble((Double)(currCount - lastDispCount)  / (currTime - lastDispTime));
					lastDispCount = currCount;
					lastDispTime = currTime;
				}
				sb.AppendC(UTF8STRC("Bytes/s"));
				Sync::MutexUsage mutUsage(me->mut);
				if (me->fileName)
				{
					sb.AppendC(UTF8STRC(" ("));
					sb.AppendSlow(me->fileName);
					sb.AppendC(UTF8STRC(")"));
				}
				mutUsage.EndUse();
				console->WriteStrC(sb.ToString(), sb.GetLength());
			}
		}
		me->threadRunning = false;
		return 0;
	}
public:
	ProgressHandler()
	{
		this->currCount = 0;
		this->lastCount = 0;
		this->name = 0;
		this->fileName = 0;

		this->threadRunning = false;
		this->threadToStop = false;
		NEW_CLASS(this->evt, Sync::Event(true));
		Sync::ThreadUtil::Create(CheckThread, this);
		while (!this->threadRunning)
		{
			Sync::SimpleThread::Sleep(1);
		}
	}

	virtual ~ProgressHandler()
	{
		this->threadToStop = true;
		this->evt->Set();
		while (this->threadRunning)
		{
			Sync::SimpleThread::Sleep(1);
		}
		DEL_CLASS(this->evt);
		SDEL_STRING(this->name);
		this->fileName = 0;
	}

	virtual void ProgressStart(Text::CString name, UInt64 count)
	{
		UOSInt i;
		Sync::MutexUsage mutUsage(this->mut);
		SDEL_STRING(this->name);
		this->name = Text::String::New(name).Ptr();
		i = Text::StrLastIndexOfCharC(this->name->v, this->name->leng, IO::Path::PATH_SEPERATOR);
		this->fileName = &this->name->v[i + 1];
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
		SDEL_STRING(this->name);
		this->fileName = 0;
		mutUsage.EndUse();
	}
};

Int32 MyMain(NN<Core::IProgControl> progCtrl)
{
	Bool showHelp = true;
	UOSInt cmdCnt;
	UTF8Char **cmdLines = progCtrl->GetCommandLines(progCtrl, cmdCnt);
	NEW_CLASS(console, IO::ConsoleWriter());
	if (cmdCnt == 2)
	{
		UOSInt cmdLen = Text::StrCharCnt(cmdLines[1]);
		IO::Path::PathType pt = IO::Path::GetPathType({cmdLines[1], cmdLen});
		if (pt == IO::Path::PathType::Unknown)
		{
			console->WriteLineC(UTF8STRC("File not found"));
		}
		else
		{
			if (Text::StrEndsWithICaseC(cmdLines[1], cmdLen, UTF8STRC(".SFV")))
			{
				Parser::FileParser::SFVParser parser;
				IO::FileCheck *fileChk;
				{
					IO::StmData::FileData fd({cmdLines[1], cmdLen}, false);
					fileChk = (IO::FileCheck *)parser.ParseFile(fd, 0, IO::ParserType::FileCheck);
				}
				if (fileChk == 0)
				{
					console->WriteLineC(UTF8STRC("Error in parsing the file"));
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
				IO::FileCheck *fileChk;
				NN<IO::FileCheck> nnfileChk;
				{
					ProgressHandler progress;
					fileChk = IO::FileCheck::CreateCheck({cmdLines[1], cmdLen}, Crypto::Hash::HashType::CRC32, &progress, false);
				}
				console->WriteLine();
				if (nnfileChk.Set(fileChk))
				{
					Text::StringBuilderUTF8 sb;
					Exporter::SFVExporter exporter;
					sb.AppendC(cmdLines[1], cmdLen);
					sb.AppendC(UTF8STRC(".sfv"));
					IO::FileStream fs(sb.ToCString(), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
					exporter.ExportFile(fs, sb.ToCString(), nnfileChk, 0);
					nnfileChk.Delete();
					showHelp = false;
				}
				else
				{
					console->WriteLineC(UTF8STRC("Error in calculating CRC"));
				}
			}
		}
	}
	if (showHelp)
	{
		console->WriteLineC(UTF8STRC("Usage: SCRC [File to check]"));
	}
	DEL_CLASS(console);
	return 0;
}
