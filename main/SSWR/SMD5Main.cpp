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
#include "Sync/SimpleThread.h"
#include "Sync/ThreadUtil.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"

IO::ConsoleWriter *console;
Bool showHelp;

class ProgressHandler : public IO::ProgressHandler
{
private:
	Sync::Mutex mut;
	Text::String *name;
	Text::CString fileName;
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
				if (me->fileName.leng > 0)
				{
					sb.AppendC(UTF8STRC(" ("));
					sb.Append(me->fileName);
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
		this->fileName = CSTR_NULL;

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
		this->fileName = CSTR_NULL;
	}

	virtual void ProgressStart(Text::CString name, UInt64 count)
	{
		UOSInt i;
		Sync::MutexUsage mutUsage(this->mut);
		SDEL_STRING(this->name);
		this->name = Text::String::New(name).Ptr();
		i = Text::StrLastIndexOfCharC(this->name->v, this->name->leng, IO::Path::PATH_SEPERATOR);
		this->fileName = this->name->ToCString().Substring(i + 1);
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
		this->fileName = CSTR_NULL;
	}
};

Bool VerifyMD5(const UTF8Char *fileName, UOSInt fileNameLen)
{
	if (Text::StrEndsWithICaseC(fileName, fileNameLen, UTF8STRC(".MD5")))
	{
		Parser::FileParser::MD5Parser parser;
		Bool succ = true;
		IO::FileCheck *fileChk;
		{
			IO::StmData::FileData fd({fileName, fileNameLen}, false);
			fileChk = (IO::FileCheck *)parser.ParseFile(fd, 0, IO::ParserType::FileCheck);
		}
		if (fileChk == 0)
		{
			console->WriteLineC(UTF8STRC("Error in parsing the file"));
		}
		else
		{
			UInt8 hash[32];
			UOSInt i;
			UOSInt j;
			
			i = 0;
			j = fileChk->GetCount();
			while (i < j)
			{
				if (!fileChk->CheckEntryHash(i, hash))
				{
					Text::StringBuilderUTF8 sb;
					sb.AppendC(UTF8STRC("File validation failed: "));
					sb.AppendOpt(fileChk->GetEntryName(i));
					succ = false;
					console->WriteLineC(sb.v, sb.leng);
					break;
				}
				i++;
			}
			showHelp = false;
			DEL_CLASS(fileChk);
		}
		return succ;
	}
	else
	{
		console->WriteLineC(UTF8STRC("File is not MD5 file"));
		return false;
	}
}

Int32 MyMain(NN<Core::IProgControl> progCtrl)
{
	UOSInt cmdCnt;
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	UTF8Char **cmdLines = progCtrl->GetCommandLines(progCtrl, cmdCnt);
	NEW_CLASS(console, IO::ConsoleWriter());
	showHelp = true;
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
			if (VerifyMD5(cmdLines[1], cmdLen))
			{
				showHelp = false;
			}
		}
	}
	else if (cmdCnt >= 3)
	{
		ProgressHandler progress;
		IO::FileCheck *fileChk = 0;
		NN<IO::FileCheck> thisChk;
		NN<IO::FileCheck> nnfileChk;
		IO::Path::FindFileSession *sess;
		Text::StringBuilderUTF8 sb;
		IO::Path::PathType pt;
		UOSInt i;
		UOSInt j = 2;
		showHelp = false;
		while (!showHelp && j < cmdCnt)
		{
			sptr = Text::StrConcat(sbuff, cmdLines[2]);
			i = Text::StrLastIndexOfCharC(sbuff, (UOSInt)(sptr - sbuff), IO::Path::PATH_SEPERATOR);
			Text::CStringNN cstr = CSTRP(sbuff, sptr).Substring(i + 1);
			if (cstr.IndexOf('*') != INVALID_INDEX || cstr.IndexOf('?') != INVALID_INDEX)
			{
				sess = IO::Path::FindFile(CSTRP(sbuff, sptr));
				if (sess)
				{
					while ((sptr = IO::Path::FindNextFile(&sbuff[i + 1], sess, 0, &pt, 0)) != 0)
					{
						if (sbuff[i + 1] == '.' && (sbuff[i + 2] == 0 || (sbuff[i + 2] == '.' && sbuff[i + 3] == 0)))
						{

						}
						else
						{
							sb.ClearStr();
							sb.AppendC(UTF8STRC("Checking "));
							sb.AppendP(&sbuff[i + 1], sptr);
							console->WriteLineC(sb.ToString(), sb.GetLength());
							if (!thisChk.Set(IO::FileCheck::CreateCheck(CSTRP(sbuff, sptr), Crypto::Hash::HashType::MD5, &progress, false)))
							{
								SDEL_CLASS(fileChk);
								showHelp = true;
								break;
							}
							else if (fileChk)
							{
								fileChk->MergeFrom(thisChk);
								thisChk.Delete();
							}
							else
							{
								fileChk = thisChk.Ptr();
							}
						}
					}
					IO::Path::FindFileClose(sess);
				}
				else
				{
					sb.ClearStr();
					sb.AppendC(UTF8STRC("Error in searching for path: "));
					sb.AppendP(sbuff, sptr);
					console->WriteLineC(sb.ToString(), sb.GetLength());
				}
			}
			else
			{
				if (!thisChk.Set(IO::FileCheck::CreateCheck(CSTRP(sbuff, sptr), Crypto::Hash::HashType::MD5, &progress, false)))
				{
					SDEL_CLASS(fileChk);
					showHelp = true;
				}
				else if (fileChk)
				{
					fileChk->MergeFrom(thisChk);
					thisChk.Delete();
				}
				else
				{
					fileChk = thisChk.Ptr();
				}
				console->WriteLine();
			}
			j++;
		}
		if (nnfileChk.Set(fileChk))
		{
			Exporter::MD5Exporter exporter;
			{
				IO::FileStream fs(Text::CStringNN::FromPtr(cmdLines[1]), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
				exporter.ExportFile(fs, sb.ToCString(), nnfileChk, 0);
			}
			nnfileChk.Delete();
			showHelp = false;
		}
		else
		{
			console->WriteLineC(UTF8STRC("Error in calculating MD5"));
		}
	}
	else
	{
		sptr = Text::StrConcatC(sbuff, UTF8STRC("Unknown parameter type ("));
		sptr = Text::StrUOSInt(sptr, cmdCnt);
		*sptr++ = ')';
		*sptr = 0;
		console->WriteLineC(sbuff, (UOSInt)(sptr - sbuff));
	}
	if (showHelp)
	{
		console->WriteLineC(UTF8STRC("Usage: SMD5 [MD5 file] (Base directory is same as MD5 file)"));
		console->WriteLineC(UTF8STRC("       SMD5 [MD5 file] [files to read]"));
	}
	DEL_CLASS(console);
	return 0;
}
