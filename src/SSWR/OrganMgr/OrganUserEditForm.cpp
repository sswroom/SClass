#include "Stdafx.h"
#include "SSWR/OrganMgr/OrganUserEditForm.h"
#include "UI/MessageDialog.h"

void __stdcall SSWR::OrganMgr::OrganUserEditForm::OnOKClicked(void *userObj)
{
	OrganUserEditForm *me = (OrganUserEditForm *)userObj;
	Text::StringBuilderUTF8 sb1;
	Text::StringBuilderUTF8 sb2;
	Text::StringBuilderUTF8 sb3;
	me->txtPassword->GetText(&sb1);
	me->txtRetype->GetText(&sb2);
	if (!sb1.Equals(&sb2))
	{
		UI::MessageDialog::ShowDialog((const UTF8Char*)"Password and retype password do not match", (const UTF8Char*)"Error", me);
		return;
	}
	sb1.ClearStr();
	me->txtUserName->GetText(&sb1);
	me->txtWatermark->GetText(&sb3);
	if (sb1.GetLength() == 0)
	{
		UI::MessageDialog::ShowDialog((const UTF8Char*)"Please enter user name", (const UTF8Char*)"Error", me);
		return;
	}
	if (sb3.GetLength() == 0)
	{
		UI::MessageDialog::ShowDialog((const UTF8Char*)"Please enter watermark", (const UTF8Char*)"Error", me);
		return;
	}
	if (me->user == 0)
	{
		if (sb2.GetLength() == 0)
		{
			UI::MessageDialog::ShowDialog((const UTF8Char*)"Please enter password", (const UTF8Char*)"Error", me);
			return;
		}
		if (me->env->AddWebUser(sb1.ToString(), sb2.ToString(), sb3.ToString(), SSWR::OrganMgr::UT_USER))
		{
			me->SetDialogResult(UI::GUIForm::DR_OK);
		}
	}
	else
	{
		if (sb2.GetLength() == 0)
		{
			if (me->env->ModifyWebUser(me->user->id, sb1.ToString(), 0, sb3.ToString()))
			{
				me->SetDialogResult(UI::GUIForm::DR_OK);
			}
		}
		else
		{
			if (me->env->ModifyWebUser(me->user->id, sb1.ToString(), sb2.ToString(), sb3.ToString()))
			{
				me->SetDialogResult(UI::GUIForm::DR_OK);
			}
		}
	}
}

void __stdcall SSWR::OrganMgr::OrganUserEditForm::OnCancelClicked(void *userObj)
{
	OrganUserEditForm *me = (OrganUserEditForm *)userObj;
	me->SetDialogResult(UI::GUIForm::DR_CANCEL);
}

SSWR::OrganMgr::OrganUserEditForm::OrganUserEditForm(UI::GUIClientControl *parent, UI::GUICore *ui, OrganEnv *env, OrganWebUser *user) : UI::GUIForm(parent, 480, 160, ui)
{
	this->SetFont(0, 0, 10.5, false);
	this->SetNoResize(true);

	this->env = env;
	this->user = user;

	if (user)
	{
		this->SetText(this->env->GetLang(UTF8STRC("UserEditTitleModify")));
	}
	else
	{
		this->SetText(this->env->GetLang(UTF8STRC("UserEditTitleModify")));
	}

	NEW_CLASS(this->lblUserName, UI::GUILabel(ui, this, this->env->GetLang(UTF8STRC("UserEditUserName")).v));
	this->lblUserName->SetRect(64, 4, 100, 23, false);
	NEW_CLASS(this->txtUserName, UI::GUITextBox(ui, this, CSTR("")));
	this->txtUserName->SetRect(164, 4, 200, 23, false);
	NEW_CLASS(this->lblPassword, UI::GUILabel(ui, this, this->env->GetLang(UTF8STRC("UserEditPassword")).v));
	this->lblPassword->SetRect(64, 28, 100, 23, false);
	NEW_CLASS(this->txtPassword, UI::GUITextBox(ui, this, CSTR("")));
	this->txtPassword->SetRect(164, 28, 200, 23, false);
	this->txtPassword->SetPasswordChar('*');
	NEW_CLASS(this->lblRetype, UI::GUILabel(ui, this, this->env->GetLang(UTF8STRC("UserEditRetype")).v));
	this->lblRetype->SetRect(64, 52, 100, 23, false);
	NEW_CLASS(this->txtRetype, UI::GUITextBox(ui, this, CSTR("")));
	this->txtRetype->SetRect(164, 52, 200, 23, false);
	this->txtRetype->SetPasswordChar('*');
	NEW_CLASS(this->lblWatermark, UI::GUILabel(ui, this, this->env->GetLang(UTF8STRC("UserEditWatermark")).v));
	this->lblWatermark->SetRect(64, 76, 100, 23, false);
	NEW_CLASS(this->txtWatermark, UI::GUITextBox(ui, this, CSTR("")));
	this->txtWatermark->SetRect(164, 76, 200, 23, false);
	NEW_CLASS(this->btnOK, UI::GUIButton(ui, this, this->env->GetLang(UTF8STRC("UserEditOk"))));
	this->btnOK->SetRect(164, 108, 75, 23, false);
	this->btnOK->HandleButtonClick(OnOKClicked, this);
	NEW_CLASS(this->btnCancel, UI::GUIButton(ui, this, this->env->GetLang(UTF8STRC("UserEditCancel"))));
	this->btnCancel->SetRect(244, 108, 75, 23, false);
	this->btnCancel->HandleButtonClick(OnCancelClicked, this);

	if (this->user)
	{
		this->txtUserName->SetText(this->user->userName->ToCString());
		this->txtWatermark->SetText(this->user->watermark->ToCString());
	}
	this->txtUserName->Focus();
	this->SetDefaultButton(this->btnOK);
	this->SetCancelButton(this->btnCancel);
	this->SetDPI(this->env->GetMonitorHDPI(this->GetHMonitor()), this->env->GetMonitorDDPI(this->GetHMonitor()));
}

SSWR::OrganMgr::OrganUserEditForm::~OrganUserEditForm()
{
}

void SSWR::OrganMgr::OrganUserEditForm::OnMonitorChanged()
{
	this->SetDPI(this->env->GetMonitorHDPI(this->GetHMonitor()), this->env->GetMonitorDDPI(this->GetHMonitor()));
}
