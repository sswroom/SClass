#include "Stdafx.h"
#include "Data/StringUTF8Map.hpp"
#include "IO/FileStream.h"
#include "IO/FileUtil.h"
#include "IO/Path.h"
#include "IO/ZIPMTBuilder.h"
#include "SSWR/AVIRead/AVIRLogBackupForm.h"

void __stdcall SSWR::AVIRead::AVIRLogBackupForm::OnStartClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRLogBackupForm> me = userObj.GetNN<SSWR::AVIRead::AVIRLogBackupForm>();
	UTF8Char sbuff[512];
	UTF8Char sbuff2[64];
	UOSInt logNameSize;
	UOSInt nameSize;
	Int32 logTime;
	UnsafeArray<UTF8Char> filePath;
	UnsafeArray<UTF8Char> filePathEnd;
	UnsafeArray<UTF8Char> sptr;
	UnsafeArray<UTF8Char> sptr2;
	NN<IO::Path::FindFileSession> sess;
	Data::DateTime currTime;
	Data::FastStringMapNN<LogGroup> logGrps;
	NN<LogGroup> logGrp;
	UOSInt i;
	UOSInt j;
	Bool succ;
	IO::Path::PathType pt;

	filePath = me->txtLogDir->GetText(sbuff).Or(sbuff);
	if (filePath == sbuff)
	{
		me->ui->ShowMsgOK(CSTR("Please enter Log Dir"), CSTR("Log Backup"), me);
		return;
	}
	if (IO::Path::GetPathType(CSTRP(sbuff, filePath)) != IO::Path::PathType::Directory)
	{
		me->ui->ShowMsgOK(CSTR("Invalid Log Dir"), CSTR("Log Backup"), me);
		return;
	}
	currTime.SetCurrTimeUTC();
	if (filePath[-1] != IO::Path::PATH_SEPERATOR)
	{
		*filePath++ = IO::Path::PATH_SEPERATOR;
	}
	sptr = me->txtLogName->GetText(filePath).Or(filePath);
	logNameSize = (UOSInt)(sptr - filePath);
	sptr2 = Text::StrConcatC(sptr, IO::Path::ALL_FILES, IO::Path::ALL_FILES_LEN);
	if (IO::Path::FindFile(CSTRP(sbuff, sptr2)).SetTo(sess))
	{
		while (IO::Path::FindNextFile(filePath, sess, 0, 0, 0).SetTo(filePathEnd))
		{
			nameSize = (UOSInt)(Text::StrConcatS(sbuff2, filePath, 63) - sbuff2);
			if (nameSize >= logNameSize + 6)
			{
				sbuff2[logNameSize + 6] = 0;
				if (Text::StrEndsWithICaseC(filePath, (UOSInt)(filePathEnd - filePath), UTF8STRC(".zip")) || Text::StrEndsWithICaseC(filePath, (UOSInt)(filePathEnd - filePath), UTF8STRC(".rar")))
				{
				}
				else
				{
					logTime = Text::StrToInt32(&sbuff2[logNameSize]);
					if (logTime > 200000 && logTime < currTime.GetYear() * 100 + currTime.GetMonth())
					{
						if (!logGrps.GetC(Text::CStringNN(sbuff2, logNameSize + 6)).SetTo(logGrp))
						{
							NEW_CLASSNN(logGrp, LogGroup());
							logGrp->logName = Text::String::New(sbuff2, logNameSize + 6);
							logGrps.Put(logGrp->logName, logGrp);
						}
						logGrp->fileNames.Add(Text::String::NewP(sbuff, filePathEnd));
					}
				}
			}
		}
		IO::Path::FindFileClose(sess);
	}

	i = 0;
	j = logGrps.GetCount();
	while (i < j)
	{
		logGrp = logGrps.GetItemNoCheck(i);
		succ = true;
		filePathEnd = Text::StrConcatC(logGrp->logName->ConcatTo(filePath), UTF8STRC(".zip"));

		{
			IO::FileStream fs(CSTRP(sbuff, filePathEnd), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
			IO::ZIPMTBuilder zip(fs, IO::ZIPOS::UNIX);
			Data::ArrayIterator<NN<Text::String>> it = logGrp->fileNames.Iterator();
			while (succ && it.HasNext())
			{
				NN<Text::String> s = it.Next();
				IO::FileStream rfs(s, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
				UInt32 unixAttr = IO::Path::GetFileUnixAttr(s->ToCString());
				Data::Timestamp lastModTime = 0;
				Data::Timestamp createTime = 0;
				Data::Timestamp accessTime = 0;
				rfs.GetFileTimes(createTime, accessTime, lastModTime);
				succ = succ & zip.AddFile(s->ToCString().Substring((UOSInt)(filePath - sbuff)), rfs, lastModTime, accessTime, createTime, Data::Compress::Inflate::CompressionLevel::BestCompression, unixAttr);
			}
		}

		if (succ)
		{
			Data::ArrayIterator<NN<Text::String>> it = logGrp->fileNames.Iterator();
			while (it.HasNext())
			{
				NN<Text::String> s = it.Next();
				pt = IO::Path::GetPathType(s->ToCString());
				if (pt == IO::Path::PathType::File)
				{
					IO::Path::DeleteFile(s->v);
				}
				else if (pt == IO::Path::PathType::Directory)
				{
					IO::FileUtil::DeleteFile(s->ToCString(), true);
				}
				s->Release();
			}
		}
		else
		{
			IO::Path::DeleteFile(sbuff);
			logGrp->fileNames.FreeAll();
		}
		logGrp->logName->Release();
		logGrp.Delete();

		i++;
	}
}

SSWR::AVIRead::AVIRLogBackupForm::AVIRLogBackupForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 200, ui)
{
	this->SetText(CSTR("Log Backup"));
	this->SetFont(nullptr, 8.25, false);
	this->SetNoResize(true);

	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->lblLogDir = ui->NewLabel(*this, CSTR("Log Dir"));
	this->lblLogDir->SetRect(4, 4, 100, 23, false);
	this->txtLogDir = ui->NewTextBox(*this, CSTR(""), false);
	this->txtLogDir->SetRect(104, 4, 600, 23, false);
	this->lblLogName = ui->NewLabel(*this, CSTR("Log Name"));
	this->lblLogName->SetRect(4, 28, 100, 23, false);
	this->txtLogName = ui->NewTextBox(*this, CSTR(""), false);
	this->txtLogName->SetRect(104, 28, 600, 23, false);
	this->btnStart = ui->NewButton(*this, CSTR("&Start"));
	this->btnStart->SetRect(104, 52, 75, 23, false);
	this->btnStart->HandleButtonClick(OnStartClicked, this);
}

SSWR::AVIRead::AVIRLogBackupForm::~AVIRLogBackupForm()
{
}

void SSWR::AVIRead::AVIRLogBackupForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
