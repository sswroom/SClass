#include "Stdafx.h"
#include "IO/FileUtil.h"
#include "IO/Path.h"
#include "SSWR/AVIRead/AVIRBatchRenameForm.h"

void __stdcall SSWR::AVIRead::AVIRBatchRenameForm::OnRenameExtUpperClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRBatchRenameForm> me = userObj.GetNN<SSWR::AVIRead::AVIRBatchRenameForm>();
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	UnsafeArray<UTF8Char> sptrEnd;
	UTF8Char sbuff2[512];
	UnsafeArray<UTF8Char> sptr2;
	UnsafeArray<UTF8Char> sptr2End;
	Text::StringBuilderUTF8 sb;
	me->txtDirectory->GetText(sb);
	if (IO::Path::GetPathType(sb.ToCString()) != IO::Path::PathType::Directory)
	{
		me->ui->ShowMsgOK(CSTR("Please enter valid directory path"), CSTR("Batch Rename"), me);
		return;
	}
	if (!sb.EndsWith(IO::Path::PATH_SEPERATOR))
	{
		sb.AppendUTF8Char(IO::Path::PATH_SEPERATOR);
	}
	sptr = sb.ConcatTo(sbuff);
	sptr2 = sb.ConcatTo(sbuff2);
	sptrEnd = Text::StrConcatC(sptr, IO::Path::ALL_FILES, IO::Path::ALL_FILES_LEN);
	NN<IO::Path::FindFileSession> sess;
	if (IO::Path::FindFile(CSTRP(sbuff, sptrEnd)).SetTo(sess))
	{
		IO::Path::PathType pt;
		while (IO::Path::FindNextFile(sptr, sess, 0, pt, 0).SetTo(sptrEnd))
		{
			UOSInt i;
			if (pt == IO::Path::PathType::File)
			{
				i = Text::StrLastIndexOfCharC(sptr, (UOSInt)(sptrEnd - sptr), '.');
				if (i != INVALID_INDEX)
				{
					sptr2End = Text::StrConcatC(sptr2, sptr, i + 1);
					sptr2End = Text::StrToUpperC(sptr2End, &sptr[i + 1], (UOSInt)(sptrEnd - &sptr[i + 1]));
					IO::FileUtil::RenameFile(sbuff, sbuff2);
				}
			}
		}
		IO::Path::FindFileClose(sess);
	}
}

void __stdcall SSWR::AVIRead::AVIRBatchRenameForm::OnRenameExtLowerClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRBatchRenameForm> me = userObj.GetNN<SSWR::AVIRead::AVIRBatchRenameForm>();
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	UnsafeArray<UTF8Char> sptrEnd;
	UTF8Char sbuff2[512];
	UnsafeArray<UTF8Char> sptr2;
	UnsafeArray<UTF8Char> sptr2End;
	Text::StringBuilderUTF8 sb;
	me->txtDirectory->GetText(sb);
	if (IO::Path::GetPathType(sb.ToCString()) != IO::Path::PathType::Directory)
	{
		me->ui->ShowMsgOK(CSTR("Please enter valid directory path"), CSTR("Batch Rename"), me);
		return;
	}
	if (!sb.EndsWith(IO::Path::PATH_SEPERATOR))
	{
		sb.AppendUTF8Char(IO::Path::PATH_SEPERATOR);
	}
	sptr = sb.ConcatTo(sbuff);
	sptr2 = sb.ConcatTo(sbuff2);
	sptrEnd = Text::StrConcatC(sptr, IO::Path::ALL_FILES, IO::Path::ALL_FILES_LEN);
	NN<IO::Path::FindFileSession> sess;
	if (IO::Path::FindFile(CSTRP(sbuff, sptrEnd)).SetTo(sess))
	{
		IO::Path::PathType pt;
		while (IO::Path::FindNextFile(sptr, sess, 0, pt, 0).SetTo(sptrEnd))
		{
			UOSInt i;
			if (pt == IO::Path::PathType::File)
			{
				i = Text::StrLastIndexOfCharC(sptr, (UOSInt)(sptrEnd - sptr), '.');
				if (i != INVALID_INDEX)
				{
					sptr2End = Text::StrConcatC(sptr2, sptr, i + 1);
					sptr2End = Text::StrToLowerC(sptr2End, &sptr[i + 1], (UOSInt)(sptrEnd - &sptr[i + 1]));
					IO::FileUtil::RenameFile(sbuff, sbuff2);
				}
			}
		}
		IO::Path::FindFileClose(sess);
	}
}

SSWR::AVIRead::AVIRBatchRenameForm::AVIRBatchRenameForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 800, 320, ui)
{
	this->SetFont(0, 0, 8.25, false);
	this->SetText(CSTR("Batch Rename"));
	this->SetNoResize(true);
	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->lblDirectory = ui->NewLabel(*this, CSTR("Directory"));
	this->lblDirectory->SetRect(4, 4, 100, 23, false);
	this->txtDirectory = ui->NewTextBox(*this, CSTR(""));
	this->txtDirectory->SetRect(104, 4, 600, 23, false);
	this->btnRenameExtUpper = ui->NewButton(*this, CSTR("Rename Ext Upper"));
	this->btnRenameExtUpper->SetRect(104, 28, 150, 23, false);
	this->btnRenameExtUpper->HandleButtonClick(OnRenameExtUpperClicked, this);
	this->btnRenameExtLower = ui->NewButton(*this, CSTR("Rename Ext Lower"));
	this->btnRenameExtLower->SetRect(254, 28, 150, 23, false);
	this->btnRenameExtLower->HandleButtonClick(OnRenameExtLowerClicked, this);
}

SSWR::AVIRead::AVIRBatchRenameForm::~AVIRBatchRenameForm()
{
}

void SSWR::AVIRead::AVIRBatchRenameForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
