#include "Stdafx.h"
#include "IO/FileStream.h"
#include "IO/Path.h"
#include "IO/ZIPBuilder.h"
#include "SSWR/AVIRead/AVIRTimedFileCopyForm.h"
#include "UI/FileDialog.h"
#include "UI/MessageDialog.h"

void __stdcall SSWR::AVIRead::AVIRTimedFileCopyForm::OnStartClicked(void *userObj)
{
	SSWR::AVIRead::AVIRTimedFileCopyForm *me = (SSWR::AVIRead::AVIRTimedFileCopyForm*)userObj;
	Data::DateTime dt1;
	Data::DateTime dt2;
	Text::StringBuilderUTF8 sb;
	UTF8Char sbuff[1024];
	UTF8Char *sptr;
	me->dtpStartTime->GetSelectedTime(&dt1);
	me->dtpEndTime->GetSelectedTime(&dt2);
	me->txtFileDir->GetText(&sb);
	if (IO::Path::GetPathType(sb.ToString()) != IO::Path::PT_DIRECTORY)
	{
		UI::MessageDialog::ShowDialog((const UTF8Char*)"The file dir is not a directory", me->GetFormName(), me);
		return;
	}
	dt1.ClearTime();
	dt2.ClearTime();
	if (dt1.CompareTo(&dt2) > 0)
	{
		UI::MessageDialog::ShowDialog((const UTF8Char*)"The start time is after end time", me->GetFormName(), me);
		return;
	}
	Double days = Data::DateTime::MS2Days(dt2.DiffMS(&dt1));
	if (days > 90)
	{
		UI::MessageDialog::ShowDialog((const UTF8Char*)"The time range is longer than 90 days", me->GetFormName(), me);
		return;
	}

	UI::FileDialog *dlg;
	NEW_CLASS(dlg, UI::FileDialog(L"SSWR", L"AVIRead", L"TimedFileCopy", true));
	dlg->AddFilter((const UTF8Char*)"*.zip", (const UTF8Char*)"Zip file");
	if (dlg->ShowDialog(me->GetHandle()))
	{
		IO::ZIPBuilder *zip;
		IO::FileStream *fs;
		NEW_CLASS(fs, IO::FileStream(dlg->GetFileName(), IO::FileStream::FILE_MODE_CREATE, IO::FileStream::FILE_SHARE_DENY_NONE, IO::FileStream::BT_NORMAL));
		if (fs->IsError())
		{
			UI::MessageDialog::ShowDialog((const UTF8Char*)"Error in creating zip file", me->GetFormName(), me);
		}
		NEW_CLASS(zip, IO::ZIPBuilder(fs));
		sptr = Text::StrConcat(sbuff, sb.ToString());
		if (sptr[-1] != IO::Path::PATH_SEPERATOR)
		{
			*sptr++ = IO::Path::PATH_SEPERATOR;
		}
		me->CopyToZip(zip, sbuff, sptr, sptr, &dt1, &dt2, true);

		DEL_CLASS(zip);
		DEL_CLASS(fs);
	}
	DEL_CLASS(dlg);
}

Bool SSWR::AVIRead::AVIRTimedFileCopyForm::CopyToZip(IO::ZIPBuilder *zip, const UTF8Char *buffStart, const UTF8Char *pathBase, UTF8Char *pathEnd, Data::DateTime *startTime, Data::DateTime *endTime, Bool monthDir)
{
	UTF8Char *sptr;
	IO::Path::FindFileSession *sess;
	Text::StrConcat(pathEnd, IO::Path::ALL_FILES);
	sess = IO::Path::FindFile(buffStart);
	if (sess)
	{
		IO::Path::FindFileClose(sess);
	}
}

SSWR::AVIRead::AVIRTimedFileCopyForm::AVIRTimedFileCopyForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 652, 180, ui)
{
	this->SetText(this->GetFormName());
	this->SetFont(0, 8.25, false);
	this->SetNoResize(true);

	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASS(this->lblFileDir, UI::GUILabel(ui, this, (const UTF8Char *)"File Dir"));
	this->lblFileDir->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtFileDir, UI::GUITextBox(ui, this, (const UTF8Char*)""));
	this->txtFileDir->SetRect(104, 4, 400, 23, false);
	NEW_CLASS(this->lblStartTime, UI::GUILabel(ui, this, (const UTF8Char*)"Start Time"));
	this->lblStartTime->SetRect(4, 28, 100, 23, false);
	NEW_CLASS(this->dtpStartTime, UI::GUIDateTimePicker(ui, this, UI::GUIDateTimePicker::ST_UPDOWN));
	this->dtpStartTime->SetFormat("yyyy-MM-dd");
	this->dtpStartTime->SetRect(104, 28, 100, 23, false);
	NEW_CLASS(this->lblEndTime, UI::GUILabel(ui, this, (const UTF8Char*)"End Time"));
	this->lblEndTime->SetRect(4, 52, 100, 23, false);
	NEW_CLASS(this->dtpEndTime, UI::GUIDateTimePicker(ui, this, UI::GUIDateTimePicker::ST_UPDOWN));
	this->dtpEndTime->SetFormat("yyyy-MM-dd");
	this->dtpEndTime->SetRect(104, 52, 100, 23, false);
	NEW_CLASS(this->btnStart, UI::GUIButton(ui, this, (const UTF8Char*)"Start"));
	this->btnStart->SetRect(104, 76, 75, 23, false);

}

SSWR::AVIRead::AVIRTimedFileCopyForm::~AVIRTimedFileCopyForm()
{

}

const UTF8Char *SSWR::AVIRead::AVIRTimedFileCopyForm::GetFormName()
{
	return (const UTF8Char*)"Timed File Copy";
}

void SSWR::AVIRead::AVIRTimedFileCopyForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
