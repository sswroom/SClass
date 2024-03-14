#include "Stdafx.h"
#include "IO/FileUtil.h"
#include "IO/Path.h"
#include "SSWR/AVIRead/AVIRFileRenameForm.h"

void __stdcall SSWR::AVIRead::AVIRFileRenameForm::OnRenameClicked(AnyType userObj)
{
	NotNullPtr<SSWR::AVIRead::AVIRFileRenameForm> me = userObj.GetNN<SSWR::AVIRead::AVIRFileRenameForm>();
	Text::StringBuilderUTF8 sbName;
	Text::StringBuilderUTF8 sbExt;
	Text::StringBuilderUTF8 sbPath;
	me->txtNewName->GetText(sbName);
	me->txtNewExt->GetText(sbExt);
	if (sbName.GetLength() == 0)
	{
		me->ui->ShowMsgOK(CSTR("Please enter file name"), CSTR("File Rename"), me);
		return;
	}
	sbPath.Append(me->fileName);
	UOSInt i = sbPath.LastIndexOf(IO::Path::PATH_SEPERATOR);
	sbPath.TrimToLength(i + 1);
	sbPath.Append(sbName);
	if (sbExt.GetLength() > 0)
	{
		sbPath.AppendUTF8Char('.');
		sbPath.Append(sbExt);
	}
	if (sbPath.Equals(me->fileName))
	{
		me->ui->ShowMsgOK(CSTR("File name not changed"), CSTR("File Rename"), me);
		return;
	}
	if (IO::FileUtil::RenameFile(me->fileName->v, sbPath.ToString()))
	{
		me->fileName->Release();
		me->fileName = Text::String::New(sbPath.ToCString());
		me->SetDialogResult(UI::GUIForm::DR_OK);
	}
	else
	{
		me->ui->ShowMsgOK(CSTR("Error in renaming file"), CSTR("File Rename"), me);
	}
}

void __stdcall SSWR::AVIRead::AVIRFileRenameForm::OnCancelClicked(AnyType userObj)
{
	NotNullPtr<SSWR::AVIRead::AVIRFileRenameForm> me = userObj.GetNN<SSWR::AVIRead::AVIRFileRenameForm>();
	me->SetDialogResult(UI::GUIForm::DR_CANCEL);
}

SSWR::AVIRead::AVIRFileRenameForm::AVIRFileRenameForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core, NotNullPtr<Text::String> fileName) : UI::GUIForm(parent, 532, 120, ui)
{
	this->SetFont(0, 0, 8.25, false);
	this->fileName = fileName->Clone();
	UOSInt i = this->fileName->LastIndexOf(IO::Path::PATH_SEPERATOR);
	Text::StringBuilderUTF8 sb;
	sb.AppendC(UTF8STRC("File rename - "));
	Text::CStringNN shortName = this->fileName->ToCString().Substring(i + 1);
	sb.Append(shortName);
	this->SetText(sb.ToCString());

	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->lblNewName = ui->NewLabel(*this, CSTR("New Name"));
	this->lblNewName->SetRect(4, 4, 100, 23, false);
	this->txtNewName = ui->NewTextBox(*this, CSTR(""));
	this->txtNewName->SetRect(104, 4, 300, 23, false);
	this->txtNewExt = ui->NewTextBox(*this, CSTR(""));
	this->txtNewExt->SetRect(404, 4, 100, 23, false);
	this->btnRename = ui->NewButton(*this, CSTR("Rename"));
	this->btnRename->SetRect(104, 28, 75, 23, false);
	this->btnRename->HandleButtonClick(OnRenameClicked, this);
	this->btnCancel = ui->NewButton(*this, CSTR("Cancel"));
	this->btnCancel->SetRect(184, 28, 75, 23, false);
	this->btnCancel->HandleButtonClick(OnCancelClicked, this);

	this->SetDefaultButton(this->btnRename);
	this->SetCancelButton(this->btnCancel);
	i = shortName.LastIndexOf('.');
	if (i != INVALID_INDEX)
	{
		this->txtNewExt->SetText(shortName.Substring(i + 1));
		sb.ClearStr();
		sb.AppendC(shortName.v, i);
		this->txtNewName->SetText(sb.ToCString());
	}
	else
	{
		this->txtNewName->SetText(shortName);
	}
}

SSWR::AVIRead::AVIRFileRenameForm::~AVIRFileRenameForm()
{
	this->fileName->Release();
}

void SSWR::AVIRead::AVIRFileRenameForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}

NotNullPtr<Text::String> SSWR::AVIRead::AVIRFileRenameForm::GetFileName() const
{
	return this->fileName;
}