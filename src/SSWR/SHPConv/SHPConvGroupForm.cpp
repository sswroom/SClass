#include "Stdafx.h"
#include "SSWR/SHPConv/SHPConvGroupForm.h"

void __stdcall SSWR::SHPConv::SHPConvGroupForm::OnOkClicked(AnyType userObj)
{
	NN<SSWR::SHPConv::SHPConvGroupForm> me = userObj.GetNN<SSWR::SHPConv::SHPConvGroupForm>();
	me->SetDialogResult(UI::GUIForm::DR_OK);
}

void __stdcall SSWR::SHPConv::SHPConvGroupForm::OnCancelClicked(AnyType userObj)
{
	NN<SSWR::SHPConv::SHPConvGroupForm> me = userObj.GetNN<SSWR::SHPConv::SHPConvGroupForm>();
	me->SetDialogResult(UI::GUIForm::DR_CANCEL);
}

SSWR::SHPConv::SHPConvGroupForm::SHPConvGroupForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui) : UI::GUIForm(parent, 300, 216, ui)
{
	this->SetText(CSTR("Group"));
	this->SetFont(0, 0, 8.25, false);
	this->SetNoResize(true);
	NEW_CLASS(this->monMgr, Media::MonitorMgr());
	this->SetDPI(this->monMgr->GetMonitorHDPI(this->GetHMonitor()), this->monMgr->GetMonitorDDPI(this->GetHMonitor()));
	
	this->lblGroup = ui->NewLabel(*this, CSTR("Groups"));
	this->lblGroup->SetRect(8, 0, 100, 23, false);
	this->lbGroup = ui->NewListBox(*this, false);
	this->lbGroup->SetRect(8, 24, 264, 121, false);
	this->btnOk = ui->NewButton(*this, CSTR("&Ok"));
	this->btnOk->SetRect(40, 152, 75, 23, false);
	this->btnOk->HandleButtonClick(OnOkClicked, this);
	this->btnCancel = ui->NewButton(*this, CSTR("&Cancel"));
	this->btnCancel->SetRect(40, 152, 75, 23, false);
	this->btnCancel->HandleButtonClick(OnCancelClicked, this);

	this->SetDefaultButton(this->btnOk);
	this->SetCancelButton(this->btnCancel);
}

SSWR::SHPConv::SHPConvGroupForm::~SHPConvGroupForm()
{
	DEL_CLASS(this->monMgr);
}

void SSWR::SHPConv::SHPConvGroupForm::OnMonitorChanged()
{
	this->SetDPI(this->monMgr->GetMonitorHDPI(this->GetHMonitor()), this->monMgr->GetMonitorDDPI(this->GetHMonitor()));
}

void SSWR::SHPConv::SHPConvGroupForm::AddGroup(Text::CStringNN grpName)
{
	this->lbGroup->AddItem(grpName, 0);
}

void SSWR::SHPConv::SHPConvGroupForm::SetCurrGroup(UOSInt currGroup)
{
	this->lbGroup->SetSelectedIndex(currGroup + 1);
}

UOSInt SSWR::SHPConv::SHPConvGroupForm::GetCurrGroup()
{
	return (UOSInt)(this->lbGroup->GetSelectedIndex() - 1);
}
