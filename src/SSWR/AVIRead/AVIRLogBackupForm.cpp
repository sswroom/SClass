#include "Stdafx.h"
#include "Data/StringUTF8Map.h"
#include "IO/FileStream.h"
#include "IO/FileUtil.h"
#include "IO/Path.h"
#include "IO/ZIPMTBuilder.h"
#include "SSWR/AVIRead/AVIRLogBackupForm.h"

void __stdcall SSWR::AVIRead::AVIRLogBackupForm::OnStartClicked(AnyType userObj)
{
	NotNullPtr<SSWR::AVIRead::AVIRLogBackupForm> me = userObj.GetNN<SSWR::AVIRead::AVIRLogBackupForm>();
	UTF8Char sbuff[512];
	UTF8Char sbuff2[64];
	UOSInt logNameSize;
	UOSInt nameSize;
	Int32 logTime;
	UTF8Char *filePath;
	UTF8Char *filePathEnd;
	UTF8Char *sptr;
	UTF8Char *sptr2;
	IO::Path::FindFileSession *sess;
	Data::DateTime currTime;
	Data::StringUTF8Map<LogGroup*> logGrps;
	NotNullPtr<const Data::ArrayList<LogGroup*>> logGrpList;
	LogGroup *logGrp;
	UOSInt i;
	UOSInt j;
	Bool succ;
	IO::Path::PathType pt;

	filePath = me->txtLogDir->GetText(sbuff);
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
	sptr = me->txtLogName->GetText(filePath);
	logNameSize = (UOSInt)(sptr - filePath);
	sptr2 = Text::StrConcatC(sptr, IO::Path::ALL_FILES, IO::Path::ALL_FILES_LEN);
	sess = IO::Path::FindFile(CSTRP(sbuff, sptr2));
	if (sess)
	{
		while ((filePathEnd = IO::Path::FindNextFile(filePath, sess, 0, 0, 0)) != 0)
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
						logGrp = logGrps.Get(sbuff2);
						if (logGrp == 0)
						{
							logGrp = MemAlloc(LogGroup, 1);
							logGrp->logName = Text::StrCopyNew(sbuff2).Ptr();
							NEW_CLASS(logGrp->fileNames, Data::ArrayListStringNN());
							logGrps.Put(sbuff2, logGrp);
						}
						logGrp->fileNames->Add(Text::String::NewP(sbuff, filePathEnd));
					}
				}
			}
		}
		IO::Path::FindFileClose(sess);
	}

	logGrpList = logGrps.GetValues();
	i = 0;
	j = logGrpList->GetCount();
	while (i < j)
	{
		logGrp = logGrpList->GetItem(i);
		succ = true;
		filePathEnd = Text::StrConcatC(Text::StrConcat(filePath, logGrp->logName), UTF8STRC(".zip"));

		{
			IO::FileStream fs(CSTRP(sbuff, filePathEnd), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
			IO::ZIPMTBuilder zip(fs, IO::ZIPOS::UNIX);
			Data::ArrayIterator<NotNullPtr<Text::String>> it = logGrp->fileNames->Iterator();
			while (succ && it.HasNext())
			{
				NotNullPtr<Text::String> s = it.Next();
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
			Data::ArrayIterator<NotNullPtr<Text::String>> it = logGrp->fileNames->Iterator();
			while (it.HasNext())
			{
				NotNullPtr<Text::String> s = it.Next();
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
			logGrp->fileNames->FreeAll();
		}
		Text::StrDelNew(logGrp->logName);
		DEL_CLASS(logGrp->fileNames);
		MemFree(logGrp);

		i++;
	}
}

SSWR::AVIRead::AVIRLogBackupForm::AVIRLogBackupForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 200, ui)
{
	this->SetText(CSTR("Log Backup"));
	this->SetFont(0, 0, 8.25, false);
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
