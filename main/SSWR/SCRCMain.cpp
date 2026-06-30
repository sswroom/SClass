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

NN<IO::ConsoleWriter> console;

class ProgressHandler : public IO::ProgressHandler
{
private:
	Sync::Mutex mut;
	Optional<Text::String> name;
	UnsafeArrayOpt<const UTF8Char> fileName;
	UInt64 currCount;
	UInt64 lastCount;

	Bool threadRunning;
	Bool threadToStop;
	NN<Sync::Event> evt;

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
				UnsafeArray<const UTF8Char> fileName;
				if (me->fileName.SetTo(fileName))
				{
					sb.AppendC(UTF8STRC(" ("));
					sb.AppendSlow(fileName);
					sb.AppendC(UTF8STRC(")"));
				}
				mutUsage.EndUse();
				console->Write(sb.ToCString());
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
		this->name = nullptr;
		this->fileName = nullptr;

		this->threadRunning = false;
		this->threadToStop = false;
		NEW_CLASSNN(this->evt, Sync::Event(true));
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
		this->evt.Delete();
		OPTSTR_DEL(this->name);
		this->fileName = nullptr;
	}

	virtual void ProgressStart(Text::CStringNN name, UInt64 count)
	{
		UIntOS i;
		NN<Text::String> nnname;
		Sync::MutexUsage mutUsage(this->mut);
		OPTSTR_DEL(this->name);
		this->name = nnname = Text::String::New(name);
		i = Text::StrLastIndexOfCharC(nnname->v, nnname->leng, IO::Path::PATH_SEPERATOR);
		this->fileName = &nnname->v[i + 1];
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
		OPTSTR_DEL(this->name);
		this->fileName = nullptr;
		mutUsage.EndUse();
	}
};

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
	Bool showHelp = true;
	UIntOS cmdCnt;
	UnsafeArray<UnsafeArray<UTF8Char>> cmdLines = progCtrl->GetCommandLines(progCtrl, cmdCnt);
	NEW_CLASSNN(console, IO::ConsoleWriter());
	if (cmdCnt == 2)
	{
		UIntOS cmdLen = Text::StrCharCnt(cmdLines[1]);
		IO::Path::PathType pt = IO::Path::GetPathType({cmdLines[1], cmdLen});
		if (pt == IO::Path::PathType::Unknown)
		{
			console->WriteLine(CSTR("File not found"));
		}
		else
		{
			if (Text::StrEndsWithICaseC(cmdLines[1], cmdLen, UTF8STRC(".SFV")))
			{
				Parser::FileParser::SFVParser parser;
				NN<IO::FileCheck> fileChk;
				{
					ProgressHandler progress;
					IO::StmData::FileData fd({cmdLines[1], cmdLen}, false);
					if (!Optional<IO::FileCheck>::ConvertFrom(parser.ParseFile(fd, nullptr, IO::ParserType::FileCheck)).SetTo(fileChk))
					{
						console->WriteLine(CSTR("Error in parsing the file"));
					}
					else
					{
						UInt8 hash[4];
						UIntOS i;
						UIntOS j;
						i = 0;
						j = fileChk->GetCount();
						while (i < j)
						{
							fileChk->CheckEntryHash(i, hash, progress, nullptr);
						}
						showHelp = false;
						fileChk.Delete();
					}
				}
			}
			else
			{
				Optional<IO::FileCheck> fileChk;
				NN<IO::FileCheck> nnfileChk;
				{
					ProgressHandler progress;
					fileChk = IO::FileCheck::CreateCheck({cmdLines[1], cmdLen}, Crypto::Hash::HashType::CRC32, &progress, false);
				}
				console->WriteLine();
				if (fileChk.SetTo(nnfileChk))
				{
					Text::StringBuilderUTF8 sb;
					Exporter::SFVExporter exporter;
					sb.AppendC(cmdLines[1], cmdLen);
					sb.AppendC(UTF8STRC(".sfv"));
					IO::FileStream fs(sb.ToCString(), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
					exporter.ExportFile(fs, sb.ToCString(), nnfileChk, nullptr);
					nnfileChk.Delete();
					showHelp = false;
				}
				else
				{
					console->WriteLine(CSTR("Error in calculating CRC"));
				}
			}
		}
	}
	if (showHelp)
	{
		console->WriteLine(CSTR("Usage: SCRC [File to check]"));
	}
	console.Delete();
	return 0;
}
