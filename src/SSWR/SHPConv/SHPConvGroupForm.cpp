#include "Stdafx.h"
#include "SSWR/SHPConv/SHPConvGroupForm.h"

void __stdcall SSWR::SHPConv::SHPConvGroupForm::OnOkClicked(void *userObj)
{
	SSWR::SHPConv::SHPConvGroupForm *me = (SSWR::SHPConv::SHPConvGroupForm *)userObj;
	me->SetDialogResult(UI::GUIForm::DR_OK);
}

void __stdcall SSWR::SHPConv::SHPConvGroupForm::OnCancelClicked(void *userObj)
{
	SSWR::SHPConv::SHPConvGroupForm *me = (SSWR::SHPConv::SHPConvGroupForm *)userObj;
	me->SetDialogResult(UI::GUIForm::DR_CANCEL);
}

SSWR::SHPConv::SHPConvGroupForm::SHPConvGroupForm(UI::GUIClientControl *parent, UI::GUICore *ui) : UI::GUIForm(parent, 300, 216, ui)
{
	this->SetText((const UTF8Char*)"Group");
	this->SetFont(0, 8.25, false);
	this->SetNoResize(true);
	NEW_CLASS(this->monMgr, Media::MonitorMgr());
	this->SetDPI(this->monMgr->GetMonitorHDPI(this->GetHMonitor()), this->monMgr->GetMonitorDDPI(this->GetHMonitor()));
	
	NEW_CLASS(this->lblGroup, UI::GUILabel(ui, this, (const UTF8Char*)"Groups"));
	this->lblGroup->SetRect(8, 0, 100, 23, false);
	NEW_CLASS(this->lbGroup, UI::GUIListBox(ui, this, false));
	this->lbGroup->SetRect(8, 24, 264, 121, false);
	NEW_CLASS(this->btnOk, UI::GUIButton(ui, this, (const UTF8Char*)"&Ok"));
	this->btnOk->SetRect(40, 152, 75, 23, false);
	this->btnOk->HandleButtonClick(OnOkClicked, this);
	NEW_CLASS(this->btnCancel, UI::GUIButton(ui, this, (const UTF8Char*)"&Cancel"));
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

void SSWR::SHPConv::SHPConvGroupForm::AddGroup(const UTF8Char *grpName)
{
	this->lbGroup->AddItem(grpName, 0);
}

void SSWR::SHPConv::SHPConvGroupForm::SetCurrGroup(OSInt currGroup)
{
	this->lbGroup->SetSelectedIndex(currGroup + 1);
}

OSInt SSWR::SHPConv::SHPConvGroupForm::GetCurrGroup()
{
	return this->lbGroup->GetSelectedIndex() - 1;
}
