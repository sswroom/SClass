#include "Stdafx.h"
#include "SSWR/OrganMgr/OrganSearchForm.h"
#include "UI/MessageDialog.h"

void __stdcall SSWR::OrganMgr::OrganSearchForm::OnOKClicked(void *userObj)
{
	UTF8Char sbuff[256];
	UTF8Char sbuff2[256];
	OrganSearchForm *me = (OrganSearchForm *)userObj;
	if (sbuff == me->txt->GetText(sbuff))
	{
		return;
	}
	Text::StrTrim(sbuff);

	me->foundGroup = me->env->SearchObject(sbuff, sbuff2, sizeof(sbuff2), &me->parentId);
	if (me->foundGroup)
	{
		me->foundStr = Text::StrCopyNew(sbuff2);
		me->SetDialogResult(DR_OK);
	}
	else
	{
		UI::MessageDialog::ShowDialog(me->env->GetLang(UTF8STRC("SearchNotFound")), me->env->GetLang(UTF8STRC("SearchTitle")), me);
		me->txt->Focus();
	}
}

void __stdcall SSWR::OrganMgr::OrganSearchForm::OnCancelClicked(void *userObj)
{
	OrganSearchForm *me = (OrganSearchForm *)userObj;
	me->SetDialogResult(DR_CANCEL);
}

SSWR::OrganMgr::OrganSearchForm::OrganSearchForm(UI::GUIClientControl *parent, UI::GUICore *ui, OrganEnv *env) : UI::GUIForm(0, 340, 90, ui)
{
	this->SetFont(0, 0, 10.5, false);
	this->SetNoResize(true);
	this->env = env;
	this->foundStr = 0;
	this->foundGroup = 0;
	this->parentId = 0;

	this->SetText(this->env->GetLang(UTF8STRC("SearchTitle")));

	NEW_CLASS(this->lbl, UI::GUILabel(ui, this, this->env->GetLang(UTF8STRC("SearchName"))));
	lbl->SetRect(4, 6, 100, 21, false);
	NEW_CLASS(this->txt, UI::GUITextBox(ui, this, CSTR("")));
	txt->SetRect(108, 6, 218, 23, false);
	NEW_CLASS(this->btnOK, UI::GUIButton(ui, this, this->env->GetLang(UTF8STRC("SearchButton"))));
	this->btnOK->SetRect(76, 36, 75, 23, false);
	this->btnOK->HandleButtonClick(OnOKClicked, this);
	NEW_CLASS(this->btnCancel, UI::GUIButton(ui, this, this->env->GetLang(UTF8STRC("SearchCancel"))));
	this->btnCancel->SetRect(180, 36, 75, 23, false);
	this->btnCancel->HandleButtonClick(OnCancelClicked, this);
	this->SetDefaultButton(this->btnOK);
	this->SetCancelButton(this->btnCancel);
	this->txt->Focus();
	this->SetDPI(this->env->GetMonitorHDPI(this->GetHMonitor()), this->env->GetMonitorDDPI(this->GetHMonitor()));
}

SSWR::OrganMgr::OrganSearchForm::~OrganSearchForm()
{
	SDEL_TEXT(this->foundStr);
}

void SSWR::OrganMgr::OrganSearchForm::OnMonitorChanged()
{
	this->SetDPI(this->env->GetMonitorHDPI(this->GetHMonitor()), this->env->GetMonitorDDPI(this->GetHMonitor()));
}

const UTF8Char *SSWR::OrganMgr::OrganSearchForm::GetFoundStr()
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
