#include "Stdafx.h"
#include "IO/Path.h"
#include "SSWR/AVIRead/AVIRProgramLinksCreateForm.h"

void __stdcall SSWR::AVIRead::AVIRProgramLinksCreateForm::OnCreateClicked(void *userObj)
{
	SSWR::AVIRead::AVIRProgramLinksCreateForm *me = (SSWR::AVIRead::AVIRProgramLinksCreateForm *)userObj;
	Text::StringBuilderUTF8 sbShortName;
	Text::StringBuilderUTF8 sbName;
	Text::StringBuilderUTF8 sbComment;
	Text::StringBuilderUTF8 sbCategories;
	Text::StringBuilderUTF8 sbCmdLine;
	me->txtShortName->GetText(sbShortName);
	me->txtName->GetText(sbName);
	me->txtComment->GetText(sbComment);
	me->txtCategories->GetText(sbCategories);
	me->txtCmdLine->GetText(sbCmdLine);
	if (sbShortName.GetLength() == 0)
	{
		me->ui->ShowMsgOK(CSTR("Please enter Short Name"), CSTR("Program Links Create"), me);
		return;
	}
	if (sbName.GetLength() == 0)
	{
		me->ui->ShowMsgOK(CSTR("Please enter Name"), CSTR("Program Links Create"), me);
		return;
	}
	if (sbCmdLine.GetLength() == 0)
	{
		me->ui->ShowMsgOK(CSTR("Please enter Command Line"), CSTR("Program Links Create"), me);
		return;
	}
	if (me->progMgr->CreateLink(me->chkThisUser->IsChecked(), sbShortName.ToCString(), sbName.ToCString(), sbComment.ToCString(), sbCategories.ToCString(), sbCmdLine.ToCString()))
	{
		me->SetDialogResult(UI::GUIForm::DR_OK);
	}
	else
	{
		me->ui->ShowMsgOK(CSTR("Failed to create program link"), CSTR("Program Links Create"), me);
		return;
	}
}

void __stdcall SSWR::AVIRead::AVIRProgramLinksCreateForm::OnCancelClicked(void *userObj)
{
	SSWR::AVIRead::AVIRProgramLinksCreateForm *me = (SSWR::AVIRead::AVIRProgramLinksCreateForm *)userObj;
	me->SetDialogResult(UI::GUIForm::DR_CANCEL);
}

SSWR::AVIRead::AVIRProgramLinksCreateForm::AVIRProgramLinksCreateForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core, IO::ProgramLinkManager *progMgr) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetText(CSTR("Program Links Create"));
	this->SetFont(0, 0, 8.25, false);

	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASS(this->chkThisUser, UI::GUICheckBox(ui, *this, CSTR("This User"), true));
	this->chkThisUser->SetRect(104, 4, 100, 23, false);
	this->lblShortName = ui->NewLabel(*this, CSTR("Short Name"));
	this->lblShortName->SetRect(4, 28, 100, 23, false);
	this->txtShortName = ui->NewTextBox(*this, CSTR(""));
	this->txtShortName->SetRect(104, 28, 100, 23, false);
	this->lblName = ui->NewLabel(*this, CSTR("Name"));
	this->lblName->SetRect(4, 52, 100, 23, false);
	this->txtName = ui->NewTextBox(*this, CSTR(""));
	this->txtName->SetRect(104, 52, 200, 23, false);
	this->lblComment = ui->NewLabel(*this, CSTR("Comment"));
	this->lblComment->SetRect(4, 76, 100, 23, false);
	this->txtComment = ui->NewTextBox(*this, CSTR(""));
	this->txtComment->SetRect(104, 76, 500, 23, false);
	this->lblCategories = ui->NewLabel(*this, CSTR("Categories"));
	this->lblCategories->SetRect(4, 100, 100, 23, false);
	this->txtCategories = ui->NewTextBox(*this, CSTR(""));
	this->txtCategories->SetRect(104, 100, 500, 23, false);
	this->lblCmdLine = ui->NewLabel(*this, CSTR("Command Line"));
	this->lblCmdLine->SetRect(4, 124, 100, 23, false);
	this->txtCmdLine = ui->NewTextBox(*this, CSTR(""));
	this->txtCmdLine->SetRect(104, 124, 500, 23, false);
	this->btnCreate = ui->NewButton(*this, CSTR("Create"));
	this->btnCreate->SetRect(104, 148, 75, 23, false);
	this->btnCreate->HandleButtonClick(OnCreateClicked, this);
	this->btnCancel = ui->NewButton(*this, CSTR("Cancel"));
	this->btnCancel->SetRect(184, 148, 75, 23, false);
	this->btnCancel->HandleButtonClick(OnCancelClicked, this);
	this->SetDefaultButton(this->btnCreate);
	this->SetCancelButton(this->btnCancel);

	UTF8Char sbuff[512];
	UTF8Char *sptr;
	sptr = IO::Path::GetProcessFileName(sbuff);
	UOSInt i = Text::StrLastIndexOfCharC(sbuff, (UOSInt)(sptr - sbuff), IO::Path::PATH_SEPERATOR);
	this->txtCmdLine->SetText(CSTRP(sbuff, sptr));
	UOSInt j = Text::StrLastIndexOfCharC(&sbuff[i + 1], (UOSInt)(sptr - &sbuff[i + 1]), '.');
	if (j != INVALID_INDEX)
	{
		sbuff[i + 1 + j] = 0;
		sptr = &sbuff[i + 1 + j];
	}
	this->txtShortName->SetText(CSTRP(&sbuff[i + 1], sptr));
	this->txtName->SetText(CSTRP(&sbuff[i + 1], sptr));
}

SSWR::AVIRead::AVIRProgramLinksCreateForm::~AVIRProgramLinksCreateForm()
{

}

void SSWR::AVIRead::AVIRProgramLinksCreateForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
