#include "Stdafx.h"
#include "IO/Path.h"
#include "SSWR/AVIRead/AVIRServiceCreateForm.h"

void __stdcall SSWR::AVIRead::AVIRServiceCreateForm::OnCreateClicked(void *userObj)
{
	SSWR::AVIRead::AVIRServiceCreateForm *me = (SSWR::AVIRead::AVIRServiceCreateForm*)userObj;
	Text::StringBuilderUTF8 sbName;
	Text::StringBuilderUTF8 sbDesc;
	Text::StringBuilderUTF8 sbCmdLine;
	me->txtName->GetText(sbName);
	me->txtDesc->GetText(sbDesc);
	me->txtCmdLine->GetText(sbCmdLine);
	if (sbName.GetLength() == 0)
	{
		me->ui->ShowMsgOK(CSTR("Please enter name"), CSTR("Create Service"), me);
		return;
	}
	if (sbName.IndexOf('.') != INVALID_INDEX)
	{
		me->ui->ShowMsgOK(CSTR("Name cannot contain . character"), CSTR("Create Service"), me);
		return;
	}
	IO::ServiceManager::ServiceDetail sd;
	if (me->svcMgr.ServiceGetDetail(sbName.ToCString(), &sd))
	{
		me->ui->ShowMsgOK(CSTR("Service Name already exist"), CSTR("Create Service"), me);
		return;
	}
	if (sbCmdLine.GetLength() == 0)
	{
		me->ui->ShowMsgOK(CSTR("Please enter command line"), CSTR("Create Service"), me);
		return;
	}
	if (me->svcMgr.ServiceCreate(sbName.ToCString(), sbDesc.ToCString(), sbCmdLine.ToCString(), (IO::ServiceInfo::ServiceState)(OSInt)me->cboState->GetSelectedItem()))
	{
		me->SetDialogResult(UI::GUIForm::DR_OK);
	}
	else
	{
		me->ui->ShowMsgOK(CSTR("Error in creating service"), CSTR("Create Service"), me);
		return;
	}
}

void __stdcall SSWR::AVIRead::AVIRServiceCreateForm::OnCancelClicked(void *userObj)
{
	SSWR::AVIRead::AVIRServiceCreateForm *me = (SSWR::AVIRead::AVIRServiceCreateForm*)userObj;
	me->SetDialogResult(UI::GUIForm::DR_CANCEL);
}

SSWR::AVIRead::AVIRServiceCreateForm::AVIRServiceCreateForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 640, 200, ui)
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	this->SetFont(0, 0, 8.25, false);
	this->SetText(CSTR("Create Service"));

	this->core = core;

	this->lblName = ui->NewLabel(*this, CSTR("Name"));
	this->lblName->SetRect(4, 4, 100, 23, false);
	this->txtName = ui->NewTextBox(*this, CSTR("AVIRead"));
	this->txtName->SetRect(104, 4, 200, 23, false);
	this->lblDesc = ui->NewLabel(*this, CSTR("Description"));
	this->lblDesc->SetRect(4, 28, 100, 23, false);
	this->txtDesc = ui->NewTextBox(*this, CSTR(""));
	this->txtDesc->SetRect(104, 28, 500, 23, false);
	this->lblState = ui->NewLabel(*this, CSTR("State"));
	this->lblState->SetRect(4, 52, 100, 23, false);
	this->cboState = ui->NewComboBox(*this, false);
	this->cboState->SetRect(104, 52, 100, 23, false);
	this->cboState->AddItem(CSTR("Active"), (void*)IO::ServiceInfo::ServiceState::Active);
	this->cboState->AddItem(CSTR("Inactive"), (void*)IO::ServiceInfo::ServiceState::Inactive);
	this->cboState->AddItem(CSTR("Manual Start"), (void*)IO::ServiceInfo::ServiceState::ManualStart);
	this->lblCmdLine = ui->NewLabel(*this, CSTR("Command Line"));
	this->lblCmdLine->SetRect(4, 76, 100, 23, false);
	sptr = IO::Path::GetProcessFileName(sbuff);
	this->txtCmdLine = ui->NewTextBox(*this, CSTRP(sbuff, sptr));
	this->txtCmdLine->SetRect(104, 76, 500, 23, false);
	this->btnCreate = ui->NewButton(*this, CSTR("Create"));
	this->btnCreate->SetRect(104, 100, 75, 23, false);
	this->btnCreate->HandleButtonClick(OnCreateClicked, this);
	this->btnCancel = ui->NewButton(*this, CSTR("Cancel"));
	this->btnCancel->SetRect(184, 100, 75, 23, false);
	this->btnCancel->HandleButtonClick(OnCancelClicked, this);
	this->cboState->SetSelectedIndex(1);
}

SSWR::AVIRead::AVIRServiceCreateForm::~AVIRServiceCreateForm()
{
}

void SSWR::AVIRead::AVIRServiceCreateForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
