#include "Stdafx.h"
#include "SSWR/OrganMgr/OrganSearchForm.h"

void __stdcall SSWR::OrganMgr::OrganSearchForm::OnOKClicked(AnyType userObj)
{
	UTF8Char sbuff[256];
	UTF8Char sbuff2[256];
	NotNullPtr<OrganSearchForm> me = userObj.GetNN<OrganSearchForm>();
	if (sbuff == me->txt->GetText(sbuff))
	{
		return;
	}
	Text::StrTrim(sbuff);

	me->foundGroup = me->env->SearchObject(sbuff, sbuff2, sizeof(sbuff2), &me->parentId);
	if (me->foundGroup)
	{
		me->foundStr = Text::String::NewNotNullSlow(sbuff2).Ptr();
		me->SetDialogResult(DR_OK);
	}
	else
	{
		me->ui->ShowMsgOK(me->env->GetLang(CSTR("SearchNotFound")), me->env->GetLang(CSTR("SearchTitle")), me);
		me->txt->Focus();
	}
}

void __stdcall SSWR::OrganMgr::OrganSearchForm::OnCancelClicked(AnyType userObj)
{
	NotNullPtr<OrganSearchForm> me = userObj.GetNN<OrganSearchForm>();
	me->SetDialogResult(DR_CANCEL);
}

SSWR::OrganMgr::OrganSearchForm::OrganSearchForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, OrganEnv *env) : UI::GUIForm(0, 340, 90, ui)
{
	this->SetFont(0, 0, 10.5, false);
	this->SetNoResize(true);
	this->env = env;
	this->foundStr = 0;
	this->foundGroup = 0;
	this->parentId = 0;

	this->SetText(this->env->GetLang(CSTR("SearchTitle")));

	this->lbl = ui->NewLabel(*this, this->env->GetLang(CSTR("SearchName")));
	this->lbl->SetRect(4, 6, 100, 21, false);
	this->txt = ui->NewTextBox(*this, CSTR(""));
	this->txt->SetRect(108, 6, 218, 23, false);
	this->btnOK = ui->NewButton(*this, this->env->GetLang(CSTR("SearchButton")));
	this->btnOK->SetRect(76, 36, 75, 23, false);
	this->btnOK->HandleButtonClick(OnOKClicked, this);
	this->btnCancel = ui->NewButton(*this, this->env->GetLang(CSTR("SearchCancel")));
	this->btnCancel->SetRect(180, 36, 75, 23, false);
	this->btnCancel->HandleButtonClick(OnCancelClicked, this);
	this->SetDefaultButton(this->btnOK);
	this->SetCancelButton(this->btnCancel);
	this->txt->Focus();
	this->SetDPI(this->env->GetMonitorHDPI(this->GetHMonitor()), this->env->GetMonitorDDPI(this->GetHMonitor()));
}

SSWR::OrganMgr::OrganSearchForm::~OrganSearchForm()
{
	SDEL_STRING(this->foundStr);
}

void SSWR::OrganMgr::OrganSearchForm::OnMonitorChanged()
{
	this->SetDPI(this->env->GetMonitorHDPI(this->GetHMonitor()), this->env->GetMonitorDDPI(this->GetHMonitor()));
}

Text::String *SSWR::OrganMgr::OrganSearchForm::GetFoundStr()
{
	return this->foundStr;
}

SSWR::OrganMgr::OrganGroup *SSWR::OrganMgr::OrganSearchForm::GetFoundGroup()
{
	return this->foundGroup;
}

Int32 SSWR::OrganMgr::OrganSearchForm::GetParentId()
{
	return this->parentId;
}
