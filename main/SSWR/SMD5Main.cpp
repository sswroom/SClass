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
#include "Text/UTF8Writer.h"

IO::ConsoleWriter *console;
Bool showHelp;

class ProgressHandler : public IO::ProgressHandler
{
private:
	Sync::Mutex mut;
	Optional<Text::String> name;
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
					sb.AppendDoubleGDP((Double)(currCount - lastDispCount)  / (currTime - lastDispTime), 3, 2, 2);
					lastDispCount = currCount;
					lastDispTime = currTime;
				}
				sb.AppendC(UTF8STRC("Bytes/s"));
				Sync::MutexUsage mutUsage(me->mut);
				Text::CStringNN nns;
				if (me->fileName.SetTo(nns) && nns.leng > 0)
				{
					sb.AppendC(UTF8STRC(" ("));
					sb.Append(nns);
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
		OPTSTR_DEL(this->name);
		this->fileName = nullptr;
	}

	virtual void ProgressStart(Text::CStringNN name, UInt64 count)
	{
		UOSInt i;
		Sync::MutexUsage mutUsage(this->mut);
		NN<Text::String> nnname;
		OPTSTR_DEL(this->name);
		this->name = nnname = Text::String::New(name);
		i = Text::StrLastIndexOfCharC(nnname->v, nnname->leng, IO::Path::PATH_SEPERATOR);
		this->fileName = nnname->ToCString().Substring(i + 1);
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
		OPTSTR_DEL(this->name);
		this->fileName = nullptr;
	}
};

