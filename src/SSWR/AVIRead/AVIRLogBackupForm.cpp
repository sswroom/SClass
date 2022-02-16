#include "Stdafx.h"
#include "Data/StringUTF8Map.h"
#include "IO/FileUtil.h"
#include "IO/MinizZIP.h"
#include "IO/Path.h"
//#include "IO/WindowZIP.h"
#include "SSWR/AVIRead/AVIRLogBackupForm.h"
#include "UI/MessageDialog.h"

void __stdcall SSWR::AVIRead::AVIRLogBackupForm::OnStartClicked(void *userObj)
{
	SSWR::AVIRead::AVIRLogBackupForm *me = (SSWR::AVIRead::AVIRLogBackupForm*)userObj;
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
	Data::ArrayList<LogGroup*> *logGrpList;
	LogGroup *logGrp;
	UOSInt i;
	UOSInt j;
	UOSInt k;
	UOSInt l;
	IO::MinizZIP *zip;
	Bool succ;
	IO::Path::PathType pt;

	filePath = me->txtLogDir->GetText(sbuff);
	if (filePath == sbuff)
	{
		UI::MessageDialog::ShowDialog((const UTF8Char*)"Please enter Log Dir", (const UTF8Char*)"Log Backup", me);
		return;
	}
	if (IO::Path::GetPathType(sbuff, (UOSInt)(filePath - sbuff)) != IO::Path::PathType::Directory)
	{
		UI::MessageDialog::ShowDialog((const UTF8Char*)"Invalid Log Dir", (const UTF8Char*)"Log Backup", me);
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
	sess = IO::Path::FindFile(sbuff, (UOSInt)(sptr2 - sbuff));
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
							logGrp->logName = Text::StrCopyNew(sbuff2);
							NEW_CLASS(logGrp->fileNames, Data::ArrayList<Text::String*>());
							logGrps.Put(sbuff2, logGrp);
						}
						logGrp->fileNames->Add(Text::String::NewNotNull(sbuff));
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
		Text::StrConcatC(Text::StrConcat(filePath, logGrp->logName), UTF8STRC(".zip"));

		NEW_CLASS(zip, IO::MinizZIP(sbuff));
		k = 0;
		l = logGrp->fileNames->GetCount();
		while (succ && k < l)
		{
			Text::String *s = logGrp->fileNames->GetItem(k);
			succ = succ & zip->AddFile(s->v, s->leng);
			k++;
		}
		DEL_CLASS(zip);

		if (succ)
		{
			k = 0;
			l = logGrp->fileNames->GetCount();
			while (k < l)
			{
				Text::String *s = logGrp->fileNames->GetItem(k);
				pt = IO::Path::GetPathType(s->v, s->leng);
				if (pt == IO::Path::PathType::File)
				{
					IO::Path::DeleteFile(s->v);
				}
				else if (pt == IO::Path::PathType::Directory)
				{
					IO::FileUtil::DeleteFile(s->v, true);
				}
				s->Release();
				k++;
			}
		}
		else
		{
			IO::Path::DeleteFile(sbuff);
			k = 0;
			l = logGrp->fileNames->GetCount();
			while (k < l)
			{
				logGrp->fileNames->GetItem(k)->Release();
				k++;
			}
		}
		Text::StrDelNew(logGrp->logName);
		DEL_CLASS(logGrp->fileNames);
		MemFree(logGrp);

		i++;
	}
}

SSWR::AVIRead::AVIRLogBackupForm::AVIRLogBackupForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 1024, 200, ui)
{
	this->SetText(CSTR("Log Backup"));
	this->SetFont(0, 0, 8.25, false);
	this->SetNoResize(true);

	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASS(this->lblLogDir, UI::GUILabel(ui, this, (const UTF8Char*)"Log Dir"));
	this->lblLogDir->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtLogDir, UI::GUITextBox(ui, this, CSTR(""), false));
	this->txtLogDir->SetRect(104, 4, 600, 23, false);
	NEW_CLASS(this->lblLogName, UI::GUILabel(ui, this, (const UTF8Char*)"Log Name"));
	this->lblLogName->SetRect(4, 28, 100, 23, false);
	NEW_CLASS(this->txtLogName, UI::GUITextBox(ui, this, CSTR(""), false));
	this->txtLogName->SetRect(104, 28, 600, 23, false);
	NEW_CLASS(this->btnStart, UI::GUIButton(ui, this, CSTR("&Start")));
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