Bool VerifyMD5(Text::CStringNN fileName, Bool flagCont, Bool flagVerbose, Optional<IO::ProgressHandler> progress)
{
	if (fileName.EndsWithICase(UTF8STRC(".MD5")))
	{
		IO::FileStream fs(CSTR("difffiles.txt"), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
		Text::UTF8Writer diffLog(fs);
		Parser::FileParser::MD5Parser parser;
		Bool succ = true;
		NN<IO::FileCheck> fileChk;
		{
			IO::StmData::FileData fd(fileName, false);
			if (!Optional<IO::FileCheck>::ConvertFrom(parser.ParseFile(fd, nullptr, IO::ParserType::FileCheck)).SetTo(fileChk))
			{
				console->WriteLine(CSTR("Error in parsing the file"));
				return false;
			}
		}
		UInt8 hash[32];
		UOSInt i;
		UOSInt j;
		NN<Text::String> entryName;
		
		i = 0;
		j = fileChk->GetCount();
		while (i < j)
		{
			if (!fileChk->CheckEntryHash(i, hash, progress, flagVerbose?console:0))
			{
				Text::StringBuilderUTF8 sb;
				sb.AppendC(UTF8STRC("File validation failed: "));
				sb.AppendOpt(fileChk->GetEntryName(i));
				succ = false;
				console->WriteLine(sb.ToCString());
				if (!flagCont)
					break;
				if (fileChk->GetEntryName(i).SetTo(entryName))
				{
					diffLog.WriteLine(entryName->ToCString());
				}
			}
			i++;
		}
		showHelp = false;
		fileChk.Delete();
		return succ;
	}
	else
	{
		console->WriteLine(CSTR("File is not MD5 file"));
		return false;
	}
}

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
	UOSInt cmdCnt;
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	UnsafeArray<UnsafeArray<UTF8Char>> cmdLines = progCtrl->GetCommandLines(progCtrl, cmdCnt);
	NEW_CLASS(console, IO::ConsoleWriter());
	showHelp = true;
	if (cmdCnt >= 2)
	{
		Bool flagCont = false;
		Bool flagVerbose = false;
		Bool foundFiles = false;
		ProgressHandler progress;
		Text::CString md5File = nullptr;
		IO::FileCheck *fileChk = 0;
		NN<IO::FileCheck> thisChk;
		NN<IO::FileCheck> nnfileChk;
		NN<IO::Path::FindFileSession> sess;
		Text::StringBuilderUTF8 sb;
		IO::Path::PathType pt;
		UOSInt i;
		UOSInt j = 1;
		while (j < cmdCnt && (!foundFiles || !showHelp))
		{
			if (cmdLines[j][0] == '-')
			{
				if (cmdLines[j][1] == 'c')
				{
					flagCont = true;
				}
				else if (cmdLines[j][1] == 'v')
				{
					flagVerbose = true;
				}
			}
			else if (md5File.v.IsNull())
			{
				md5File = Text::CStringNN::FromPtr(cmdLines[j]);
				showHelp = false;
			}
			else
			{
				foundFiles = true;
				sptr = Text::StrConcat(sbuff, cmdLines[j]);
				i = Text::StrLastIndexOfCharC(sbuff, (UOSInt)(sptr - sbuff), IO::Path::PATH_SEPERATOR);
				Text::CStringNN cstr = CSTRP(sbuff, sptr).Substring(i + 1);
				if (cstr.IndexOf('*') != INVALID_INDEX || cstr.IndexOf('?') != INVALID_INDEX)
				{
					if (IO::Path::FindFile(CSTRP(sbuff, sptr)).SetTo(sess))
					{
						while (IO::Path::FindNextFile(&sbuff[i + 1], sess, 0, pt, 0).SetTo(sptr))
						{
							if (sbuff[i + 1] == '.' && (sbuff[i + 2] == 0 || (sbuff[i + 2] == '.' && sbuff[i + 3] == 0)))
							{

							}
							else
							{
								sb.ClearStr();
								sb.AppendC(UTF8STRC("Checking "));
								sb.AppendP(&sbuff[i + 1], sptr);
								console->WriteLine(sb.ToCString());
								if (!IO::FileCheck::CreateCheck(CSTRP(sbuff, sptr), Crypto::Hash::HashType::MD5, progress, false).SetTo(thisChk))
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
						console->WriteLine(sb.ToCString());
					}
				}
				else
				{
					if (!IO::FileCheck::CreateCheck(CSTRP(sbuff, sptr), Crypto::Hash::HashType::MD5, progress, false).SetTo(thisChk))
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
			}
			j++;
		}
		if (md5File.leng == 0)
		{
			console->WriteLine(CSTR("MD5 file missing"));
		}
		else if (!foundFiles)
		{
			IO::Path::PathType pt = IO::Path::GetPathType(md5File.OrEmpty());
			if (pt == IO::Path::PathType::Unknown)
			{
				console->WriteLine(CSTR("MD5 File not found"));
			}
			else
			{
				if (VerifyMD5(md5File.OrEmpty(), flagCont, flagVerbose, progress))
				{
					console->WriteLine(Text::StringBuilderUTF8().Append(CSTR("File "))->AppendOpt(md5File)->Append(CSTR(" verify success"))->ToCString());
					showHelp = false;
				}
			}

		}
		else if (nnfileChk.Set(fileChk))
		{
			Exporter::MD5Exporter exporter;
			{
				IO::FileStream fs(md5File.OrEmpty(), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
				exporter.ExportFile(fs, sb.ToCString(), nnfileChk, nullptr);
			}
			nnfileChk.Delete();
			showHelp = false;
		}
		else
		{
			console->WriteLine(CSTR("Error in calculating MD5"));
		}
	}
	else
	{
		sptr = Text::StrConcatC(sbuff, UTF8STRC("Unknown parameter type ("));
		sptr = Text::StrUOSInt(sptr, cmdCnt);
		*sptr++ = ')';
		*sptr = 0;
		console->WriteLine(CSTRP(sbuff, sptr));
	}
	if (showHelp)
	{
		console->WriteLine(CSTR("Usage: SMD5 [options] [MD5 file] (Base directory is same as MD5 file)"));
		console->WriteLine(CSTR("       SMD5 [options] [MD5 file] [files to read]"));
		console->WriteLine(CSTR("Options: -c   Continue on verify error"));
		console->WriteLine(CSTR("         -v   Verbose"));
	}
	DEL_CLASS(console);
	return 0;
}
